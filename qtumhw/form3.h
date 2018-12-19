#ifndef FORM3_H
#define FORM3_H

#include <QWidget>
#include <QPaintEvent>
#include "hex.h"


namespace Ui {
class Form3;
}


class Form3 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form3(QWidget *parent = 0);
    ~Form3();

signals:
    void subClosedSig();
    void getQrSig();


private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

 //   void reshow();

    void digitClicked();
    void pointClicked();
    void clear();
    void on_pushButton_exit_clicked();

private:
    Ui::Form3 *ui;

 //   void paintEvent(QPaintEvent *);


};

#endif // FORM3_H
