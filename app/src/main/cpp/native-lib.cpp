#include <jni.h>
#include <string>
#include "util/LogUtil.h"


extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
}


jstring native_GetFFmpegVersion(JNIEnv *env, jclass clazz) {
    LOGCATD(__func__)
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGCATI("   \n%s", strBuffer);

    //ASanTestCase::MainTest();

    return env->NewStringUTF(strBuffer);
}


jlong native_Init(JNIEnv *env, jobject thiz, jstring url,
                  jint player_type, jint render_type,
                  jobject surface) {
    LOGCATD(__func__)
    // TODO: implement native_Init()
}

void
native_Play(JNIEnv *env, jobject thiz,
            jlong player_handle) {
    LOGCATD(__func__)
    // TODO: implement native_Play()
}

void native_SeekToPosition(JNIEnv *env, jobject thiz,
                           jlong player_handle,
                           jfloat position) {
    LOGCATD(__func__)
    // TODO: implement native_SeekToPosition()
}

void
native_Pause(JNIEnv *env, jobject thiz,
             jlong player_handle) {
    LOGCATD(__func__)
    // TODO: implement native_Pause()
}

void
native_Stop(JNIEnv *env, jobject thiz,
            jlong player_handle) {
    LOGCATD(__func__)
    // TODO: implement native_Stop()
}

void
native_UnInit(JNIEnv *env, jobject thiz,
              jlong player_handle) {
    LOGCATD(__func__)
    // TODO: implement native_UnInit()
}

jlong
native_GetMediaParams(JNIEnv *env, jobject thiz,
                      jlong player_handle,
                      jint param_type) {
    LOGCATD(__func__)
    // TODO: implement native_GetMediaParams()
}

void
native_SetMediaParams(JNIEnv *env, jobject thiz,
                      jlong player_handle,
                      jint param_type, jobject param) {
    LOGCATD(__func__)
    // TODO: implement native_SetMediaParams()
}

void
native_OnSurfaceCreated(JNIEnv *env, jclass clazz,
                        jint render_type) {
    LOGCATD(__func__)

}

void
native_OnSurfaceChanged(JNIEnv *env, jclass clazz,
                        jint render_type, jint width,
                        jint height) {
    LOGCATD(__func__)
    // TODO: implement native_OnSurfaceChanged()
}

void
native_OnDrawFrame(JNIEnv *env, jclass clazz,
                   jint render_type) {
    LOGCATD(__func__)
    // TODO: implement native_OnDrawFrame()
}

void
native_SetGesture(JNIEnv *env, jclass clazz,
                  jint render_type, jfloat x_rotate_angle,
                  jfloat y_rotate_angle, jfloat scale) {
    LOGCATD(__func__)
    // TODO: implement native_SetGesture()
}

void
native_SetTouchLoc(JNIEnv *env, jclass clazz,
                   jint render_type, jfloat touch_x,
                   jfloat touch_y) {
    LOGCATD(__func__)
    // TODO: implement native_SetTouchLoc()
}


JNIEnv *mEnv = NULL;
JavaVM *mJavaVM = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGCATD(__func__)
    mJavaVM = vm;
    if (vm->GetEnv(reinterpret_cast<void **>(&mEnv), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_FALSE;
    }

    jclass cls = mEnv->FindClass("com/better/year/media/FFMediaPlayer");
    if (cls == NULL) {
        return JNI_ERR;
    }

    JNINativeMethod methods[] = {
            {"native_GetFFmpegVersion", "()Ljava/lang/String;",                      (void *) native_GetFFmpegVersion},
            {"native_Init",             "(Ljava/lang/String;IILjava/lang/Object;)J", (void *) native_Init},
            {"native_Play",             "(J)V",                                      (void *) native_Play},
            {"native_SeekToPosition",   "(JF)V",                                     (void *) native_SeekToPosition},
            {"native_Pause",            "(J)V",                                      (void *) native_Pause},
            {"native_Stop",             "(J)V",                                      (void *) native_Stop},
            {"native_UnInit",           "(J)V",                                      (void *) native_UnInit},
            {"native_GetMediaParams",   "(JI)J",                                     (void *) native_GetMediaParams},
            {"native_SetMediaParams",   "(JILjava/lang/Object;)V",                   (void *) native_SetMediaParams},
            {"native_OnSurfaceCreated", "(I)V",                                      (void *) native_OnSurfaceCreated},
            {"native_OnSurfaceChanged", "(III)V",                                    (void *) native_OnSurfaceChanged},
            {"native_OnDrawFrame",      "(I)V",                                      (void *) native_OnDrawFrame},
            {"native_SetGesture",       "(IFFF)V",                                   (void *) native_SetGesture},
            {"native_SetTouchLoc",      "(IFF)V",                                    (void *) native_SetTouchLoc},
    };

    jint registerNativeResult = mEnv->RegisterNatives(cls, methods,
                                                      sizeof(methods) / sizeof(methods[0]));
    LOGCATI("   registerNativeResult :%d", registerNativeResult);
    if (registerNativeResult < 0) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
