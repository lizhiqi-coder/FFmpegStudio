cmake_minimum_required(VERSION 3.6)
project(FFmpegStudio)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        src/core/h264_frame.h
        main.cpp
        mainwindow.cpp
        src/core/h264_decoder.cpp
        src/core/camera_client.cpp
        src/opengl/gl_widget.h src/opengl/gl_widget.cpp
        src/VideoPlayer.h src/VideoPlayer.cpp
        src/core/ffmpeg_capturer.cpp src/core/ffmpeg_capturer.h
        src/core/ffmpeg_frame.h src/core/ffmpeg_frame.cpp
        src/windows/video_controller_bar.h src/windows/video_controller_bar.cpp
        src/core/streamer/video_streamer.cpp src/core/streamer/video_streamer.h)


set(BUILD_DIR ${PROJECT_SOURCE_DIR}/../build)

include_directories(
        ${BUILD_DIR}
        C:\\Qt\\5.9.1\\mingw53_32\\include
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/src/opengl)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(FFmpegStudio ${SOURCE_FILES})
target_link_libraries(FFmpegStudio
        -pthread -lpthread
        -lavcodec -lavformat -lavfilter
        -lavdevice -lavutil -lswresample -lswscale)