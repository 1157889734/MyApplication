#ifndef RECORDPLAYWIDGET_H
#define RECORDPLAYWIDGET_H

#include <QWidget>
#include <QTimer>
#include "myslider.h"
#include <QTableWidget>
#include "timeset.h"
#include <QMouseEvent>
#include "pvmsmonitorwidget.h"
#include "pmsgcli.h"
#include "state.h"



#define MAX_RECORD_SEACH_NUM 10000
#define MAX_RECFILE_PATH_LEN 256



namespace Ui {
class recordPlayWidget;
}

class recordPlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit recordPlayWidget(QWidget *parent = 0);
    ~recordPlayWidget();
    mySlider *m_playSlider;    //播放进度条
    timeSet *timeSetWidget;    //时间设置控制窗体
    bool Mouseflag;
    QMouseEvent *e;

protected:
    void mousePressEvent(QMouseEvent *event);

public slots:
    void alarmPushButoonClickSlot();
    void openStartTimeSetWidgetSlot();
    void openStopTimeSetWidgetSlot();
    void timeSetRecvMsg(QString year, QString month, QString day, QString hour, QString min, QString sec);
    void recordQuerySlot();
    void registOutButtonClick();
    void recordDownloadSlot();
    void recordPlayStartSlot();
    void recordPlayStopSlot();
    void carNoChangeSlot();



signals:
    void alarmPushButoonClickSignal();
    void registOutSignal(int iType);     //注销信号，iType:表示执行注销的页面类型，这里应该为2，表示受电弓监控页面,

private:
    Ui::recordPlayWidget *ui;
    pvmsMonitorWidget *m_pvmsMonitorPage;
    QTimer *m_alarmHappenTimer;
    QTimer *m_recorQueryTimer;
    QStyle *m_tableWidgetStyle;
    QWidget *m_playWin;     //播放窗体
    char *m_pcRecordFileBuf;
    int m_iTotalLen;
    char m_acFilePath[MAX_RECORD_SEACH_NUM][MAX_RECFILE_PATH_LEN];   //记录查询到的录像文件路径全名
    PMSG_HANDLE m_Phandle[MAX_SERVER_NUM];    //服务器PMSG通信句柄


};

#endif // RECORDPLAYWIDGET_H
