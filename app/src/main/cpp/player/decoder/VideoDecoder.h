//
// Created by dev on 2024/4/15.
//

#ifndef STUDYFFMPEGBYANDROID2042_VIDEODECODER_H
#define STUDYFFMPEGBYANDROID2042_VIDEODECODER_H


extern "C" {

#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavcodec/jni.h>

}


#include "DecoderBase.h"
#include "VideoRender.h"

class VideoDecoder : public DecoderBase {
public:
    VideoDecoder(char *url) {
        Init(url, AVMEDIA_TYPE_VIDEO);

    }

    virtual ~VideoDecoder() {
        UnInit();
    }

    int GetVideoHeight() {
        return m_VideoHeight;
    }

    int GetVideoWidth() {
        return m_VideoHeight;
    }
    /**
     * 简单的函数，直接在头文件中实现
     * @param videoRender
     */
    void SetVideoRender(VideoRender *videoRender)
    {
        m_VideoRender = videoRender;
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
    /**
     * 源图像数据的指针
     */
    uint8_t *m_FrameBuffer = nullptr;


    VideoRender *m_VideoRender = nullptr;
    SwsContext *m_SwsContext = nullptr;
    // todo SingleVideoRecorder 视频渲染


};


#endif //STUDYFFMPEGBYANDROID2042_VIDEODECODER_H
