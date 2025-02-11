#include "sign_in.h"
#include "ui_sign_in.h"
#include "log_in.h"

sign_in::sign_in(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::sign_in)
{
    ui->setupUi(this);

    /*设置user*/
    ui->lineEdit_user->setLabel("账户:");
    ui->lineEdit_user->installEventFilter(this);

    /*设置password*/
    ui->lineEdit_password->setLabel("密码:");
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_password->installEventFilter(this);

    /*设置确认密码*/
    ui->lineEdit_entPsd->setLabel("确认密码:");
    ui->lineEdit_entPsd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_entPsd->installEventFilter(this);

    /*设置姓名*/
    ui->lineEdit_name->setLabel("姓名:");
    ui->lineEdit_name->installEventFilter(this);

    /*设置年龄*/
    ui->lineEdit_age->setLabel("年龄:");
    // QIntValidator *validator_age = new QIntValidator(0, 100, this); // 限制输入范围为 0 到 100
    // ui->lineEdit_age->setValidator(validator_age);
    ui->lineEdit_age->installEventFilter(this);


    /*设置身高*/
    ui->lineEdit_height->setLabel("身高(cm):");
    QDoubleValidator *validator_height = new QDoubleValidator(50.0, 250.0, 1, this); // 限制输入范围为 50.0 到 250.0，保留 1 位小数
    ui->lineEdit_height->setValidator(validator_height);
    ui->lineEdit_height->installEventFilter(this);

    /*设置体重*/
    ui->lineEdit_weight->setLabel("体重(kg):");
    QDoubleValidator *validator_weight = new QDoubleValidator(40.0, 300.0, 1, this); // 限制输入范围为 40.0 到 300.0，保留 1 位小数
    ui->lineEdit_weight->setValidator(validator_weight);
    ui->lineEdit_weight->installEventFilter(this);

    /*注册样式*/
    //ui->pushButton_signin->setText("注册");
    ui->pushButton_signin->setCornerRadius(10);
    ui->pushButton_signin->setBackgroundColor("#55557f");

    /*返回样式*/
    //ui->pushButton_back->setText("返回");
    ui->pushButton_back->setCornerRadius(10);
    ui->pushButton_back->setBackgroundColor("#55557f");

    /*设置无边框窗口*/
    this->setWindowFlags(Qt::FramelessWindowHint);




}

sign_in::~sign_in()
{
    delete ui;
}

/*返回登录界面*/
void sign_in::on_pushButton_back_clicked()
{
    static QPointer<log_in> w; // 使用 QPointer 自动管理
    if (w.isNull()) {
        w = new log_in;
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->showFullScreen();
        this->close();
    } else {
        if (!w->isVisible()) {
            w->showFullScreen(); // 如果窗口被关闭但未销毁，重新显示
        }
        w->activateWindow();
        qDebug() << "窗口已经存在！" << Qt::endl;
    }
}

// 密码加密函数
QString encryptPassword(const QString &password) {
    QByteArray passwordData = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}


