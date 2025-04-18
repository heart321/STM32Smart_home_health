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
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAbstractAxis>

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


    // Getter 方法，用于访问 MQTT 数据
    QString getHeartRate() const { return heartRate; }
    QString getBloodOxygen() const { return bloodOxygen; }
    QString getTemperature() const { return temperature; }
    QString getHumidity() const { return humidity; }
    QString getTVOC() const { return tvoc; }
    QString getCO2() const { return co2; }
    QString getHCHO() const { return hcho; }
    QString getPM25() const { return pm25; }
    QString getNoise() const { return noise; }
    QString getPeopleTemp() const { return peopleTemp; }

private slots:
    void time_reflash(void);//时间刷新

    /*接收消息*/
    void MQTT_RevData_Success(const QByteArray &message);
    /*雾化器按钮*/
    void on_checkBox_wuhua_toggled(bool checked);
    /*继电器按钮*/
    void on_checkBox_door_toggled(bool checked);
    /*舵机按钮*/
    void on_checkBox_window_toggled(bool checked);
    /*风扇按钮*/
    void on_checkBox_fenshang_toggled(bool checked);
private:
    void setupChart(); // 初始化折线图
private:
    Ui::health_home *ui;

    /*创建mqtt类*/
    mqtt my_mqtt;

    // 获取时间
    QString myData;
    QString myTime;

    QChart *chart; // 合并后的折线图
    QLineSeries *hrSeries;
    QLineSeries *spo2Series;
    const int maxPoints = 60; // 最大显示60个数据点（例如60秒的数据）

    // 存储 MQTT 接收的数据
    QString heartRate;
    QString bloodOxygen;
    QString temperature;
    QString humidity;
    QString tvoc;
    QString co2;
    QString hcho;
    QString pm25;
    QString noise;
    QString peopleTemp;

};

#endif // HEALTH_HOME_H
