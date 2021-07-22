#ifndef RECORDPLAYWIDGET_H
#define RECORDPLAYWIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class recordPlayWidget;
}

class recordPlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit recordPlayWidget(QWidget *parent = 0);
    ~recordPlayWidget();

public slots:
    void alarmPushButoonClickSlot();



signals:
    void alarmPushButoonClickSignal();


private:
    Ui::recordPlayWidget *ui;
    QTimer *m_alarmHappenTimer;

};

#endif // RECORDPLAYWIDGET_H
