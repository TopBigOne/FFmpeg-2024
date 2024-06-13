//
// Created by dev on 2024/6/2.
//

#include "OpenSLRender.h"
#include "LogUtil.h"


void OpenSLRender::Init() {
    LOGCATD(__FUNCTION__)
    int result = -1;
    do {
        // step 1:
        result = CreateEngine();
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateEngine fail. result=%d", result);
            break;
        }
        // step 2:
        result = CreateOutputMixer();
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateOutputMixer fail. result=%d", result);
            break;
        }
        // step 3:
        result = CreateAudioPlayer();
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateAudioPlayer fail. result=%d", result);
        }
        // 创建线程
        m_thread = new std::thread(CreateSLWaitingThread, this);


    } while (false);

    if (result != SL_RESULT_SUCCESS) {
        LOGCATE("   Init fail. result=%d", result);
        UnInit();
    }
}

/**
 *  create openel es engine
 * @return
 */
int OpenSLRender::CreateEngine() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        // step 1: slCreateEngine函数用于创建引擎对象，并将其存储在m_EngineObj指针中。
        result = slCreateEngine(&m_EngineObj, 0, nullptr, 0, nullptr, nullptr);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateEngine slCreateEngine fail. result=%d", result);
            break;
        }
        // step 2: 引擎对象调用Realize函数进行实例化
        result = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateEngine Realize fail. result=%d", result);
            break;
        }
        // step 3:使用引擎对象调用GetInterface函数获取引擎接口。该函数获取引擎对象的指定接口，并将其存储在m_EngineEngine变量中。
        result = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_EngineEngine);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateEngine GetInterface fail. result=%d", result);
            break;
        }

    } while (false);

    return result;

}

/**
 * step 2:
 * @return
 */
int OpenSLRender::CreateOutputMixer() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
        result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObj, 1, mids, mreq);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateOutputMix fail. result=%d", result);
            break;
        }

        result = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("   CreateOutputMix fail. result=%d", result);
            break;
        }


    } while (false);
    return result;

}

int OpenSLRender::CreateAudioPlayer() {
    LOGCATD(__FUNCTION__)
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) 2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT |
            SL_SPEAKER_FRONT_RIGHT, // channel mask
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObj};
    // 表示音频或视频数据的目标位置或目标格式
    SLDataSink slDataSink = {&outputMix, nullptr};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    SLresult result;
    do {
        result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AudioPlayerObj,
                                                      &slDataSource, &slDataSink, 3, ids, req);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer CreateAudioPlayer fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->Realize(m_AudioPlayerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer Realize fail. result=%d", result);
            break;
        }

        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_PLAY,
                                                   &m_AudioPlayerPlay);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result)
            break;
        }
        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_BUFFERQUEUE,
                                                   &m_BufferQueue);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*m_BufferQueue)->RegisterCallback(m_BufferQueue, AudioPlayerCallback, this);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer RegisterCallback fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_VOLUME,
                                                   &m_AudioPlayerVolume);
        if (result != SL_RESULT_SUCCESS) {
            LOGCATE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }


    } while (false);

    return (int) result;

}


void OpenSLRender::CreateSLWaitingThread(OpenSLRender *openSlRender) {
    openSlRender->StartRender();
}


void
OpenSLRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    OpenSLRender *openSLRender = static_cast<OpenSLRender *>(context);
    openSLRender->HandleAudioFrameQueue();
}


void OpenSLRender::HandleAudioFrameQueue() {
    LOGCATD(__FUNCTION__)
    if (m_AudioPlayerPlay == nullptr) {
        return;
    }
    // 等待队列满
    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !m_Exit) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(10));
    }

    std::unique_lock<std::mutex> lock(m_Mutex);
    AudioFrame *audioFrame = m_AudioFrameQueue.front();
    if (audioFrame != nullptr && m_AudioPlayerPlay) {
        SLresult lresult = (*m_BufferQueue)->Enqueue(m_BufferQueue, audioFrame->data,
                                                     (SLuint32) audioFrame->dataSize);
        if (lresult == SL_RESULT_SUCCESS) {
            m_AudioFrameQueue.pop();
            // 销毁AudioFrame
            LOGCATI("    销毁AudioFrame audioFrame ptr is : %p", audioFrame->data)
            delete audioFrame;
        }
    }
    lock.unlock();

}


void OpenSLRender::StartRender() {
    LOGCATD(__FUNCTION__)
    while (GetAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !m_Exit) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(15));
        lock.unlock();
    }
    (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(m_BufferQueue, this);
}

int OpenSLRender::GetAudioFrameQueueSize() {
    //  LOGCATD(__FUNCTION__)
    std::unique_lock<std::mutex> lock(m_Mutex);
    return static_cast<int>(m_AudioFrameQueue.size());
}


void OpenSLRender::RenderAudioFrame(uint8_t *pData, int dataSize) {
    LOGCATD(__FUNCTION__)
    LOGCATI("   audio data ptr : %p", pData)
    if (m_AudioPlayerPlay == nullptr) {
        LOGCATE("   m_AudioPlayerPlay is NULL.")
        return;
    }
    if (pData == nullptr || dataSize <= 0) {
        LOGCATE("   pData== nullptr || dataSize<=0")
        return;
    }
    while (GetAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE && !m_Exit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    // RTTI 直接上锁
    std::unique_lock<std::mutex> lock(m_Mutex);
    AudioFrame *audioFrame = new AudioFrame(pData, dataSize);
    m_AudioFrameQueue.push(audioFrame);
    m_Cond.notify_all();
    lock.unlock();


}


void OpenSLRender::ClearAudioCache() {
    LOGCATD(__FUNCTION__)
    std::unique_lock<std::mutex> lock(m_Mutex);
    for (int i = 0; i < m_AudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = m_AudioFrameQueue.front();
        m_AudioFrameQueue.pop();
        delete audioFrame;
    }
}

void OpenSLRender::UnInit() {
    if (m_AudioPlayerPlay) {
        (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_STOPPED);
        m_AudioPlayerPlay = nullptr;
    }

    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Exit = true;
    m_Cond.notify_all();
    lock.unlock();

    if (m_AudioPlayerObj) {
        (*m_AudioPlayerObj)->Destroy(m_AudioPlayerObj);
        m_AudioPlayerObj = nullptr;
        m_BufferQueue = nullptr;
    }

    if (m_OutputMixObj) {
        (*m_OutputMixObj)->Destroy(m_OutputMixObj);
        m_OutputMixObj = nullptr;
    }

    if (m_EngineObj) {
        (*m_EngineObj)->Destroy(m_EngineObj);
        m_EngineObj = nullptr;
        m_EngineEngine = nullptr;
    }

    lock.lock();
    for (int i = 0; i < m_AudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = m_AudioFrameQueue.front();
        m_AudioFrameQueue.pop();
        delete audioFrame;
    }
    lock.unlock();

    if (m_thread != nullptr) {
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
    }

    // AudioGLRender::ReleaseInstance();


}













