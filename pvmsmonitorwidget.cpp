#include "pvmsmonitorwidget.h"
#include "ui_pvmsmonitorwidget.h"
#include <pthread.h>
#include <QDebug>
#include <sys/sysinfo.h>
#include <QMessageBox>
#include "log.h"
#include <QEvent>
#include <QMouseEvent>
static pthread_mutex_t g_tCmpCtrlMutex;



static int g_iPNum = 0;
QButtonGroup *g_buttonGroup = NULL;
#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面

typedef enum _E_CAMERA_SWITCH_STATE    //摄像机切换状态
{
    NORMAL = 0,    //正常，不切换
    LASTONE = 1,   //切换到上一个
    NEXTONE = 2    //切换到下一个
} E_CAMERA_SWITCH_STATE;





int PutNodeToCmpQueue(PT_CMP_QUEUE ptCmpQueue, PT_CMP_PACKET ptPkt)
{
    T_CMP_PACKET_LIST *ptPktList = NULL;

    if ((NULL == ptCmpQueue) || (NULL == ptPkt))
    {
        return -1;
    }
    ptPktList = (T_CMP_PACKET_LIST *)malloc(sizeof(T_CMP_PACKET_LIST));
    if (NULL == ptPktList)
    {
        return -1;
    }

    memset(ptPktList, 0, sizeof(T_CMP_PACKET_LIST));
    ptPktList->tPkt = *ptPkt;

    if (ptCmpQueue->pMutex)
    {
        pthread_mutex_lock(ptCmpQueue->pMutex);
    }

    if (NULL == ptCmpQueue->ptLast)
    {
        ptCmpQueue->ptFirst = ptPktList;
    }
    else
    {
        ptCmpQueue->ptLast->next = ptPktList;
    }
    ptCmpQueue->ptLast = ptPktList;
    ptCmpQueue->iPktCount++;

    if (ptCmpQueue->pMutex)
    {
        pthread_mutex_unlock(ptCmpQueue->pMutex);
    }

    return 0;
}



PT_CMP_QUEUE CreateCmpQueue(pthread_mutex_t *pMutex, INT32 iQueueType)
{
    PT_CMP_QUEUE ptCmpQueue = NULL;

    ptCmpQueue = (PT_CMP_QUEUE)malloc(sizeof(T_CMP_QUEUE));
    if (NULL == ptCmpQueue)
    {
        return NULL;
    }
    memset(ptCmpQueue, 0, sizeof(T_CMP_QUEUE));
    ptCmpQueue->pMutex = pMutex;
    ptCmpQueue->iQueueType = iQueueType;
    ptCmpQueue->ptLast = NULL;
    ptCmpQueue->ptFirst = NULL;
    ptCmpQueue->iPktCount= 0;

    return ptCmpQueue;
}

int DestroyCmpQueue(PT_CMP_QUEUE ptCmpQueue)
{
    T_CMP_PACKET_LIST *ptPktList = NULL, *ptTmp = NULL;

    if (NULL == ptCmpQueue)
    {
        return -1;
    }

    if (ptCmpQueue->pMutex)
    {
        pthread_mutex_lock(ptCmpQueue->pMutex);
    }

    ptPktList = ptCmpQueue->ptFirst;
    while (ptPktList)
    {
        ptTmp = ptPktList;
        ptPktList = ptPktList->next;
        free(ptTmp);
        ptTmp = NULL;
    }

    ptCmpQueue->ptLast = NULL;
    ptCmpQueue->ptFirst = NULL;
    ptCmpQueue->iPktCount= 0;

    if (ptCmpQueue->pMutex)
    {
        pthread_mutex_unlock(ptCmpQueue->pMutex);
    }

    free(ptCmpQueue);
    ptCmpQueue = NULL;

    return 0;
}








