//
// Created by dev on 2024/4/10.
//

#include "PlayerWrapper.h"

PlayerWrapper::PlayerWrapper() {

}

PlayerWrapper::~PlayerWrapper() {

}

void PlayerWrapper::Init(JNIEnv *jniEnv, jobject obj, char *url, int playerType, int renderType,
                         jobject surface) {
    switch (playerType) {
        case FFMEDIA_PLAYER:
            m_MediaPlayer = new FFMediaPlayer;
            break;
        case HWCODEC_PLAYER:
            // todo 硬解码
            break;
        default:
            break;
    }
    if (m_MediaPlayer) {
        m_MediaPlayer->Init(jniEnv, obj, url, renderType, surface);
    }
}

void PlayerWrapper::UnInit() {
    if(m_MediaPlayer) {
        m_MediaPlayer->UnInit();
        delete m_MediaPlayer;
        m_MediaPlayer = nullptr;
    }

}

void PlayerWrapper::Play() {
    if (m_MediaPlayer){
        m_MediaPlayer->Play();
        return;
    }
    LOGCATE("   m_MediaPlayer is NULL.")
}

void PlayerWrapper::Pause() {
    if(m_MediaPlayer) {
        m_MediaPlayer->Pause();
    }

}

void PlayerWrapper::Stop() {
    if(m_MediaPlayer) {
        m_MediaPlayer->Stop();
    }

}

void PlayerWrapper::SeekToPosition(float position) {

}

long PlayerWrapper::GetMediaParams(int paramType) {
    if(m_MediaPlayer) {
        return m_MediaPlayer->GetMediaParams(paramType);
    }

    return 0;
}

void PlayerWrapper::SetMediaParams(int paramType, jobject obj) {
    if (m_MediaPlayer){
        m_MediaPlayer->SetMediaParams(paramType,obj);

    }

}
