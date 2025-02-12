#ifndef HEALTH_HOME_H
#define HEALTH_HOME_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>


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

    void getSysTime(void);//获取系统时间

private slots:
    void time_reflash(void);//时间刷新

private:
    Ui::health_home *ui;

    // 获取时间
    QString myData;
    QString myTime;
};

#endif // HEALTH_HOME_H