pvmsMonitorWidget::pvmsMonitorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pvmsMonitorWidget)
{
    ui->setupUi(this);
//    this->setWindowFlags(Qt::FramelessWindowHint);
    pthread_mutexattr_t	mutexattr;
    int i = 0;
//    /*将界面所有控件加入event事件过滤器进行监听，并设置鼠标移动能捕捉到，以便进行全屏检测*/
    ui->label_3->installEventFilter(this);  //加入事件过滤器
    ui->label_3->setMouseTracking(true);   //设置鼠标移动能捕捉到
    ui->label_4->installEventFilter(this);
    ui->label_4->setMouseTracking(true);
    ui->label_5->installEventFilter(this);
    ui->label_5->setMouseTracking(true);
    ui->label_6->installEventFilter(this);
    ui->label_6->setMouseTracking(true);
    ui->label_7->installEventFilter(this);
    ui->label_7->setMouseTracking(true);
    ui->label_8->installEventFilter(this);
    ui->label_8->setMouseTracking(true);
    ui->label_9->installEventFilter(this);
    ui->label_9->setMouseTracking(true);
    ui->ptzUpPushButton->installEventFilter(this);
    ui->ptzUpPushButton->setMouseTracking(true);
    ui->ptzDownPushButton->installEventFilter(this);
    ui->ptzDownPushButton->setMouseTracking(true);
    ui->ptzLeftPushButton->installEventFilter(this);
    ui->ptzLeftPushButton->setMouseTracking(true);
    ui->ptzRightPushButton->installEventFilter(this);
    ui->ptzRightPushButton->setMouseTracking(true);
    ui->zoomInPushButton->installEventFilter(this);
    ui->zoomInPushButton->setMouseTracking(true);
    ui->zoomOutPushButton->installEventFilter(this);
    ui->zoomOutPushButton->setMouseTracking(true);
    ui->focusFarPushButton->installEventFilter(this);
    ui->focusFarPushButton->setMouseTracking(true);
    ui->focusNearPushButton->installEventFilter(this);
    ui->focusNearPushButton->setMouseTracking(true);
    ui->presetSetPushButton->installEventFilter(this);
    ui->presetSetPushButton->setMouseTracking(true);
    ui->presetGetPushButton->installEventFilter(this);
    ui->presetGetPushButton->setMouseTracking(true);
    ui->cameraSwitchPushButton->installEventFilter(this);
    ui->cameraSwitchPushButton->setMouseTracking(true);
    ui->fillLightSwitchPushButton->installEventFilter(this);
    ui->fillLightSwitchPushButton->setMouseTracking(true);
    ui->startPollingPushButton->installEventFilter(this);
    ui->startPollingPushButton->setMouseTracking(true);
    ui->pausePollingPushButton->installEventFilter(this);
    ui->pausePollingPushButton->setMouseTracking(true);
    ui->pollingLastOnePushButton->installEventFilter(this);
    ui->pollingLastOnePushButton->setMouseTracking(true);
    ui->pollingNextOnePushButton->installEventFilter(this);
    ui->pollingNextOnePushButton->setMouseTracking(true);
    ui->temporarySavePushButton->installEventFilter(this);
    ui->temporarySavePushButton->setMouseTracking(true);
    ui->alarmPushButton->installEventFilter(this);
    ui->alarmPushButton->setMouseTracking(true);
    m_presetNoPushbutton[0] = ui->presetNoPushButton;
    m_presetNoPushbutton[1] = ui->presetNoPushButton_2;
    m_presetNoPushbutton[2] = ui->presetNoPushButton_3;
    m_presetNoPushbutton[3] = ui->presetNoPushButton_4;
    m_presetNoPushbutton[4] = ui->presetNoPushButton_5;
    m_presetNoPushbutton[5] = ui->presetNoPushButton_6;
    m_presetNoPushbutton[6] = ui->presetNoPushButton_7;
    m_presetNoPushbutton[7] = ui->presetNoPushButton_8;
    for (i = 0; i < 8; i++)
    {
        m_presetNoPushbutton[i]->installEventFilter(this);
        m_presetNoPushbutton[i]->setMouseTracking(true);
    }

    /*设置8个预置点按钮、预置点设置和调用按钮、预置点调焦和聚焦4个按钮、预置点方向控制4个按钮、摄像机开关按钮、补光灯开关按钮、轮询播放控制4个按钮的普通、鼠标移上等样式*/
    for (i = 0; i < 8; i++)
    {
        m_presetNoPushbutton[i]->setFocusPolicy(Qt::NoFocus); // 得到焦点时，不显示虚线框
    }
    ui->cameraSwitchPushButton->setChecked(true);
    ui->fillLightSwitchPushButton->setChecked(false);
    ui->startPollingPushButton->setChecked(true);
    ui->pausePollingPushButton->setChecked(false);

    ui->cameraSwitchPushButton->setFocusPolicy(Qt::NoFocus);
    ui->fillLightSwitchPushButton->setFocusPolicy(Qt::NoFocus);
    ui->presetSetPushButton->setFocusPolicy(Qt::NoFocus);
    ui->presetGetPushButton->setFocusPolicy(Qt::NoFocus);
    ui->zoomInPushButton->setFocusPolicy(Qt::NoFocus);
    ui->zoomOutPushButton->setFocusPolicy(Qt::NoFocus);
    ui->focusFarPushButton->setFocusPolicy(Qt::NoFocus);
    ui->focusNearPushButton->setFocusPolicy(Qt::NoFocus);
    ui->ptzUpPushButton->setFocusPolicy(Qt::NoFocus);
    ui->ptzDownPushButton->setFocusPolicy(Qt::NoFocus);
    ui->ptzLeftPushButton->setFocusPolicy(Qt::NoFocus);
    ui->ptzRightPushButton->setFocusPolicy(Qt::NoFocus);
    ui->startPollingPushButton->setFocusPolicy(Qt::NoFocus);
    ui->pausePollingPushButton->setFocusPolicy(Qt::NoFocus);
    ui->pollingLastOnePushButton->setFocusPolicy(Qt::NoFocus);
    ui->pollingNextOnePushButton->setFocusPolicy(Qt::NoFocus);
    ui->temporarySavePushButton->setFocusPolicy(Qt::NoFocus);
    ui->alarmPushButton->setFocusPolicy(Qt::NoFocus);

    g_buttonGroup = new QButtonGroup();
    g_buttonGroup->addButton(ui->presetNoPushButton,1);
    g_buttonGroup->addButton(ui->presetNoPushButton_2,2);
    g_buttonGroup->addButton(ui->presetNoPushButton_3,3);
    g_buttonGroup->addButton(ui->presetNoPushButton_4,4);
    g_buttonGroup->addButton(ui->presetNoPushButton_5,5);
    g_buttonGroup->addButton(ui->presetNoPushButton_6,6);
    g_buttonGroup->addButton(ui->presetNoPushButton_7,7);
    g_buttonGroup->addButton(ui->presetNoPushButton_8,8);

    connect(g_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(presetNoGroupButtonClickSlot(int)));     //预置点按钮组按键信号连接响应槽函数


    connect(ui->canselPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));

    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    connect(ui->startPollingPushButton, SIGNAL(clicked(bool)), this, SLOT(startPollingSlot()));   //播放按钮按键信号响应开启轮询
    connect(ui->pausePollingPushButton, SIGNAL(clicked(bool)), this, SLOT(pausePollingSlot()));   //暂停按钮按键信号响应暂停轮询
    connect(ui->pollingLastOnePushButton, SIGNAL(clicked(bool)), this, SLOT(manualSwitchLastCameraSlot()));
    connect(ui->pollingNextOnePushButton, SIGNAL(clicked(bool)), this, SLOT(manualSwitchNextCameraSlot()));
    connect(ui->presetSetPushButton, SIGNAL(clicked(bool)), this, SLOT(presetSetCtrlSlot()));    //预置点设置按钮按键信号连接响应槽函数
    connect(ui->presetGetPushButton, SIGNAL(clicked(bool)), this, SLOT(presetGetCtrlSlot()));    //预置点调用按钮按键信号连接响应槽函数
    connect(ui->temporarySavePushButton, SIGNAL(clicked(bool)), this, SLOT(temporarySaveBeginSlot()));
    connect(ui->cameraSwitchPushButton,  SIGNAL(clicked(bool)), this, SLOT(cameraSwitchSlot()));  //摄像头开关按钮按键信号连接响应槽函数
    connect(ui->fillLightSwitchPushButton,  SIGNAL(clicked(bool)), this, SLOT(fillLightSwitchSlot()));  //补光灯开关按钮按键信号连接响应槽函数



    connect(this, SIGNAL(videoPollingSignal()), this, SLOT(videoPollingSignalCtrl()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(setFullScreenSignal()), this, SLOT(setFullScreenSignalCtrl()));
    connect(this, SIGNAL(presetReturnSignal(int)), this, SLOT(presetReturnSignalCtrl(int)));
    connect(this, SIGNAL(recordPlayCtrlSignal()), this, SLOT(recordPlayCtrlSlot()));
    connect(this, SIGNAL(cmpOptionCtrlSignal(int, int)), this, SLOT(cmpOptionCtrlSlot(int, int)), Qt::BlockingQueuedConnection );
    connect(this, SIGNAL(chLabelDisplayCtrlSignal()), this, SLOT(chLabelDisplayCtrlSlot()));
    connect(this, SIGNAL(chStateLabelTextCtrlSignal(int)), this, SLOT(chStateLabelTextCtrlSlot(int)));
    connect(this, SIGNAL(camSwitchButtonTextCtrlSignal(int)), this, SLOT(camSwitchButtonTextCtrlSlot(int)));
    connect(this, SIGNAL(fillLightSwitchButtonTextCtrlSignal(int)), this, SLOT(fillLightSwitchButtonTextCtrlSlot(int)));


    //参数初始化
    m_alarmHappenTimer = NULL;
    m_manualSwitchTimer = NULL;
    m_fillLightSwitchTimer = NULL;
    m_cameraSwitchTimer = NULL;
    m_temporarySaveTimer = NULL;
    m_iCameraSwitchState = NORMAL;   //摄像头切换状态默认为正常，表示不切换
    m_iPresetPasswdOkFlag = 0;
    m_presetPasswdConfirmPage = NULL;
    m_iCameraNum = 0;
    m_iCameraPlayNo = 0;
    m_iPollingFlag = 1;   //默认轮询开启
    m_iSelectPresetNo = 1;  //预置点选中编号默认为1
    m_iPtzMoveType = E_STOP_MOVE;
    m_iRecordPlayFlag = 0;
    m_iFullScreenFlag = 0;
    m_iAlarmNotCtrlFlag = 0;
    m_iBlackScreenFlag = 0;

    m_playWin = NULL;

    memset(m_tCameraInfo, 0, sizeof(T_CAMERA_INFO)*MAX_SERVER_NUM*MAX_CAMERA_OFSERVER);

    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_settype(&mutexattr,PTHREAD_MUTEX_TIMED_NP);
    pthread_mutex_init(&tMutex, &mutexattr);
    pthread_mutex_init(&g_tCmpCtrlMutex, &mutexattr);
    pthread_mutexattr_destroy(&mutexattr);
    m_ptQueue = CreateCmpQueue(&tMutex, 0);
}

