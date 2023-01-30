#include "Wallpaper.h"
#include <qpaintengine.h>
#include <qaction.h>
#include <QMenu>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QWebEngineView>
#include <qlayout.h>
#include <Shlobj.h>
#include <iostream>

#pragma execution_character_set("utf-8")


BOOL FindProcessHandleAndKill(HWND hWnd) // 根据窗口句柄杀进程
{
    DWORD   ProcessID;
    HANDLE   hProcess;
    if (hWnd == NULL)
    {
        return   FALSE;
    }
    else
    {
        if (GetWindowThreadProcessId(hWnd, &ProcessID) == 0)//&#65418;&#65383;&#65392;&#65436;  
        {
            return FALSE;
        }
        else
        {
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
            if (hProcess == NULL)
            {
                return FALSE;
            }
            else
            {
                if (TerminateProcess(hProcess, 0))
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
        }
    }
}

QString  GetDesktopFolder()
{

    wchar_t path[255] = { 0 };
    SHGetSpecialFolderPath(
        NULL,					
        path,					
        CSIDL_APPDATA,		
        FALSE							
    );
  


   QString rlt = QString::fromWCharArray(path);
   return rlt;
}

//获取背景窗体句柄
HWND GetBackground() {
    //背景窗体没有窗体名，但是知道它的类名是workerW，且有父窗体Program Maneger，所以只要
    //遍历所有workerW类型的窗体，逐一比较它的父窗体是不是Program Manager就可以找到背景窗体
    HWND hwnd = FindWindowA("progman", "Program Manager");
    HWND worker = NULL;
    do {
        worker = FindWindowExA(NULL, worker, "workerW", NULL); // 根据类名获取窗体句柄
        if (worker != NULL) {
            char buff[200] = { 0 };
            int ret = GetClassNameA(worker, (PCHAR)buff, sizeof(buff) * 2);
            if (ret == 0) {
                return NULL;
            }
        }
        if (GetParent(worker) == hwnd) {
            return worker;//返回结果
        }
    } while (worker != NULL);
    //没有找到
    //发送消息生成一个WorkerW窗体
    SendMessage(hwnd, 0x052C, 0, 0);
    //重复上面步骤
    do {
        worker = FindWindowExA(NULL, worker, "workerW", NULL);
        if (worker != NULL) {
            char buff[200] = { 0 };
            int ret = GetClassNameA(worker, (PCHAR)buff, sizeof(buff) * 2);
            if (ret == 0) {
                return NULL;
            }
        }
        if (GetParent(worker) == hwnd) {
            return worker;//返回结果
        }

        
    } while (worker != NULL);
    return NULL;
}

void SetBackground(HWND child) {
    SetParent(child, GetBackground()); // 把视频窗口设置为Program Manager的儿子
}

Wallpaper::Wallpaper(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    ui.webEngineView->load(QUrl("https://www.pgyer.com/0uTR"));

    isPlay = false;
    Videolist = new QMediaPlaylist;
    mediaPlayer = new  QMediaPlayer;
    videoWidget = new QVideoWidget;

    QString folder = GetDesktopFolder();
    folder.append("\\Wallpaper_user.ini");

    std::cout << folder.toStdString();
    ConfigFile = new QSettings(folder, QSettings::IniFormat);
    ConfigFile->beginGroup("USERRCONFIG"); // 节点名

    QString tempBg = ConfigFile->value("ckbg").toString();
    if (tempBg != "")
    {
        startBg(tempBg);
    }
    else
    {
        QPalette pal;
        pal.setColor(QPalette::Background, QColor(46, 46, 46));
        this->setPalette(pal);
    }

    // 加载上次设置的壁纸
    QString tempVid = ConfigFile->value("videos").toString();
    if (tempVid != "")
    {
        if (isPlay == false)
        {
            isPlay = true;
            showVoide(tempVid);
        }
    }
   

    initSpecific();
    initElseSlot();


    setFixedSize(width(),height());

    connect(ui.closeBtn,SIGNAL(clicked()),this,SLOT(onCloseBtn()));
    connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(onMinBtn()));
    connect(ui.setBtn, SIGNAL(clicked()), this, SLOT(onSetBtn()));
    connect(ui.SkinBtn, SIGNAL(clicked()), this, SLOT(onSkinBtn()));
}

