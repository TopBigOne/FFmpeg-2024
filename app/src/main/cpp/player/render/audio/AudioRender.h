//
// Created by dev on 2024/6/2.
//

#ifndef STUDYFFMPEGBYANDROID2042_AUDIORENDER_H
#define STUDYFFMPEGBYANDROID2042_AUDIORENDER_H

#include <stdint.h>
#include <stdlib.h>


class AudioFrame {
public:
    AudioFrame(uint8_t *data, int dataSize, bool hardCopy = false) {
        this->data = data;
        this->dataSize = dataSize;
        this->hardCopy = hardCopy;
    }

    ~AudioFrame() {
        if (hardCopy && this->hardCopy) {
            this->data = nullptr;
        }
        free(this->data);
    }

public:
    uint8_t *data = nullptr;
    int dataSize = 0;
    bool hardCopy = true;

};

class AudioRender {
public:
    virtual ~AudioRender() {};

    virtual void Init() = 0;

    virtual void ClearAudioCache() = 0;

    virtual void RenderAudioFrame(uint8_t *pData, int dataSize) = 0;

    virtual void UnInit() = 0;

};


#endif //STUDYFFMPEGBYANDROID2042_AUDIORENDER_H
