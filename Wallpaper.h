#pragma once

#include <QtWidgets/QWidget>
#include "ui_Wallpaper.h"
#include <qevent.h>
#include <qsystemtrayicon.h>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QSettings>
#include <Windows.h>



class Wallpaper : public QWidget
{
    Q_OBJECT

public:
    Wallpaper(QWidget *parent = nullptr);
    ~Wallpaper();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void onCloseBtn();
    void onMinBtn();
    void onSetBtn();
    void onSkinBtn();
    void onAction1(); // 托盘的
    void onAction2(); // 托盘的

    void onInstalledBtn();
    void onDiscovBtn();
    void onMoberBtn();
    void onStopBtn();
    void onStartBtn();
    void onNextBtn();
    void onSliderSetNum(int);

    void onWall1();
    void onWall2();
    void onWall3();

private slots:
    void onGetVideo();

private:
    Ui::WallpaperClass ui;
    QPoint _Point;
    QSystemTrayIcon* systemtrayicon;
    bool isPlay; // 记录是否创建过窗口
    HWND windowClose; // 用于关闭的窗口句柄

    QVideoWidget* videoWidget;
    QMediaPlayer* mediaPlayer;
    QMediaPlaylist* Videolist;
    QSettings *ConfigFile; // 配置文件操作
    int checkBtn; // 是否开关音量

    HWND _wordKill;
private:
    void initSpecific(); // init特殊处理的
    void showVoide(QString Name);
    void initElseSlot();
    void NextVideo(QString Name);
    void startBg(QString);
  
};
