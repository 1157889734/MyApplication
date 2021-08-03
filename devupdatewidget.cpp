#include "devupdatewidget.h"
#include "ui_devupdatewidget.h"
#include <QButtonGroup>
#include <QDebug>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include "stdio.h"
#include "log.h"


static int g_iVNum = 0;
#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面

#define NVR_RESTART_PORT 11001
QButtonGroup *g_buttonGroup1 = NULL, *g_buttonGroup2 = NULL, *g_buttonGroup3 = NULL;
devUpdateWidget *g_devUpdateThis = NULL;

char *parseFileNameFromPath(char *pcSrcStr)     //根据导入文件路径全名解析得到单纯的导入文件名
{
    char *pcTmp = NULL;

    if (NULL == pcSrcStr)
    {
        return NULL;
    }

    pcTmp = strrchr(pcSrcStr, '/');
    if (NULL == pcTmp)
    {
        return NULL;
    }

    if (NULL == (pcTmp+1))
    {
        return NULL;
    }
    return pcTmp+1;
}


devUpdateWidget::devUpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devUpdateWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setGeometry(0,0,1024,665);
    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    ui->alarmPushButton->setFocusPolicy(Qt::NoFocus);
    m_alarmHappenTimer = NULL;


//    usermanagePage = new userManage(this);
//    usermanagePage->setGeometry(245, 189, usermanagePage->width(), usermanagePage->height());
//    usermanagePage->hide();

//    connect(ui->permissonManagePushButton, SIGNAL(clicked(bool)), this, SLOT(userManageSlot()));


    connect(ui->configFilelookPushButton,   SIGNAL(clicked(bool)),   this, SLOT(configFileSelectionSlot()));

    connect(ui->configFilelookPushButton_2, SIGNAL(clicked(bool)), this, SLOT(configUpdateFileSLOT()));

    connect(ui->configFileImportPushButton, SIGNAL(clicked(bool)), this, SLOT(configFileImportSlot()));

    connect(ui->updateBeginPushButton, SIGNAL(clicked(bool)), this, SLOT(devUpdateSlot()));

    connect(ui->clientRebootPushButton, SIGNAL(clicked(bool)), this, SLOT(devRebootSlot()));


    g_buttonGroup1 = new QButtonGroup();      //轮询时间设置单选按钮组成一组，以保证改组中的单选框同时只能选一个，同时与以下其他类别的单选框之间互不影响
    g_buttonGroup1->addButton(ui->pollingTimeSetRadioButton,1);
    g_buttonGroup1->addButton(ui->pollingTimeSetRadioButton_2,2);
    g_buttonGroup1->addButton(ui->pollingTimeSetRadioButton_3,3);
    g_buttonGroup1->addButton(ui->pollingTimeSetRadioButton_4,4);

    g_buttonGroup2 = new QButtonGroup();     //预置点返回时间设置单选按钮组成一组，以保证改组中的单选框同时只能选一个，同时与其他类别的单选框之间互不影响
    g_buttonGroup2->addButton(ui->presetReturnTimeSetRadioButton,1);
    g_buttonGroup2->addButton(ui->presetReturnTimeSetRadioButton_2,2);
    g_buttonGroup2->addButton(ui->presetReturnTimeSetRadioButton_3,3);
    g_buttonGroup2->addButton(ui->presetReturnTimeSetRadioButton_4,4);

