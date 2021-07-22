#include "loginwidget.h"
#include "ui_loginwidget.h"

loginWidget::loginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->ensure->setFocusPolicy(Qt::NoFocus); // 得到焦点时，不显示虚线框
    ui->cansel->setFocusPolicy(Qt::NoFocus);

    connect(ui->ensure, SIGNAL(clicked(bool)), this, SLOT(okButtonSlot()));	   //确定按钮的按键信号连接相应槽函数
    connect(ui->cansel, SIGNAL(clicked(bool)), this, SLOT(canselButtonSlot()));	   //确定按钮的按键信号连接相应槽函数


    ui->username_Edit->setText("admin");
    ui->username_Edit->setFont(QFont("宋体",20));
    ui->username_Edit->setAlignment(Qt::AlignCenter);
    ui->Passwd_Edit->setEchoMode(QLineEdit::Password);

}

loginWidget::~loginWidget()
{
    delete ui;
}
void loginWidget::okButtonSlot()
{
    if (ui->Passwd_Edit->text() == "12345")
    {
        emit gotoPvmsMenuPageSignal();
    }

}
void loginWidget::canselButtonSlot()
{
    this->hide();
    emit loginCanselSignal();
}
void loginWidget::showPageSlot(int iType)
{
    this->show();
}
void loginWidget::canselSlot()
{
    this->hide();
}
