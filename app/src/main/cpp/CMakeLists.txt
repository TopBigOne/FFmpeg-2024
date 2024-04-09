
cmake_minimum_required(VERSION 3.22.1)

set(libname learn-ffmpeg)

set(JNI_LIBS ${CMAKE_SOURCE_DIR}/../jniLibs)
project(${libname})


include_directories(
        include
        glm
        common
        #        ${CMAKE_SOURCE_DIR}/recorder
        ${CMAKE_SOURCE_DIR}/util
        #        ${CMAKE_SOURCE_DIR}/player
        #        ${CMAKE_SOURCE_DIR}/player/decoder
        #        ${CMAKE_SOURCE_DIR}/player/render
        #        ${CMAKE_SOURCE_DIR}/player/render/audio
        #        ${CMAKE_SOURCE_DIR}/player/render/video)
)


link_directories(
        ${JNI_LIBS}/${ANDROID_ABI})


file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        #        ${CMAKE_SOURCE_DIR}/common/*.cpp
        #        ${CMAKE_SOURCE_DIR}/player/*.cpp
        #        ${CMAKE_SOURCE_DIR}/player/decoder/*.cpp
        #        ${CMAKE_SOURCE_DIR}/player/render/video/*.cpp
        #        ${CMAKE_SOURCE_DIR}/player/render/audio/*.cpp
        #        ${CMAKE_SOURCE_DIR}/recorder/SingleVideoRecorder.cpp
        #        ${CMAKE_SOURCE_DIR}/recorder/SingleAudioRecorder.cpp
        #        ${CMAKE_SOURCE_DIR}/recorder/MediaRecorderContext.cpp
        #        ${CMAKE_SOURCE_DIR}/recorder/GLCameraRender.cpp
        #        ${CMAKE_SOURCE_DIR}/recorder/MediaRecorder.cpp
        )

add_library( # Sets the name of the library.
        ${libname}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
        )


set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        fdk-aac
        x264
        )

set(native-libs
        android
        mediandk
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        ${libname}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${third-party-libs}
        ${native-libs}

        )