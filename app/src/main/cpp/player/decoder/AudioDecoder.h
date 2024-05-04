//
// Created by dev on 2024/4/19.
//

#ifndef STUDYFFMPEGBYANDROID2042_AUDIODECODER_H
#define STUDYFFMPEGBYANDROID2042_AUDIODECODER_H

extern "C" {
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
}


#include "DecoderBase.h"
class AudioDecoder : public DecoderBase{

};


#endif //STUDYFFMPEGBYANDROID2042_AUDIODECODER_H
