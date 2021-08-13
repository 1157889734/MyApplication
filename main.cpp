#include "MyApplication.h"
#include <QApplication>
#include "choicelogindevwidget.h"
#include "loginwidget.h"
#include "pvmsmenuwidget.h"
#include <QtVirtualKeyboard>
#include <QPixmap>
#include <QSplashScreen>
#include <QTextCodec>
#include <QDebug>
#include <QSqlDatabase>
#include <QtPlugin>
#include "log.h"
#include <arpa/inet.h>



choiceLoginDevWidget *g_choiceLoginDevPage = NULL;    //选择登录设备页面
loginWidget *g_loginPage = NULL;   //登录页面
pvmsMenuWidget *g_pvmsMenuPage = NULL;   //受电弓监控主菜单页面
recordPlayWidget *g_recordPage = NULL;
pvmsMonitorWidget *g_monitorPage = NULL;

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication a(argc, argv);
    char acNvrServerIp[128] = {0}, acClientVersion[64] = {0};
    short sYear = 0;
    int iRet = 0,i = 0;
    T_LOG_INFO tLogInfo;
    T_TRAIN_CONFIG tTrainConfigInfo;
    T_TIME_INFO tTimeInfo;
    T_PIS_INFO tPisConfigInfo;
    T_PVMS_INFO tPvmsInfo;
    PMSG_HANDLE pmsgHandle = 0;
    PRS485_HANDLE pRs485Handle = 0;


    LOG_Init();    //本地日志模块初始化

    /*程序启动记录日志*/
    memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
    tLogInfo.iLogType = 0;
    STATE_GetSysVersion(acClientVersion, sizeof(acClientVersion));
    snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "Startup %s!", acClientVersion);
    LOG_WriteLog(&tLogInfo);

    PMSG_Init();    //协议通信模块初始化

    STATE_ReadTrainConfigFile();	//程序运行起来读取一次车型配置文件
    memset(&tTrainConfigInfo, 0, sizeof(T_TRAIN_CONFIG));
    STATE_GetCurrentTrainConfigInfo(&tTrainConfigInfo);
    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        memset(acNvrServerIp, 0, sizeof(acNvrServerIp));
        snprintf(acNvrServerIp, sizeof(acNvrServerIp), "192.168.%d.81", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO);
        iRet = PMSG_CreateConnect(acNvrServerIp, 10100);
//        qDebug()<<"********PMSG_CreateConnect--:"<<iRet<<endl;
        qDebug()<<"********PMSG_CreateConnect--acNvrServerIp:"<<acNvrServerIp;
        printf("********PMSG_CreateConnect--%d\n",iRet);
        if (0 == iRet)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "create connection to server:%s error!\n",acNvrServerIp);
            qDebug()<<"DEBUG_UI_ERROR_PRINT create connection to server eroor"<<__FUNCTION__<<__LINE__<<acNvrServerIp<<endl;
            continue;
        }
        if (STATE_SetNvrServerPmsgHandle(i, (PMSG_HANDLE)iRet) < 0)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "save server:%s pmsg handle error!\n",acNvrServerIp);
            qDebug()<<"DEBUG_UI_ERROR_PRINT,save server pmsg handle error"<<__FUNCTION__<<__LINE__<<acNvrServerIp<<endl;
        }
    }

    STATE_ReadPisConfig();
    memset(&tPisConfigInfo, 0, sizeof(T_PIS_INFO));
    STATE_GetPisConfigInfo(&tPisConfigInfo);
    iRet = PMSG_CreateConnect(tPisConfigInfo.acIpAddr, tPisConfigInfo.iPort);
    if (0 == iRet)
    {
//        DebugPrint(DEBUG_UI_ERROR_PRINT, "create connection to server:%s error!\n",acNvrServerIp);
        qDebug()<<"create connection to server:%s error"<<__FUNCTION__<<__LINE__<<acNvrServerIp<<endl;
    }
    if (STATE_SetPisPmsgHandle((PMSG_HANDLE)iRet) < 0)
    {
//        DebugPrint(DEBUG_UI_ERROR_PRINT, "save pis server pmsg handle error!\n");
        qDebug()<<"save pis server pmsg handle error"<<__FUNCTION__<<__LINE__<<endl;

    }

    pRs485Handle = RS485_CreateConnect();
    if (0 == pRs485Handle)
    {
//        DebugPrint(DEBUG_UI_ERROR_PRINT, "rs485 connection error!\n");
        qDebug()<<"rs485 connection error!"<<__FUNCTION__<<__LINE__<<endl;

    }

    usleep(1*1000*1000);

    QDateTime time = QDateTime::currentDateTime();
    sYear = time.date().year();
    tTimeInfo.year = htons(sYear);
    tTimeInfo.mon = time.date().month();
    tTimeInfo.day = time.date().day();
    tTimeInfo.hour = time.time().hour();
    tTimeInfo.min = time.time().minute();
    tTimeInfo.sec = time.time().second();
    /*发送设置服务器连接的受电弓信息的报文给服务器，告知服务器受电弓信息, 并且对服务器进行校时*/
    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        memset(&tPvmsInfo, 0, sizeof(T_PVMS_INFO));
        tPvmsInfo.i8PvmsVideoNum = tTrainConfigInfo.tNvrServerInfo[i].iPvmsCameraNum;
        tPvmsInfo.i8PvmsCarriageNo = tTrainConfigInfo.tNvrServerInfo[i].iPvmsCarriageNO;
        pmsgHandle = STATE_GetNvrServerPmsgHandle(i);