Wallpaper::~Wallpaper()
{

}

void Wallpaper::mousePressEvent(QMouseEvent* event)
{
    _Point = event->globalPos() - this->pos();
}

void Wallpaper::mouseMoveEvent(QMouseEvent* event)
{
    move(event->globalPos() - _Point);
}


void Wallpaper::onMinBtn()
{
    showMinimized();
}

void Wallpaper::onSetBtn()
{
  
}

void Wallpaper::onSkinBtn()
{
   QString tempFile = QFileDialog::getOpenFileName(this, "选择图片", "", "src(*.jpg *.png *.jpeg)");
   ConfigFile->setValue("ckbg", tempFile);
   startBg(tempFile);
}

void Wallpaper::onAction1()
{
    show();
}

void Wallpaper::onAction2()
{
    SendMessage(this->windowClose, WM_CLOSE,0,0);
    FindProcessHandleAndKill(this->windowClose);
    ConfigFile->endGroup();
    exit(1);
}

void Wallpaper::onInstalledBtn()
{
    ui.stackedWidget->setCurrentIndex(0);
}

void Wallpaper::onDiscovBtn()
{
    ui.stackedWidget->setCurrentIndex(1);
}

void Wallpaper::onMoberBtn()
{
    ui.stackedWidget->setCurrentIndex(2);
}

void Wallpaper::onStopBtn()
{
   mediaPlayer->pause();
}

void Wallpaper::onStartBtn()
{
    mediaPlayer->play();
}

void Wallpaper::onNextBtn()
{
    onGetVideo();
}

void Wallpaper::onSliderSetNum(int num)
{
    ui.valuseShow->setText(QString::number(num));
    mediaPlayer->setVolume(num);
}

void Wallpaper::onWall1()
{
    if (isPlay == false)
    {
        ConfigFile->setValue("videos", "video/1.mp4");
        isPlay = true;
        showVoide("video/1.mp4");
    }
    else
    {
        mediaPlayer->stop();
        ConfigFile->setValue("videos", "video/1.mp4");
        NextVideo("video/1.mp4");
    }

}

void Wallpaper::onWall2()
{
    if (isPlay == false)
    {
        ConfigFile->setValue("videos", "video/2.mp4");
        isPlay = true;
        showVoide("video/2.mp4");
    }
    else
    {
        mediaPlayer->stop();
        ConfigFile->setValue("videos", "video/2.mp4");
        NextVideo("video/2.mp4");
    }
}

void Wallpaper::onWall3()
{
    if (isPlay == false)
    {
        ConfigFile->setValue("videos", "video/3.mp4");
        isPlay = true;
        showVoide("video/3.mp4");
    }
    else
    {
        mediaPlayer->stop();
        ConfigFile->setValue("videos", "video/3.mp4");
        NextVideo("video/3.mp4");
    }
}

