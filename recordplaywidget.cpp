#include "recordplaywidget.h"
#include "ui_recordplaywidget.h"
#include <QStyleFactory>
#include <QDateTime>
#include <event.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include "state.h"
#include "log.h"
#include <QDebug>
#include "qplayer.h"
#include "myslider.h"
#include <QSlider>

int g_iDateEditNo = 0;      //要显示时间的不同控件的编号
static int g_iRNum = 0;
#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面
pthread_mutex_t g_sliderValueSetMutex;


recordPlayWidget::recordPlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::recordPlayWidget)
{
    char timestr[128] = {0};
    int i = 0;
    int iYear = 0, iMonth= 0, iDay = 0;
    QString string = "";
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

//    mousePressEvent(e);

    ui->fileDownloadProgressBar->hide();
    ui->fileDownloadProgressBar->setRange(0,100);

    ui->alarmPushButton->setFocusPolicy(Qt::NoFocus);


    m_playSlider = new mySlider(this);    //创建播放进度条
    m_playSlider->setOrientation(Qt::Horizontal);    //设置水平方向
    m_playSlider->setGeometry(320, 584, 702, 29);
    m_playSlider->show();
    /*定义播放进度条样式*/
    m_playSlider->setStyleSheet("QSlider::groove:horizontal{border: 1px solid #4A708B;background: #C0C0C0;height: 5px;border-radius: 1px;padding-left:-1px;padding-right:-1px;}"
                                "QSlider::sub-page:horizontal{background: qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #B1B1B1,stop:1 #c4c4c4);background:qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,stop: 0 #5DCCFF,stop: 1 #1874CD);border: 1px solid #4A708B;height: 10px;border-radius: 2px;}"
                                "QSlider::add-page:horizontal{background: #575757;border: 0px solid #777;height: 10px;border-radius: 2px;}"
                                "QSlider::handle:horizontal{background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,stop:0.6 #45ADED, stop:0.778409 rgba(255, 255, 255, 255));width: 11px;margin-top: -3px;margin-bottom: -3px;border-radius: 5px;}"
                                "QSlider::handle:horizontal:hover{background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.6 #2A8BDA,stop:0.778409 rgba(255, 255, 255, 255));width: 11px;margin-top: -3px;margin-bottom: -3px;border-radius: 5px;}"
                                "QSlider::sub-page:horizontal:disabled{background: #00009C;border-color: #999;}"
                                "QSlider::add-page:horizontal:disabled{background: #eee;border-color: #999;}"
                                "QSlider::handle:horizontal:disabled{background: #eee;border: 1px solid #aaa;border-radius: 4px;}");

    connect(m_playSlider, SIGNAL(presliderPressSianal(int)), this, SLOT(playSliderPressSlot(int)));   //点击进度条信号响应
    connect(m_playSlider, SIGNAL(presliderMoveSianal(int)), this, SLOT(playSliderMoveSlot(int)));   //拖动进度条信号响应



    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    m_alarmHappenTimer = NULL;

    m_tableWidgetStyle = QStyleFactory::create("windows");
    ui->recordFileTableWidget->setStyle(m_tableWidgetStyle);   //设置tablewidget显示风格为windows风格，否则里面的checkbox选中默认显示叉而不是勾
    ui->recordFileTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->recordFileTableWidget->horizontalHeader()->setSectionsClickable(false); ////设置表头不可点击
    ui->recordFileTableWidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->recordFileTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->recordFileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置整行选中方式
    ui->recordFileTableWidget->setSelectionMode(QAbstractItemView::NoSelection); //设置只能选择一行，不能多行选中
    ui->recordFileTableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->recordFileTableWidget->horizontalHeader()->resizeSection(0,46); ////设置表头第一列的宽度为46
    ui->recordFileTableWidget->horizontalHeader()->resizeSection(1,46);
    ui->recordFileTableWidget->horizontalHeader()->resizeSection(2,219);

    ui->playPushButton->setFocusPolicy(Qt::NoFocus);
    ui->stopPushButton->setFocusPolicy(Qt::NoFocus);
    ui->playLastOnePushButton->setFocusPolicy(Qt::NoFocus);
    ui->playNextOnePushButton->setFocusPolicy(Qt::NoFocus);
    ui->fastForwardPushButton->setFocusPolicy(Qt::NoFocus);
    ui->slowForwardPushButton->setFocusPolicy(Qt::NoFocus);
    ui->plusStepPushButton->setFocusPolicy(Qt::NoFocus);
    ui->minusStepPushButton->setFocusPolicy(Qt::NoFocus);
    ui->playSpeedLineEdit->setFocusPolicy(Qt::NoFocus);


    //
    m_iPlayFlag = 0;

    setPlayButtonStyleSheet();
    getTrainConfig();
    mediaInit();


//    Mouseflag = true;
    ui->StartdateEdit->setCalendarPopup(true);
    ui->EnddateEdit->setCalendarPopup(true);
//    ui->StartdateEdit->setAttribute(Qt::WA_TransparentForMouseEvents,Mouseflag);

    ui->EnddateEdit->setDateTime(QDateTime::currentDateTime());
    ui->EndtimeEdit->setDateTime(QDateTime::currentDateTime());


    ui->StartdateEdit->setDateTime(QDateTime::currentDateTime());
    ui->StarttimeEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    connect(ui->canselPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));

    connect(ui->queryPushButton, SIGNAL(clicked(bool)), this, SLOT(recordQuerySlot()));    //录像查询按钮按键信号响应
    connect(ui->downLoadPushButton, SIGNAL(clicked(bool)), this, SLOT(recordDownloadSlot()));    //录像下载按钮按键信号响应
    connect(ui->playPushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlayStartSlot()));	   //播放按钮按键信号响应
    connect(ui->stopPushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlayStopSlot()));		//停止按钮按键信号响应
    connect(ui->fastForwardPushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlayFastForwardSlot()));    //快进按钮按键信号响应
    connect(ui->slowForwardPushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlaySlowForwardSlot()));    //慢放按钮按键信号响应
    connect(ui->playLastOnePushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlayLastOneSlot()));    //播放上一个按钮按键信号响应
    connect(ui->playNextOnePushButton, SIGNAL(clicked(bool)), this, SLOT(recordPlayNextOneSlot()));	 //播放下一个按钮按键信号响应
    connect(ui->plusStepPushButton, SIGNAL(clicked(bool)), this, SLOT(playPlusStepSlot()));   //拖动进度条信号响应
    connect(ui->minusStepPushButton, SIGNAL(clicked(bool)), this, SLOT(playMinusStepSlot()));   //拖动进度条信号响应



    connect(ui->carSeletionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(carNoChangeSlot()));  //车厢选择下拉框当前索引改变信号响应


    QObject::connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(getduration(qint64)));
    QObject::connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionchaged(qint64)));