void pvmsMonitorWidget::startVideoPolling()    //开启视频轮询的处理
{
#if 0
    m_iFullScreenFlag = 1;

    m_playWin = new QWidget(this->parentWidget());    //新建一个与目前窗体同属一个父窗体的播放子窗体，方便实现全屏
    m_playWin->setGeometry(0, 0, 1024, 768);      //设置窗体在父窗体中的位置，默认一开始为全屏
    m_playWin->show();  //默认显示
    m_playWin->setObjectName("m_playWin");
    m_playWin->setStyleSheet("#m_playWin{background-color: rgb(0, 0, 0);}");     //设置播放窗口背景色为黑色
    m_playWin->installEventFilter(this);     //播放窗体注册进事件过滤器
    m_playWin->setMouseTracking(true);

    m_channelStateLabel = new QLabel(this->parentWidget());
    m_channelStateLabel->setGeometry(452, 360, 121, 50);
    m_channelStateLabel->setStyleSheet("QLabel{color:rgb(255, 255, 255);font: 24pt;background-color: rgb(0, 0, 0);}");
    m_channelStateLabel->setAttribute(Qt::WA_TranslucentBackground, true); //设置控件背景透明
    m_channelStateLabel->show();

    m_channelNoLabel = new QLabel(this->parentWidget());
    m_channelNoLabel->setGeometry(20, 690, 65, 50);
    m_channelNoLabel->setStyleSheet("QLabel{color:rgb(255, 255, 255);font: 24pt;background-color: rgb(0, 0, 0);}");
    m_channelNoLabel->setAttribute(Qt::WA_TranslucentBackground, true);
    m_channelNoLabel->show();
#endif
}


void pvmsMonitorWidget::registOutButtonClick()
{

    m_iPresetPasswdOkFlag = 0;
    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型

}

void pvmsMonitorWidget::presetNoGroupButtonClickSlot(int iButtonNo)   //预置点编号按钮组按键信号响应槽函数，iButtonNo为按钮编号
{
    int i = 0;
    m_iSelectPresetNo = iButtonNo;   //保存选中的预置点编号
}
void pvmsMonitorWidget::alarmPushButoonClickSlot()
{
    emit alarmPushButoonClickSignal();
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    m_iAlarmNotCtrlFlag = 0;
    g_iPNum = 0;

}
void pvmsMonitorWidget::startPollingSlot()
{
    m_iPollingFlag = 1;

    /*开始轮询按钮按下，设置开始按钮为按下时样式，暂停按钮恢复正常样式*/
    ui->startPollingPushButton->setChecked(true);
    ui->pausePollingPushButton->setChecked(false);

    struct sysinfo s_info;
    sysinfo(&s_info);
    tPollingOparateTime = s_info.uptime;

}
void pvmsMonitorWidget::pausePollingSlot()
{
    m_iPollingFlag = 0;

    /*暂停轮询按钮按下，设置暂停按钮为按下时样式，开始按钮恢复正常样式*/
    ui->startPollingPushButton->setChecked(false);
    ui->pausePollingPushButton->setChecked(true);

    struct sysinfo s_info;
    sysinfo(&s_info);
    tPollingOparateTime = s_info.uptime;
}
void pvmsMonitorWidget::manualSwitchLastCameraSlot()
{
    m_iCameraSwitchState = LASTONE;

    ui->pollingLastOnePushButton->setEnabled(false);
    ui->pollingNextOnePushButton->setEnabled(false);
    if (NULL == m_manualSwitchTimer)
    {
        m_manualSwitchTimer = new QTimer(this);
    }
    m_manualSwitchTimer->start(1*1000);
    connect(m_manualSwitchTimer,SIGNAL(timeout()), this,SLOT(manualSwitchEndSlot()));

}
void pvmsMonitorWidget::manualSwitchNextCameraSlot()
{
    m_iCameraSwitchState = NEXTONE;

    ui->pollingLastOnePushButton->setEnabled(false);
    ui->pollingNextOnePushButton->setEnabled(false);
    if (NULL == m_manualSwitchTimer)
    {
        m_manualSwitchTimer = new QTimer(this);
    }
    m_manualSwitchTimer->start(1*1000);
    connect(m_manualSwitchTimer,SIGNAL(timeout()), this,SLOT(manualSwitchEndSlot()));

}
void pvmsMonitorWidget::manualSwitchEndSlot()
{
    ui->pollingLastOnePushButton->setEnabled(true);
    ui->pollingNextOnePushButton->setEnabled(true);

    if (m_manualSwitchTimer != NULL)
    {
        if (m_manualSwitchTimer->isActive())
        {
            m_manualSwitchTimer->stop();
        }
        delete m_manualSwitchTimer;
        m_manualSwitchTimer = NULL;
    }
}

