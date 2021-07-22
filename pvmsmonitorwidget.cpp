#include "pvmsmonitorwidget.h"
#include "ui_pvmsmonitorwidget.h"
#include <pthread.h>
#include <QDebug>

static int g_iPNum = 0;
QButtonGroup *g_buttonGroup = NULL;

pvmsMonitorWidget::pvmsMonitorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pvmsMonitorWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    pthread_mutexattr_t	mutexattr;
    int i = 0;

    /*将界面所有控件加入event事件过滤器进行监听，并设置鼠标移动能捕捉到，以便进行全屏检测*/
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


}
void pvmsMonitorWidget::pausePollingSlot()
{

}
void pvmsMonitorWidget::manualSwitchLastCameraSlot()
{


}
void pvmsMonitorWidget::manualSwitchNextCameraSlot()
{


}
void pvmsMonitorWidget::presetSetCtrlSlot()
{



}
void pvmsMonitorWidget::presetGetCtrlSlot()
{


}
void pvmsMonitorWidget::temporarySaveBeginSlot()
{


}
void pvmsMonitorWidget::cameraSwitchSlot()
{


}
void pvmsMonitorWidget::fillLightSwitchSlot()
{


}

pvmsMonitorWidget::~pvmsMonitorWidget()
{
    delete ui;
}
