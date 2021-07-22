#ifndef RECORDPLAYWIDGET_H
#define RECORDPLAYWIDGET_H

#include <QWidget>

namespace Ui {
class recordPlayWidget;
}

class recordPlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit recordPlayWidget(QWidget *parent = 0);
    ~recordPlayWidget();

private:
    Ui::recordPlayWidget *ui;
};

#endif // RECORDPLAYWIDGET_H
