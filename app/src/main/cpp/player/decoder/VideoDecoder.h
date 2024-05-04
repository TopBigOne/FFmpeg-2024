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







};


#endif //STUDYFFMPEGBYANDROID2042_VIDEODECODER_H
