#include "devupdatewidget.h"
#include "ui_devupdatewidget.h"
static int g_iVNum = 0;
devUpdateWidget::devUpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devUpdateWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->alarmPushButton, SIGNAL(clicked(bool)), this, SLOT(alarmPushButoonClickSlot()));   //报警按钮按键信号响应打开报警信息界面
    ui->alarmPushButton->setFocusPolicy(Qt::NoFocus);
    m_alarmHappenTimer = NULL;


}

devUpdateWidget::~devUpdateWidget()
{
    delete ui;
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