//    g_buttonGroup3 = new QButtonGroup();
//    g_buttonGroup3->addButton(ui->setManalTimeRadioButton,0);
//    g_buttonGroup3->addButton(ui->setSysTimeRadioButton,1);
    connect(ui->canselPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));

    connect(g_buttonGroup1, SIGNAL(buttonClicked(int)), this, SLOT(pollingTimeChange(int)));     //单选按钮组按键信号连接响应槽函数
    connect(g_buttonGroup2, SIGNAL(buttonClicked(int)), this, SLOT(presetReturnTimeChange(int)));

    connect(ui->timeSetPushButton,SIGNAL(clicked()),this,SLOT(monitorSysTime()));
    connect(ui->timeAdjustPushButton,SIGNAL(clicked()),this,SLOT(systimeSlot()));
    connect(ui->imageParamSetPushButton, SIGNAL(clicked(bool)), this, SLOT(setCameraImageParamSlot()));     //图像参数设置


    ui->pollingTimeSetLineEdit->setValidator(new QIntValidator(1,24*3600,this));   //只能输入1-24*3600的整数，不能输入字母或其他数字
    ui->presetReturnTimeSetLineEdit->setValidator(new QIntValidator(1,24*60,this));
    ui->brightnessLineEdit->setValidator(new QIntValidator(0,255,this));
    ui->saturationLineEdit->setValidator(new QIntValidator(0,255,this));
    ui->contrastLineEdit->setValidator(new QIntValidator(0,255,this));


    connect(ui->trainTypeSetPushButton, SIGNAL(clicked(bool)), this, SLOT(setTrainType()));


    m_sys_timer = new QTimer(this);
    connect(m_sys_timer,SIGNAL(timeout()),this,SLOT(showSysTime()));
    m_sys_timer->start(1000);

//    ui->sysDataAdjustLabel->setText();
    m_TrainType = "";

    setTrainTypeCombox();
    getTrainConfig();//获取车型配置信息，填充页面
    setPollingTimeRadioButton();  //设置轮询时间单选按钮组的样式
    setPresetReturnTimeRadioButton(); //设置预置点返回时间单选按钮组的样式


}

devUpdateWidget::~devUpdateWidget()
{
    delete ui;
}


void devUpdateWidget::registOutButtonClick()
{

//    m_pvmsMonitorPage->m_iPresetPasswdOkFlag = 0;
    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型

}

void devUpdateWidget::monitorSysTime()
{
    timeTd = QDateTime::currentDateTime();
    timeTd.setDate(ui->dateEdit->date());
    timeTd.setTime(ui->timeEdit->time());
    time_t tt = (time_t)timeTd.toTime_t();
    stime(&tt);


}

void devUpdateWidget::showSysTime()
{
    timeTd = QDateTime::currentDateTime();
    QString str_data = timeTd.toString("yyyy-MM-dd");
    QString str_time = timeTd.toString("hh:mm:ss");
    ui->sysDataAdjustLabel->setText(str_data);
    ui->systimeAdjustLabel->setText(str_time);

}

void devUpdateWidget::systimeSlot()
{



}

void devUpdateWidget::setTrainTypeCombox()     //读取系统配置文件，获取系统当前设置的车型，以使得车型设置下拉框显示的是当前系统车型
{
    T_TRAIN_TYPE_LIST tTrainTypeList;
    int iRet = 0, i = 0;

    memset(&tTrainTypeList, 0, sizeof(tTrainTypeList));
    iRet = STATE_GetTrainTypeList(&tTrainTypeList);
    if (iRet < 0)
    {
        return;
    }

    for(i = 0; i < tTrainTypeList.iTypeNum; i++)
    {
        ui->trainTypeSetComboBox->addItem(QString(QLatin1String(tTrainTypeList.acTrainTypeName[i])));
    }
}

void devUpdateWidget::getTrainConfig()
{
    int i = 0;
    QString item = "";

    ui->carriageSelectionComboBox->setCurrentIndex(-1);
    ui->carriageSelectionComboBox->clear();

    char acTrainType[16] = {0};
    T_TRAIN_CONFIG tTrainConfigInfo;
    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);
    STATE_GetCurrentTrainType(acTrainType, sizeof(acTrainType));

    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        item = "";
        item = QString::number(tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        item += tr("号车厢");
        ui->carriageSelectionComboBox->addItem(item);
        m_Phandle[i] = STATE_GetNvrServerPmsgHandle(i);
    }

    for (i = 0; i < ui->trainTypeSetComboBox->count(); i++)
    {
        if (ui->trainTypeSetComboBox->itemText(i) == QString(QLatin1String(acTrainType)))
        {
            ui->trainTypeSetComboBox->setCurrentIndex(i);
            m_TrainType = ui->trainTypeSetComboBox->currentText();
            break;
        }
    }
}

