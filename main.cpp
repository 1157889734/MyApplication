#include "MyApplication.h"
#include <QApplication>
#include "choicelogindevwidget.h"
#include "loginwidget.h"

choiceLoginDevWidget *g_choiceLoginDevPage = NULL;    //选择登录设备页面
loginWidget *g_loginPage = NULL;   //登录页面

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    g_choiceLoginDevPage = new choiceLoginDevWidget();
    g_loginPage = new loginWidget();
    g_choiceLoginDevPage->show();


    QObject::connect(g_choiceLoginDevPage, SIGNAL(confirmDevTypeSignal(int)), g_loginPage, SLOT(showPageSlot(int)));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽
    QObject::connect(g_choiceLoginDevPage, SIGNAL(canselSignal()), g_loginPage, SLOT(showPageSlot(int)));    //选择登录设备页面的确认设备类型信号连接登录页面的页面显示槽

//    MyApplication w;
//    w.show();

    return a.exec();
}
