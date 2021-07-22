#include "inteanalywidget.h"
#include "ui_inteanalywidget.h"

inteAnalyWidget::inteAnalyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inteAnalyWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

}

inteAnalyWidget::~inteAnalyWidget()
{
    delete ui;
}
