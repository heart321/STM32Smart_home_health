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
#include <QtMqtt/qmqttclient.h>
#include "mqtt.h"

namespace Ui {
class health_home;
}

class health_home : public QWidget
{
    Q_OBJECT

public:
    explicit health_home(QWidget *parent = nullptr);
    ~health_home();

    //查询sql数据库中的信息
    void sql_body_information(void);
    //计算BMI值
    double computeBMI(double height,double weight,QString* BMI_R);
    void getSysTime(void);//获取系统时间



private slots:
    void time_reflash(void);//时间刷新

    /*接收消息*/
    void MQTT_RevData_Success(const QByteArray &message);


    void on_checkBox_wuhua_toggled(bool checked);

    void on_checkBox_door_toggled(bool checked);

    void on_checkBox_window_toggled(bool checked);

    void on_checkBox_fenshang_toggled(bool checked);

private:
    Ui::health_home *ui;

    /*创建mqtt类*/
    mqtt my_mqtt;

    // 获取时间
    QString myData;
    QString myTime;
};

#endif // HEALTH_HOME_H
