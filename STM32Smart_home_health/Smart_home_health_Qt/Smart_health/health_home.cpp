#include "health_home.h"
#include "ui_health_home.h"

health_home::health_home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_home)
{
    ui->setupUi(this);




}

health_home::~health_home()
{
    delete ui;
}
