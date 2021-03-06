#ifndef FORM41_H
#define FORM41_H

#include <QWidget>

namespace Ui {
class Form41;
}

class Form41 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form41(QWidget *parent = 0);
    ~Form41();

signals:
    void subClosedSig();


private slots:
    void on_pushButton_exit_clicked();

private:
    Ui::Form41 *ui;
};

#endif // FORM41_H
