#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class MyApplication : public QWidget
{
    Q_OBJECT

public:
    MyApplication(QWidget *parent = 0);
    ~MyApplication();
};

#endif // WIDGET_H