//    QObject::connect(m_playSlider,SIGNAL(QSlider::sliderMoved()),this,SLOT(setpostion()));
}

recordPlayWidget::~recordPlayWidget()
{

    pthread_mutex_destroy(&g_sliderValueSetMutex);
    closePlayWin();

    if (m_tableWidgetStyle != NULL)
    {
        delete m_tableWidgetStyle;
        m_tableWidgetStyle = NULL;
    }
    delete m_playSlider;
    m_playSlider = NULL;
    delete m_playWin;
    m_playWin = NULL;

    delete ui;
}

void recordPlayWidget::mediaInit()
{
    /*新建一个播放窗体*/

    m_playWin = new QWidget(this);
    m_playWin->setGeometry(320, 7, 698, 580);
    m_playWin->show();
    m_playWin->setStyleSheet("QWidget{background-color: rgb(0, 0, 0);}");

//    list = new QMediaPlaylist;
//    list->addMedia(QUrl("/oem/SampleVideo_1280x720_5mb.mp4"));

    QFile file("/userdata/apink.mp4");

    QUrl url("rtsp://admin:admin123@168.168.102.20");

    player = new QMediaPlayer();
//    player->setPlaylist(list);
//    player->setMedia(url);
    if(file.exists())
    {
        player->setMedia(QUrl::fromLocalFile(file.fileName()));
    }

    videoViewer = new QVideoWidget(m_playWin);
    videoViewer->setGeometry(0, 7, 698, 580);
    player->setVideoOutput(videoViewer);

//    player->play();


}