void devUpdateWidget::setPollingTimeRadioButton()     //获取轮询时间，设置轮询时间单选按钮组的样式
{
    int iPollingTime = 0;
    iPollingTime = STATE_GetPollingTime();

    ui->pollingTimeSetLineEdit->setText(QString::number(iPollingTime));
    m_pollingtTimeText = ui->pollingTimeSetLineEdit->text();

    if (10 == iPollingTime)   //轮询时间为10秒，则第一个表示10秒的RadioButton被选中
    {
        ui->pollingTimeSetRadioButton->setChecked(true);
        ui->pollingTimeSetLineEdit->setReadOnly(true);
    }
    else if (20 == iPollingTime)
    {
        ui->pollingTimeSetRadioButton_2->setChecked(true);
        ui->pollingTimeSetLineEdit->setReadOnly(true);
    }
    else if (30 == iPollingTime)
    {
        ui->pollingTimeSetRadioButton_3->setChecked(true);
        ui->pollingTimeSetLineEdit->setReadOnly(true);
    }
    else
    {
        ui->pollingTimeSetRadioButton_4->setChecked(true);
        ui->pollingTimeSetLineEdit->setReadOnly(false);
    }
}

void devUpdateWidget::setPresetReturnTimeRadioButton()     //获取预置点返回时间，设置预置点返回时间单选按钮组的样式
{
    int iReturnTime = 0;
    iReturnTime = STATE_GetPresetReturnTime();

    ui->presetReturnTimeSetLineEdit->setText(QString::number(iReturnTime));
    m_presetReturnTimeText = ui->presetReturnTimeSetLineEdit->text();

    if (5 == iReturnTime)   //预置点返回时间为5分钟，则第一个表示5分钟的RadioButton被选中
    {
        ui->presetReturnTimeSetRadioButton->setChecked(true);
        ui->presetReturnTimeSetLineEdit->setReadOnly(true);
    }
    else if (10 == iReturnTime)
    {
        ui->presetReturnTimeSetRadioButton_2->setChecked(true);
        ui->presetReturnTimeSetLineEdit->setReadOnly(true);
    }
    else if (15 == iReturnTime)
    {
        ui->presetReturnTimeSetRadioButton_3->setChecked(true);
        ui->presetReturnTimeSetLineEdit->setReadOnly(true);
    }
    else
    {
        ui->presetReturnTimeSetRadioButton_4->setChecked(true);
        ui->presetReturnTimeSetLineEdit->setReadOnly(false);
    }
}


void devUpdateWidget::setTrainType()
{
    char acUserType[16] = {0}, acTrainType[128] = {0};
    int iRet = 0;
    T_LOG_INFO tLogInfo;

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget trainTypeSet button pressed!\n");

    STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
    if (!strcmp(acUserType, "operator"))   //操作员不能设置车型
    {
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to set train type!\n");
        QMessageBox box(QMessageBox::Warning,QString::fromUtf8("提示"),QString::fromUtf8("无权限设置!"));     //新建消息提示框，提示错误信息
        box.setStandardButtons (QMessageBox::Ok);   //设置提示框只有一个标准按钮
        box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));     //将按钮显示改成"确 定"
        box.exec();
    }
    else
    {
        if (m_TrainType != ui->trainTypeSetComboBox->currentText())   //只有当车型选择被改变才进行设置
        {
//            DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget set train type will reboot client, confirm?\n");
            QMessageBox msgBox(QMessageBox::Question,QString(tr("提示")),QString(tr("将重启使车型设置生效，是否继续？")));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.button(QMessageBox::Yes)->setText("确 定");
            msgBox.button(QMessageBox::No)->setText("取 消");
            iRet=msgBox.exec();
            if(iRet != QMessageBox::Yes)
            {
                return;
            }
            snprintf(acTrainType, sizeof(acTrainType), "%s", ui->trainTypeSetComboBox->currentText().toLatin1().data());
            STATE_SetCurrentTrainType(acTrainType);
//            DebugPrint(DEBUG_UI_NOMAL_PRINT, "[%s] set currernt traintype to %s\n", __FUNCTION__, acTrainType);

            memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
            tLogInfo.iLogType = 0;
            snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "change traintype to %s and monitor Client reboot!", acTrainType);
            LOG_WriteLog(&tLogInfo);

            QApplication *app;
            app->exit();
        }
    }
}

