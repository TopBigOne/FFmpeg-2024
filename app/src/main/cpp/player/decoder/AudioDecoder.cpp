//
// Created by dev on 2024/4/19.
//

#include "AudioDecoder.h"
#include "LogUtil.h"

void AudioDecoder::OnDecoderReady() {
    LOGCATD(__FUNCTION__)
    // case 1:
    if (m_AudioRender == nullptr) {
        LOGCATE("   m_AudioRender == null");
        return;
    }
    // case 2:
    m_SwrContext = swr_alloc();
    AVCodecContext *codeCtx = GetCodecContext();
    av_opt_set_int(m_SwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

    av_opt_set_int(m_SwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLE_FORMAT, 0);

    swr_init(m_SwrContext);

    LOGCATI("   audio metadata sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
            codeCtx->sample_rate, codeCtx->channels, codeCtx->sample_fmt, codeCtx->frame_size,
            codeCtx->channel_layout);


    // 重采样
    m_nbSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE, codeCtx->sample_rate,AV_ROUND_UP);
    m_DstFrameDataSize = av_samples_get_buffer_size(nullptr, AUDIO_DST_CHANNEL_COUNTS, m_nbSamples,
                                                    DST_SAMPLE_FORMAT, 1);
    m_AudioOutBuffer = (uint8_t *) malloc(m_DstFrameDataSize);
    m_AudioRender->Init();
}


void AudioDecoder::OnFrameAvailable(AVFrame *frame) {
    LOGCATD(__FUNCTION__)
    LOGCATI("   a audio frame is Available")
    if (m_AudioRender == nullptr) {
        LOGCATE("   m_AudioRender == null")
        return;
    }
    int result = swr_convert(m_SwrContext, &m_AudioOutBuffer, m_DstFrameDataSize / 2,(const uint8_t **) frame->data, frame->nb_samples);

    if (result > 0) {
        LOGCATI("   start render audio frame")
        m_AudioRender->RenderAudioFrame(m_AudioOutBuffer, m_DstFrameDataSize);
    }

}


void AudioDecoder::OnDecoderDone() {
    LOGCATD(__FUNCTION__)
    if (m_AudioRender) {
        m_AudioRender->UnInit();
    }

    if (m_AudioOutBuffer) {
        free(m_AudioOutBuffer);
        m_AudioOutBuffer = nullptr;
    }

    if (m_SwrContext) {
        swr_free(&m_SwrContext);
        m_SwrContext = nullptr;
    }
}

void AudioDecoder::ClearCache() {
    DecoderBase::ClearCache();
    LOGCATD(__FUNCTION__)
    if (m_AudioRender) {
        m_AudioRender->ClearAudioCache();
    }
}
