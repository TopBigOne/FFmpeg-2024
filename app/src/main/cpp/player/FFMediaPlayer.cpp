//
// Created by dev on 2024/4/19.
//

#include "FFMediaPlayer.h"

#include "NativeRender.h"

void
FFMediaPlayer::Init(JNIEnv *jniEnv, jobject obj, char *url, int videoRenderType, jobject surface) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);
    // 视频解码器
    m_VideoDecoder = new VideoDecoder(url);
    // 音频解码器

    if (videoRenderType == VIDEO_RENDER_ANWINDOW) {
        m_VideoRender = new NativeRender(jniEnv, surface);
        m_VideoDecoder->SetVideoRender(m_VideoRender);
    }


}

void FFMediaPlayer::Play() {
    LOGCATD("   FFMediaPlayer::Play")
    if (m_VideoDecoder) {
        m_VideoDecoder->Start();
    }


}

void FFMediaPlayer::Pause() {

}

void FFMediaPlayer::Stop() {

}

void FFMediaPlayer::SeekToPosition(float position) {

}

long FFMediaPlayer::GetMediaParams(int paramType) {
    return 0;
}

void FFMediaPlayer::UnInit() {

}

JNIEnv *FFMediaPlayer::GetJNIEnv(bool *isAttach) {
    return nullptr;
}

jobject FFMediaPlayer::GetJavaObj() {
    return nullptr;
}

JavaVM *FFMediaPlayer::GetJavaVM() {
    return nullptr;
}

void FFMediaPlayer::PostMessage(void *context, int msgType, float msgCode) {

}
