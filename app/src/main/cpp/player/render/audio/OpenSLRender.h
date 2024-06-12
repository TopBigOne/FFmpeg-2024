//
// Created by dev on 2024/6/2.
//

#ifndef STUDYFFMPEGBYANDROID2042_OPENSLRENDER_H
#define STUDYFFMPEGBYANDROID2042_OPENSLRENDER_H

#include <thread>
#include "AudioRender.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>

#define MAX_QUEUE_BUFFER_SIZE 3

class OpenSLRender : public AudioRender {

public:
    OpenSLRender() {};

    virtual  ~OpenSLRender() {};

    virtual void Init();

    virtual void ClearAudioCache();

    virtual void RenderAudioFrame(uint8_t *pData, int dataSize);

    virtual void UnInit();

private:
    /**
     * step :
     * @return
     */
    int CreateEngine();

    int CreateOutputMixer();

    int CreateAudioPlayer();

    int GetAudioFrameQueueSize();

    void StartRender();

    void HandleAudioFrameQueue();

    static void CreateSLWaitingThread(OpenSLRender *openSlRender);

    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

    SLObjectItf m_EngineObj = nullptr;
    SLEngineItf m_EngineEngine = nullptr;

    SLObjectItf m_OutputMixObj = nullptr;
    SLObjectItf m_AudioPlayerObj = nullptr;
    SLPlayItf m_AudioPlayerPlay = nullptr;
    SLVolumeItf m_AudioPlayerVolume = nullptr;
    SLAndroidSimpleBufferQueueItf m_BufferQueue;
    std::queue<AudioFrame *> m_AudioFrameQueue;

    std::thread *m_thread = nullptr;
    std::mutex m_Mutex;
    std::condition_variable m_Cond;

    volatile bool m_Exit = false;


};


#endif //STUDYFFMPEGBYANDROID2042_OPENSLRENDER_H
