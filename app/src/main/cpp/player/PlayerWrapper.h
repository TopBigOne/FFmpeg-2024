//
// Created by dev on 2024/4/10.
//

#ifndef STUDYFFMPEGBYANDROID2042_PLAYERWRAPPER_H
#define STUDYFFMPEGBYANDROID2042_PLAYERWRAPPER_H


#include <Jni.h>
#include "MediaPlayer.h"

#include "FFMediaPlayer.h"

// 软解
static const int FFMEDIA_PLAYER = 0;
// 编解码器（Hardware Codec）
static const int HWCODEC_PLAYER = 1;

class PlayerWrapper {
public:
    PlayerWrapper();

    virtual ~PlayerWrapper();

    void
    Init(JNIEnv *jniEnv, jobject obj, char *url, int playerType, int renderType, jobject surface);

    void UnInit();

    void Play();

    void Pause();

    void Stop();

    void SeekToPosition(float position);

    long GetMediaParams(int paramType);

    void SetMediaParams(int paramType, jobject obj);

private:
    MediaPlayer *m_MediaPlayer = nullptr;


};


#endif //STUDYFFMPEGBYANDROID2042_PLAYERWRAPPER_H
