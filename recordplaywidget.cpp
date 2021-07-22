#include "recordplaywidget.h"
#include "ui_recordplaywidget.h"

recordPlayWidget::recordPlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::recordPlayWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

}

recordPlayWidget::~recordPlayWidget()
{
    delete ui;
}
