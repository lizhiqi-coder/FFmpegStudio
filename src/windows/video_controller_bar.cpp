//
// Created by 58boy on 2017/10/13.
//

#include "video_controller_bar.h"

VideoControllerBar::VideoControllerBar(QWidget *parent) :
        QWidget(parent), ui(new Ui::Form) {
    ui->setupUi(this);
}

VideoControllerBar::~VideoControllerBar() {
    delete ui;
}
