#include "devmanagewidget.h"
#include "ui_devmanagewidget.h"
#include <QMessageBox>
#include <QDateTime>
#include "log.h"
#include <stdio.h>
#include <netinet/in.h>
#include <QDebug>

static int g_iDNum = 0;

devManageWidget::devManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devManageWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->showFullScreen();


    ui->devStorageTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->devStorageTableWidget->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击
    ui->devStorageTableWidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->devStorageTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->devStorageTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->devStorageTableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->devStorageTableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(0,46); //设置表头第一列的宽度为46
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(1,136);
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(2,136);
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(3,136);
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(4,116);
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(5,116);
    ui->devStorageTableWidget->horizontalHeader()->resizeSection(6,111);


    ui->devStatusTableWidget->setFocusPolicy(Qt::NoFocus);
    ui->devStatusTableWidget->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击
    ui->devStatusTableWidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->devStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->devStatusTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->devStatusTableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->devStatusTableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(0,46); //设置表头第一列的宽度为46px
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(1,130);
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(2,130);
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(3,130);
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(4,100);
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(5,100);
    ui->devStatusTableWidget->horizontalHeader()->resizeSection(6,100);
    //ui->devStatusTableWidget->horizontalHeader()->resizeSection(7,61);

    ui->alarmPushButton->setFocusPolicy(Qt::NoFocus);
    ui->TrainNumberSetPushButton->setFocusPolicy(Qt::NoFocus);

    int i = 0, j = 0;

    for (i = 0; i < MAX_SERVER_NUM; i++)
    {
        m_aiServerIdex[i] = 0;
        m_NvrServerPhandle[i] = 0;
        m_iNoCheckDiskErrNum[i] = 0;
        m_iCheckDiskErrFlag[i] = 0;
        for (j = 0; j < MAX_CAMERA_OFSERVER; j++)
        {
            m_aiNvrOnlineFlag[i] = 0;
            m_aiCameraIdex[i][j] = 0;
            m_aiCameraOnlineFlag[i][j] = 0;
        }
    }
    m_TrainNumEditSave = "";

    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    connect(ui->TrainNumberSetPushButton, SIGNAL(clicked(bool)), this, SLOT(trainNumberButtonClickSlot()));

    connect(ui->TrainNumberLineEdit, SIGNAL(textChanged(QString)), this, SLOT(trainNumberChange(QString)));


    getTrainConfig();


    m_alarmHappenTimer = NULL;

}

devManageWidget::~devManageWidget()
{
    delete ui;
}
void devManageWidget::alarmPushButoonClickSlot()
{
    emit alarmPushButoonClickSignal();
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iDNum = 0;
}
void devManageWidget::alarmHappenSlot()
{
    if (NULL == m_alarmHappenTimer)
    {
        m_alarmHappenTimer = new QTimer(this);
        connect(m_alarmHappenTimer,SIGNAL(timeout()), this,SLOT(alarmHappenCtrlSlot()));
        m_alarmHappenTimer->start(500);
    }
}

void devManageWidget::alarmHappenCtrlSlot()
{
    if (this->isHidden() != 1)
    {
        if (0 == g_iDNum%2)
        {
            ui->alarmPushButton->setChecked(true);
        }
        else
        {
            ui->alarmPushButton->setChecked(false);
        }
        g_iDNum++;
    }
}

void devManageWidget::alarmClearSlot()
{
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iDNum = 0;
}
void devManageWidget::trainNumberButtonClickSlot()
{

    trainNumberSetSlot_fuction();

    ui->TrainNumberLineEdit->setEnabled(true);
    ui->TrainNumberLineEdit->setFocus();

    m_TrainNumEditSave = ui->TrainNumberLineEdit->text();


}

void devManageWidget::trainNumberChange(QString TrainNumberStr)
{
    char acTrainNumber[128] = {0};
    if (TrainNumberStr.length() > 7)
    {
        strncpy(acTrainNumber, TrainNumberStr.toLatin1().data(), 7);
        ui->TrainNumberLineEdit->setText(QString(QLatin1String(acTrainNumber)));
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devManageWidget input train number len can't over 7!\n");
        QMessageBox box(QMessageBox::Warning,QString::fromUtf8("提示"),QString::fromUtf8("输入的车次字符数不能超过7!"));     //提示框
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));
        box.exec();
    }
}

void devManageWidget::trainNumberSetSlot_fuction()
{

    int i = 0, j = 0, iRet = 0;
    short year = 0;
    char acTrainNumber[128] = {0};
    T_TRAIN_CONFIG tTrainConfigInfo;
    T_OSD_INFO tOsdInfo;
    T_LOG_INFO tLogInfo;
    memset(&tOsdInfo, 0, sizeof(T_OSD_INFO));
    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));

//    DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] send CLI_SERV_MSG_TYPE_SET_OSD msg to all server to set train number\n", __FUNCTION__);

    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);
    snprintf(acTrainNumber, sizeof(acTrainNumber), "%s", ui->TrainNumberLineEdit->text().toLatin1().data());
    STATE_SetTrainNumber(acTrainNumber);

    QDateTime time = QDateTime::currentDateTime();
    year = time.date().year();
    tOsdInfo.i16Year = htons(year);
    tOsdInfo.i8Mon = time.date().month();
    tOsdInfo.i8day = time.date().day();
    tOsdInfo.i8Hour = time.time().hour();
    tOsdInfo.i8Min = time.time().minute();
    tOsdInfo.i8Sec = time.time().second();
    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        for (j = 0; j < tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum; j++)
        {
            tOsdInfo.i8CarriageNo = tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO;
            tOsdInfo.i8PvmsCarriageNO = 8+j;
            strncpy((char *)tOsdInfo.acTrainNum, acTrainNumber, strlen(acTrainNumber));
            iRet = PMSG_SendPmsgData(m_NvrServerPhandle[i], CLI_SERV_MSG_TYPE_SET_OSD, (char *)&tOsdInfo, sizeof(T_OSD_INFO));
            if (iRet < 0)
            {
//                DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_OSD error!iRet=%d,server=%d\n", __FUNCTION__, iRet,i+1);
            }
            else
            {
                memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
                tLogInfo.iLogType = 0;
                snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), " camera %d.%d set train number to %s", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO, 200+j, acTrainNumber);
                LOG_WriteLog(&tLogInfo);
            }
        }
    }
