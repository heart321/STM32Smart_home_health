#include "health_home.h"
#include "ui_health_home.h"
#include "log_in.h"

health_home::health_home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_home)
{
    ui->setupUi(this);


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

health_home::~health_home()
{
    delete ui;
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

