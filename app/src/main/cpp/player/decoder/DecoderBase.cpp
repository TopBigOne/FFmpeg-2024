//
// Created by dev on 2024/4/15.
//

#include "DecoderBase.h"
#include "LogUtil.h"

void DecoderBase::Start() {
    LOGCATD("DecoderBase::Start()")
    // case 1:
    if (m_Thread == nullptr) {
        LOGCATI("   case 1=>")
        StartDecodingThread();
        return;
    }
    // case 2:
    LOGCATI("   case 2=>")
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();
}

void DecoderBase::Pause() {
    LOGCATD(__FUNCTION__)
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_DecoderState = STATE_PAUSE;
}

void DecoderBase::Stop() {
    LOGCATD(__FUNCTION__)
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_DecoderState = STATE_STOP;
    m_Cond.notify_all();
}

void DecoderBase::SeekToPosition(float position) {
    LOGCATD(__FUNCTION__)


}

float DecoderBase::GetCurrentPosition() {
    //std::unique_lock<std::mutex> lock(m_Mutex);//读写保护
    //单位 ms
    return m_CurTimeStamp;
}


int DecoderBase::Init(const char *url, AVMediaType mediaType) {
    LOGCATD(__FUNCTION__)
    LOGCATI("   url : %s, mediaType : %d", url, mediaType)
    strcpy(m_Url, url);
    m_MediaType = mediaType;
    return 0;
}


void DecoderBase::UnInit() {

}


int DecoderBase::InitFFDecoder() {
    LOGCATD(__FUNCTION__)
    int result;
    do {
        // step 1:
        m_AVFormatContext = avformat_alloc_context();
        // step 2: open the file.
        result = avformat_open_input(&m_AVFormatContext, m_Url, nullptr, nullptr);
        if (result != 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
            av_strerror(result, errbuf, AV_ERROR_MAX_STRING_SIZE);
            LOGCATE("   avformat_open_input fail : %s", errbuf)
            break;
        }

        // step 3: 获取音视频流信息
        result = avformat_find_stream_info(m_AVFormatContext, nullptr);
        if (result < 0) {
            LOGCATE("   avformat_find_stream_info fail.");
            break;
        }
        // step 4: 获取音视频流index
        for (int i = 0; i < m_AVFormatContext->nb_streams; i++) {
            if (m_AVFormatContext->streams[i]->codecpar->codec_type == m_MediaType) {
                m_StreamIndex = i;
                break;
            }
        }
        if (m_StreamIndex == -1) {
            LOGCATE("   Fail to find stream index.");
            break;
        }
        // step 5： 获取解码器参数
        AVCodecParameters *codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;

        // step 6: 获取解码器
        m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (m_AVCodec == nullptr) {
            LOGCATE("   avcodec_find_decoder fail.")
            break;
        }

        // step 7: create the AVCodecContext
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
        if (avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0) {
            LOGCATE("   avcodec_parameters_to_context fail.");
            break;

        }

        // config AVDictionary
        AVDictionary *pAVDictionary = nullptr;
        // 指定缓冲区大小为1024000字节。这个值影响着数据传输时的缓冲区大小。
        av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
        // 设置超时时间为20000000微秒（20秒）。这个值表示在连接建立的过程中，等待服务器响应的最大时间。
        av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
        // 设置最大延迟时间为30000000微秒（30秒）。如果数据包在这个时间内没有到达，则会被丢弃。
        av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
        // 指定RTSP传输协议为TCP。RTSP是用于实时流传输的协议，这里设置为使用TCP作为传输协议。
        av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

        // step 8 ： 打开解码器
        result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
        if (result < 0) {
            LOGCATE("DecoderBase::InitFFDecoder avcodec_open2 fail. result=%d", result);
            break;
        }
        result = 0;

        m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms
        //创建 AVPacket 存放编码数据
        m_Packet = av_packet_alloc();
        //创建 AVFrame 存放解码后的数据
        m_Frame = av_frame_alloc();
    } while (false);

    if (result != 0 && m_MsgContext && m_MsgCallback)
        m_MsgCallback(m_MsgContext, MSG_DECODER_INIT_ERROR, 0);

    return result;
}


void DecoderBase::UnInitDecoder() {
    LOGCATD(__FUNCTION__)
    if (m_Frame != nullptr) {
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }

    if (m_Packet != nullptr) {
        av_packet_free(&m_Packet);
        m_Packet = nullptr;
    }

    if (m_AVCodecContext != nullptr) {
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }

    if (m_AVFormatContext != nullptr) {
        avformat_close_input(&m_AVFormatContext);
        avformat_free_context(m_AVFormatContext);
        m_AVFormatContext = nullptr;
    }

}

