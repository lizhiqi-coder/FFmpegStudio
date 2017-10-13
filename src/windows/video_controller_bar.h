//
// Created by 58boy on 2017/10/13.
//

#ifndef FFMPEGSTUDIO_VIDEO_CONTROLLER_BAR_H
#define FFMPEGSTUDIO_VIDEO_CONTROLLER_BAR_H

#include <QtWidgets/QWidget>
#include "ui_video_controller_bar.h"


class VideoControllerBar : public QWidget {
Q_OBJECT
public:
    VideoControllerBar(QWidget *parent = 0);

    ~VideoControllerBar();

private:
    Ui::Form *ui;
};


#endif //FFMPEGSTUDIO_VIDEO_CONTROLLER_BAR_H
