//
// Created by dev on 2024/4/19.
//

#ifndef STUDYFFMPEGBYANDROID2042_FFMEDIAPLAYER_H
#define STUDYFFMPEGBYANDROID2042_FFMEDIAPLAYER_H
#include "MediaPlayer.h"

/**
 * 软解播放器
 */
class FFMediaPlayer : public MediaPlayer{

public:
    FFMediaPlayer(){};
    virtual ~FFMediaPlayer(){};

    virtual void Init(JNIEnv *jniEnv, jobject obj, char *url, int renderType, jobject surface);



    /**
     * 播放
     */
    virtual void Play();

    virtual void Pause();
    virtual void Stop();
    virtual void SeekToPosition(float position);
    virtual long GetMediaParams(int paramType);

    virtual void UnInit();

private:
    virtual JNIEnv *GetJNIEnv(bool *isAttach);
    virtual jobject GetJavaObj();
    virtual JavaVM *GetJavaVM();

    static void PostMessage(void *context, int msgType, float msgCode);

    VideoDecoder *m_VideoDecoder = nullptr;
    AudioDecoder *m_AudioDecoder = nullptr;

    VideoRender *m_VideoRender = nullptr;
    // todo 音频渲染

};


#endif //STUDYFFMPEGBYANDROID2042_FFMEDIAPLAYER_H
