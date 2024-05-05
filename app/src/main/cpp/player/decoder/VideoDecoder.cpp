//
// Created by dev on 2024/4/15.
//

#include "VideoDecoder.h"

void VideoDecoder::OnDecoderReady() {
    LOGCATD(__FUNCTION__)
    m_VideoWidth = GetCodecContext()->width;
    m_VideoHeight = GetCodecContext()->height;
    if (m_MsgContext &&m_MsgCallback){
        m_MsgCallback(m_MsgContext,MSG_DECODER_READY,0);
    }



}

void VideoDecoder::OnDecoderDone() {
    LOGCATD(__FUNCTION__)

}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    LOGCATD(__FUNCTION__)
    LOGCATI("   得到一个可用帧")

}
