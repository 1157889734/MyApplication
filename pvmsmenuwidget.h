#ifndef PVMSMENUWIDGET_H
#define PVMSMENUWIDGET_H

#include <QWidget>
#include "pvmsmonitorwidget.h"
#include "recordplaywidget.h"
#include "inteanalywidget.h"
#include "devmanagewidget.h"
#include "devupdatewidget.h"
#include "alarmwidget.h"



namespace Ui {
class pvmsMenuWidget;
}

class pvmsMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit pvmsMenuWidget(QWidget *parent = 0);
    ~pvmsMenuWidget();

public slots:
    void registOutButtonClick();
    void menuButtonClick();
    void alarmPageShowSlot();
    void closeAlarmWidget();

signals:
    void registOutSignal(int iType);     //注销信号，iType:表示执行注销的页面类型，这里应该为2，表示受电弓监控页面,

private:
    Ui::pvmsMenuWidget *ui;
    pvmsMonitorWidget *m_pvmsMonitorPage;
    recordPlayWidget *m_recordPlayPage;
    inteAnalyWidget *m_inteAnalyPage;
    devManageWidget *m_devManagePage;
    devUpdateWidget *m_devUpdatePage;
    alarmWidget *m_alarmPage;
    int m_iAlarmPageOpenFlag;

public slots:
    void showPageSlot();
};

#endif // PVMSMENUWIDGET_H
