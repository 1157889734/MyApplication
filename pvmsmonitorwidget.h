#ifndef PVMSMONITORWIDGET_H
#define PVMSMONITORWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QButtonGroup>


namespace Ui {
class pvmsMonitorWidget;
}

class pvmsMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit pvmsMonitorWidget(QWidget *parent = 0);
    ~pvmsMonitorWidget();
    QPushButton *m_presetNoPushbutton[8];
    QButtonGroup *g_buttonGroup;

    int m_iSelectPresetNo;   //保存选中的预置点编号
    int m_iAlarmNotCtrlFlag;   //有报警信息未处理标志
signals:
    void alarmPushButoonClickSignal();

public slots:
    void presetNoGroupButtonClickSlot(int iButtonNo);     //预置点编号按钮组按键信号响应槽函数，iButtonNo为按钮编号
    void alarmPushButoonClickSlot();
    void startPollingSlot();
    void pausePollingSlot();
    void manualSwitchLastCameraSlot();
    void manualSwitchNextCameraSlot();
    void presetSetCtrlSlot();
    void presetGetCtrlSlot();
    void temporarySaveBeginSlot();
    void cameraSwitchSlot();
    void fillLightSwitchSlot();


private:
    Ui::pvmsMonitorWidget *ui;
    QTimer *m_alarmHappenTimer;


};

#endif // PVMSMONITORWIDGET_H
