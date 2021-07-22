#ifndef DEVUPDATEWIDGET_H
#define DEVUPDATEWIDGET_H

#include <QWidget>

namespace Ui {
class devUpdateWidget;
}

class devUpdateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit devUpdateWidget(QWidget *parent = 0);
    ~devUpdateWidget();

private:
    Ui::devUpdateWidget *ui;
};

#endif // DEVUPDATEWIDGET_H
