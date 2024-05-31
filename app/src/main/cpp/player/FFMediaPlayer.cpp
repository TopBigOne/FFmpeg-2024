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

    m_VideoDecoder->SetMessageCallback(this, PostMessage);
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
    LOGCATD(__FUNCTION__)
    LOGCATD("   paramType=%d", paramType)
    long value = 0;
    switch (paramType) {
        case MEDIA_PARAM_VIDEO_WIDTH:
            value = m_VideoDecoder != nullptr ? m_VideoDecoder->GetVideoWidth() : 0;
            break;
        case MEDIA_PARAM_VIDEO_HEIGHT:
            value = m_VideoDecoder != nullptr ? m_VideoDecoder->GetVideoHeight() : 0;
            break;
        case MEDIA_PARAM_VIDEO_DURATION:
            value = m_VideoDecoder != nullptr ? m_VideoDecoder->GetDuration() : 0;
            break;
        default:
            value = 0;
    }
    return value;
}

void FFMediaPlayer::UnInit() {

}

JNIEnv *FFMediaPlayer::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == m_JavaVM) {
        LOGCATE("FFMediaPlayer::GetJNIEnv m_JavaVM == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = m_JavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = m_JavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGCATE("FFMediaPlayer::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

jobject FFMediaPlayer::GetJavaObj() {
    return m_JavaObj;
}

JavaVM *FFMediaPlayer::GetJavaVM() {
    return m_JavaVM;
}

void FFMediaPlayer::PostMessage(void *context, int msgType, float msgCode) {
    if (context == nullptr) {
        return;
    }
    auto *player = static_cast<FFMediaPlayer *>(context);
    bool isAttach = false;
    JNIEnv *env = player->GetJNIEnv(&isAttach);
    if (env == nullptr) {
        return;
    }
    jobject javaObj = player->GetJavaObj();
    jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj),
                                     JAVA_PLAYER_EVENT_CALLBACK_API_NAME, "(IF)V");
    env->CallVoidMethod(javaObj, mid, msgType, msgCode);
    if (isAttach) {
        player->GetJavaVM()->DetachCurrentThread();
    }


}
