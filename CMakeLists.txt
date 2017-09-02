cmake_minimum_required(VERSION 3.6)
project(FFmpegStudio)

set(SOURCE_FILES
        main.cpp
        mainwindow.cpp
        src/core/h264_decoder.cpp
        src/core/h264_frame.h
        )

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(FFmpegStudio ${SOURCE_FILES})
target_link_libraries(FFmpegStudio -lavcodec -lavformat -lavfilter
                       -lavdevice -lavutil -lswresample -lswscale)