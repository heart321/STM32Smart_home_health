#ifndef LOG_IN_H
#define LOG_IN_H

#include <QDialog>
#include "qtmaterialtextfield.h"
#include "qtmaterialautocomplete.h"

namespace Ui {
class log_in;
}

class log_in : public QDialog
{
    Q_OBJECT

public:
    explicit log_in(QWidget *parent = nullptr);
    ~log_in();

private:
    Ui::log_in *ui;
};

#endif // LOG_IN_H
