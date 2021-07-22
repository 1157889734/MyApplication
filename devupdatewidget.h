#ifndef DEVUPDATEWIDGET_H
#define DEVUPDATEWIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class devUpdateWidget;
}

class devUpdateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit devUpdateWidget(QWidget *parent = 0);
    ~devUpdateWidget();
public slots:
    void alarmPushButoonClickSlot();

signals:
    void alarmPushButoonClickSignal();

private:
    Ui::devUpdateWidget *ui;
    QTimer *m_alarmHappenTimer;

};

#endif // DEVUPDATEWIDGET_H
