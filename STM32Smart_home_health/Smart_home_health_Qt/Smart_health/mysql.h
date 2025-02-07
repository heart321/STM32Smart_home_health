#ifndef MYSQL_H
#define MYSQL_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>



/*定义一个全局变量*/
extern QSqlDatabase globalDb;

class mysql : public QWidget
{
    Q_OBJECT
public:
    explicit mysql(QWidget *parent = nullptr);
    ~mysql();
    bool mysql_init(void);

signals:



};

#endif // MYSQL_H