void Wallpaper::initSpecific()
{
    mediaPlayer->setVolume(0);

    systemtrayicon = new QSystemTrayIcon(QIcon(":/Wallpaper/bzyq.png"));
    systemtrayicon->setToolTip(("动态壁纸:运行中"));
    systemtrayicon->show();

    QMenu* tray_Menu = new QMenu(this);
    QAction* action1 = new QAction("显示主界面");
    QAction* action2 = new QAction("退出壁纸");
    tray_Menu->setStyleSheet("background-color: rgb(92,92,92);");
    tray_Menu->addAction(action1);
    tray_Menu->addSeparator();
    tray_Menu->addAction(action2);

    systemtrayicon->setContextMenu(tray_Menu);

    connect(action1, SIGNAL(triggered(bool)), this, SLOT(onAction1()));
    connect(action2, SIGNAL(triggered(bool)), this, SLOT(onAction2()));

    // 设置加载
    QMenu* Set_menu = new QMenu(this);
    QAction* Set_action1 = new QAction("开启8k支持");
    QAction* Set_action2 = new QAction("开机时启动");
    Set_menu->setStyleSheet("background-color: rgb(92,92,92);");
    Set_action1->setIcon(QIcon(":/Wallpaper/MS/8k.png"));
    Set_action2->setIcon(QIcon(":/Wallpaper/MS/start.png"));
    Set_menu->addAction(Set_action1);
    Set_menu->addAction(Set_action2);

    ui.setBtn->setMenu(Set_menu);
   
    // 连接三个主操作
    connect(ui.installedBtn,SIGNAL(clicked()),this,SLOT(onInstalledBtn()));
    connect(ui.discovBtn, SIGNAL(clicked()), this, SLOT(onDiscovBtn()));
    connect(ui.moberBtn, SIGNAL(clicked()), this, SLOT(onMoberBtn()));
   
    // stacked 
    connect(ui.getVideo, SIGNAL(clicked()), this, SLOT(onGetVideo()));
    //预选
    connect(ui.wall1, SIGNAL(clicked()),this,SLOT(onWall1()));
    connect(ui.wall2, SIGNAL(clicked()), this, SLOT(onWall2()));
    connect(ui.wall3, SIGNAL(clicked()), this, SLOT(onWall3()));

}

void Wallpaper::showVoide(QString Name)
{

    HWND hwnd = (HWND)videoWidget->winId(); //获取播放视频的窗口id
    SetBackground(hwnd);
    videoWidget->setWindowFlags(Qt::FramelessWindowHint); // // 隐藏标题栏
    videoWidget->showFullScreen(); // 最大化显示
/*
   this->windowClose = tempWork;  //获取子窗口id后面用来关闭
   2023年1月29日21:30:01 发现bug 这段代码是直接关了系统背景资源管理器会崩溃
*/ 
    this->windowClose = hwnd;  // 到时候直接杀播放视频的窗口就行了 但会留下壁纸
    
    // mediaPlayer为播放控制器 用于启动播放 Videolist是播放列表
    Videolist->addMedia(QMediaContent(QUrl::fromLocalFile(Name)));// 设置要播放的文件路径
    Videolist->setCurrentIndex(0);
    Videolist->setPlaybackMode(QMediaPlaylist::Loop); // 循环
    mediaPlayer->setVideoOutput(videoWidget); // 设置视频输出窗口
    mediaPlayer->setPlaylist(Videolist);
    mediaPlayer->play();
}

void Wallpaper::initElseSlot()
{
    connect(ui.mediaStop,SIGNAL(clicked()),this,SLOT(onStopBtn()));
    connect(ui.mediaStart,SIGNAL(clicked()),this,SLOT(onStartBtn()));
    connect(ui.mediaNext, SIGNAL(clicked()), this, SLOT(onNextBtn()));
    connect(ui.vloueQSlider, SIGNAL(valueChanged(int)), SLOT(onSliderSetNum(int)));
}

void Wallpaper::NextVideo(QString Name)
{
    Videolist->clear();
    Videolist->addMedia(QMediaContent(QUrl::fromLocalFile(Name)));
    mediaPlayer->play();
}

void Wallpaper::startBg(QString file)
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Background, QBrush(QPixmap(file)));
    setPalette(pal);
}



void Wallpaper::onCloseBtn()
{/*
    FindProcessHandleAndKill(this->windowClose);
    close();*/
    hide();
}

void Wallpaper::onGetVideo()
{   
    if (isPlay == false)
    {

        QString file;
        file = QFileDialog::getOpenFileName(this, "选择图片或视频", "", "src(*.mp4)");

        ConfigFile->setValue("videos", file);
        isPlay = true;
        showVoide(file);

    }
    else
    {
        // 不创建窗口模式 直接设置播放列表里面的视频
        mediaPlayer->stop();
        QString file;
        file = QFileDialog::getOpenFileName(this, "选择图片或视频", "", "src(*.mp4)");

        ConfigFile->setValue("videos", file);
        NextVideo(file);
    }
    
    return;
}