#ifndef HEALTH_USER_H
#define HEALTH_USER_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QStyledItemDelegate>


namespace Ui {
class health_user;
}

class health_user : public QWidget
{
    Q_OBJECT

public:
    explicit health_user(QWidget *parent = nullptr);
    ~health_user();

private:
    Ui::health_user *ui;
};

#endif // HEALTH_USER_H
