#include "health_home.h"
#include "ui_health_home.h"
#include "log_in.h"


health_home::health_home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_home)
{
    ui->setupUi(this);

    /*查询身高 体重等身体信息*/
    sql_body_information();

    /*订阅消息*/
    my_mqtt.Subscribe(my_mqtt.Sub_Topic,0);

    connect(&Client,SIGNAL(messageReceived(const QByteArray,const QMqttTopicName)),this,
            SLOT(MQTT_RevData_Success(const QByteArray)));//接收消息成功

    setupChart();

}

health_home::~health_home()
{
    delete ui;
}

// 初始化折线图
// 初始化折线图
void health_home::setupChart()
{
    // 创建心率和血氧的折线图系列
    hrSeries = new QLineSeries();
    hrSeries->setName("心率 (BPM)");
    hrSeries->setPen(QPen(Qt::blue, 2)); // 心率折线为蓝色，粗细为2

    spo2Series = new QLineSeries();
    spo2Series->setName("血氧 (%)");
    spo2Series->setPen(QPen(Qt::yellow, 2)); // 血氧折线为黄色，粗细为2

    // 创建图表并添加系列
    chart = new QChart();
    chart->addSeries(hrSeries);
    chart->addSeries(spo2Series);
    chart->setTitle(""); // 无标题

    // 设置背景颜色为白色
    chart->setBackgroundBrush(QBrush(QColor(255, 255, 255)));

    // 设置X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setLabelsVisible(false); // 隐藏标签
    axisX->setTickCount(10); // 设置10个刻度
    axisX->setGridLineColor(Qt::black); // 刻度线颜色为黑色
    axisX->setGridLinePen(QPen(Qt::black, 0.5)); // 设置刻度线粗细为0.5
    chart->addAxis(axisX, Qt::AlignBottom);
    hrSeries->attachAxis(axisX);
    spo2Series->attachAxis(axisX);

    // 设置左侧Y轴（心率）
    QValueAxis *hrAxisY = new QValueAxis();
    hrAxisY->setRange(50, 150); // 心率范围 50-150
    // hrAxisY->setLabelsVisible(false); // 取消隐藏标签
    hrAxisY->setTickCount(5); // 设置5个刻度
    hrAxisY->setGridLineColor(Qt::black); // 刻度线颜色为黑色
    hrAxisY->setGridLinePen(QPen(Qt::black, 0.5)); // 设置刻度线粗细为0.5
    chart->addAxis(hrAxisY, Qt::AlignLeft);
    hrSeries->attachAxis(hrAxisY);

    // 设置右侧Y轴（血氧）
    QValueAxis *spo2AxisY = new QValueAxis();
    spo2AxisY->setRange(90, 100); // 血氧范围 90-100
    // spo2AxisY->setLabelsVisible(false); // 取消隐藏标签
    spo2AxisY->setTickCount(5); // 设置5个刻度
    spo2AxisY->setGridLineColor(Qt::black); // 刻度线颜色为黑色
    spo2AxisY->setGridLinePen(QPen(Qt::black, 0.5)); // 设置刻度线粗细为0.5
    chart->addAxis(spo2AxisY, Qt::AlignRight);
    spo2Series->attachAxis(spo2AxisY);

    // 创建 QChartView 并添加到 frame_4
    QChartView *chartView = new QChartView(chart, ui->frame_4);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedSize(381, 241); // 设置折线图尺寸与 frame_4 一致

    // 设置边框和圆角
    chartView->setStyleSheet(
        "QChartView {"
        "border: 1px solid rgb(200, 200, 200);"
        "border-radius: 10px;"
        "}"
        );

    // 为 frame_4 设置布局并添加 chartView
    QVBoxLayout *layout = new QVBoxLayout(ui->frame_4);
    layout->setContentsMargins(0, 0, 0, 0); // 移除边距
    layout->addWidget(chartView);
    ui->frame_4->setLayout(layout);

    // 显示图例
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom); // 图例显示在底部
}