void recordPlayWidget::playSliderPressSlot(int iPosTime)
{

    QString playSpeedStr = "";

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "recordPlayWidget  press play slider!\n");

    if (iPosTime < 0)
    {
        return;
    }
    else if (iPosTime == m_playSlider->value())    //时间值没有变化不进行处理
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] value is not change, do not set!\n", __FUNCTION__);
        return;
    }
    else if (0 == iPosTime) //防止pos值为0而服务器不处理，所有值最小为1
    {
        iPosTime = 1;
    }
    if(player->state()== QMediaPlayer::StoppedState)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = 1.00;
    playSpeedStr = "1.00x";
    ui->playSpeedLineEdit->setText(playSpeedStr);

    pthread_mutex_lock(&g_sliderValueSetMutex);
    m_playSlider->setValue(iPosTime);

//    player->setPosition(iPosTime*player->duration()/ 1000);

     player->setPosition(iPosTime);
    pthread_mutex_unlock(&g_sliderValueSetMutex);


}
void recordPlayWidget::getduration(qint64 playtime)
{
    playtime =player->duration();
    totalplaytime = playtime /= 1000;

}

void recordPlayWidget::positionchaged(qint64 pos)
{
    m_playSlider->setMaximum(totalplaytime);
    m_playSlider->setValue(pos/1000);
}
void recordPlayWidget::playSliderMoveSlot(int iPosTime)
{

    QString playSpeedStr = "";

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "recordPlayWidget  press play slider!\n");

    if (iPosTime < 0)
    {
        return;
    }
    else if (iPosTime == m_playSlider->value())    //时间值没有变化不进行处理
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] value is not change, do not set!\n", __FUNCTION__);
        return;
    }
    else if (0 == iPosTime) //防止pos值为0而服务器不处理，所有值最小为1
    {
        iPosTime = 1;
    }
    if(player->state()== QMediaPlayer::StoppedState)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = 1.00;
    playSpeedStr = "1.00x";
    ui->playSpeedLineEdit->setText(playSpeedStr);

    pthread_mutex_lock(&g_sliderValueSetMutex);
    //m_iSliderValue = iPosTime;
    m_playSlider->setValue(iPosTime);

    player->setPosition(iPosTime);

    pthread_mutex_unlock(&g_sliderValueSetMutex);



}

void recordPlayWidget::recordQuerySlot()
{


}
void recordPlayWidget::recordDownloadSlot()
{



}

void recordPlayWidget::setPlayButtonStyleSheet()
{
    if (0 == m_iPlayFlag)
    {
        ui->playPushButton->setStyleSheet("QPushButton{border-image: url(:/res/play_nor.png);background-color: rgb(255, 255, 255);}"
                                            "QPushButton:hover{border-image: url(:/res/play_hover.png);background-color: rgb(255, 255, 255);}"
                                            "QPushButton:pressed{border-image: url(:/res/play_down.png);background-color: rgb(255, 255, 255);}");
    }
    else
    {
        ui->playPushButton->setStyleSheet("QPushButton{border-image: url(:/res/pause_nor.png);background-color: rgb(255, 255, 255);}"
                                            "QPushButton:hover{border-image: url(:/res/pause_hover.png);background-color: rgb(255, 255, 255);}"
                                            "QPushButton:pressed{border-image: url(:/res/pause_down.png);background-color: rgb(255, 255, 255);}");
    }
}

void recordPlayWidget::getTrainConfig()    	//获取车型配置文件，初始化车厢及摄像机下拉框
{
    int i = 0, j = 0;
    QString item = "";

    T_TRAIN_CONFIG tTrainConfigInfo;
    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);

