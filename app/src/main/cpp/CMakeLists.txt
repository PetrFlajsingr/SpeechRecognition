cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")


add_library(raw_audio_recorder SHARED
            RawAudioRecorder.cpp
            RawAudioRecorder.h
            AudioSubsampler.h
            AudioSubsampler.cpp
            audio.cpp)


# Include libraries needed for native-audio-jni lib
target_link_libraries(raw_audio_recorder
                      android
                      log
                      OpenSLES)
