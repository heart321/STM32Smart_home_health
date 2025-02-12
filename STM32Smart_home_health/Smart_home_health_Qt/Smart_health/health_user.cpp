#include "health_user.h"
#include "ui_health_user.h"

health_user::health_user(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::health_user)
{
    ui->setupUi(this);



    QSqlTableModel *model = new QSqlTableModel();
    model->setTable("user");
    model->select(); //加载数据

    ui->tableView_user->setModel(model);
    ui->tableView_user->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_user->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 隐藏 "password"列
    int passwordColumn = model->fieldIndex("password"); // 获取 password 字段的列索引
    if (passwordColumn != -1) { // 确保字段存在
        ui->tableView_user->setColumnHidden(passwordColumn, true);
    }

}

health_user::~health_user()
{
    delete ui;
}
