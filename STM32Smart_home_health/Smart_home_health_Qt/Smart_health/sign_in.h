#ifndef SIGN_IN_H
#define SIGN_IN_H

#include <QDialog>
#include "Keyboard/Keyboard.h"
#include "Keyboard/NumberKeyboard.h"

namespace Ui {
class sign_in;
}

class sign_in : public QDialog
{
    Q_OBJECT

public:
    explicit sign_in(QWidget *parent = nullptr);
    ~sign_in();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_signin_clicked();

    //重写事件用来判断 QLineEdit是否被点击
    bool eventFilter(QObject *watched,QEvent *event);

    //调用软键盘
    void callKeyBoard_sigin(void);

    //调用数字键盘
    //void callNumKeyBoard_sigin(void);

private:
    Ui::sign_in *ui;

    QWidget* keyboardWindow_sigin = nullptr;
    AeaQt::Keyboard* keyboard_sigin = nullptr;

};

#endif // SIGN_IN_H
