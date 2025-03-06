#include "mqtt.h"

QMqttClient Client; //mqtt客户端对象

mqtt::mqtt(QWidget *parent)
    : QWidget{parent}
{
    connect(&Client,SIGNAL(connected()),this,SLOT(mqtt_connect_success()));//绑定服务连接成功的信号和槽
}

mqtt::~mqtt()
{
    // if(Client.state() == QMqttClient::Connected)
    // {
    //     Client.disconnectFromHost();//断开连接
    //     qDebug() << "mqtt服务器已断开！！！" << Qt::endl;
    // }
}

/*连接mqtt服务器*/
void mqtt::mqtt_connect()
{
    /*判断mqtt 服务器是否连接*/
    if(Client.state() == QMqttClient::Disconnected)
    {
        /*服务器IP*/
        Client.setHostname("47.109.187.11");
        /*端口号*/
        Client.setPort(1883);
        /*设置用户名*/
        Client.setUsername("xchen");
        /*设置密码*/
        Client.setPassword("3321");

        //用于指定客户端与代理断开连接时是否清除之前的会话状态。
        //当"Clean Session"设置为true时，表示每次连接都是一个新的会话，之前的订阅、发布等状态都会被清除。
        //当"Clean Session"设置为false时，表示客户端与代理断开连接后，会话状态会被保留，包括之前的订阅和发布消息。
        Client.setCleanSession(false);

        //设置心跳时间间隔60S
        Client.setKeepAlive(60);
        //设置mqtt版本
        Client.setProtocolVersion(QMqttClient::ProtocolVersion::MQTT_3_1_1);
        //连接mqtt服务器
        Client.connectToHost();

        //等待服务器连接成功
        //loop.exec();
    }
}

/*订阅消息*/
void mqtt::Subscribe(const QString &topic, int qos)
{
    QMqttSubscription *substate = Client.subscribe(QMqttTopicFilter(topic),qos);
    QTimer time;
    int timerout = 50;
    connect(&time,&QTimer::timeout,&loop,&QEventLoop::quit);
    time.start(timerout);
    loop.exec();

    if(substate->state() == QMqttSubscription::Subscribed)
    {
        qDebug () << "订阅成功！" << Qt::endl;
    }
}

/*发布消息*/
int mqtt::Publish(const QString &topic, const QString &message, int qos)
{
    auto result = Client.publish(QMqttTopicName(topic),message.toUtf8(),qos,true);
    return result;
}

/*分离json数据*/
QString mqtt::get_mqttValue(QJsonDocument JsonDocument, QString Key)
{
    if(JsonDocument.isObject())
    {
        QJsonObject jsonObj = JsonDocument.object();//将Doucoument转换为json对象
        if(jsonObj.contains(Key))
        {
            QJsonValue jsonVal = jsonObj.value(Key);
            if(jsonVal.isString())
            {
                if(jsonVal.isNull())
                {
                    return QString("");
                }
                else
                {
                    return jsonVal.toString();
                }
            }
        }
    }
    return QString("");
}



/*mqtt 连接成功*/
void mqtt::mqtt_connect_success()
{
    qDebug() << "mqtt服务器连接成功！！！" << Qt::endl;
}