//    emit enterKeyPressSignal();
    ui->TrainNumberLineEdit->setEnabled(false);




}

void devManageWidget::getTrainConfig()   //获取车型配置信息，填充页面
{
    int i = 0, j = 0, row = 0;
    QString item = "";
    QString devStatus = tr("离线");     //设备状态初始默认值为离线
    T_TRAIN_CONFIG tTrainConfigInfo;

    /*设备状态和设备存储列表清空*/
    row = ui->devStorageTableWidget->rowCount();
    for (i = 0; i < row; i++)
    {
        ui->devStorageTableWidget->removeRow(i);
    }
    ui->devStorageTableWidget->setRowCount(0);

    row = ui->devStatusTableWidget->rowCount();
    ui->devStorageTableWidget->setRowCount(0);
    for (i = 0; i < row; i++)
    {
        ui->devStatusTableWidget->removeRow(i);
    }
    ui->devStatusTableWidget->setRowCount(0);

    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);

    /*获取编组信息，再填充编组设置单选框*/
    if (1 == tTrainConfigInfo.iFormationType)
    {
        ui->groupSetRadioButton->setChecked(true);
        ui->groupSetRadioButton_2->setChecked(false);
    }
    else if (2 == tTrainConfigInfo.iFormationType)
    {
        ui->groupSetRadioButton->setChecked(false);
        ui->groupSetRadioButton_2->setChecked(true);
    }

    /*获取各服务器即摄像机信息，填充相应的列表控件*/
    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        m_NvrServerPhandle[i] = STATE_GetNvrServerPmsgHandle(i);
        row = ui->devStatusTableWidget->rowCount();//获取表格中当前总行数
        ui->devStatusTableWidget->insertRow(row);//添加一行
        m_aiServerIdex[i] = row+1;
        item = "";
        item = QString::number(row+1);
        ui->devStatusTableWidget->setItem(row, 0, new QTableWidgetItem(item));  //新建一个文本列并插入到列表中
        ui->devStatusTableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);    //设置列文件对齐方式为居中对齐
        ui->devStatusTableWidget->setItem(row, 1, new QTableWidgetItem(tr("受电弓监控服务器")));
        ui->devStatusTableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item = "";
        item = QString::number(tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += tr("车视频监控服务器");
        ui->devStatusTableWidget->setItem(row, 2, new QTableWidgetItem(item));
        ui->devStatusTableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item = "";
        item = "192.168.";
        item += QString::number(100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += ".81";
        ui->devStatusTableWidget->setItem(row, 3, new QTableWidgetItem(item));
        ui->devStatusTableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->devStatusTableWidget->setItem(row, 5, new QTableWidgetItem(devStatus));
        ui->devStatusTableWidget->item(row, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        for (j = 0; j < tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum; j++)
        {
            row = ui->devStatusTableWidget->rowCount();//获取表格中当前总行数
            ui->devStatusTableWidget->insertRow(row);//添加一行
            m_aiCameraIdex[i][j] = row+1;
            item = "";
            item = QString::number(row+1);
            ui->devStatusTableWidget->setItem(row, 0, new QTableWidgetItem(item));
            ui->devStatusTableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->devStatusTableWidget->setItem(row, 1, new QTableWidgetItem(tr("受电弓摄像机")));
            ui->devStatusTableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            item = "";
            item = QString::number(tTrainConfigInfo.tNvrServerInfo[i].iPvmsCarriageNO);
            item += tr("车");
            item += QString::number(j+1);
            item += "号相机";
            ui->devStatusTableWidget->setItem(row, 2, new QTableWidgetItem(item));
            ui->devStatusTableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            item = "";
            item = "192.168.";
            item += QString::number(100+tTrainConfigInfo.tNvrServerInfo[i].iPvmsCarriageNO);
            item += ".";
            item += QString::number(200+j);
            ui->devStatusTableWidget->setItem(row, 3, new QTableWidgetItem(item));
            ui->devStatusTableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->devStatusTableWidget->setItem(row, 5, new QTableWidgetItem(devStatus));
            ui->devStatusTableWidget->item(row, 5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }


        row = ui->devStorageTableWidget->rowCount();//获取表格中当前总行数
        ui->devStorageTableWidget->insertRow(row);//添加一行
        item = "";
        item = QString::number(row+1);
        ui->devStorageTableWidget->setItem(row, 0, new QTableWidgetItem(item));
        ui->devStorageTableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->devStorageTableWidget->setItem(row, 1, new QTableWidgetItem(tr("受电弓监控服务器")));
        ui->devStorageTableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item = "";
        item = QString::number(tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += tr("车视频监控服务器");
        ui->devStorageTableWidget->setItem(row, 2, new QTableWidgetItem(item));
        ui->devStorageTableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item = "";
        item = "192.168.";
        item += QString::number(100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += ".81";
        ui->devStorageTableWidget->setItem(row, 3, new QTableWidgetItem(item));
        ui->devStorageTableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}