void devUpdateWidget::setCameraImageParamSlot()
{
    int idex = 0, iRet = 0;
    char acUserType[64] = {0};
    T_PIC_ATTRIBUTE picParam;
    STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
    if (!strcmp(acUserType, "operator"))	 //操作员无权校时
    {
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to set CameraImage Param!\n");
        QMessageBox box(QMessageBox::Warning,tr("提示"),tr("无权限设置!"));	  //新建消息提示框，提示错误信息
        box.setStandardButtons (QMessageBox::Ok);	//设置提示框只有一个标准按钮
        box.setButtonText (QMessageBox::Ok,tr("确 定")); 	//将按钮显示改成"确 定"
        box.exec();
    }
    else
    {
        idex = ui->carriageSelectionComboBox->currentIndex();
        if (idex < 0)
        {
            return;
        }

        memset(&picParam, 0, sizeof(T_PIC_ATTRIBUTE));
        picParam.iBrightness = ui->brightnessLineEdit->text().toInt();
        picParam.iSaturation = ui->saturationLineEdit->text().toInt();
        picParam.iContrast = ui->contrastLineEdit->text().toInt();


        iRet = PMSG_SendPmsgData(m_Phandle[idex], CLI_SERV_MSG_TYPE_SET_PIC_ATTRIBUTE, (char *)&picParam, sizeof(T_PIC_ATTRIBUTE));    //发送设置图像效果参数命令
        if (iRet < 0)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "[%s] PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_PIC_ATTRIBUTE error!iRet=%d,server=%d\n", __FUNCTION__, iRet, idex+1);
        }
        QMessageBox box(QMessageBox::Information,QString::fromUtf8("注意"),QString::fromUtf8("图像参数设置成功!"));
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));
        box.exec();

    }
}

void devUpdateWidget::userManageSlot()  //点击用户管理按钮响应函数，弹出用户管理界面
{
//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget userManage set button pressed!\n");
#if 0
    if (usermanagePage == NULL)
    {
        usermanagePage = new userManage(this);
        usermanagePage->setGeometry(245, 189, usermanagePage->width(), usermanagePage->height());
        usermanagePage->show();
        connect(usermanagePage, SIGNAL(SendEscape()), this, SLOT(closeUserManageWidget()));
    }
    else
    {
        usermanagePage->show();
    }
#endif
}


