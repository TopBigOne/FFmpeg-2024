//
// Created by dev on 2024/4/15.
//

#ifndef STUDYFFMPEGBYANDROID2042_DECODERBASE_H
#define STUDYFFMPEGBYANDROID2042_DECODERBASE_H


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
}

#include <thread>
#include "Decoder.h"

#include "LogUtil.h"


#define MAX_PATH   2048
#define DELAY_THRESHOLD 100 //100ms

using namespace std;
enum DecoderState {
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP

};
enum DecoderMsg {
    MSG_DECODER_INIT_ERROR,
    MSG_DECODER_READY,
    MSG_DECODER_DONE,
    MSG_REQUEST_RENDER,
    MSG_DECODING_TIME

};

class DecoderBase : public Decoder {


public:
    DecoderBase() {};

    virtual ~DecoderBase() {};

    //开始播放
    virtual void Start();

    //暂停播放
    virtual void Pause();

    //停止
    virtual void Stop();

    /**
     * ms--> s
     * @return
     */
    virtual float GetDuration() {
        return 0;

    }

    virtual void SeekToPosition(float position);

    virtual float GetCurrentPosition();

    virtual void ClearCache();

    /**
     * 事件回调
     * @param context
     * @param callback
     */
    virtual void SetMessageCallback(void *context, MessageCallback callback);

    /**
     * 设置音频和视频同步回调
     * @param context
     * @param callback
     */
    virtual void SetAVSyncCallback(void *context, AVSyncCallback callback) {
        // todo
    }

protected:
    void *m_MsgContext = nullptr;
    MessageCallback m_MessageCallback = nullptr;

protected:
    virtual int Init(const char *url, AVMediaType mediaType);

    virtual void UnInit();

    virtual void OnDecoderReady() = 0;

    virtual void OnDecoderDone() = 0;

    AVCodecContext *GetCodecContext() {
        return m_AVCodecContext;
    }

    /**
     *
     * @param avFrame
     */
    virtual void OnFrameAvailable(AVFrame *avFrame) = 0;


private:
    int InitFFDecoder();

    void UnInitDecoder();

    void StartDecodingThread();

    /**
     * 循环解码
     */
    void DecodingLoop();

    /**
     * 更新时间戳
     */
    void UpdateTimeStamp();

    /**
     * 音视频同步
     * @return
     */
    long AVSync();

    /**
     * 解码一个 AVPacket
     * @return
     */
    int DecodeOnePacket();


    /*
     * 解码线程的函数
     */
    static void DoAVDecoding(DecoderBase *decoder);


private:
    AVFormatContext *m_AVformatContext = nullptr;
    AVCodecContext *m_AVCodecContext = nullptr;
    AVCodec *m_AVCodec = nullptr;
    AVPacket *m_AVPacket = nullptr;
    AVFrame *m_AVFrame = nullptr;
    AVMediaType m_MediaType = AVMEDIA_TYPE_UNKNOWN;

    // 文件地址
    char m_Url[MAX_PATH] = {0};

    // 当前播放时间
    long m_CurTimeStamp = 0;
    // 播放起始时间
    long m_StartTimeStamp = -1;
    // 总时长 ms
    long m_Duration = 0;
    // 数据流索引： 视频，音频，字幕
    int m_StreamIndex = -1;
    mutex m_Mutex;

    condition_variable m_Cond;
    thread *m_Thread = nullptr;

    int m_DecoderState = DecoderState::STATE_UNKNOWN;

    // Seek position
    volatile float m_SeekPosition = 0;
    // Seek AV result
    volatile bool m_SeekSuccess = false;
    AVSyncCallback m_AVSyncCallback = nullptr;


};


#endif //STUDYFFMPEGBYANDROID2042_DECODERBASE_H
