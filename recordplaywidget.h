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
#include <QVideoWidget>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include "qplayer.h"


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
    int pmsgCtrl(PMSG_HANDLE pHandle, unsigned char ucMsgCmd, char *pcMsgData, int iMsgDataLen);     //与服务器通信消息处理
    mySlider *m_playSlider;    //播放进度条
    timeSet *timeSetWidget;    //时间设置控制窗体
    bool Mouseflag;
    QMouseEvent *e;
    int m_iPlayFlag;   //播放标志，0-暂停状态，未播放，1-在播放
    double m_dPlaySpeed;   //播放速度
    qint64 videoTime;
    qint64 totalplaytime;
    void closePlayWin();
    int m_iPlayRange;    //录像文件总播放时长
    void recordQueryCtrl(char *pcMsgData, int iMsgDataLen);
    void mediaInit();

protected:
    void mousePressEvent(QMouseEvent *event);

public slots:
    void alarmPushButoonClickSlot();
    void alarmHappenCtrlSlot();
    void alarmHappenSlot();
    void alarmClearSlot();
    void recordQuerySlot();
    void registOutButtonClick();
    void recordDownloadSlot();
    void recordPlayStartSlot();
    void recordPlayStopSlot();
    void recordPlayFastForwardSlot();
    void recordPlaySlowForwardSlot();
    void recordPlayLastOneSlot();
    void recordPlayNextOneSlot();
    void playPlusStepSlot();
    void playMinusStepSlot();
    void carNoChangeSlot();

    void playSliderMoveSlot(int iPosTime);
    void playSliderPressSlot(int iPosTime);
    void positionchaged(qint64 pos);
    void getduration(qint64  playtime);

    void onTimerOut();

signals:
    void alarmPushButoonClickSignal();
    void registOutSignal(int iType);     //注销信号，iType:表示执行注销的页面类型，这里应该为2，表示受电弓监控页面,
    void setRecordPlayFlagSignal(int iFlag);
    void recordTableWidgetFillSignal();

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
    void setPlayButtonStyleSheet();
    void getTrainConfig();     //获取车型配置信息


    QMediaPlayer *player;
    QMediaPlaylist *list;
    QVideoWidget *videoViewer;
    QPlayer *vidoplayer;

    QStringList mVideoList;
    QString mVideoNmae;
    QTimer *posTimer;
    int maxValue = 1000;
};

#endif // RECORDPLAYWIDGET_H
