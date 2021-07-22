#include "alarmwidget.h"
#include "ui_alarmwidget.h"

#define MAX_ITEM_NUM  50   //报警列表最多的记录行数
#define MAX_NVR_NUM  4
#define MAX_CAMERA_NUM 4

typedef struct _T_NVRALARM_INFO
{
    int iCarriageNO;      //NVR服务器车厢号
    int iDiskErrFlag;  //0-无报警，1-硬盘丢失报警，2-硬盘错误报警
} T_NVRALARM_INFO, *PT_NVRALARM_INFO;

typedef struct _T_CAMERAALARM_INFO
{
    int iCarriageNO;  //NVR服务器车厢号
    int iDevPos;      //位置号
    int iVideoErrFlag;  //0-无报警，1-视频遮挡报警，2-视频丢失报警
} T_CAMERAALARM_INFO, *PT_CAMERAALARM_INFO;

static T_NVRALARM_INFO g_NvrAlarmInfo[MAX_NVR_NUM];
static T_CAMERAALARM_INFO g_CameraAlarmInfo[MAX_NVR_NUM*MAX_CAMERA_NUM];


alarmWidget::alarmWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::alarmWidget)
{
    ui->setupUi(this);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->closePushButton->setFocusPolicy(Qt::NoFocus);
    ui->closeRPushButton->setFocusPolicy(Qt::NoFocus);

    ui->tableWidget->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->tableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->tableWidget->horizontalHeader()->resizeSection(0,95); //设置表头第一列的宽度为95
    ui->tableWidget->horizontalHeader()->resizeSection(1,145); //设置表头第二列的宽度为145

    connect(ui->closePushButton, SIGNAL(clicked(bool)), this, SLOT(closeButtonClick()));
    connect(ui->closeRPushButton, SIGNAL(clicked(bool)), this, SLOT(closeButtonClick()));
    connect(ui->clearPushButton, SIGNAL(clicked(bool)), this, SLOT(clearAllSlot()));


}

alarmWidget::~alarmWidget()
{
    delete ui;
}
void alarmWidget::triggerAlarmCleanSignal()
{
    emit alarmClearSignal();

}
void alarmWidget::closeButtonClick()
{
    emit SendEscape();
    return;
}
void alarmWidget::clearAllSlot()
{


}