void devUpdateWidget::configFileSelectionSlot()
{
    QString filename = "";
    char acUserType[64] = {0};

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget configFileSelection button pressed!\n");

        STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
        if (!strcmp(acUserType, "operator"))	 //操作员无权校时
        {
//            DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to select config file!\n");
            QMessageBox box(QMessageBox::Warning,tr("提示"),tr("无权限设置!"));	  //新建消息提示框，提示错误信息
            box.setStandardButtons (QMessageBox::Ok);	//设置提示框只有一个标准按钮
            box.setButtonText (QMessageBox::Ok,tr("确 定")); 	//将按钮显示改成"确 定"
            box.exec();
        }
        else
        {
            if (access("//mnt/sdcard/", F_OK) < 0)
            {
//                DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget::%s %d not get USB device!\n",__FUNCTION__,__LINE__);
                QMessageBox msgBox(QMessageBox::Warning,QString(tr("注意")),QString(tr("未检测到U盘,请插入!")));
                msgBox.setStandardButtons(QMessageBox::Yes);
                msgBox.button(QMessageBox::Yes)->setText("确 定");
                msgBox.exec();
                ui->clientRebootPushButton->setEnabled(true);
                return;
            }
            else
            {
                if (0 == STATE_FindUsbDev())   //这里处理一个特殊情况:U盘拔掉时umount失败，/mnt/usb/u/路径还存在，但是实际U盘是没有再插上的
                {
//                    DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget::%s %d not get USB device!\n",__FUNCTION__,__LINE__);
                    QMessageBox msgBox(QMessageBox::Warning,QString(tr("注意")),QString(tr("未检测到U盘,请插入!")));
                    msgBox.setStandardButtons(QMessageBox::Yes);
                    msgBox.button(QMessageBox::Yes)->setText("确 定");
                    msgBox.exec();
                    ui->clientRebootPushButton->setEnabled(true);
                    return;
                }
            }

            if (STATE_ParseUsbLicense("/mnt/sdcard/") < 0)
            {
//                DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget configFileSelection check License error!\n");
                QMessageBox box(QMessageBox::Warning,QString::fromUtf8("错误"),QString::fromUtf8("授权失败!"));
                box.setStandardButtons (QMessageBox::Ok);
                box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));
                box.exec();
                return;
            }

            filename = QFileDialog::getOpenFileName(this, "打开文件", "/mnt/sdcard/", "ini文件(*.ini)");
            if (!filename.isNull())
            {
                //QMessageBox::information(this, "Document", "Has document", QMessageBox::Ok | QMessageBox::Cancel);
                ui->configFileDisplayLineEdit->setText(filename);
            }
        }


}

