#include "sign_in.h"
#include "ui_sign_in.h"

sign_in::sign_in(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::sign_in)
{
    ui->setupUi(this);


}

sign_in::~sign_in()
{
    delete ui;
}
