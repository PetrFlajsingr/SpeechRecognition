# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


# variables for files
set(DEBUG_MODEL 0)
set(RS_FILES "")
set(RS_OBJ_FILES "")
set(RENDER_SCRIPT_HEADERS_PATH ${ANDROID_NDK}/toolchains/renderscript/prebuilt/${ANDROID_HOST_TAG}/platform/rs)

# create the rs_libs
execute_process(COMMAND mkdir -p "${PROJECT_SOURCE_DIR}/src/main/rs_libs/${ANDROID_ABI}")

include_directories(src/main/cpp/include)
include_directories(src/main/cpp/Decoder/include)
include_directories(src/main/cpp/Utility/include)
include_directories(src/main/cpp/Feature_Extraction/include)
include_directories(src/main/cpp/Voice_Activity_Detection/include)
include_directories(src/main/cpp/Threads/include)
include_directories(src/main/cpp/kissfft)
include_directories(src/main/cpp/firfilter)
include_directories(${RENDER_SCRIPT_HEADERS_PATH}/cpp
                    ${RENDER_SCRIPT_HEADERS_PATH}/scriptc
                    ${RENDER_SCRIPT_HEADERS_PATH})
link_directories(${ANDROID_NDK}/toolchains/renderscript/prebuilt/${ANDROID_HOST_TAG}/platform/${ANDROID_SYSROOT_ABI})

file(GLOB_RECURSE RS_FILES src/main/rs/*)

if(DEBUG_MODEL)
    include_directories(build/generated/source/rs/debug)
    file(GLOB_RECURSE RS_CPP_FILES build/generated/source/rs/debug/*)
else()
    include_directories(build/generated/source/rs/release)
    file(GLOB_RECURSE RS_CPP_FILES build/generated/source/rs/release/*)
endif()

foreach(rs_file ${RS_FILES})
    get_filename_component(name ${rs_file} NAME_WE)
    list(APPEND RS_OBJ_FILES ${PROJECT_SOURCE_DIR}/src/main/obj/local/${ANDROID_ABI}/objs/voicerecognition/${name}.o)
    configure_file(${PROJECT_SOURCE_DIR}/src/main/obj/local/${ANDROID_ABI}/librs.${name}.so ${PROJECT_SOURCE_DIR}/src/main/rs_libs/${ANDROID_ABI} COPYONLY)
endforeach()

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
              speech_recognition

              # Sets the library as a shared library.
              SHARED

              # UTILITY
              src/main/cpp/Utility/RawAudioRecorder.cpp
              src/main/cpp/Utility/AudioSubsampler.cpp
              src/main/cpp/Utility/FeatureMatrix.cpp
              src/main/cpp/Utility/Utils.cpp
              src/main/cpp/Utility/WavReader.cpp

              # FEATURE EXTRACTION
              src/main/cpp/Feature_Extraction/AudioFrame.cpp
              src/main/cpp/Feature_Extraction/RSMelFilterBank.cpp
              src/main/cpp/Feature_Extraction/RSNeuralNetwork.cpp

              # THIRD PARTY
              src/main/cpp/kissfft/kiss_fft.c
              src/main/cpp/kissfft/kiss_fftr.c
              src/main/cpp/firfilter/filt.cpp

              # DECODER
              src/main/cpp/Decoder/HMMGraph.cpp
              src/main/cpp/Decoder/GraphNode.cpp
              src/main/cpp/Decoder/ViterbiDecoder.cpp
              src/main/cpp/Decoder/AcousticModel.cpp
              src/main/cpp/Decoder/LanguageModel.cpp
              src/main/cpp/Decoder/Word.cpp
              src/main/cpp/Decoder/LMWord.cpp
              src/main/cpp/Decoder/Phoneme.cpp
              src/main/cpp/Decoder/Token.cpp
              src/main/cpp/Decoder/Bigram.cpp
              src/main/cpp/Decoder/WordLinkRecord.cpp

              # VOICE ACTIVITY DETECTION
              src/main/cpp/Voice_Activity_Detection/VoiceActivityDetector.cpp

              # THREADS
              src/main/cpp/Threads/MelBankThread.cpp
              src/main/cpp/Threads/NNThread.cpp
              src/main/cpp/Threads/DecoderThread.cpp
              src/main/cpp/Threads/FileStreamThread.cpp
              src/main/cpp/Threads/JavaCallbacks.cpp

              ${RS_OBJ_FILES}
              ${RS_FILES}
              src/main/cpp/SpeechRecognitionAPI.cpp
              src/main/cpp/audio.cpp


)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       speech_recognition
                       OpenSLES
                       RScpp_static

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )