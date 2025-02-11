#include "health_report.h"
#include "ui_health_report.h"

health_report::health_report(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_report)
{
    ui->setupUi(this);
}

health_report::~health_report()
{
    delete ui;
}
