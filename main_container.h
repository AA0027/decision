#pragma once

#include <QWidget>
#include <QWidget>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QImage>
#include <QSlider>
#include <QPushButton>
class Main_Container : public QWidget
{
    Q_OBJECT
public:
    Main_Container(QWidget* parent = nullptr);

private:
    QMediaPlayer* player;
    QVideoSink* sink;
    QImage currentFrame;

    QPushButton* ctr_btn;
};
