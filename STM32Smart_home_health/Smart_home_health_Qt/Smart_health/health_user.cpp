#include "health_user.h"
#include "ui_health_user.h"

health_user::health_user(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_user)
{
    ui->setupUi(this);
}

health_user::~health_user()
{
    delete ui;
}