//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] GetCurrentTrainConfigInfo, nvr server num=%d\n",__FUNCTION__,tTrainConfigInfo.iNvrServerCount);

    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        item = "";
        item = QString::number(tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += tr("号车厢");
        ui->carSeletionComboBox->addItem(item);
        m_Phandle[i] = STATE_GetNvrServerPmsgHandle(i);
        if (0 == i)
        {
//            DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] the first server has camera num=%d\n",__FUNCTION__,tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum);
            for (j = 0; j < tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum; j++)
            {
                item = "";
                item = QString::number(8+j);
                item += tr("号摄像机");
                ui->cameraSelectionComboBox->addItem(item);
            }
        }
    }
}


void recordPlayWidget::recordPlayStartSlot()
{
//    int h,m,s;
    qint64 playtime;

    QString playSpeedStr = "1.00x";
    if (0 == m_iPlayFlag)
    {
        m_iPlayFlag = 1;
        m_dPlaySpeed = 1.00;
        switch(player->state()) {
        case QMediaPlayer::PlayingState:
            player->pause();
            break;
        default:
            player->play();
            break;
        }
        player->setPlaybackRate(m_dPlaySpeed);
        ui->playSpeedLineEdit->setText(playSpeedStr);
    }
    else
    {
        player->pause();
        m_iPlayFlag = 0;
    }
    videoTime =  player->duration();
    setPlayButtonStyleSheet();

    int h,m,s;
    h=totalplaytime/3600;
    m=(totalplaytime-h*3600)/60;
    s=totalplaytime-h*3600-m*60;
    char acStr[32] = {0};

    snprintf(acStr, sizeof(acStr), "%02d", m);
    ui->rangeMinLabel->setText(QString(QLatin1String(acStr)));

    memset(acStr, 0, sizeof(acStr));
    snprintf(acStr, sizeof(acStr), "%02d", s);
    ui->rangeSecLabel->setText(QString(QLatin1String(acStr)));

//    m_playSlider->setValue(totalplaytime);



}

void recordPlayWidget::onTimerOut()
{
//    player->setPosition(m_playSlider->value()*player->duration()/maxValue);

}

void recordPlayWidget::recordPlayStopSlot()
{
    closePlayWin();
    setPlayButtonStyleSheet();

    if(player->state()!= QMediaPlayer::StoppedState)
    {
        m_iPlayFlag = 0;
        player->stop();
    }


}

void recordPlayWidget::closePlayWin()
{
    m_playSlider->setRange(0, 0);
    m_playSlider->setValue(0);

    ui->playMinLabel->setText("00");
    ui->playSecLabel->setText("00");
    ui->rangeMinLabel->setText("00");
    ui->rangeSecLabel->setText("00");
    ui->playSpeedLineEdit->setText("1.00x");

    m_iPlayFlag = 0;

}
void recordPlayWidget::recordPlayFastForwardSlot()
{
    QString playSpeedStr;

    if(player->state()!= QMediaPlayer::PlayingState)
    {
        return;
    }
    if (m_dPlaySpeed >= 4.00)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = m_dPlaySpeed*2;

    playSpeedStr = QString::number(m_dPlaySpeed);
    if (m_dPlaySpeed == (int)m_dPlaySpeed)
    {
        playSpeedStr += ".00x";
    }
    else
    {
        playSpeedStr += "x";
    }
    ui->playSpeedLineEdit->setText(playSpeedStr);
    player->setPlaybackRate(m_dPlaySpeed);
    setPlayButtonStyleSheet();


}
void recordPlayWidget::recordPlaySlowForwardSlot()
{
    QString playSpeedStr;
    if(player->state()!= QMediaPlayer::PlayingState)
    {
        return;
    }
    if (m_dPlaySpeed <= 1.25)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = m_dPlaySpeed/2;

    playSpeedStr = QString::number(m_dPlaySpeed);
    if (m_dPlaySpeed == (int)m_dPlaySpeed)
    {
        playSpeedStr += ".00x";
    }
    else
    {
        playSpeedStr += "x";
    }
    ui->playSpeedLineEdit->setText(playSpeedStr);
    player->setPlaybackRate(m_dPlaySpeed);
    setPlayButtonStyleSheet();
}

