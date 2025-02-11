#include "log_in.h"
#include "ui_log_in.h"
#include "sign_in.h"
#include "widget.h"

log_in::log_in(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::log_in)
{
    ui->setupUi(this);

    /*创建软键盘*/
    //keyboard = new AeaQt::Keyboard(this);

    /*设置user样式*/
    ui->lineEdit_user->setLabel("账号:");
    ui->lineEdit_user->installEventFilter(this);

    /*设置password样式*/
    ui->lineEdit_password->setLabel("密码:");
    ui->lineEdit_password->installEventFilter(this);
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);

    /*设置登录按钮*/
    //ui->pushButton_login->setText("登录");
    ui->pushButton_login->setCornerRadius(10);
    ui->pushButton_login->setBackgroundColor("#55557f");
    ui->pushButton_login->clearFocus();

    /*设置注册按钮*/
    //ui->pushButton_signin->setText("注册");
    ui->pushButton_signin->setCornerRadius(10);
    ui->pushButton_signin->setBackgroundColor("#55557f");

    /*忘记密码设置*/
    //ui->pushButton_password->setText("忘记密码？");
    ui->pushButton_password->setForegroundColor("#5555ff");

    /*设置无边框窗口*/
    this->setWindowFlags(Qt::FramelessWindowHint);



    //设置焦点
    //ui->lineEdit_user->setFocus();



}

log_in::~log_in()
{
    delete keyboardWindow; // 先删除包含键盘的窗口
    delete ui;
}

/*登录功能实现*/
void log_in::on_pushButton_login_clicked()
{
    // 得到输入的账户名和密码
     QString username = ui->lineEdit_user->text();
     QString password = ui->lineEdit_password->text();

    // 判断用户名或密码是否为空
    if(username.isEmpty() || password.isEmpty())
    {
        qDebug() << "用户名或密码为空!!" << Qt::endl;
        QMessageBox::warning(this, tr("错误"), tr("用户名或密码为空"), QMessageBox::Ok);
        ui->lineEdit_user->clearFocus();
        ui->lineEdit_password->clearFocus();  // 取消焦点，避免连续触发
        return;
        // 登录失败时隐藏键盘
        if (keyboardWindow && keyboardWindow->isVisible()) {
            keyboardWindow->hide();

        }

    }
    else
    {
        // 使用参数化查询防止SQL注入
        QString sql = "SELECT password FROM user WHERE username = :username";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":username",username);// 绑定用户名参数

        // 执行sql语句
        if(!query.exec())
        {
            //查询失败
            qDebug() << "查询失败:" << query.lastError().text() << Qt::endl;
            QMessageBox::warning(this,tr("错误"),tr("数据库查询失败！"),QMessageBox::Ok);
            return;
        }

        // 查询得到结果
        if(query.next())
        {
            QString storedHash = query.value(0).toString();// 获取存储的密码哈希

            //使用 QCryptographicHash 计算输入密码的哈希
            QByteArray inputHash = QCryptographicHash::hash(
                                       password.toUtf8(),
                                       QCryptographicHash::Sha256
                                       ).toHex();

            //对比哈希值
            if(inputHash == storedHash) //直接对比字符串
            {
                qDebug() << "登录成功！" <<Qt::endl;
                static QPointer<Widget> w;
                if (w.isNull()) {
                    w = new Widget;
                    w->setAttribute(Qt::WA_DeleteOnClose);
                    w->showFullScreen();
                    this->close();
                }
            }
            else
            {
                qDebug() << "密码错误!!" << Qt::endl;
                QMessageBox::warning(this, tr("登录失败"), tr("密码错误"), QMessageBox::Ok);
                // 清空输入框
                ui->lineEdit_password->clear();
                ui->lineEdit_password->clearFocus();  // **清除焦点，防止回车连续触发**
                // 登录失败时隐藏键盘
                if (keyboardWindow && keyboardWindow->isVisible()) {
                    keyboardWindow->hide();
                }
            }
        }
        else
        {
            qDebug() << "用户不存在!!" << Qt::endl;
            QMessageBox::warning(this, tr("登录失败"), tr("用户名不存在"), QMessageBox::Ok);
            //清空输入框
            ui->lineEdit_user->clear();
            ui->lineEdit_password->clear();
            ui->lineEdit_user->clearFocus();
            ui->lineEdit_password->clearFocus();  // **同样清除焦点**
            // 登录失败时隐藏键盘
            if (keyboardWindow && keyboardWindow->isVisible()) {
                keyboardWindow->hide();
            }

        }
    }
}



/*跳转到注册界面*/
void log_in::on_pushButton_signin_clicked()
{
    static QPointer<sign_in> w; // 使用 QPointer 自动管理
    if (w.isNull()) {
        w = new sign_in;
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->showFullScreen();
        this->close();
    } else {
        if (!w->isVisible()) {
            w->showFullScreen(); // 如果窗口被关闭但未销毁，重新显示
            qDebug() << "窗口未被销毁！" << Qt::endl;
        }
        w->activateWindow();
        qDebug() << "窗口已经存在！"<< Qt::endl;
    }

}


bool log_in::eventFilter(QObject *watched,QEvent *event)
{
    // 判断是否点击了 lineEdit 如果点击了就显示键盘
    if((watched == ui->lineEdit_user) && (event->type() == QEvent::MouseButtonPress))
    {
        callKeyBoard();
    }
    if((watched == ui->lineEdit_password) && (event->type() == QEvent::MouseButtonPress))
    {
        callKeyBoard();
    }

    // 监听输入框失去焦点的事件，隐藏软键盘
    if ((watched == ui->lineEdit_user || watched == ui->lineEdit_password) && event->type() == QEvent::FocusOut)
    {
        if (keyboardWindow && keyboardWindow->isVisible()) {
            keyboardWindow->hide();
        }
    }

    /*避免连续触发*/
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            return true;  // 阻止默认行为，避免重复触发
        }
    }

    return QWidget::eventFilter(watched,event);     // 最后将事件交给上层对话框
}

// 调用软键盘
void log_in::callKeyBoard()
{
    if (!keyboard) {
        keyboard = new AeaQt::Keyboard(this);
        keyboardWindow = new QWidget(this);
        keyboardWindow->resize(680, 350);


        QVBoxLayout* v = new QVBoxLayout;
        v->addWidget(keyboard, 5);
        keyboardWindow->setLayout(v);

    }

    if (!keyboardWindow->isVisible()) {
        keyboardWindow->move(0, 250);
        keyboardWindow->show();
    }


}



