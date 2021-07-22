#include "devupdatewidget.h"
#include "ui_devupdatewidget.h"

devUpdateWidget::devUpdateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devUpdateWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

}

devUpdateWidget::~devUpdateWidget()
{
    delete ui;
}
