#ifndef DEVUPDATEWIDGET_H
#define DEVUPDATEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include "state.h"
#include "pmsgcli.h"
#include "timeset.h"
#include "pvmsmonitorwidget.h"

#include "usermanage.h"

namespace Ui {
class devUpdateWidget;
}

class devUpdateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit devUpdateWidget(QWidget *parent = 0);
    ~devUpdateWidget();
    QDateTime timeTd;


public slots:

    void userManageSlot();
    void configFileSelectionSlot();
    void configUpdateFileSLOT();
    void configFileImportSlot();
    void alarmPushButoonClickSlot();
    void alarmHappenCtrlSlot();
    void alarmHappenSlot();
    void alarmClearSlot();
    void presetReturnTimeChange(int iComboBoxId);
    void pollingTimeChange(int iComboBoxId);
    void showSysTime();
    void monitorSysTime();
    void systimeSlot();
    void registOutButtonClick();

    void setCameraImageParamSlot();


signals:
    void alarmPushButoonClickSignal();
    void registOutSignal(int iType);     //注销信号，iType:表示执行注销的页面类型，这里应该为2，表示受电弓监控页面,

private:
    Ui::devUpdateWidget *ui;
    QTimer *m_alarmHappenTimer;
    QString m_pollingtTimeText;
    QString m_presetReturnTimeText;
    QTimer *m_sys_timer;
    PMSG_HANDLE m_Phandle[MAX_SERVER_NUM];    //服务器PMSG通信句柄
    pvmsMonitorWidget *m_pvmsMonitorPage;

//    userManage *usermanagePage;

};

#endif // DEVUPDATEWIDGET_H