void pvmsMonitorWidget::presetSetCtrlSlot()
{
    int iRet = 0;
    char acSendBuf[4] = {0};

    if (0 == m_iPresetPasswdOkFlag)
    {
        if (NULL == m_presetPasswdConfirmPage)
        {
            m_presetPasswdConfirmPage = new presetPasswdConfirm(this);  //新建一个确认密码的子窗体
        }
        m_presetPasswdConfirmPage->move(300, 270);
        m_presetPasswdConfirmPage->show();
        connect(m_presetPasswdConfirmPage, SIGNAL(sendCloseSignal()), this, SLOT(closePresetPasswdPageSlot()));  //密码验证不通过会受到close信号，closePresetPasswdPageSlot里只会关闭密码验证界面
        connect(m_presetPasswdConfirmPage, SIGNAL(sendOkSignal()), this, SLOT(setPresetSlot()));  //密码验证正确会受到OK信号，setPresetSlot里会关闭密码验证界面，并执行预置点设置操作

    }
    else
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "pvmsMonitorWidget preset set option ctrl!\n");

        /*发送调用或设置预置点的消息给服务器，消息内容为3个字节，第一个字节表示控制类型:设置还是调用预置点，第二个字节表示预置点号，第三个字节表示受电弓摄像机位置号*/
        acSendBuf[0] = E_PRESET_SET;
        acSendBuf[1] = m_iSelectPresetNo;
        acSendBuf[2] = this->m_tCameraInfo[m_iCameraPlayNo].iPosNO;      //发送消息的第3个字节表示受电弓摄像机位置号
        iRet = PMSG_SendPmsgData(this->m_tCameraInfo[m_iCameraPlayNo].phandle, CLI_SERV_MSG_TYPE_SET_PRESETS, acSendBuf, 3);    //发送预置点控制命令
        if (iRet < 0)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] camera %d send CLI_SERV_MSG_TYPE_SET_PRESETS failed,iRet=%d\n", __FUNCTION__, m_iCameraPlayNo, iRet);
        }
        struct sysinfo s_info;
        sysinfo(&s_info);
        this->m_tCameraInfo[m_iCameraPlayNo].tPtzOprateTime = s_info.uptime;
        this->m_tCameraInfo[m_iCameraPlayNo].iPresetNo = m_iSelectPresetNo;
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "pvmsMonitorWidget set preserNo %d success!\n", m_iSelectPresetNo);
        QMessageBox box(QMessageBox::Information,QString::fromUtf8("注意"),QString::fromUtf8("预置点设置成功!"));
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));
        box.exec();

    }

}
void pvmsMonitorWidget::presetGetCtrlSlot()
{
    int iRet = 0;
      char acSendBuf[4] = {0};

//      DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget preset get button pressed! PresetNo=%d\n", m_iSelectPresetNo);


      /*发送调用或设置预置点的消息给服务器，消息内容为3个字节，第一个字节表示控制类型:设置还是调用预置点，第二个字节表示预置点号，第三个字节表示受电弓摄像机位置号*/
      acSendBuf[0] = E_PRESET_GET;
      acSendBuf[1] = m_iSelectPresetNo;
      acSendBuf[2] = this->m_tCameraInfo[m_iCameraPlayNo].iPosNO;	  //发送消息的第3个字节表示受电弓摄像机位置号
      iRet = PMSG_SendPmsgData(this->m_tCameraInfo[m_iCameraPlayNo].phandle, CLI_SERV_MSG_TYPE_SET_PRESETS, acSendBuf, 3);	//发送预置点控制命令
      if (iRet < 0)
      {
//          DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_PRESETS error!iRet=%d, cameraNo=%d\n",__FUNCTION__,iRet, m_iCameraPlayNo);
      }
      struct sysinfo s_info;
      memset(&s_info,0,sizeof(s_info));
      sysinfo(&s_info);
      this->m_tCameraInfo[m_iCameraPlayNo].tPtzOprateTime = s_info.uptime;
      this->m_tCameraInfo[m_iCameraPlayNo].iPresetNo = m_iSelectPresetNo;

}
void pvmsMonitorWidget::temporarySaveBeginSlot()
{
    char acSendBuf[4] = {0};
    int iRet = 0, i = 0, j = 0;
    T_TRAIN_CONFIG tTrainConfigInfo;
    T_LOG_INFO tLogInfo;
    PMSG_HANDLE pmsgHandle = 0;

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget temporarySave button pressed!\n");

//    DebugPrint(DEBUG_UI_MESSAGE_PRINT, "pvmsMonitorWidget temporarySave confirm!\n");
    QMessageBox msgBox(QMessageBox::Information,QString(tr("提示")),QString(tr("是否立即保存!")));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("确 定");
    msgBox.button(QMessageBox::No)->setText("取 消");
    iRet = msgBox.exec();
    if(iRet != QMessageBox::Yes)
    {
//        DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget temporarySave cancle!\n");
        return;
    }
//    DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget temporarySave confirm!\n");

    ui->temporarySavePushButton->setText(tr("保存中"));
    ui->temporarySavePushButton->setEnabled(false);

    if (NULL == m_temporarySaveTimer)
    {
        m_temporarySaveTimer = new QTimer(this);
    }
    m_temporarySaveTimer->start(30*1000);
    connect(m_temporarySaveTimer,SIGNAL(timeout()), this,SLOT(temporarySaveEndSlot()));


    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);

    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] server%d's PvmsCameraNum=%d\n",__FUNCTION__,i, tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum);
        acSendBuf[0] = tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO;	  //发送消息的第1个字节表示受电弓服务器车厢号
        for (j = 0; j < tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum; j++)
        {
            acSendBuf[1] = 8+j;   //发送消息的第2个字节表示受电弓摄像机位置号
//            DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] camera %d, no=%d send CLI_SERV_MSG_TYPE_SEND_TEMPORARY_SAVE\n", __FUNCTION__, j, 8+j);
            pmsgHandle = STATE_GetNvrServerPmsgHandle(i);
            iRet = PMSG_SendPmsgData(pmsgHandle, CLI_SERV_MSG_TYPE_SEND_TEMPORARY_SAVE, acSendBuf, 2);    //发送临时存储命令
            if (iRet <= 0)
            {
//                DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] camera %d, no=%d send CLI_SERV_MSG_TYPE_SEND_TEMPORARY_SAVE failed\n", __FUNCTION__, j, 8+j);
            }
            else
            {
                memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
                tLogInfo.iLogType = 0;
                snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "camera %d.%d temporary save OK", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+j);
                LOG_WriteLog(&tLogInfo);
//                DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] camera %d, no=%d send CLI_SERV_MSG_TYPE_SEND_TEMPORARY_SAVE Ok\n", __FUNCTION__, j, 8+j);
            }
        }
    }

}
void pvmsMonitorWidget::cameraSwitchSlot()
{
    char acSendBuf[4] = {0};
        int iRet = 0, i = 0;
        int iFlag = 0;
        T_TRAIN_CONFIG tTrainConfigInfo;
        T_LOG_INFO tLogInfo;
        T_CMP_PACKET tPkt;

//        DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget cameraSwitch button pressed!\n");

        if (1 == m_iPollingFlag)
        {
            m_iPollingFlag = 0;   //如果开启了轮询，先将轮询标志置0，保证这一刻不轮询，以免造成m_iCameraPlayNo瞬间变化而导致以下操作针对的相机不匹配
            iFlag = 1;
        }

        if (CAMERA_ON == m_tCameraInfo[m_iCameraPlayNo].iCameraSwitchState)
        {
//            DebugPrint(DEBUG_UI_NOMAL_PRINT, "pvmsMonitorWidget close camera!\n");
            emit chStateLabelTextCtrlSignal(0);  //触发让通道状态标签显示文本的信号
            emit camSwitchButtonTextCtrlSignal(1);  //触发让摄像头开关按钮显示文本的信号

            acSendBuf[0] = 2; //操作类型为关闭摄像头
            m_tCameraInfo[m_iCameraPlayNo].iCameraSwitchState = CAMERA_OFF;

            tPkt.iMsgCmd = CMP_CMD_DESTORY_CH;
            tPkt.iCh = m_iCameraPlayNo;
            PutNodeToCmpQueue(m_ptQueue, &tPkt);


            /*把相机的状态切换成关闭状态时，同时需显示通道状态和通道号*/
            emit chLabelDisplayCtrlSignal();  //触发通道状态和通道号标签显示处理信号

        }
#if 0

        else
        {
//                DebugPrint(DEBUG_UI_NOMAL_PRINT, "pvmsMonitorWidget open camera!\n");
            emit chStateLabelTextCtrlSignal(1);  //触发让通道状态标签显示文本的信号
            emit camSwitchButtonTextCtrlSignal(0);  //触发让摄像头开关按钮显示文本的信号

            acSendBuf[0] = 1;  //操作类型为开启摄像头
            m_tCameraInfo[m_iCameraPlayNo].iCameraSwitchState = CAMERA_ON;

            if (0 == m_tCameraInfo[i].iCmpOpenFlag)
            {
                tPkt.iMsgCmd = CMP_CMD_CREATE_CH;
                tPkt.iCh = m_iCameraPlayNo;
                PutNodeToCmpQueue(m_ptQueue, &tPkt);

                tPkt.iMsgCmd = CMP_CMD_ENABLE_CH;
                tPkt.iCh = m_iCameraPlayNo;
                PutNodeToCmpQueue(m_ptQueue, &tPkt);
            }

            emit chLabelDisplayCtrlSignal();  //触发通道状态和通道号标签显示处理信号
        }

        acSendBuf[1] = this->m_tCameraInfo[m_iCameraPlayNo].iPosNO;      //发送消息的第2个字节表示受电弓摄像机位置号

        iRet = PMSG_SendPmsgData(this->m_tCameraInfo[m_iCameraPlayNo].phandle, CLI_SERV_MSG_TYPE_PVMS_IPC_CTRL, acSendBuf, 2);    //发送摄像头开关控制命令
        if (iRet < 0)
        {
//                DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_PVMS_IPC_CTRL error!iRet=%d, cameraNo=%d\n",__FUNCTION__,iRet, m_iCameraPlayNo);
        }
        else
        {
            memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
            STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);

            for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
            {
                if (m_tCameraInfo[m_iCameraPlayNo].phandle == STATE_GetNvrServerPmsgHandle(i))
                {
                    memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
                    tLogInfo.iLogType = 0;
                    if (1 == acSendBuf[0])
                    {
                        snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "open camera %d.%d stream", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+m_tCameraInfo[m_iCameraPlayNo].iPosNO-8);
                    }
                    else
                    {
                        snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "close camera %d.%d stream", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+m_tCameraInfo[m_iCameraPlayNo].iPosNO-8);
                    }
                    LOG_WriteLog(&tLogInfo);
                    break;
                }
            }

            ui->cameraSwitchPushButton->setEnabled(false);
            if (NULL == m_cameraSwitchTimer)
            {
                m_cameraSwitchTimer = new QTimer(this);
            }
            m_cameraSwitchTimer->start(2*1000);
            connect(m_cameraSwitchTimer,SIGNAL(timeout()), this,SLOT(cameraSwitchEndSlot()));
        }

        if (1 == iFlag)
        {
            m_iPollingFlag = 1;   //如果上面手动将轮询标志清0，这里需要恢复为1
        }