/*查询数据库中的信息*/
void health_home::sql_body_information()
{
    // 1. 初始化SQL语句
    QSqlQuery query;
    QString sql = "SELECT name, age, sex, height, weight FROM user WHERE username = :username";

    query.prepare(sql);
    query.bindValue(":username",g_username); // 绑定用户名参数

    // 2. 执行查询
    if (!query.exec()) {
        qDebug() << "数据库查询失败:" << query.lastError().text();
        QMessageBox::warning(this, "错误", "查询失败，请检查数据库连接", QMessageBox::Ok);
        return;
    }

    // 3. 提取数据
    if (query.next()) { // 如果查询有结果
        QString name = query.value(0).toString();      // 获取 name
        int age = query.value(1).toInt();             // 获取 age
        QString sex = query.value(2).toString();      // 获取 sex
        double height = query.value(3).toDouble();    // 获取 height
        double weight = query.value(4).toDouble();    // 获取 weight

        // 4.计算BMI
        QString BMI_r;
        double BMI = computeBMI(height,weight,&BMI_r);

        // 5.显示数据
        ui->label_name->setText(name);
        ui->label_age->setText(QString::number(age));
        ui->label_sex->setText(sex);
        ui->label_height->setText(QString::number(height));
        ui->label_weight->setText(QString::number(weight));
        ui->label_BMI->setText(QString::number(BMI));
        ui->label_BMIRadius->setText(BMI_r);
    }


    QTimer *getSysTime = new QTimer(this);//时间
    connect(getSysTime,SIGNAL(timeout()),this,SLOT(time_reflash()));
    getSysTime->start(1000);//定时器启动
}

/* 计算 BMI 值  和 范围*/
double health_home::computeBMI(double height, double weight, QString* BMI_R)
{
    if (height <= 0 || weight <= 0) {
        qDebug() << "无效的身高或体重！";
        if (BMI_R) *BMI_R = "无效数据";
        return -1;  // 返回 -1 代表无效值
    }

    double heightInMeters = height / 100.0;  // cm 转换为 m
    double bmi = weight / (heightInMeters * heightInMeters);

    // 分类 BMI 结果
    if (bmi < 18.5)
        *BMI_R = "偏瘦";
    else if (bmi >= 18.5 && bmi < 24.9)
        *BMI_R = "正常";
    else if (bmi >= 25 && bmi < 29.9)
        *BMI_R = "超重";
    else
        *BMI_R = "肥胖";

    return bmi;
}

/*获取系统时间*/
void health_home::getSysTime()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    QDate date = currentTime.date();
    int year = date.year();
    int month = date.month();
    int day = date.day();

    QTime time = currentTime.time();
    int hour = time.hour();
    int minute = time.minute();
    int second = time.second();

    myData =  QString("%1-%2-%3").arg(year).arg(month).arg(day);
    myTime = QString("%1:%2:%3")
                    .arg(hour, 2, 10, QChar('0'))  // 保证小时为 2 位数
                    .arg(minute, 2, 10, QChar('0')) // 保证分钟为 2 位数
                    .arg(second, 2, 10, QChar('0')); // 保证秒钟为 2 位数
}





/*时间刷新函数*/
void health_home::time_reflash()
{
    getSysTime();
    ui->label_myData->setText(myData);
    ui->label_myTime->setText(myTime);

}