void devUpdateWidget::configUpdateFileSLOT()
{

    QString filename = "";
    char acUserType[64] = {0};

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget configFileSelection button pressed!\n");

        STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
        if (!strcmp(acUserType, "operator"))	 //操作员无权校时
        {
//            DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to select config file!\n");
            QMessageBox box(QMessageBox::Warning,tr("提示"),tr("无权限设置!"));	  //新建消息提示框，提示错误信息
            box.setStandardButtons (QMessageBox::Ok);	//设置提示框只有一个标准按钮
            box.setButtonText (QMessageBox::Ok,tr("确 定")); 	//将按钮显示改成"确 定"
            box.exec();
        }
        else
        {
            if (access("//mnt/sdcard/", F_OK) < 0)
            {
//                DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget::%s %d not get USB device!\n",__FUNCTION__,__LINE__);
                QMessageBox msgBox(QMessageBox::Warning,QString(tr("注意")),QString(tr("未检测到U盘,请插入!")));
                msgBox.setStandardButtons(QMessageBox::Yes);
                msgBox.button(QMessageBox::Yes)->setText("确 定");
                msgBox.exec();
                ui->clientRebootPushButton->setEnabled(true);
                return;
            }
            else
            {
                if (0 == STATE_FindUsbDev())   //这里处理一个特殊情况:U盘拔掉时umount失败，/mnt/usb/u/路径还存在，但是实际U盘是没有再插上的
                {
//                    DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget::%s %d not get USB device!\n",__FUNCTION__,__LINE__);
                    QMessageBox msgBox(QMessageBox::Warning,QString(tr("注意")),QString(tr("未检测到U盘,请插入!")));
                    msgBox.setStandardButtons(QMessageBox::Yes);
                    msgBox.button(QMessageBox::Yes)->setText("确 定");
                    msgBox.exec();
                    ui->clientRebootPushButton->setEnabled(true);
                    return;
                }
            }

            if (STATE_ParseUsbLicense("/mnt/sdcard/") < 0)
            {
//                DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget configFileSelection check License error!\n");
                QMessageBox box(QMessageBox::Warning,QString::fromUtf8("错误"),QString::fromUtf8("授权失败!"));
                box.setStandardButtons (QMessageBox::Ok);
                box.setButtonText (QMessageBox::Ok,QString::fromUtf8("确 定"));
                box.exec();
                return;
            }

            filename = QFileDialog::getOpenFileName(this, "打开文件", "/mnt/sdcard/", "ini文件(*.bin)");
            if (!filename.isNull())
            {
                //QMessageBox::information(this, "Document", "Has document", QMessageBox::Ok | QMessageBox::Cancel);
                ui->configFileDisplayLineEdit_2->setText(filename);
            }
        }


}
void devUpdateWidget::devUpdateSlot()
{




}
void devUpdateWidget::devRebootSlot()
{
    char acUserType[64] = {0};
    T_LOG_INFO tLogInfo;

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget client reboot!\n");

    STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
    if (!strcmp(acUserType, "operator"))	 //操作员无权校时
    {
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to reboot client!\n");
        QMessageBox box(QMessageBox::Warning,tr("提示"),tr("无权限设置!"));	  //新建消息提示框，提示错误信息
        box.setStandardButtons (QMessageBox::Ok);	//设置提示框只有一个标准按钮
        box.setButtonText (QMessageBox::Ok,tr("确 定")); 	//将按钮显示改成"确 定"
        box.exec();
    }
    else
    {
        memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
        tLogInfo.iLogType = 0;
        snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "monitor Client reboot!");
        LOG_WriteLog(&tLogInfo);

        QApplication *app;
        app->exit();
    }


}
void devUpdateWidget::configFileImportSlot()
{

    int iRet = 0;
    char *pcfileName = NULL;
    char acUserType[64] = {0};

//    DebugPrint(DEBUG_UI_OPTION_PRINT, "devUpdateWidget configFileImport button pressed!\n");

    STATE_GetCurrentUserType(acUserType, sizeof(acUserType));
    if (!strcmp(acUserType, "operator"))	 //操作员无权校时
    {
//        DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget this user type has no right to import config file!\n");
        QMessageBox box(QMessageBox::Warning,tr("提示"),tr("无权限设置!"));	  //新建消息提示框，提示错误信息
        box.setStandardButtons (QMessageBox::Ok);	//设置提示框只有一个标准按钮
        box.setButtonText (QMessageBox::Ok,tr("确 定")); 	//将按钮显示改成"确 定"
        box.exec();
    }
    else
    {
        if (0 == strlen(ui->configFileDisplayLineEdit->text().toLatin1().data()))
        {
//            DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget not select any config file!\n");
            QMessageBox msgBox(QMessageBox::Question,QString(tr("注意")),QString(tr("请选择配置文件")));
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.button(QMessageBox::Yes)->setText("确 定");
            msgBox.exec();
            return;
        }

        pcfileName = parseFileNameFromPath(ui->configFileDisplayLineEdit->text().toLatin1().data());
        if (NULL == pcfileName)
        {
            return;
        }

        if (strncmp(pcfileName, "Station.ini", strlen(pcfileName)) != 0)
        {
//            DebugPrint(DEBUG_UI_MESSAGE_PRINT, "devUpdateWidget select error config file!\n");
            QMessageBox msgBox(QMessageBox::Question,QString(tr("注意")),QString(tr("配置文件选择错误")));
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.button(QMessageBox::Yes)->setText("确 定");
            msgBox.exec();
            return;
        }

        QMessageBox msgBox(QMessageBox::Question,QString(tr("提示")),QString(tr("确认导入配置文件？")));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.button(QMessageBox::Yes)->setText("确 定");
        msgBox.button(QMessageBox::No)->setText("取 消");
        iRet=msgBox.exec();
        if(iRet != QMessageBox::Yes)
        {
            return;
        }

        system("cp /mnt/usb/u/Station.ini /home/data/emuVideoMornitorClient/Station.ini");
        system("sync");

        QMessageBox msgBox2(QMessageBox::Information,QString(tr("注意")),QString(tr("导入成功，请拔出U盘!")));
        msgBox2.setStandardButtons(QMessageBox::Yes);
        msgBox2.button(QMessageBox::Yes)->setText("确 定");
        msgBox2.exec();
        return;
    }




}

