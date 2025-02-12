#ifndef HEALTH_RECORD_H
#define HEALTH_RECORD_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class health_record;
}

class health_record : public QWidget
{
    Q_OBJECT

public:
    explicit health_record(QWidget *parent = nullptr);
    ~health_record();

private:
    Ui::health_record *ui;
};

#endif // HEALTH_RECORD_H
