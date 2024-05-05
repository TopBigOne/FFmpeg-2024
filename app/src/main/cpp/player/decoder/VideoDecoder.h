//
// Created by dev on 2024/4/15.
//

#ifndef STUDYFFMPEGBYANDROID2042_VIDEODECODER_H
#define STUDYFFMPEGBYANDROID2042_VIDEODECODER_H


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>

}


#include "DecoderBase.h"


class VideoDecoder : public DecoderBase {
public:
    VideoDecoder(char *url) {
        Init(url, AVMEDIA_TYPE_VIDEO);

    }

    virtual ~VideoDecoder() {
        UnInit();
    }


private:
    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *frame);

    /**
     * 像素类型
     */
    const AVPixelFormat DST_PIXEL_FORMAT = AV_PIX_FMT_RGBA;

    int m_VideoWidth = 0;
    int m_VideoHeight = 0;

    int m_RenderWidth = 0;
    int m_RenderHeight = 0;
    AVFrame *m_RGBAFrame = nullptr;
    uint8_t *m_FrameBuffer = nullptr;

    // todo 视频渲染





};


#endif //STUDYFFMPEGBYANDROID2042_VIDEODECODER_H
