#include "devmanagewidget.h"
#include "ui_devmanagewidget.h"

devManageWidget::devManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devManageWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

}

devManageWidget::~devManageWidget()
{
    delete ui;
}
