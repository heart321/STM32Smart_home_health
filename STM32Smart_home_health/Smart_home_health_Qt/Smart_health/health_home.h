#ifndef HEALTH_HOME_H
#define HEALTH_HOME_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class health_home;
}

class health_home : public QWidget
{
    Q_OBJECT

public:
    explicit health_home(QWidget *parent = nullptr);
    ~health_home();

    //计算BMI值
    double computeBMI(double height,double weight,QString* BMI_R);

private:
    Ui::health_home *ui;
};

#endif // HEALTH_HOME_H
