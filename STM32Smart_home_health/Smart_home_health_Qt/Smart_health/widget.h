#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QButtonGroup>
#include <QStackedWidget>

//#include "qtmaterialflatbutton.h"


#include "health_record.h"
#include "health_home.h"
#include "health_report.h"
#include "health_user.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_toolButton_logout_clicked();

private:
    Ui::Widget *ui;
    /*创建各界面的窗口*/
    health_home homeWid;
    health_record recordWid;
    health_report reportWid;
    health_user userWid;

    /*按钮组*/
    QButtonGroup btnGroup;
};
#endif // WIDGET_H
