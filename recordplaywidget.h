#ifndef RECORDPLAYWIDGET_H
#define RECORDPLAYWIDGET_H

#include <QWidget>
#include <QTimer>
#include "myslider.h"
#include <QTableWidget>
#include "timeset.h"


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


public slots:
    void alarmPushButoonClickSlot();
    void openStartTimeSetWidgetSlot();
    void openStopTimeSetWidgetSlot();
    void timeSetRecvMsg(QString year, QString month, QString day, QString hour, QString min, QString sec);



signals:
    void alarmPushButoonClickSignal();


private:
    Ui::recordPlayWidget *ui;
    QTimer *m_alarmHappenTimer;
    QStyle *m_tableWidgetStyle;
    QWidget *m_playWin;     //播放窗体


};

#endif // RECORDPLAYWIDGET_H
