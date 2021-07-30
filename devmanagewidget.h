#ifndef DEVMANAGEWIDGET_H
#define DEVMANAGEWIDGET_H

#include <QWidget>
#include <QTimer>
namespace Ui {
class devManageWidget;
}

class devManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit devManageWidget(QWidget *parent = 0);
    ~devManageWidget();

public slots:
    void alarmPushButoonClickSlot();
    void alarmHappenSlot();
    void alarmHappenCtrlSlot();
    void alarmClearSlot();


signals:
    void alarmPushButoonClickSignal();

private:
    Ui::devManageWidget *ui;
    QTimer *m_alarmHappenTimer;

};

#endif // DEVMANAGEWIDGET_H
