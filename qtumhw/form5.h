#ifndef FORM5_H
#define FORM5_H

#include <QWidget>
 #include <QShowEvent>


namespace Ui {
class Form5;
}

class Form5 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form5(QWidget *parent = 0);
    ~Form5();
    
signals:
    void subClosedSig();
    void openSubSig(int);

private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_sign_2_clicked();

private:
    Ui::Form5 *ui;


    void qtumDecodeInfo();


protected:
    void showEvent(QShowEvent *event);

};

#endif // FORM5_H
