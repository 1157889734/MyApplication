#include "inteanalywidget.h"
#include "ui_inteanalywidget.h"
#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面

static int g_iDNum = 0;

inteAnalyWidget::inteAnalyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inteAnalyWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->canselPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));
    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面


    ui->StartdateEdit->setCalendarPopup(true);
    ui->EnddateEdit->setCalendarPopup(true);

    ui->EnddateEdit->setDateTime(QDateTime::currentDateTime());
    ui->EndtimeEdit->setDateTime(QDateTime::currentDateTime());


    ui->StartdateEdit->setDateTime(QDateTime::currentDateTime());
    ui->StarttimeEdit->setDateTime(QDateTime::currentDateTime());

    m_alarmHappenTimer = NULL;



}

inteAnalyWidget::~inteAnalyWidget()
{
    delete ui;
}


void inteAnalyWidget::registOutButtonClick()
{

    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型

}
void inteAnalyWidget::alarmPushButoonClickSlot()
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
void inteAnalyWidget::alarmHappenSlot()
{
    if (NULL == m_alarmHappenTimer)
    {
        m_alarmHappenTimer = new QTimer(this);
        connect(m_alarmHappenTimer,SIGNAL(timeout()), this,SLOT(alarmHappenCtrlSlot()));
        m_alarmHappenTimer->start(500);
    }
}

void inteAnalyWidget::alarmHappenCtrlSlot()
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

void inteAnalyWidget::alarmClearSlot()
{
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iDNum = 0;
}
