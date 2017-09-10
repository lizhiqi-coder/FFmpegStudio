//
// Created by 58boy on 2017/9/5.
//

#include "camera_client.h"
void CameraClient::connect() {
//    detachThreadCreate();
}

int CameraClient::detachThreadCreate(pthread_t *pthread, void *start_routine, void *arg) {
    return 0;
}

void CameraClient::startConnection()
{

}

void CameraClient::doStartThread(void *arg) {

    CameraClient *cclient = (CameraClient *) arg;
    if (cclient != NULL) {
        cclient->startConnection();
    }
}


