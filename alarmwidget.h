#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

#include <QWidget>

namespace Ui {
class alarmWidget;
}

class alarmWidget : public QWidget
{
    Q_OBJECT

public:
    explicit alarmWidget(QWidget *parent = 0);
    ~alarmWidget();
    void triggerAlarmCleanSignal();

public slots:
    void closeButtonClick();
    void clearAllSlot();

signals:
    void alarmClearSignal();
    void SendEscape();

private:
    Ui::alarmWidget *ui;
};

#endif // ALARMWIDGET_H
