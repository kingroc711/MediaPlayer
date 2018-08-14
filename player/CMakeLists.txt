
cmake_minimum_required(VERSION 3.4.1)
include_directories(src/main/cpp/include)

add_library(ffmpeg SHARED IMPORTED)


set_target_properties(ffmpeg
                        PROPERTIES IMPORTED_LOCATION
                        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libffmpeg.so)


add_library(
             native-lib
             SHARED
             src/main/cpp/AudioPlayer
             src/main/cpp/AudioQueue
             src/main/cpp/native-lib )

find_library(
              log-lib
              log )

target_link_libraries( # Specifies the target library.
                       native-lib
                       OpenSLES
                       ffmpeg
                       ${log-lib} )