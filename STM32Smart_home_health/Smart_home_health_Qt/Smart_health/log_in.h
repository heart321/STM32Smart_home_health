#ifndef LOG_IN_H
#define LOG_IN_H

#include <QDialog>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPointer>

#include <QCryptographicHash>


// #include "qtmaterialtextfield.h"
// #include "qtmaterialautocomplete.h"
// #include "qtmaterialflatbutton.h"
// #include "qtmaterialraisedbutton.h"
//#include "qtmaterialdialog.h"

namespace Ui {
class log_in;
}

class log_in : public QDialog
{
    Q_OBJECT

public:
    explicit log_in(QWidget *parent = nullptr);
    ~log_in();



private slots:
    void on_pushButton_signin_clicked();

    void on_pushButton_login_clicked();

private:
    Ui::log_in *ui;


};

#endif // LOG_IN_H
