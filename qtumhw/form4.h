#ifndef FORM4_H
#define FORM4_H

#include <QWidget>
#include "dialog.h"


namespace Ui {
class Form4;
}

class Form4 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form4(QWidget *parent = 0);
    ~Form4();

    Dialog *waitPage = Dialog::GetInstance();

signals:
    void subClosedSig();
    void openSubSig(int formNum);

    void waitCloseSig();

private slots:
    void on_pushButton_payto_clicked();
    void on_pushButton_exit_clicked();
    void on_pushButton_sign_clicked();
    void on_pushButton_broadcast_clicked();
    void on_pushButton_check_clicked();

    void reshow();

    void dialogClose();

private:
    Ui::Form4 *ui;

};

#endif // FORM4_H
