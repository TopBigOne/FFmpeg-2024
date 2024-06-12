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
#include "AudioRender.h"

static const int AUDIO_DST_SAMPLE_RATE = 44100;
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
static const int AUDIO_DST_BIT_RATE = 64000;
static const int ACC_NB_SAMPLES = 64000;


class AudioDecoder : public DecoderBase {
public:
    AudioDecoder(char *url) {
        Init(url, AVMEDIA_TYPE_AUDIO);
    }

    virtual ~AudioDecoder() {
        UnInit();
    }

    void SetAudioRender(AudioRender *audioRender) {
        m_AudioRender = audioRender;
    }

private:
    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *avFrame);

    virtual void ClearCache();

    /**
     * 每个采样点的取值范围是从 -32768 到 32767，其中 0 表示无声音。采样点的值表示音频波形在该时间点的振幅或音量。
     */
    const AVSampleFormat DST_SAMPLE_FORMAT = AV_SAMPLE_FMT_S16;
    AudioRender *m_AudioRender = nullptr;
    SwrContext *m_SwrContext = nullptr;
    uint8_t *m_AudioOutBuffer = nullptr;

    int m_nbSamples = 0;
    int m_DstFrameDataSize = 0;


};


#endif //STUDYFFMPEGBYANDROID2042_AUDIODECODER_H