#endif
}
void pvmsMonitorWidget::fillLightSwitchSlot()
{
    char acSendBuf[4] = {0};
    int iRet = 0, i = 0;
    T_TRAIN_CONFIG tTrainConfigInfo;
    T_LOG_INFO tLogInfo;

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget fillLightSwitch button pressed!\n");


    /*发送开关补光灯的消息给服务器，消息内容为2个字节，第一个字节表示操作类型:开启还是关闭补光灯，第二个字节表示受电弓摄像机位置号*/
    if (FILLLIGHT_ON == m_tCameraInfo[m_iCameraPlayNo].iFillLightSwitchState)
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "pvmsMonitorWidget close fillLight!\n");
        emit fillLightSwitchButtonTextCtrlSignal(1);  //触发让补光灯开关按钮显示文本的信号

        acSendBuf[0] = 2;  //操作类型为关闭补光灯
        m_tCameraInfo[m_iCameraPlayNo].iFillLightSwitchState = FILLLIGHT_OFF;
    }
    else
    {
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "pvmsMonitorWidget open fillLight!\n");
        emit fillLightSwitchButtonTextCtrlSignal(0);  //触发让补光灯开关按钮显示文本的信号

        acSendBuf[0] = 1;  //操作类型为开启补光灯
        m_tCameraInfo[m_iCameraPlayNo].iFillLightSwitchState = FILLLIGHT_ON;
    }
    acSendBuf[1] = this->m_tCameraInfo[m_iCameraPlayNo].iPosNO;      //发送消息的第2个字节表示受电弓摄像机位置号
    iRet = PMSG_SendPmsgData(this->m_tCameraInfo[m_iCameraPlayNo].phandle, CLI_SERV_MSG_TYPE_PVMS_LIGHT_CTRL, acSendBuf, 2);    //发送补光灯开关控制命令
    if (iRet < 0)
    {
//        DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_PVMS_LIGHT_CTRL error!iRet=%d, cameraNo=%d\n",__FUNCTION__,iRet, m_iCameraPlayNo);
    }
    else
    {
        if (0 == m_tCameraInfo[m_iCameraPlayNo].iManualFillLightCtrlFlag)
        {
            m_tCameraInfo[m_iCameraPlayNo].iManualFillLightCtrlFlag = 1;
        }
        memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
        STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);

        for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
        {
            if (m_tCameraInfo[m_iCameraPlayNo].phandle == STATE_GetNvrServerPmsgHandle(i))
            {
                memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
                tLogInfo.iLogType = 0;
                if (1 == acSendBuf[0])
                {
                    snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "open camera %d.%d fillLight", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+m_tCameraInfo[m_iCameraPlayNo].iPosNO-8);
                }
                else
                {
                    snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "close camera %d.%d fillLight", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+m_tCameraInfo[m_iCameraPlayNo].iPosNO-8);
                }
                LOG_WriteLog(&tLogInfo);
                break;
            }
        }

        ui->fillLightSwitchPushButton->setEnabled(false);
        if (NULL == m_fillLightSwitchTimer)
        {
            m_fillLightSwitchTimer = new QTimer(this);
        }
        m_fillLightSwitchTimer->start(2*1000);
        connect(m_fillLightSwitchTimer,SIGNAL(timeout()), this,SLOT(fillLightSwitchEndSlot()));
    }

}

