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