void recordPlayWidget::recordPlayLastOneSlot()
{





}
void recordPlayWidget::recordPlayNextOneSlot()
{




}
void recordPlayWidget::playPlusStepSlot()
{
    qint64 iPosTime = 0;
    QString playSpeedStr;
    if(player->state()!= QMediaPlayer::PlayingState)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = 1.00;
    playSpeedStr = "1.00x";
    ui->playSpeedLineEdit->setText(playSpeedStr);
    setPlayButtonStyleSheet();

    iPosTime = player->position() + 60;
    if (iPosTime > 0)
    {
        pthread_mutex_lock(&g_sliderValueSetMutex);
        m_playSlider->setValue(iPosTime);
        player->setPosition(iPosTime);
        pthread_mutex_unlock(&g_sliderValueSetMutex);

    }
    else
    {
        iPosTime = 1;
        pthread_mutex_lock(&g_sliderValueSetMutex);
        m_playSlider->setValue(iPosTime);
        player->setPosition(iPosTime);
        pthread_mutex_unlock(&g_sliderValueSetMutex);

    }

}

void recordPlayWidget::playMinusStepSlot()
{
    qint64 iPosTime = 0;
    QString playSpeedStr;
    if(player->state()!= QMediaPlayer::PlayingState)
    {
        return;
    }
    m_iPlayFlag = 1;
    m_dPlaySpeed = 1.00;
    playSpeedStr = "1.00x";
    ui->playSpeedLineEdit->setText(playSpeedStr);
    setPlayButtonStyleSheet();

    iPosTime = player->position() - 60;
    qDebug()<<"111playMinusStepSlot"<<iPosTime;
    if (iPosTime > 0)
    {
        pthread_mutex_lock(&g_sliderValueSetMutex);
        m_playSlider->setValue(iPosTime);
        player->setPosition(iPosTime);
        pthread_mutex_unlock(&g_sliderValueSetMutex);

    }
    else
    {
        iPosTime = 1;
        pthread_mutex_lock(&g_sliderValueSetMutex);
        m_playSlider->setValue(iPosTime);
        player->setPosition(iPosTime);
        pthread_mutex_unlock(&g_sliderValueSetMutex);

    }
}

void recordPlayWidget::registOutButtonClick()
{
    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型
}

void recordPlayWidget::carNoChangeSlot()   //车厢号切换信号响应槽函数
{
    int i = 0, idex = ui->carSeletionComboBox->currentIndex();    //获取当前车厢选择下拉框的索引
    QString item = "";
    T_TRAIN_CONFIG tTrainConfigInfo;
//    DebugPrint(DEBUG_UI_OPTION_PRINT, "recordPlayWidget change server carriage No!\n");

    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);
    ui->cameraSelectionComboBox->setCurrentIndex(-1);
    ui->cameraSelectionComboBox->clear();


    for (i = 0; i < tTrainConfigInfo.tNvrServerInfo[idex].iPvmsCameraNum; i++)        //根据不同车厢位置的NVR服务器的摄像机数量个数跟新摄像机选择下拉框
    {
        item = "";
        item = QString::number(8+i);
        item += tr("号摄像机");
        ui->cameraSelectionComboBox->addItem(item);
    }

}

void recordPlayWidget:: mousePressEvent(QMouseEvent *event)
{
    int x =event->x();
    int y = event->y();
    if(((90 < x < 190) && (50 < y < 80)) || ((90 < y < 190) && (50 < y < 80)))
    {
        Mouseflag = false;
    }
    else
    {
        Mouseflag = true;

    }

}