void pvmsMonitorWidget::fillLightSwitchEndSlot()
{
    ui->fillLightSwitchPushButton->setEnabled(true);

    if (m_fillLightSwitchTimer != NULL)
    {
        if (m_fillLightSwitchTimer->isActive())
        {
            m_fillLightSwitchTimer->stop();
        }
        delete m_fillLightSwitchTimer;
        m_fillLightSwitchTimer = NULL;
    }
}

void pvmsMonitorWidget::closePresetPasswdPageSlot()    //关闭预置点密码确认界面
{
    if(NULL==this->m_presetPasswdConfirmPage)
    {
        return;
    }
    else
    {
        delete this->m_presetPasswdConfirmPage;
    }
    this->m_presetPasswdConfirmPage=NULL;
}
void pvmsMonitorWidget::setPresetSlot()
{

    m_iPresetPasswdOkFlag = 1;

    if(NULL==this->m_presetPasswdConfirmPage)
    {
        return;
    }
    else
    {
        delete this->m_presetPasswdConfirmPage;
    }
    this->m_presetPasswdConfirmPage=NULL;


}

void pvmsMonitorWidget::getChStreamState(int iCh)
{
    if (iCh < 0)
    {
        return;
    }
    pthread_mutex_lock(&g_tCmpCtrlMutex);
//    m_tCameraInfo[iCh].iStreamState = CMP_GetStreamState(m_tCameraInfo[iCh].cmpHandle);     ????????
    pthread_mutex_unlock(&g_tCmpCtrlMutex);
}


void pvmsMonitorWidget::videoPollingSignalCtrl()
{
    QString ChannelNoStr = tr("通道");
    char acChNo[32] = {0};
    char acSendBuf[4] = {0};
    int i = 0, iLastCamaraNo = 0;
    T_CMP_PACKET tPkt;

//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] camera %d Polling Ctrl!\n", __FUNCTION__, m_iCameraPlayNo);

    /*只有全局使能情况下的当前摄像头使能显示，其他摄像头全部禁止显示*/
    for (i = 0; i < m_iCameraNum; i++)
    {
        tPkt.iCh = i;
        tPkt.iMsgCmd = CMP_CMD_DISABLE_CH;
        PutNodeToCmpQueue(m_ptQueue, &tPkt);
    }
    for (i = 0; i < m_iCameraNum; i++)
    {
        if ((1 == m_iDisplayEnable) && (i == m_iCameraPlayNo))
        {
            tPkt.iCh = i;
            tPkt.iMsgCmd = CMP_CMD_ENABLE_CH;
            PutNodeToCmpQueue(m_ptQueue, &tPkt);
        }
    }

    if (m_iPtzMoveType != E_STOP_MOVE)   //切换相机，检测之前是否有云台控制未停止，检测到了就手动停止
    {
        acSendBuf[0] = E_STOP_MOVE;
        acSendBuf[1] = m_iPtzCtrType;
        iLastCamaraNo = m_iCameraPlayNo-1;
        if (iLastCamaraNo < 0)
        {
            iLastCamaraNo = m_iCameraNum - 1;
        }
        acSendBuf[2] = m_tCameraInfo[iLastCamaraNo].iPosNO;      //发送消息的第3个字节表示受电弓摄像机位置号
        PMSG_SendPmsgData(m_tCameraInfo[iLastCamaraNo].phandle, CLI_SERV_MSG_TYPE_SET_PTZ, acSendBuf, 3);    //发送云台控制命令

        m_iPtzMoveType = E_STOP_MOVE;
    }

    #if 1
    /*设置对应预置点按钮为按下样式*/
    for (i = 1; i <= 8; i++)
    {
        if (i == m_tCameraInfo[m_iCameraPlayNo].iPresetNo)
        {
            m_presetNoPushbutton[i-1]->setChecked(true);
        }
    }
    m_iSelectPresetNo = m_tCameraInfo[m_iCameraPlayNo].iPresetNo;

    snprintf(acChNo, sizeof(acChNo), "%d", m_iCameraPlayNo + 1);
    ChannelNoStr += QString(QLatin1String(acChNo));
    m_channelNoLabel->setText(ChannelNoStr);

    /*检测当前摄像头码流状态(是否有码流)，状态为1(有流)则需要隐藏通道状态和通道号，否则需要显示*/
    emit chLabelDisplayCtrlSignal();  //触发通道状态和通道号标签显示处理信号

    if (CAMERA_ON == m_tCameraInfo[m_iCameraPlayNo].iCameraSwitchState)
    {
        emit chStateLabelTextCtrlSignal(1);  //触发让通道状态标签显示文本的信号
        emit camSwitchButtonTextCtrlSignal(0);  //触发让摄像头开关按钮显示文本的信号
    }
    else
    {
        emit chStateLabelTextCtrlSignal(0);  //触发让通道状态标签显示文本的信号
        emit camSwitchButtonTextCtrlSignal(1);  //触发让摄像头开关按钮显示文本的信号
    }

    if (FILLLIGHT_ON == m_tCameraInfo[m_iCameraPlayNo].iFillLightSwitchState)
    {
        emit fillLightSwitchButtonTextCtrlSignal(0);  //触发让补光灯开关按钮显示文本的信号
    }
    else
    {
        emit fillLightSwitchButtonTextCtrlSignal(1);  //触发让补光灯开关按钮显示文本的信号
    }
    #endif

    if (m_presetPasswdConfirmPage != NULL)    //摄像头切换了，如果弹出了预置点密码确认界面，则关闭，避免出现跨云台设置预置点的问题
    {
        delete m_presetPasswdConfirmPage;
        m_presetPasswdConfirmPage = NULL;
    }

    if (this->isHidden() != 1)
    {
//        QWSServer::sendKeyEvent(0x01000003, Qt::Key_Escape, Qt::NoModifier, true, false);  //发送一个模拟键盘ESC键，以免通道切换了还有messagebox在
    }

    videoChannelCtrl();   //通道视频开关处理
}

void pvmsMonitorWidget::setFullScreenSignalCtrl()
{
    T_CMP_PACKET tPkt;
#if 0
//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] fullScreen Ctrl!\n", __FUNCTION__);
    if ((this->isHidden() != 1)  && (m_iAlarmNotCtrlFlag != 1) && (m_iBlackScreenFlag != 1))    //当前未显示，不做全屏监视处理,有报警信息未处理也不做全屏监视处理,处于黑屏状态也不做全屏监视处理
    {
//        m_playWin->move(0, 0);
//        m_playWin->resize(1024, 768);

        tPkt.iMsgCmd = CMP_CMD_CHG_ALL_VIDEOWIN;
        tPkt.iCh = 0;
        PutNodeToCmpQueue(m_ptQueue, &tPkt);

        m_channelStateLabel->setGeometry(452, 360, 121, 50);
        m_channelNoLabel->setGeometry(20, 690, 65, 50);
        if (m_presetPasswdConfirmPage != NULL)
        {
            m_presetPasswdConfirmPage->hide();
#if 0   ////////
//            if ((m_presetPasswdConfirmPage->p_ipmethod != NULL) && (m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget != NULL))
//            {
//                m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget->hide();
//            }
#endif
        }
        emit hideAlarmWidgetSignal();
//        QWSServer::sendKeyEvent(0x01000003, Qt::Key_Escape, Qt::NoModifier, true, false);    //发送一个模拟键盘ESC键，让全屏下messagebox自动关闭
        m_iFullScreenFlag = 1;
    }
#endif

}

