//
// Created by dev on 2024/4/10.
//

#ifndef STUDYFFMPEGBYANDROID2042_MEDIAPLAYER_H
#define STUDYFFMPEGBYANDROID2042_MEDIAPLAYER_H




#include <jni.h>

#include "decoder/VideoDecoder.h"
#include "decoder/AudioDecoder.h"
//#include "render/audio/AudioRender.h"


#define JAVA_PLAYER_EVENT_CALLBACK_API_NAME "playerEventCallback"

#define MEDIA_PARAM_VIDEO_WIDTH         0x0001
#define MEDIA_PARAM_VIDEO_HEIGHT        0x0002
#define MEDIA_PARAM_VIDEO_DURATION      0x0003

#define MEDIA_PARAM_ASSET_MANAGER       0x0020


class MediaPlayer {
public:
    MediaPlayer() {};

    virtual ~MediaPlayer() {};

    virtual void Init(JNIEnv *jniEnv, jobject obj, char *url, int renderType, jobject surface) = 0;

    virtual void UnInit() = 0;

    virtual void Play() = 0;

    virtual void Pause() = 0;

    virtual void Stop() = 0;

    virtual void SeekToPosition(float position) = 0;

    virtual long GetMediaParams(int paramType) = 0;

    virtual void SetMediaParams(int paramType, jobject obj) {}

    virtual JNIEnv *GetJNIEnv(bool *isAttach) = 0;

    virtual jobject GetJavaObj() = 0;

    virtual JavaVM *GetJavaVM() = 0;

    JavaVM *m_JavaVM = nullptr;
    jobject m_JavaObj = nullptr;


};


#endif //STUDYFFMPEGBYANDROID2042_MEDIAPLAYER_H
