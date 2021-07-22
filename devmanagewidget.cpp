#include "devmanagewidget.h"
#include "ui_devmanagewidget.h"
static int g_iDNum = 0;

devManageWidget::devManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devManageWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);



    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
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
