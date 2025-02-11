#include "mysql.h"


QSqlDatabase globalDb;

mysql::mysql(QWidget *parent)
    : QWidget{parent}
{

}

/*mysql对象的析构函数*/
mysql::~mysql()
{
    if (globalDb.isOpen()) {
        globalDb.close();
        qDebug() << "数据库连接已关闭" << Qt::endl;
    }
}

bool mysql::mysql_init()
{
    globalDb = QSqlDatabase::addDatabase("QMYSQL");
    //数据库的地址
    globalDb.setHostName("47.108.217.29");
    globalDb.setUserName("xchen");
    globalDb.setDatabaseName("Smart_health");
    globalDb.setPassword("3321");
    globalDb.setPort(3306);

    //连接数据库服务器
    if(!globalDb.open())
    {
        qDebug() << "失败原因：" << globalDb.lastError().text() << Qt::endl;
        return false;
    }
    qDebug() << "连接数据库成功！！！" << Qt::endl;
    return true;

}


