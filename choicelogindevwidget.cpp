#include "choicelogindevwidget.h"
#include "ui_choicelogindevwidget.h"
#include "loginwidget.h"

choiceLoginDevWidget::choiceLoginDevWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::choiceLoginDevWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->cvmsPushButton->setFocusPolicy(Qt::NoFocus); // 得到焦点时，不显示虚线框
    ui->pvmsPushButton->setFocusPolicy(Qt::NoFocus);

    connect(ui->cvmsPushButton, SIGNAL(clicked()), this, SLOT(choiceCvmsSlot()));
    connect(ui->pvmsPushButton, SIGNAL(clicked()), this, SLOT(choicePvmsSlot()));
}

choiceLoginDevWidget::~choiceLoginDevWidget()
{
    delete ui;
}
void choiceLoginDevWidget::showPageSlot()
{
    this->show();
}

void choiceLoginDevWidget::choiceCvmsSlot()
{

    this->hide();
    emit confirmDevTypeSignal(E_CVMSTYPE);
}

void choiceLoginDevWidget::choicePvmsSlot()
{
//    this->hide();
//    emit confirmDevTypeSignal(E_PVMSTYPE);
}
