//
// Created by dev on 2024/5/29.
//

#ifndef STUDYFFMPEGBYANDROID2042_NATIVERENDER_H
#define STUDYFFMPEGBYANDROID2042_NATIVERENDER_H

#include "VideoRender.h"
#include <jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

class NativeRender : public VideoRender {
public:
    NativeRender( JNIEnv *env, jobject surface);

    virtual  ~NativeRender();

    virtual void Init(int videoWidth, int videoHeight, int *dstSize);

    virtual void RenderVideoFrame(NativeImage *pImage);

    virtual void UnInit();

private:
    ANativeWindow_Buffer m_NativeWindowBuffer;
    ANativeWindow *m_NativeWindow = nullptr;
    int m_DstWidth;
    int m_DstHeight;
};


#endif //STUDYFFMPEGBYANDROID2042_NATIVERENDER_H
