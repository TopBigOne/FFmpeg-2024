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
    int result = -1;

    do {
        // step 1:
        m_AVformatContext = avformat_alloc_context();
        // step 2: open the file.
        result = avformat_open_input(&m_AVformatContext, m_Url, nullptr, nullptr);
        if (result != 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
            av_strerror(result, errbuf, AV_ERROR_MAX_STRING_SIZE);
            LOGCATE("   avformat_open_input fail : %s", errbuf);
            break;
        }

        // step 3: 获取音视频流信息
        result = avformat_find_stream_info(m_AVformatContext, nullptr);
        if (result != 0) {
            LOGCATE("   avformat_find_stream_info fail.");
            break;
        }
        // step 4: 获取音视频流index
        for (int i = 0; i < m_AVformatContext->nb_streams; ++i) {
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
        AVDictionary *avDictionary = nullptr;
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
        result = avcodec_open2(m_AVCodecContext, m_AVCodec, &avDictionary);

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

    if (result != 0 && m_MsgContext && m_MessageCallback) {
        m_MessageCallback(m_MsgContext, static_cast<int>(DecoderMsg::MSG_DECODER_INIT_ERROR), 0);
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

}

void DecoderBase::StartDecodingThread() {
    LOGCATD(__FUNCTION__)
    m_Thread = new thread(DoAVDecoding, this);
}

void DecoderBase::DecodingLoop() {

}

void DecoderBase::UpdateTimeStamp() {

}

long DecoderBase::AVSync() {
    return 0;
}

int DecoderBase::DecodeOnePacket() {
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
        LOGCATI("   OnDecoderReady OK")

        // step 3:
        decoder->DecodingLoop();
        LOGCATI("   DecodingLoop OK")
    } while (false);
    decoder->UnInitDecoder();
    decoder->OnDecoderDone();
}
