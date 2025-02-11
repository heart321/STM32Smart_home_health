#ifndef HEALTH_REPORT_H
#define HEALTH_REPORT_H

#include <QWidget>

namespace Ui {
class health_report;
}

class health_report : public QWidget
{
    Q_OBJECT

public:
    explicit health_report(QWidget *parent = nullptr);
    ~health_report();

private:
    Ui::health_report *ui;
};

#endif // HEALTH_REPORT_H
