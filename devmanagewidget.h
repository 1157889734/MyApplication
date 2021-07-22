#ifndef DEVMANAGEWIDGET_H
#define DEVMANAGEWIDGET_H

#include <QWidget>

namespace Ui {
class devManageWidget;
}

class devManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit devManageWidget(QWidget *parent = 0);
    ~devManageWidget();

private:
    Ui::devManageWidget *ui;
};

#endif // DEVMANAGEWIDGET_H
