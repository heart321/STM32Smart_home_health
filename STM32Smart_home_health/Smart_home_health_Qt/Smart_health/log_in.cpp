#include "log_in.h"
#include "ui_log_in.h"

log_in::log_in(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::log_in)
{
    ui->setupUi(this);



    /*设置user样式*/
    ui->label_user->setLabel("账号:");
    /*设置password样式*/
    ui->label_password->setLabel("密码:");


}

log_in::~log_in()
{
    delete ui;
}
