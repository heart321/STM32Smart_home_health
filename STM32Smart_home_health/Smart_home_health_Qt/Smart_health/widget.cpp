#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*添加有顺序*/
    ui->stackedWidget->addWidget(&homeWid);   //1
    ui->stackedWidget->addWidget(&recordWid); //2
    ui->stackedWidget->addWidget(&reportWid); //3
    ui->stackedWidget->addWidget(&userWid);   //4

    /*向按钮组添加按钮和 stackedWidget的索引 一一对应*/
    btnGroup.addButton(ui->toolButton_home,1);
    btnGroup.addButton(ui->toolButton_record,2);
    btnGroup.addButton(ui->toolButton_report,3);
    btnGroup.addButton(ui->toolButton_usrInfo,4);

    /*将QToolButton的checked信号和QStackedWidget的页面切换槽函数setCurrentIndex连接*/
    connect(&btnGroup, QOverload<int>::of(&QButtonGroup::idClicked), ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // 设置默认选中的页面
    btnGroup.button(1)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(1);


    /*设置无边框窗口*/
    this->setWindowFlags(Qt::FramelessWindowHint);


}
Widget::~Widget()
{
    delete ui;
}
