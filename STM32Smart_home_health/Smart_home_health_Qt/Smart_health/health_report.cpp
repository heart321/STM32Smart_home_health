/*
 * @file      health_report.c
 *
 * @brief     主要实现AI健康报告 调用百度AI大模型
 *
 * @version   V1.0
 *
 * @date     2025/2/11 星期二
 *
 * @auther   chenxuan_123
 */

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
