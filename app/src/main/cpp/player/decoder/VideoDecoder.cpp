//
// Created by dev on 2024/4/15.
//

#include "VideoDecoder.h"

void VideoDecoder::OnDecoderReady() {
    LOGCATD(__FUNCTION__)
    m_VideoWidth = GetCodecContext()->width;
    m_VideoHeight = GetCodecContext()->height;
    // case 1:
    if (m_MsgContext && m_MsgCallback) {
        m_MsgCallback(m_MsgContext, MSG_DECODER_READY, 0);
    } else{
        LOGCATE("  m_MsgContext or m_MsgCallback is NULL.");
    }
    // case 2:
    if (m_VideoRender == nullptr) {
        LOGCATE("   m_VideoRender == NULL");
        return;
    }
    // case 3:
    int dstSize[2] = {0};
    m_VideoRender->Init(m_VideoWidth, m_VideoHeight, dstSize);
    m_RenderWidth = dstSize[0];
    m_RenderHeight = dstSize[1];
    // 使用原生渲染
    if (m_VideoRender->GetRenderType() == VIDEO_RENDER_ANWINDOW) {
        // todo SingleVideoRecoder : 有待研究，这是干什么的；
        int fps = 25;
        long videoBitRate = m_RenderWidth * m_RenderHeight * 0.2;
    }
    m_RGBAFrame = av_frame_alloc();
    int bufferSize = av_image_get_buffer_size(DST_PIXEL_FORMAT, m_RenderWidth, m_RenderHeight, 1);
    m_FrameBuffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof(uint8_t)));
    av_image_fill_arrays(m_RGBAFrame->data, m_RGBAFrame->linesize, m_FrameBuffer, DST_PIXEL_FORMAT,
                         m_RenderWidth,
                         m_RenderHeight, 1);
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight, GetCodecContext()->pix_fmt,
                                  m_RenderWidth,
                                  m_RenderHeight, DST_PIXEL_FORMAT, SWS_FAST_BILINEAR, nullptr,
                                  nullptr, nullptr);

}

void VideoDecoder::OnDecoderDone() {
    LOGCATD(__FUNCTION__)

}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    LOGCATD(__FUNCTION__)
    if (m_VideoRender == nullptr || frame == nullptr) {
        LOGCATE("   m_VideoRender  or frame is NULL")
        return;
    }
    LOGCATI("   得到一个可用帧-得到一个可用帧-得到一个可用帧-得到一个可用帧-得到一个可用帧")
    NativeImage image;
    // 做视频像素格式和分辨率的转换
    if (m_VideoRender->GetRenderType() == VIDEO_RENDER_ANWINDOW) {
        sws_scale(m_SwsContext, frame->data, frame->linesize, 0, m_VideoHeight, m_RGBAFrame->data,
                  m_RGBAFrame->linesize);
        image.format = IMAGE_FORMAT_RGBA;
        image.width = m_RenderWidth;
        image.height = m_RenderHeight;
        image.ppPlane[0] = m_RGBAFrame->data[0];
        image.pLineSize[0] = image.width * 4;
    }
    m_VideoRender->RenderVideoFrame(&image);
    if (m_MsgContext && m_MsgCallback) {
        m_MsgCallback(m_MsgContext, MSG_REQUEST_RENDER, 0);
    }


}
