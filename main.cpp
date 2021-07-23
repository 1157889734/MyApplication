#include "MyApplication.h"
#include <QApplication>
#include "choicelogindevwidget.h"
#include "loginwidget.h"
#include "pvmsmenuwidget.h"
#include <QtVirtualKeyboard>


choiceLoginDevWidget *g_choiceLoginDevPage = NULL;    //选择登录设备页面
loginWidget *g_loginPage = NULL;   //登录页面
pvmsMenuWidget *g_pvmsMenuPage = NULL;   //受电弓监控主菜单页面

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication a(argc, argv);

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

//    MyApplication w;
//    w.show();

    return a.exec();
}
