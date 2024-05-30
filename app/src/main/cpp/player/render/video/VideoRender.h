//
// Created by dev on 2024/4/15.
//

#ifndef STUDYFFMPEGBYANDROID2042_VIDEORENDER_H
#define STUDYFFMPEGBYANDROID2042_VIDEORENDER_H

#include "ImageDef.h"


#define VIDEO_RENDER_ANWINDOW           1

class VideoRender {
public:
    VideoRender(int type) {
        m_RenderType = type;
    }

    virtual ~VideoRender(){}

    virtual void Init(int videoHeight, int videoWidth, int *dstSize) = 0;

    /**
     * 开始绘制画面
     * @param pImage
     */
    virtual void RenderVideoFrame(NativeImage *pImage) = 0;

    virtual void UnInit() = 0;

    int GetRenderType() {
        return m_RenderType;
    }


private:
    int m_RenderType = VIDEO_RENDER_ANWINDOW;


};

#endif //STUDYFFMPEGBYANDROID2042_VIDEORENDER_H