void devUpdateWidget::alarmPushButoonClickSlot()  //点击报警按钮的响应函数，删除报警定时器并恢复按钮为正常不闪烁样式
{
    emit alarmPushButoonClickSignal();
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iVNum = 0;
}
void devUpdateWidget::alarmHappenCtrlSlot()    //报警触发定时器处理函数，间隔的切换按钮样式，达到闪烁效果
{
    if (this->isHidden() != 1)
    {
        if (0 == g_iVNum%2)
        {
            ui->alarmPushButton->setChecked(true);
        }
        else
        {
            ui->alarmPushButton->setChecked(false);
        }
        g_iVNum++;
    }
}
void devUpdateWidget::alarmHappenSlot()    //报警触发的响应函数
{
    if (NULL == m_alarmHappenTimer)    //启动一个定时器，每500毫秒切换按钮样式，达到闪烁效果
    {
        m_alarmHappenTimer = new QTimer(this);
        connect(m_alarmHappenTimer,SIGNAL(timeout()), this,SLOT(alarmHappenCtrlSlot()));
        m_alarmHappenTimer->start(500);
    }
}
void devUpdateWidget::alarmClearSlot()     //报警清除的响应函数，删除报警定时器并恢复按钮为正常不闪烁样式
{
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iVNum = 0;
}


void devUpdateWidget::pollingTimeChange(int iComboBoxId)
{
    static int iOldId = 0;
    if (iComboBoxId != iOldId)
    {
        switch (iComboBoxId) //根据单选按钮组按键信号传入的单选按钮编号区分不同的单选按钮，做出不同的设置
        {
            case 1:
            {
                ui->pollingTimeSetLineEdit->setReadOnly(true);   //非自定义单选按钮被选中时，轮询时间设置输入框设置为只读
                ui->pollingTimeSetLineEdit->setText("10");
                break;
            }
            case 2:
            {
                ui->pollingTimeSetLineEdit->setReadOnly(true);
                ui->pollingTimeSetLineEdit->setText("20");
                break;
            }
            case 3:
            {
                ui->pollingTimeSetLineEdit->setReadOnly(true);
                ui->pollingTimeSetLineEdit->setText("30");
                break;
            }
            case 4:
            {
                ui->pollingTimeSetLineEdit->setReadOnly(false);    //自定义单选按钮被选中时，轮询时间设置输入框设置为可读写
                break;
            }
            default:
                break;
        }
        iOldId = iComboBoxId;
    }
    m_pollingtTimeText = ui->pollingTimeSetLineEdit->text();
}

void devUpdateWidget::presetReturnTimeChange(int iComboBoxId)
{
    static int iOldId = 0;
    if (iComboBoxId != iOldId)
    {
        switch (iComboBoxId)  //根据单选按钮组按键信号传入的单选按钮编号区分不同的单选按钮，做出不同的设
        {
            case 1:
            {
                ui->presetReturnTimeSetLineEdit->setReadOnly(true);    //非自定义单选按钮被选中时，预置点返回时间设置输入框设置为只读
                ui->presetReturnTimeSetLineEdit->setText("5");
                break;
            }
            case 2:
            {
                ui->presetReturnTimeSetLineEdit->setReadOnly(true);
                ui->presetReturnTimeSetLineEdit->setText("10");
                break;
            }
            case 3:
            {
                ui->presetReturnTimeSetLineEdit->setReadOnly(true);
                ui->presetReturnTimeSetLineEdit->setText("15");
                break;
            }
            case 4:
            {
                ui->presetReturnTimeSetLineEdit->setReadOnly(false);   //自定义单选按钮被选中时，预置点返回时间设置输入框设置为可读写
                break;
            }
            default:
                break;
        }
        iOldId = iComboBoxId;
    }
    m_presetReturnTimeText = ui->presetReturnTimeSetLineEdit->text();
}
