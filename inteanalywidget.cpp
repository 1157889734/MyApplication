#include "inteanalywidget.h"
#include "ui_inteanalywidget.h"
#define PVMSPAGETYPE  2    //此页面类型，2表示受电弓监控页面


inteAnalyWidget::inteAnalyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inteAnalyWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->canselPushButton, SIGNAL(clicked()), this, SLOT(registOutButtonClick()));


}

inteAnalyWidget::~inteAnalyWidget()
{
    delete ui;
}


void inteAnalyWidget::registOutButtonClick()
{

    this->hide();
    emit registOutSignal(PVMSPAGETYPE);    //触发注销信号，带上当前设备类型

}
