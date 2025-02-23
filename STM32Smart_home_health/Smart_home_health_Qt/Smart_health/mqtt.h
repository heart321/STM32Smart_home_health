#ifndef MQTT_H
#define MQTT_H

#include <QWidget>
#include <QDebug>
#include <QEventLoop>
#include <QtMqtt/qmqttclient.h>

extern QMqttClient Client; //mqtt客户端对象

class mqtt : public QWidget
{
    Q_OBJECT
    enum MessageQoS{
        m_QoS0,
        m_QoS1,
        m_Qos2
    };
public:
    explicit mqtt(QWidget *parent = nullptr);
    ~mqtt();
    // 连接mqtt服务器
    void mqtt_connect(void);

private slots:
    /*mqtt连接成功*/
    void mqtt_connect_success(void);

public:
    QString Sub_Topic; //订阅
    QString Pub_Topic;  //发布

    QEventLoop loop;

    //设消息等级
    enum MessageQoS m_MessageQos = m_QoS1;

signals:
};

#endif // MQTT_H
