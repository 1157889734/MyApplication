#include "recordplaywidget.h"
#include "ui_recordplaywidget.h"

static int g_iRNum = 0;

recordPlayWidget::recordPlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::recordPlayWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);




    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    m_alarmHappenTimer = NULL;

}

recordPlayWidget::~recordPlayWidget()
{
    delete ui;
}
void recordPlayWidget::alarmPushButoonClickSlot()
{
    emit alarmPushButoonClickSignal();
    if (m_alarmHappenTimer != NULL)
    {
        delete m_alarmHappenTimer;
        m_alarmHappenTimer = NULL;
    }
    ui->alarmPushButton->setChecked(false);

    g_iRNum = 0;
}