void pvmsMonitorWidget::presetReturnSignalCtrl(int iCameraNO)
{
    char acSendBuf[4] = {0};
    int iRet = 0;

//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] camera %d presetReturn Ctrl!\n", __FUNCTION__, iCameraNO);

    /*发送调用预置点1的消息给服务器，消息内容为3个字节，第一个字节表示控制类型:设置还是调用预置点，第二个字节表示预置点号，第三个字节表示受电弓摄像机位置号*/
    acSendBuf[0] = E_PRESET_GET;
    acSendBuf[1] = 1;
    acSendBuf[2] = m_tCameraInfo[iCameraNO].iPosNO;
    iRet = PMSG_SendPmsgData(m_tCameraInfo[iCameraNO].phandle, CLI_SERV_MSG_TYPE_SET_PRESETS, acSendBuf, 3);    //发送预置点控制命令
    if (iRet < 0)
    {
//        DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_PRESETS prestNo 1 error!iRet=%d!cameraNo=%d\n", __FUNCTION__, iRet, iCameraNO);
    }
    else
    {
        m_tCameraInfo[iCameraNO].iPresetNo = 1;
        m_iSelectPresetNo = 1;
    }
}


void pvmsMonitorWidget::recordPlayCtrlSlot()
{
    videoChannelCtrl();   //通道视频开关处理
}



void pvmsMonitorWidget::chLabelDisplayCtrlSlot()   //通道状态和通道号标签是否显示的处理函数
{
#if 0

    T_CMP_PACKET tPkt;

    if (1 == m_iDisplayEnable)
    {
        /*
        tPkt.iMsgCmd = CMP_CMD_GET_STREAM_STATE;
        tPkt.iCh = m_iCameraPlayNo;
        PutNodeToCmpQueue(m_ptQueue, &tPkt);
        */
        getChStreamState(m_iCameraPlayNo);

        if (1 == m_tCameraInfo[m_iCameraPlayNo].iStreamState)
        {
            if (CAMERA_ON == m_tCameraInfo[m_iCameraPlayNo].iCameraSwitchState)   //状态为1时需要进一步判断相机状态，来决定是否需要显示通道状态和通道号,相机状态为开启时隐藏，否则显示
            {
                m_channelStateLabel->hide();
                m_channelNoLabel->hide();
            }
            else
            {
                m_channelStateLabel->show();
                m_channelNoLabel->show();
            }
        }
        else
        {
            m_channelStateLabel->show();
            m_channelNoLabel->show();
        }
    }
    else  		//播放窗体全部禁止时，通道状态和通道号也需同时隐藏
    {
        m_channelStateLabel->hide();
        m_channelNoLabel->hide();
    }
#endif


}

void pvmsMonitorWidget::chStateLabelTextCtrlSlot(int iFlag)   //通道状态标签文本显示的处理函数，0-显示关闭，1-显示开启
{
    if (0 == iFlag)
    {
        m_channelStateLabel->setText(tr("摄像头关闭"));
    }
    else
    {
        m_channelStateLabel->setText(tr("摄像头开启"));
    }
}

void pvmsMonitorWidget::camSwitchButtonTextCtrlSlot(int iFlag)   //摄像机开关状态按钮文本显示的处理函数，0-显示关闭，1-显示开启
{
    if (0 == iFlag)
    {
        ui->cameraSwitchPushButton->setText(tr("摄像头关"));
        ui->cameraSwitchPushButton->setChecked(true);
    }
    else
    {
        ui->cameraSwitchPushButton->setText(tr("摄像头开"));
        ui->cameraSwitchPushButton->setChecked(false);
    }
}

void pvmsMonitorWidget::fillLightSwitchButtonTextCtrlSlot(int iFlag)  //补光灯开关状态按钮文本显示的处理函数，0-显示关闭，1-显示开启
{
    if (0 == iFlag)
    {
        ui->fillLightSwitchPushButton->setText(tr("补光灯关"));
        ui->fillLightSwitchPushButton->setChecked(true);
    }
    else
    {
        ui->fillLightSwitchPushButton->setText(tr("补光灯开"));
        ui->fillLightSwitchPushButton->setChecked(false);
    }
}


void pvmsMonitorWidget::videoChannelCtrl()
{
    int i = 0, iLastCamaraNo = 0, iNextCamaraNo = 0;
    T_CMP_PACKET tPkt;

//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] videoChannel Ctrl!,m_iCameraPlayNo=%d,recordPlay Flag=%d\n", __FUNCTION__, m_iCameraPlayNo,m_iRecordPlayFlag);

    /*根据是否在回放来决定打开那些相机的流。如果在回放，则只打开当前相机的流。如果没有在回放，则打开当前以及上一个和下一个三个相机的流*/
    iLastCamaraNo = m_iCameraPlayNo-1;
    if (iLastCamaraNo < 0)
    {
        iLastCamaraNo = m_iCameraNum - 1;
    }
    iNextCamaraNo = m_iCameraPlayNo+1;
    if (iNextCamaraNo > (m_iCameraNum-1))
    {
        iNextCamaraNo = 0;
    }
    for (i = 0; i < m_iCameraNum; i++)
    {
        if (1 == m_iRecordPlayFlag)
        {
            if (i == m_iCameraPlayNo)
            {
                if (0 == m_tCameraInfo[i].iCmpOpenFlag)
                {
                    tPkt.iMsgCmd = CMP_CMD_CREATE_CH;
                    tPkt.iCh = i;
                    PutNodeToCmpQueue(m_ptQueue, &tPkt);
                }
                else
                {
                    tPkt.iMsgCmd = CMP_CMD_DISABLE_CH;
                    tPkt.iCh = i;
                    PutNodeToCmpQueue(m_ptQueue, &tPkt);  //保证回放时当前实时通道也不使能，确保不存在回放和实时同时使能的情况
                }
            }
            else
            {
                if (1 == m_tCameraInfo[i].iCmpOpenFlag)
                {
                    tPkt.iMsgCmd = CMP_CMD_DESTORY_CH;
                    tPkt.iCh = i;
                    PutNodeToCmpQueue(m_ptQueue, &tPkt);
                }
            }
        }
        else
        {
            if ((i == m_iCameraPlayNo) || (i == iLastCamaraNo) || (i == iNextCamaraNo))
            {
                if (0 == m_tCameraInfo[i].iCmpOpenFlag)
                {
                    tPkt.iMsgCmd = CMP_CMD_CREATE_CH;
                    tPkt.iCh = i;
                    PutNodeToCmpQueue(m_ptQueue, &tPkt);
                }
            }
            else
            {
                if (1 == m_tCameraInfo[i].iCmpOpenFlag)
                {
                    tPkt.iMsgCmd = CMP_CMD_DESTORY_CH;
                    tPkt.iCh = i;
                    PutNodeToCmpQueue(m_ptQueue, &tPkt);
                }
            }
        }
    }
}


