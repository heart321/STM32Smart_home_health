#include <QApplication>
#include <QThread>
#include "log_in.h"
#include "mysql.h"




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Widget w;
    // w.show();

    /*程序开始时连接数据库  当程序结束时dbManager对象销毁 执行mysql的析构函数 关闭数据库*/
    mysql dbManager;
    if(!dbManager.mysql_init()) //连接失败
    {
        QMessageBox::critical(nullptr, "错误", "无法连接数据库，程序将退出！");
        return -1;
    }


    log_in login;
    login.showFullScreen();

    // sign_in signin;
    // signin.show();

    return a.exec();
}
