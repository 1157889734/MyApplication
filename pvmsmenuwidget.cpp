#include "pvmsmenuwidget.h"
#include "ui_pvmsmenuwidget.h"

#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面

pvmsMenuWidget::pvmsMenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pvmsMenuWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_pvmsMonitorPage = new pvmsMonitorWidget(this);   //受电弓监控页面
    m_pvmsMonitorPage->setGeometry(0, 103, m_pvmsMonitorPage->width(), m_pvmsMonitorPage->height());   //设置位置
    m_recordPlayPage = new recordPlayWidget(this);     //录像回放页面
    m_recordPlayPage->setGeometry(0, 103, m_recordPlayPage->width(), m_recordPlayPage->height());
    m_inteAnalyPage = new inteAnalyWidget(this);		 //智能分析页面
    m_inteAnalyPage->setGeometry(0, 103, m_inteAnalyPage->width(), m_inteAnalyPage->height());
    m_devManagePage = new devManageWidget(this);       //设备管理页面
    m_devManagePage->setGeometry(0, 103, m_devManagePage->width(), m_devManagePage->height());
    m_devUpdatePage = new devUpdateWidget(this);       //设备更新页面
    m_devUpdatePage->setGeometry(0, 103, m_devUpdatePage->width(), m_devUpdatePage->height());

    m_pvmsMonitorPage->hide();
    m_recordPlayPage->hide();
    m_inteAnalyPage->hide();
    m_devManagePage->hide();
    m_devUpdatePage->hide();
    m_alarmPage = NULL;


    ui->pvmsMonitorMenuPushButton->setFocusPolicy(Qt::NoFocus); // 得到焦点时，不显示虚线框
    ui->recordPlayMenuPushButton->setFocusPolicy(Qt::NoFocus);
    ui->inteAnalyMenuPushButton->setFocusPolicy(Qt::NoFocus);
    ui->devManageMenuPushButton->setFocusPolicy(Qt::NoFocus);
    ui->devUpdateMenuPushButton->setFocusPolicy(Qt::NoFocus);
    ui->loginOutPushButton->setFocusPolicy(Qt::NoFocus);

    connect(ui->loginOutPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));
    connect(ui->pvmsMonitorMenuPushButton, SIGNAL(clicked()), this, SLOT(menuButtonClick()));     //连接受电弓监控菜单按钮的按键信号和响应函数
    connect(ui->recordPlayMenuPushButton, SIGNAL(clicked()), this, SLOT(menuButtonClick()));	  //连接录像回放菜单按钮的按键信号和响应函数
    connect(ui->inteAnalyMenuPushButton, SIGNAL(clicked()), this, SLOT(menuButtonClick()));		  //连接智能分析菜单按钮的按键信号和响应函数
    connect(ui->devManageMenuPushButton, SIGNAL(clicked()), this, SLOT(menuButtonClick()));		  //连接设备管理菜单按钮的按键信号和响应函数
    connect(ui->devUpdateMenuPushButton, SIGNAL(clicked()), this, SLOT(menuButtonClick()));		  //连接设备更新菜单按钮的按键信号和响应函数

    connect(m_pvmsMonitorPage, SIGNAL(alarmPushButoonClickSignal()), this, SLOT(alarmPageShowSlot()));
    connect(m_recordPlayPage, SIGNAL(alarmPushButoonClickSignal()), this, SLOT(alarmPageShowSlot()));
    connect(m_devManagePage, SIGNAL(alarmPushButoonClickSignal()), this, SLOT(alarmPageShowSlot()));
    connect(m_devUpdatePage, SIGNAL(alarmPushButoonClickSignal()), this, SLOT(alarmPageShowSlot()));


    ui->pvmsMonitorMenuPushButton->setChecked(true);
    ui->recordPlayMenuPushButton->setChecked(false);
    ui->inteAnalyMenuPushButton->setChecked(false);
    ui->devManageMenuPushButton->setChecked(false);
    ui->devUpdateMenuPushButton->setChecked(false);

}

pvmsMenuWidget::~pvmsMenuWidget()
{
    delete m_pvmsMonitorPage;
    m_pvmsMonitorPage = NULL;
    delete m_recordPlayPage;
    m_recordPlayPage = NULL;
    delete m_inteAnalyPage;
    m_inteAnalyPage = NULL;
    delete m_devManagePage;
    m_devManagePage = NULL;
    delete m_devUpdatePage;
    m_devUpdatePage = NULL;
    if (m_alarmPage!= NULL)
    {
        delete m_alarmPage;
    }
    delete ui;

}