/* 接收消息 */
void health_home::MQTT_RevData_Success(const QByteArray &message)
{
    static int dataPointCount = 0; // 记录数据点数量
    QString hrRec, spo2Rec, rec;
    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &parseError);

    if (parseError.error == QJsonParseError::NoError)
    {
        if ((hrRec = my_mqtt.get_mqttValue(jsonDocument, "HR")) != "")
        {
            heartRate = hrRec; // 存储心率
            ui->label_heartRate->setText(hrRec);
        }
        if ((spo2Rec = my_mqtt.get_mqttValue(jsonDocument, "SpO2")) != "")
        {
            bloodOxygen = spo2Rec; // 存储血氧
            ui->label_bloodOxygen->setText(spo2Rec);
        }

        // 如果同时接收到心率和血氧数据，则添加一个数据点
        if (!hrRec.isEmpty() && !spo2Rec.isEmpty())
        {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            hrSeries->append(currentTime, hrRec.toInt());
            spo2Series->append(currentTime, spo2Rec.toDouble());
            dataPointCount++; // 增加数据点计数

            // 动态调整 X 轴范围，始终显示最近的数据
            QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis*>(chart->axisX());
            if (axisX)
            {
                if (dataPointCount == 1)
                {
                    // 第一个数据点，设置 X 轴范围从当前时间开始
                    axisX->setRange(QDateTime::fromMSecsSinceEpoch(currentTime),
                                    QDateTime::fromMSecsSinceEpoch(currentTime + 60000));
                }
                else
                {
                    // 后续数据点，保持 X 轴的起始时间不变，更新结束时间
                    QDateTime startTime = axisX->min();
                    axisX->setRange(startTime, QDateTime::fromMSecsSinceEpoch(currentTime));
                }
            }

            // 当数据点达到 10 个时，清除折线图并重置计数
            if (dataPointCount >= 10)
            {
                hrSeries->clear();
                spo2Series->clear();
                dataPointCount = 0; // 重置计数
            }
        }

        // 更新其他标签并存储数据
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "Temp")) != "")
        {
            temperature = rec; // 存储温度
            ui->label_temp->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "Humi")) != "")
        {
            humidity = rec; // 存储湿度
            ui->label_humi->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "TVOC")) != "")
        {
            tvoc = rec; // 存储 TVOC
            ui->label_TVOC->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "CO2")) != "")
        {
            co2 = rec; // 存储 CO2
            ui->label_CO2->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "HCHO")) != "")
        {
            hcho = rec; // 存储甲醛
            ui->label_HCHO->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "PM25")) != "")
        {
            pm25 = rec; // 存储 PM2.5
            ui->label_PM25->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "DB")) != "")
        {
            noise = rec; // 存储噪声
            ui->label_db->setText(rec);
        }
        if ((rec = my_mqtt.get_mqttValue(jsonDocument, "people_temp")) != "")
        {
            peopleTemp = rec; // 存储人体温度
            ui->label_peopleTemp->setText(rec);
        }
    }
    qDebug() << "message:" << message << Qt::endl;
}


/*打开关闭雾化器*/
void health_home::on_checkBox_wuhua_toggled(bool checked)
{
    QJsonObject jsonObj;
    jsonObj["Wuhua"] = checked ? 1 : 0;

    QJsonDocument doc(jsonObj);
    QString str = doc.toJson(QJsonDocument::Compact);

    if(Client.state() == QMqttClient::Connected)
    {
        my_mqtt.Publish(my_mqtt.Pub_Topic,str.toLocal8Bit(),my_mqtt.m_MessageQos);
        qDebug() << "消息发送成功:" << str << Qt::endl;
    }
}

/*打开关闭门*/
void health_home::on_checkBox_door_toggled(bool checked)
{
    QJsonObject jsonObj;
    jsonObj["Door"] = checked ? 1 : 0;

    QJsonDocument doc(jsonObj);
    QString str = doc.toJson(QJsonDocument::Compact);

    if(Client.state() == QMqttClient::Connected)
    {
        my_mqtt.Publish(my_mqtt.Pub_Topic,str.toLocal8Bit(),my_mqtt.m_MessageQos);
        qDebug() << "消息发送成功:" << str << Qt::endl;
    }
}

/*打开关闭窗户*/
void health_home::on_checkBox_window_toggled(bool checked)
{
    QJsonObject jsonObj;
    jsonObj["Window"] = checked ? 1 : 0;

    QJsonDocument doc(jsonObj);
    QString str = doc.toJson(QJsonDocument::Compact);

    if(Client.state() == QMqttClient::Connected)
    {
        my_mqtt.Publish(my_mqtt.Pub_Topic,str.toLocal8Bit(),my_mqtt.m_MessageQos);
        qDebug() << "消息发送成功:" << str << Qt::endl;
    }
}


/*打开关闭风扇*/
void health_home::on_checkBox_fenshang_toggled(bool checked)
{
    QJsonObject jsonObj;
    jsonObj["Fenshang"] = checked ? 1 : 0;

    QJsonDocument doc(jsonObj);
    QString str = doc.toJson(QJsonDocument::Compact);

    if(Client.state() == QMqttClient::Connected)
    {
        my_mqtt.Publish(my_mqtt.Pub_Topic,str.toLocal8Bit(),my_mqtt.m_MessageQos);
        qDebug() << "消息发送成功:" << str << Qt::endl;
    }
}