void pvmsMonitorWidget::closePlayWin()
{


    if (m_playWin != NULL)
    {
        delete m_playWin;
        m_playWin = NULL;
//        DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] delete playWin!\n", __FUNCTION__);
    }



}

void pvmsMonitorWidget::alarmHappenSlot()
{
    T_CMP_PACKET tPkt;
#if 0
    if ((1 == m_iFullScreenFlag) && (m_playWin != NULL))  //有报警发生时退出全屏
    {
        struct sysinfo s_info;
        sysinfo(&s_info);
        m_lastActionTime = s_info.uptime;  //更新最后一次操作计时
        m_playWin->move(6, 110);
        m_playWin->resize(782, 656);
        m_iFullScreenFlag = 0;

        tPkt.iMsgCmd = CMP_CMD_CHG_ALL_VIDEOWIN;
        tPkt.iCh = 0;
        PutNodeToCmpQueue(m_ptQueue, &tPkt);

 #if 0
        if (m_channelStateLabel != NULL)
        {
            m_channelStateLabel->setGeometry(320, 385, 121, 50);
        }
        if (m_channelNoLabel != NULL)
        {
            m_channelNoLabel->setGeometry(20, 690, 65, 50);
        }

#endif
//        if (m_presetPasswdConfirmPage != NULL)
//        {
//            m_presetPasswdConfirmPage->show();
//            if ((m_presetPasswdConfirmPage->p_ipmethod != NULL) && (m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget != NULL))
//            {
//                m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget->show();
//            }
 //       }
    }

    m_iAlarmNotCtrlFlag = 1;

    /*当报警触发时，启动一个定时器，每500ms刷新一下报警按钮的背景色，以达到报警按钮闪烁的效果*/
    if (NULL == m_alarmHappenTimer)
    {
        m_alarmHappenTimer = new QTimer(this);
        connect(m_alarmHappenTimer,SIGNAL(timeout()), this,SLOT(alarmHappenCtrlSlot()));
        m_alarmHappenTimer->start(500);
    }
#endif
}
void pvmsMonitorWidget::alarmClearSlot()
{
    /*删除样式刷新定时器，并恢复报警按钮样式为正常样式*/
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    m_iAlarmNotCtrlFlag = 0;
    g_iPNum = 0;
}



#if 0
bool pvmsMonitorWidget::eventFilter(QObject *target, QEvent *event)    //事件过滤器，过滤处理不同控件的不同事件
{

    int iRet = 0;
    T_CMP_PACKET tPkt;
    if (event->type()==QEvent::MouseButtonPress || event->type()==QEvent::MouseMove) //判断界面操作
    {
            //DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] a mousemove or movebuttonpress or a keypress is checked!\n", __FUNCTION__);
            if (event->type()==QEvent::MouseMove)
            {
                QMouseEvent *mEvent = (QMouseEvent *)event;

                if ((m_iMousePosX != mEvent->globalPos().x()) || (m_iMousePosY != mEvent->globalPos().y()))    //防止实际没动鼠标而系统生成了mouseMove事件
                {
                    m_iMousePosX = mEvent->globalPos().x();
                    m_iMousePosY = mEvent->globalPos().y();
                }
                else
                {
                    return true;
                }
            }

            struct sysinfo s_info;
            sysinfo(&s_info);
            m_lastActionTime = s_info.uptime;  //更新最后一次操作计时  //更新最后一次操作计时

            /*当播放窗体处于全屏状态时，再次单击退出全屏,全屏标志清0，并恢复播放窗体原始播放状态*/
            if ((1 == m_iFullScreenFlag) && (target == m_playWin) && (event->type()==QEvent::MouseButtonPress))
            {
//                DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget quit full screen!\n");
                QMouseEvent *mouseEvent=static_cast<QMouseEvent*>(event);
                if(mouseEvent->button()==Qt::RightButton)    //只响应鼠标左击
                {
                    return true;
                }

                m_iFullScreenFlag = 0;
                m_playWin->move(6, 110);
                m_playWin->resize(782, 656);



//                tPkt.iMsgCmd = CMP_CMD_CHG_ALL_VIDEOWIN;
//                tPkt.iCh = 0;
//                PutNodeToCmpQueue(m_ptQueue, &tPkt);

//                m_channelStateLabel->setGeometry(320, 385, 121, 50);
//                m_channelNoLabel->setGeometry(20, 690, 65, 50);
//                if (m_presetPasswdConfirmPage != NULL)
//                {
//                    m_presetPasswdConfirmPage->show();
//                    if ((m_presetPasswdConfirmPage->p_ipmethod != NULL) && (m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget != NULL))
//                    {
//                        m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget->show();
//                    }
//                }
//                emit showAlarmWidgetSignal();
            }
    }
    if (target == m_playWin)
    {
        if (event->type()==QEvent::MouseButtonDblClick && (m_iAlarmNotCtrlFlag != 1))   //双击全屏,但是如何有报警未处理也不全屏
        {
            if (0 == m_iFullScreenFlag)
            {
//                DebugPrint(DEBUG_UI_OPTION_PRINT, "pvmsMonitorWidget mouse double click to full screen!\n");
#if 0 ///////////????????????????????????????????
                m_playWin->move(0, 0);
                m_playWin->resize(1024, 768);
#endif

//                tPkt.iMsgCmd = CMP_CMD_CHG_ALL_VIDEOWIN;
//                tPkt.iCh = 0;
//                PutNodeToCmpQueue(m_ptQueue, &tPkt);

//                m_channelStateLabel->setGeometry(452, 360, 121, 50);
//                m_channelNoLabel->setGeometry(20, 690, 65, 50);
//                if (m_presetPasswdConfirmPage != NULL)
//                {
//                    m_presetPasswdConfirmPage->hide();
//                    if ((m_presetPasswdConfirmPage->p_ipmethod != NULL) && (m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget != NULL))
//                    {
//                        m_presetPasswdConfirmPage->p_ipmethod->p_inputwidget->hide();
//                    }
//               }
                m_iFullScreenFlag = 1;
            }
        }
    }
}
#endif

pvmsMonitorWidget::~pvmsMonitorWidget()
{
    delete ui;
}
