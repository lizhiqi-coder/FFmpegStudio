//
// Created by 58boy on 2017/9/5.
//

#ifndef FFMPEGSTUDIO_CAMERA_CLIENT_H
#define FFMPEGSTUDIO_CAMERA_CLIENT_H


#include <pthread.h>
class CameraClient {

public:
    void connect();

    int detachThreadCreate(pthread_t *pthread,void *start_routine,void *arg);

    void startConnection();
    static void doStartThread(void *arg);



};


#endif //FFMPEGSTUDIO_CAMERA_CLIENT_H