void pvmsMenuWidget::alarmPageShowSlot()
{
    if (m_alarmPage== NULL)
    {
        m_alarmPage = new alarmWidget(this);      //新建报警信息显示窗体
        m_alarmPage->setGeometry(195, 189, m_alarmPage->width(), m_alarmPage->height());
        m_alarmPage->show();
        connect(m_alarmPage, SIGNAL(SendEscape()), this, SLOT(closeAlarmWidget()));

    }
    else
    {
        m_alarmPage->show();
    }
    m_iAlarmPageOpenFlag = 1;
    m_alarmPage->triggerAlarmCleanSignal();
}
void pvmsMenuWidget::showPageSlot()
{
    this->show();
    m_pvmsMonitorPage->show();
    m_recordPlayPage->hide();
    m_inteAnalyPage->hide();
    m_devManagePage->hide();
    m_devUpdatePage->hide();
}
void pvmsMenuWidget::closeAlarmWidget()
{
    m_iAlarmPageOpenFlag = 0;
    m_alarmPage->hide();
}
void pvmsMenuWidget::registOutButtonClick()
{

    m_pvmsMonitorPage->m_iPresetPasswdOkFlag = 0;
    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型
}

void pvmsMenuWidget::menuButtonClick()
{
    QObject* Sender = sender();     //Sender->objectName(),可区分不同的信号来源，也就是不同的按钮按键
    if(Sender==0)
    {
        return ;
    }
    if (Sender->objectName() == "pvmsMonitorMenuPushButton")     //受电弓监控按钮被按，则切换到受电弓监控页面
    {
        m_recordPlayPage->hide();
        m_inteAnalyPage->hide();
        m_devManagePage->hide();
        m_devUpdatePage->hide();
        m_pvmsMonitorPage->show();

        ui->pvmsMonitorMenuPushButton->setChecked(true);
        ui->recordPlayMenuPushButton->setChecked(false);
        ui->inteAnalyMenuPushButton->setChecked(false);
        ui->devManageMenuPushButton->setChecked(false);
        ui->devUpdateMenuPushButton->setChecked(false);

    }
    else if (Sender->objectName() == "recordPlayMenuPushButton")     //录像回放按钮被按，则切换到录像回放页面
    {
        m_pvmsMonitorPage->hide();
        m_inteAnalyPage->hide();
        m_devManagePage->hide();
        m_devUpdatePage->hide();
        m_recordPlayPage->show();

        ui->pvmsMonitorMenuPushButton->setChecked(false);
        ui->recordPlayMenuPushButton->setChecked(true);
        ui->inteAnalyMenuPushButton->setChecked(false);
        ui->devManageMenuPushButton->setChecked(false);
        ui->devUpdateMenuPushButton->setChecked(false);


    }

    else if (Sender->objectName() == "inteAnalyMenuPushButton")      //智能分析按钮被按，则切换到智能分析页面
    {
        m_pvmsMonitorPage->hide();
        m_recordPlayPage->hide();
        m_devManagePage->hide();
        m_devUpdatePage->hide();
        m_inteAnalyPage->show();


        ui->pvmsMonitorMenuPushButton->setChecked(false);
        ui->recordPlayMenuPushButton->setChecked(false);
        ui->inteAnalyMenuPushButton->setChecked(true);
        ui->devManageMenuPushButton->setChecked(false);
        ui->devUpdateMenuPushButton->setChecked(false);

    }
    else if (Sender->objectName() == "devManageMenuPushButton")      //设备管理按钮被按，则切换到设备管理页面
    {
        m_pvmsMonitorPage->hide();
        m_recordPlayPage->hide();
        m_inteAnalyPage->hide();
        m_devUpdatePage->hide();
        m_devManagePage->show();

        ui->pvmsMonitorMenuPushButton->setChecked(false);
        ui->recordPlayMenuPushButton->setChecked(false);
        ui->inteAnalyMenuPushButton->setChecked(false);
        ui->devManageMenuPushButton->setChecked(true);
        ui->devUpdateMenuPushButton->setChecked(false);

    }
    else if (Sender->objectName() == "devUpdateMenuPushButton")     //设备更新按钮被按，则切换到设备更新页面
    {
        m_pvmsMonitorPage->hide();
        m_recordPlayPage->hide();
        m_inteAnalyPage->hide();
        m_devManagePage->hide();
        m_devUpdatePage->show();

        ui->pvmsMonitorMenuPushButton->setChecked(false);
        ui->recordPlayMenuPushButton->setChecked(false);
        ui->inteAnalyMenuPushButton->setChecked(false);
        ui->devManageMenuPushButton->setChecked(false);
        ui->devUpdateMenuPushButton->setChecked(true);

    }
}

