//
// Created by dev on 2024/4/15.
//

#ifndef STUDYFFMPEGBYANDROID2042_DECODER_H
#define STUDYFFMPEGBYANDROID2042_DECODER_H


typedef void (*MessageCallback)(void *, int, float);

typedef long (*AVSyncCallback)(void *);

/**
 *  我觉得叫:IDecoder 更合适
 */
class Decoder {
public:
    virtual void Start() = 0;

    virtual void Pause() = 0;

    virtual void Stop() = 0;

    virtual float GetDuration() = 0;

    virtual void SeekToPosition(float position) = 0;

    virtual float GetCurrentPosition() = 0;

    /**
     * 解码事件回调
     * @param context
     * @param callback
     */
    virtual void SetMessageCallback(void *context, MessageCallback callback) = 0;


};


#endif //STUDYFFMPEGBYANDROID2042_DECODER_H
