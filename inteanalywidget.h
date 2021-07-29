#ifndef INTEANALYWIDGET_H
#define INTEANALYWIDGET_H

#include <QWidget>
#include "pvmsmonitorwidget.h"

namespace Ui {
class inteAnalyWidget;
}

class inteAnalyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit inteAnalyWidget(QWidget *parent = 0);
    ~inteAnalyWidget();

public slots:
    void registOutButtonClick();

signals:
    void registOutSignal(int iType);     //注销信号，iType:表示执行注销的页面类型，这里应该为2，表示受电弓监控页面,

private:
    Ui::inteAnalyWidget *ui;
    pvmsMonitorWidget *m_pvmsMonitorPage;

};

#endif // INTEANALYWIDGET_H