//实现注册功能
void sign_in::on_pushButton_signin_clicked()
{
    QString username = ui->lineEdit_user->text();                       //账户名
    QString password = ui->lineEdit_password->text();                   //密码
    QString confirm_password = ui->lineEdit_entPsd->text();             //确认密码

    QString name = ui->lineEdit_name->text();                           //姓名
    int age = ui->lineEdit_age->text().toInt();                         //年龄
    QString sex = ui->comboBox_sex->currentText();                      //性别
    double height = ui->lineEdit_height->text().toDouble();             //身高
    double weight = ui->lineEdit_weight->text().toDouble();             //体重

    //判断是否有空
    if( username == NULL                       ||
        password == NULL                       ||
        confirm_password == NULL               ||
        name == NULL                           ||
        ui->lineEdit_age->text() == NULL       ||
        ui->lineEdit_height->text() == NULL    ||
        ui->lineEdit_weight->text() == NULL )
    {
        qDebug() << "有空!!" << Qt::endl;
        QMessageBox::warning(this, tr("错误"), tr("有选项没有填写！"), QMessageBox::Ok);
        return;
    }
    else if(password != confirm_password)
    {
        qDebug() << "密码不一致!!" << Qt::endl;
        QMessageBox::warning(this, tr("错误"), tr("密码不一致！"), QMessageBox::Ok);
        return;
    }
    else
    {
        // 判断输入的值是否合法
        // 1.年龄范围 0 ~ 100
        if(age < 0 || age > 100)
        {
            qDebug() << "年龄超出范围!!" << Qt::endl;
            QMessageBox::warning(this, tr("错误"), tr("年龄超出了范围！重新填写！"), QMessageBox::Ok);
            return;
        }
        // 2.身高范围 50cm ~ 250cm
        if(height < 50.0 || height > 250.0)
        {
            qDebug() << "身高超出范围!!" << Qt::endl;
            QMessageBox::warning(this, tr("错误"), tr("身高超出了范围！重新填写！"), QMessageBox::Ok);
            return;
        }
        // 3，体重范围 40kg ~ 300kg
        if(weight < 40.0 || weight > 300.0)
        {
            qDebug() << "体重超出范围!!" << Qt::endl;
            QMessageBox::warning(this, tr("错误"), tr("体重超出了范围！重新填写！"), QMessageBox::Ok);
            return;
        }
    }
    //向mysql数据库插入数据
    // 1，将密码进行加密
    QString encryPassword = encryptPassword(password);

    // 2.SQL插入语句
    QSqlQuery query;
    query.prepare("INSERT INTO user (username,password,name,age,sex,height,weight)"
                  "VALUES (:username,:password,:name,:age,:sex,:height,:weight)");

    // 3.绑定参数
    query.bindValue(":username", username);
    query.bindValue(":password", encryPassword); // 使用加密后的密码
    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":sex", sex);
    query.bindValue(":height", height);
    query.bindValue(":weight", weight);

    // 4.执行插入操作
    if(!query.exec())
    {
        qDebug() << "插入失败:" << query.lastError().text() << Qt::endl;
    }
    else
    {
        qDebug() << "插入成功！" << Qt::endl;
        QMessageBox::information(this, tr("成功"), tr("请返回登录界面登录!"), QMessageBox::Ok);
    }



}

/*重写事件过滤器 判断QlineEdit是否被点击*/
bool sign_in::eventFilter(QObject *watched, QEvent *event)
{
    // 判断是否点击了 lineEdit 如果点击了就显示键盘
    if((watched == ui->lineEdit_user        ||
        watched == ui->lineEdit_password    ||
        watched == ui->lineEdit_entPsd      ||
        watched == ui->lineEdit_name        ||
        watched == ui->lineEdit_age         ||
        watched == ui->lineEdit_height      ||
        watched == ui->lineEdit_weight
         ) && (event->type() == QEvent::MouseButtonPress))
    {
        callKeyBoard_sigin();
    }

    // 监听输入框失去焦点的事件，隐藏软键盘
    if ((watched == ui->lineEdit_user        ||
         watched == ui->lineEdit_password    ||
         watched == ui->lineEdit_entPsd      ||
         watched == ui->lineEdit_name        ||
         watched == ui->lineEdit_age         ||
         watched == ui->lineEdit_height      ||
         watched == ui->lineEdit_weight
         ) && event->type() == QEvent::FocusOut)
    {
        if (keyboardWindow_sigin && keyboardWindow_sigin->isVisible()) {
            keyboardWindow_sigin->hide();
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

/*调用字母键盘*/
void sign_in::callKeyBoard_sigin()
{
    if (!keyboard_sigin) {
        keyboard_sigin = new AeaQt::Keyboard(this);
        keyboardWindow_sigin = new QWidget(this);
        keyboardWindow_sigin->resize(680, 350);


        QVBoxLayout* v = new QVBoxLayout;
        v->addWidget(keyboard_sigin, 5);
        keyboardWindow_sigin->setLayout(v);

    }

    if (!keyboardWindow_sigin->isVisible()) {
        keyboardWindow_sigin->move(0, 250);
        keyboardWindow_sigin->show();
    }
}



