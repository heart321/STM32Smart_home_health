#ifndef HEALTH_HOME_H
#define HEALTH_HOME_H

#include <QWidget>

namespace Ui {
class health_home;
}

class health_home : public QWidget
{
    Q_OBJECT

public:
    explicit health_home(QWidget *parent = nullptr);
    ~health_home();

private:
    Ui::health_home *ui;
};

#endif // HEALTH_HOME_H