//        qDebug()<<"*******************---111111:"<<tPvmsInfo.i8PvmsVideoNum<<tPvmsInfo.i8PvmsCarriageNo<<endl;

        printf("*******************---111111=%d--=%d\n",tPvmsInfo.i8PvmsVideoNum,tPvmsInfo.i8PvmsCarriageNo);
        iRet = PMSG_SendPmsgData(pmsgHandle, CLI_SERV_MSG_TYPE_SET_PVMS_INFO, (char *)&tPvmsInfo, sizeof(T_PVMS_INFO));
        if (iRet < 0)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_PVMS_INFO to server %d error!iRet=%d\n", i+1,iRet);
              qDebug()<<"PMSG_SendPmsgData CLI_SERV_MSG_TYPE_SET_PVMS_INFO to server %d error"<<i+1<<iRet<<__FUNCTION__<<__LINE__<<endl;
        }

        iRet = PMSG_SendPmsgData(pmsgHandle, CLI_SERV_MSG_TYPE_CHECK_TIME, (char *)&tTimeInfo, sizeof(T_TIME_INFO));    //发送校时命令
        if (iRet < 0)
        {
//            DebugPrint(DEBUG_UI_ERROR_PRINT, "PMSG_SendPmsgData CLI_SERV_MSG_TYPE_CHECK_TIME to server %d error!iRet=%d\n", i+1,iRet);
              qDebug()<<"PMSG_SendPmsgData CLI_SERV_MSG_TYPE_CHECK_TIME to server %d error!iRet"<<i+1<<iRet<<__FUNCTION__<<__LINE__<<endl;
        }
        else
        {
            memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
            tLogInfo.iLogType = 0;
            snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "notify server %d Time Check OK %4d-%02d-%02d %02d:%02d:%02d!", 100+tTrainConfigInfo.tNvrServerInfo[i].iCarriageNO,
                time.date().year(), time.date().month(), time.date().day(), time.time().hour(), time.time().minute(), time.time().second());
            LOG_WriteLog(&tLogInfo);
        }
    }
    a.setWindowIcon(QIcon(":/res/info.png"));   //设置窗口图标，这里主要是messagebox窗体会显示，而避免出现QT图标
//    qDebug() << "drivers------------------------"<< QSqlDatabase::drivers();


//    QTextCodec *codec = QTextCodec::codecForName("System");
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::setCodecForCStrings(codec);
//    QTextCodec::setCodecForTr(codec);

    QPixmap pixmap(":/res/background.png");
    QSplashScreen splash(pixmap);
    splash.showFullScreen();
    splash.show();
    a.processEvents();


    g_choiceLoginDevPage = new choiceLoginDevWidget();
    g_loginPage = new loginWidget();
    g_pvmsMenuPage = new pvmsMenuWidget();


    g_choiceLoginDevPage->show();
    g_loginPage->hide();
    g_pvmsMenuPage->hide();

    g_pvmsMenuPage->m_pRs485Handle = pRs485Handle;

    QObject::connect(g_choiceLoginDevPage, SIGNAL(confirmDevTypeSignal(int)), g_loginPage, SLOT(showPageSlot(int)));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽

    QObject::connect(g_loginPage, SIGNAL(loginCanselSignal()), g_choiceLoginDevPage, SLOT(showPageSlot()));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽

    QObject::connect(g_loginPage,SIGNAL(gotoPvmsMenuPageSignal()),g_pvmsMenuPage,SLOT(showPageSlot()));

    QObject::connect(g_pvmsMenuPage, SIGNAL(registOutSignal(int)), g_loginPage, SLOT(showPageSlot(int)));       //受电弓监控主菜单页面的注销信号连接登录页面的页面显示槽


    usleep(1*1000*1000);

    splash.finish(g_choiceLoginDevPage);

    a.exec();

    RS485_DestroyConnect(pRs485Handle);

    for (i = 0; i < tTrainConfigInfo.iNvrServerCount; i++)
    {
        pmsgHandle = STATE_GetNvrServerPmsgHandle(i);
        PMSG_DestroyConnect(pmsgHandle);
    }
    PMSG_DestroyConnect(STATE_GetPisPmsgHandle());


    usleep(1*1000*1000);

    delete g_choiceLoginDevPage;
    g_choiceLoginDevPage = NULL;
    delete g_loginPage;
    g_loginPage = NULL;
    delete g_pvmsMenuPage;
    g_pvmsMenuPage = NULL;

//    QObject::connect(g_monitorPage,SIGNAL(registOutSignal(int)),g_loginPage,SLOT(showPageSlot(int)));

//    MyApplication w;
//    w.show();

    return 0;
}
