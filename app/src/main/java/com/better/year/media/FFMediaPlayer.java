package com.better.year.media;

public class FFMediaPlayer {
    static {
        System.loadLibrary("learn-ffmpeg");
    }



    public static String GetFFmpegVersion() {
        return native_GetFFmpegVersion();
    }

    private static native String native_GetFFmpegVersion();


}