void recordPlayWidget::alarmPushButoonClickSlot()
{
    emit alarmPushButoonClickSignal();
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iRNum = 0;
}
void recordPlayWidget::alarmHappenCtrlSlot()
{
    if (this->isHidden() != 1)
    {
        if (0 == g_iRNum%2)
        {
            ui->alarmPushButton->setChecked(true);
        }
        else
        {
            ui->alarmPushButton->setChecked(false);
        }
        g_iRNum++;
    }
}
void recordPlayWidget::alarmHappenSlot()
{
    if (NULL == m_alarmHappenTimer)
    {
        m_alarmHappenTimer = new QTimer(this);
        connect(m_alarmHappenTimer,SIGNAL(timeout()), this,SLOT(alarmHappenCtrlSlot()));
        m_alarmHappenTimer->start(500);
    }
}

void recordPlayWidget::alarmClearSlot()
{
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iRNum = 0;
}
void recordPlayWidget::openStartTimeSetWidgetSlot()    //时间设置按钮按键响应槽函数，按键显示时间设置窗体，根据按钮不同，其出现的位置也有不同
{
#if 0
//    QString timeStr = ui->startTimeLabel->text();
    char acTimeStr[256] = {0};
    int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMin = 0, iSec = 0;

//	DebugPrint(DEBUG_UI_OPTION_PRINT, "recordPlayWidget startTimeSetPushButton pressed!\n");
    strcpy(acTimeStr, timeStr.toLatin1().data());
    if (strlen(acTimeStr) != 0)
    {
//		DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] timeStr:%s!\n", __FUNCTION__, acTimeStr);
        sscanf(acTimeStr, "%4d-%02d-%02d %02d:%02d:%02d", &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
//		DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] %d-%d-%d %d:%d:%d!\n", __FUNCTION__, iYear, iMonth, iDay, iHour, iMin, iSec);
    }

#if 0
    timeSetWidget->setGeometry(280, 50, timeSetWidget->width(), timeSetWidget->height());
    g_iDateEditNo = 1;
    timeSetWidget->se#define MAX_RECORD_SEACH_NUM 10000
        #define MAX_RECFILE_PATH_LEN 256
tTimeLabelText(iYear, iMonth, iDay, iHour, iMin, iSec);
    timeSetWidget->show();
#endif
#endif

}
void recordPlayWidget::openStopTimeSetWidgetSlot()    //时间设置按钮按键响应槽函数，按键显示时间设置窗体，根据按钮不同，其出现的位置也有不同
{
#if 0

//    QString timeStr = ui->endTimeLabel->text();
    char acTimeStr[256] = {0};
    int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMin = 0, iSec = 0;

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "recordPlayWidget stopTimeSetPushButton pressed!\n");
    strcpy(acTimeStr, timeStr.toLatin1().data());
    if (strlen(acTimeStr) != 0)
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] timeStr:%s!\n", __FUNCTION__, acTimeStr);
        sscanf(acTimeStr, "%4d-%02d-%02d %02d:%02d:%02d", &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec);
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] %d-%d-%d %d:%d:%d!\n", __FUNCTION__, iYear, iMonth, iDay, iHour, iMin, iSec);
    }
#if 0
    timeSetWidget->setGeometry(280, 90, timeSetWidget->width(), timeSetWidget->height());
    g_iDateEditNo = 2;
    timeSetWidget->setTimeLabelText(iYear, iMonth, iDay, iHour, iMin, iSec);
    timeSetWidget->show();
#endif
#endif

}
void recordPlayWidget::timeSetRecvMsg(QString year, QString month, QString day, QString hour, QString min, QString sec)     //响应时间设置控件信号，更新起始、结束时间显示label的显示文本
{
#if 0
    char timestr[128] = {0};
    snprintf(timestr, sizeof(timestr), "%s-%s-%s %s:%s:%s", year.toStdString().data(), month.toStdString().data(), day.toStdString().data(),
            hour.toStdString().data(), min.toStdString().data(), sec.toStdString().data());
//    QString string = QString(QLatin1String(timestr)) ;
//    if (1 == g_iDateEditNo)
//    {
//        ui->startTimeLabel->setText(string);
//    }
//    else if (2 == g_iDateEditNo)
//    {
//        ui->endTimeLabel->setText(string);
//    }
#endif
}
