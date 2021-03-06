#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QApplication>
#include <pthread.h>
#include <stdio.h>



class MyApplication : public QWidget
{
    Q_OBJECT

public:
    MyApplication(QWidget *parent = 0);

    ~MyApplication();

    pthread_t m_threadId;
    int m_iThreadRunFlag;
    int m_iMousePosX;
    int m_iMousePosY;
    bool notify(QObject *obj, QEvent *event);
    void triggerBlackScreenSignal();
    void triggerMouseHideSignal();

public slots:
    void blackScreenSignalCtrl();
    void mouseHideSignalCtrl();
    void alarmHappenSignalCtrl();
    void alarmClearSignalCtrl();

signals:
    void blackScreenSignal();
    void mouseHideSignal();
    void blackScreenExitSignal();

private:
    QWidget *m_blackScreenWidget;
    int m_iAlarmFlag;
    int m_iMouseHideFlag; //鼠标是否隐藏状态，0-不隐藏，1-隐藏

};

#endif // WIDGET_H
