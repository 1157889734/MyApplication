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
    T_LOG_INFO tLogInfo;


    LOG_Init();    //本地日志模块初始化

    /*程序启动记录日志*/
    memset(&tLogInfo, 0, sizeof(T_LOG_INFO));
    tLogInfo.iLogType = 0;
    STATE_GetSysVersion(acClientVersion, sizeof(acClientVersion));
    snprintf(tLogInfo.acLogDesc, sizeof(tLogInfo.acLogDesc), "Startup %s!", acClientVersion);
    LOG_WriteLog(&tLogInfo);

    PMSG_Init();    //协议通信模块初始化

    STATE_ReadTrainConfigFile();	//程序运行起来读取一次车型配置文件


    a.setWindowIcon(QIcon(":/res/info.png"));   //设置窗口图标，这里主要是messagebox窗体会显示，而避免出现QT图标
    qDebug() << "drivers------------------------"<< QSqlDatabase::drivers();


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

    QObject::connect(g_choiceLoginDevPage, SIGNAL(confirmDevTypeSignal(int)), g_loginPage, SLOT(showPageSlot(int)));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽

    QObject::connect(g_loginPage, SIGNAL(loginCanselSignal()), g_choiceLoginDevPage, SLOT(showPageSlot()));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽

    QObject::connect(g_loginPage,SIGNAL(gotoPvmsMenuPageSignal()),g_pvmsMenuPage,SLOT(showPageSlot()));

    QObject::connect(g_pvmsMenuPage, SIGNAL(registOutSignal(int)), g_loginPage, SLOT(showPageSlot(int)));       //受电弓监控主菜单页面的注销信号连接登录页面的页面显示槽


    usleep(1*1000*1000);

    splash.finish(g_choiceLoginDevPage);

    a.exec();

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
