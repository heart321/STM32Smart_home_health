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
#include <QLineEdit>

#include <QCryptographicHash>

#include "Keyboard/Keyboard.h"


// #include "qtmaterialtextfield.h"
// #include "qtmaterialautocomplete.h"
// #include "qtmaterialflatbutton.h"
// #include "qtmaterialraisedbutton.h"
//#include "qtmaterialdialog.h"

extern QString g_username;

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

    //重写事件用来判断 QLineEdit是否被点击
    bool eventFilter(QObject *watched,QEvent *event);
    //void keyPressEvent(QKeyEvent * event);

    //调用软键盘
    void callKeyBoard(void);


private:
    Ui::log_in *ui;

    QWidget* keyboardWindow = nullptr;
    AeaQt::Keyboard* keyboard = nullptr;




};

#endif // LOG_IN_H
