package com.better.year.media;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;

public class MySurfaceView extends SurfaceView {
    private static final String TAG          = "MySurfaceView";
    private              int    mRatioWidth  = 0;
    private              int    mRatioHeight = 0;

    public MySurfaceView(Context context) {
        this(context, null);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width  = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        // case 1:
        if (mRatioWidth == 0 || mRatioHeight == 0) {
            setMeasuredDimension(width, height);
            return;
        }

        // case 2:
        if (width < height * mRatioWidth / mRatioHeight) {
            setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
        } else {
            setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
        }
    }


    /**
     * 设置 屏幕高宽比
     *
     * @param width
     * @param height
     */
    public void setAspectRatio(int width, int height) {
        Log.d(TAG, "setAspectRatio() called with: width = [" + width + "], height = [" + height + "]");
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }

        mRatioWidth = width;
        mRatioHeight = height;
        requestLayout();
    }


}
