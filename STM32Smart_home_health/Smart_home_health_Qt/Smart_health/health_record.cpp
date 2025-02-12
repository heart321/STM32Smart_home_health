#include "health_record.h"
#include "ui_health_record.h"

health_record::health_record(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_record)
{
    ui->setupUi(this);

}

health_record::~health_record()
{
    delete ui;
}