void DecoderBase::StartDecodingThread() {
    LOGCATD(__FUNCTION__)
    m_Thread = new thread(DoAVDecoding, this);
}

void DecoderBase::DecodingLoop() {
    LOGCATD(__FUNCTION__)
    LOGCATI("    DecodingLoop m_MediaType : %d", m_MediaType)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_DecoderState = STATE_DECODING;
        lock.unlock();
    }
    for (;;) {
        // case 1:解码暂停10毫秒，便于渲染线程消费avframe数据
        while (m_DecoderState == STATE_PAUSE) {
            LOGCATI("   case 1 : pause.")
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(10));
            m_StartTimeStamp = GetSysCurrentTime() - m_CurTimeStamp;

        }
        // case 2:
        if (m_DecoderState == STATE_STOP) {
            LOGCATI("   case 2 : stop.")
            break;
        }
        // case 3:
        if (m_StartTimeStamp == -1) {
            LOGCATI("   case 3 : get current time.")
            m_StartTimeStamp = GetSysCurrentTime();
        }
        // case 4:
        if (DecodeOnePacket() != 0) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_DecoderState = STATE_PAUSE;
            break;
        }


    }

    LOGCATI("   DecodingLoop end.")


}

int DecoderBase::DecodeOnePacket() {
    LOGCATD(__FUNCTION__)
    LOGCATI("   case 4 : real decode AVPacket.")
    // case 1: seek AV file.
    // case 2: real decode the AV file.
    int result = av_read_frame(m_AVFormatContext, m_Packet);
    LOGCATI("  step 1 av_read_frame result : %d", result)
    while (result == 0) {
        if (m_Packet->stream_index == m_StreamIndex) {
            // case 2-1: EOF
            if (avcodec_send_packet(m_AVCodecContext, m_Packet) == AVERROR_EOF) {
                result = -1;
                goto __EXIT;
            }
            // case 2-2:
            int frameCount = 0;
            while (avcodec_receive_frame(m_AVCodecContext, m_Frame) == 0) {
                // 更新时间戳
                UpdateTimeStamp();
                // AV Sync
                AVSync();
                // 渲染
                LOGCATI("       m_MediaType=%d", m_MediaType)
                OnFrameAvailable(m_Frame);
                frameCount++;
            }

            if (frameCount > 0) {
                result = 0;
                goto __EXIT;
            }

        }
        // 复用AVPacket
        av_packet_unref(m_Packet);
        result = av_read_frame(m_AVFormatContext, m_Packet);
        LOGCATI("  step 2 av_read_frame result : %d", result)
    }
    __EXIT:
    av_packet_unref(m_Packet);
    return result;
}

void DecoderBase::UpdateTimeStamp() {
    LOGCATD(__FUNCTION__)
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (m_Frame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pkt_dts;
    } else if (m_Frame->pts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pts;
    } else {
        m_CurTimeStamp = 0;
    }

    m_CurTimeStamp = (int64_t) (
            (m_CurTimeStamp * av_q2d(m_AVFormatContext->streams[m_StreamIndex]->time_base)) * 1000
    );

    if (m_SeekPosition > 0 && m_SeekSuccess) {
        m_StartTimeStamp = GetSysCurrentTime() - m_CurTimeStamp;
        m_SeekPosition = 0;
        m_SeekSuccess = false;
    }

}

long DecoderBase::AVSync() {
    LOGCATD(__FUNCTION__)
    long curSysTime = GetSysCurrentTime();
    long elapsedTime = curSysTime - m_StartTimeStamp;
    if (m_MsgContext && m_MsgCallback && m_MediaType == AVMEDIA_TYPE_AUDIO) {
        m_MsgCallback(m_MsgContext, MSG_DECODING_TIME, m_CurTimeStamp * 1.0f / 1000);
    }
    long delay = 0;
    if (m_CurTimeStamp > elapsedTime) {
        // sleep time
        auto sleepTime = static_cast<unsigned int >(m_CurTimeStamp - elapsedTime);
        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD : sleepTime;
        av_usleep(sleepTime * 1000);
    }
    delay = elapsedTime - m_CurTimeStamp;

    return delay;
}


void DecoderBase::DoAVDecoding(DecoderBase *decoder) {
    LOGCATD(__FUNCTION__)
    do {
        // step 1:
        if (decoder->InitFFDecoder() != 0) {
            LOGCATE("   InitFFDecoder ERROR")
            break;
        }
        LOGCATI("   InitFFDecoder OK")
        // step 2:
        decoder->OnDecoderReady();
        // step 3:
        decoder->DecodingLoop();
    } while (false);
    decoder->UnInitDecoder();
    decoder->OnDecoderDone();
    // Guangyazhou1234
}


