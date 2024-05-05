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

int DecoderBase::Init(const char *url, AVMediaType mediaType) {
    LOGCATD("DecoderBase::Init url : %s, mediaType : %d", url, mediaType);
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
        m_AVformatContext = avformat_alloc_context();
        // step 2: open the file.
        result = avformat_open_input(&m_AVformatContext, m_Url, nullptr, nullptr);
        // result = avformat_open_input(&m_AVformatContext,"/storage/emulated/0/byteflow/one_piece.mp4", nullptr,nullptr);
        if (result != 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
            av_strerror(result, errbuf, AV_ERROR_MAX_STRING_SIZE);
            LOGCATE("   avformat_open_input fail : %s", errbuf)
            break;
        }

        // step 3: 获取音视频流信息
        result = avformat_find_stream_info(m_AVformatContext, nullptr);
        if (result != 0) {
            LOGCATE("   avformat_find_stream_info fail.");
            break;
        }
        // step 4: 获取音视频流index
        for (int i = 0; i < m_AVformatContext->nb_streams; i++) {
            if (m_AVformatContext->streams[i]->codecpar->codec_type == m_MediaType) {
                m_StreamIndex = i;
                break;
            }
        }
        if (m_StreamIndex == -1) {
            LOGCATE("   Fail to find stream index.");
            break;
        }
        // step 5： 获取解码器参数
        AVCodecParameters *codecParameters = m_AVformatContext->streams[m_StreamIndex]->codecpar;

        // step 6: 获取解码器
        m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (m_AVCodec == nullptr) {
            LOGCATE("   avcodec_find_decoder fail.")
            break;
        }

        // step 7: create the AVCodecContext
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);

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
            LOGCATE("    avcodec_open2 fail. result=%d", result);
            break;
        }

        result = 0;

        // 持续时间(us to ms)
        m_Duration = m_AVformatContext->duration / AV_TIME_BASE * 1000;
        m_AVPacket = av_packet_alloc();
        m_AVFrame = av_frame_alloc();


    } while (false);

    if (result != 0 && m_MsgContext && m_MsgCallback) {
        m_MsgCallback(m_MsgContext, static_cast<int>(DecoderMsg::MSG_DECODER_INIT_ERROR), 0);
    }

    return result;
}

void DecoderBase::SeekToPosition(float position) {
    LOGCATD(__FUNCTION__)


}

float DecoderBase::GetCurrentPosition() {
    LOGCATD(__FUNCTION__)
    return 0;
}

void DecoderBase::ClearCache() {
    LOGCATD(__FUNCTION__)

}

void DecoderBase::SetMessageCallback(void *context, MessageCallback callback) {
    LOGCATD(__FUNCTION__)

}

void DecoderBase::Pause() {
    LOGCATD(__FUNCTION__)

}

void DecoderBase::Stop() {
    LOGCATD(__FUNCTION__)

}

void DecoderBase::UnInitDecoder() {
    LOGCATD(__FUNCTION__)

}

void DecoderBase::StartDecodingThread() {
    LOGCATD(__FUNCTION__)
    m_Thread = new thread(DoAVDecoding, this);
}

void DecoderBase::DecodingLoop() {
    LOGCATD(__FUNCTION__)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_DecoderState = STATE_DECODING;
        lock.unlock();
    }
    for (;;) {
        // case 1:解码暂停10毫秒，便于渲染线程消费avframe数据
        while (m_DecoderState == STATE_PAUSE) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(10));
            m_StartTimeStamp = GetSysCurrentTime() - m_CurTimeStamp;
            LOGCATI("   case 1 : pause.")
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
    int result = av_read_frame(m_AVformatContext, m_AVPacket);

    LOGCATI("   av_read_frame result : %d", result)
    while (result == 0) {
        if (m_AVPacket->stream_index == m_StreamIndex) {
            LOGCATI("   流index匹配  🌹🌹🌹🌹🌹🌹")

            // case 2-1: EOF
            if (avcodec_send_packet(m_AVCodecContext, m_AVPacket) == AVERROR_EOF) {
                result = -1;
                goto __EXIT;
            }
            // case 2-2:
            int frameCount = 0;

            int avcodec_receive_frame_result;
            while ((avcodec_receive_frame_result = avcodec_receive_frame(m_AVCodecContext,m_AVFrame)) == 0) {
                LOGCATI("       case 2-2")
                // 更新时间戳
                UpdateTimeStamp();
                // AV Sync
                AVSync();
                // 渲染
                LOGCATV("   DecodeOnePacket 000 m_MediaType=%d", m_MediaType)
                OnFrameAvailable(m_AVFrame);
                LOGCATV("   DecodeOnePacket 000 m_MediaType=%d", m_MediaType)

                if (avcodec_receive_frame_result < 0) {
                    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
                    av_strerror(avcodec_receive_frame_result, errbuf, AV_ERROR_MAX_STRING_SIZE);
                    LOGCATE("       avcodec_receive_frame result : %d",
                            avcodec_receive_frame_result)
                    LOGCATE("       avcodec_receive_frame fail   : %s", errbuf)

                }
                frameCount++;
            }





            // todo 判断一个AVPacket 是否解码完成，我认为不合理；应该使用 result== AVERROR_EOF
            if (frameCount > 0) {
                result = 0;
                LOGCATI("       case 2-3 ： 解码一个AVPacket 结束")
                goto __EXIT;
            }

        } else {
            LOGCATI("   流index不匹配 ⚠️⚠️⚠️⚠️⚠️⚠️")
        }
        // 复用AVPacket
        av_packet_unref(m_AVPacket);
        result = av_read_frame(m_AVformatContext, m_AVPacket);
        LOGCATI("   av_read_frame result : %d", result)

    }
    __EXIT:
    av_packet_unref(m_AVPacket);
    return result;
}

void DecoderBase::UpdateTimeStamp() {
    LOGCATD(__FUNCTION__)

}

long DecoderBase::AVSync() {
    return 0;
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

        if (decoder->m_MediaType == AVMEDIA_TYPE_VIDEO) {
            LOGCATI("   OnDecoderReady width  : %d", decoder->m_AVCodecContext->width)
            LOGCATI("   OnDecoderReady height : %d", decoder->m_AVCodecContext->height)
        }


        LOGCATI("   OnDecoderReady OK")

        // step 3:
        decoder->DecodingLoop();
        LOGCATI("   DecodingLoop OK")
    } while (false);
    decoder->UnInitDecoder();
    decoder->OnDecoderDone();
}
