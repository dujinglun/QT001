#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include <QDateTime>
#include <QThread>

#include <QTimer>

#include "form1.h"
#include "form2.h"
#include "form3.h"
#include "form4.h"
#include "form5.h"



namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit mainWidget(QWidget *parent = 0);

    ~mainWidget();

    
private slots:
    void subshow(int formNum);
    void reshow();


    void digitClicked();
    void pointClicked();
    void clear();

    void tickSlot();

    void on_pushButton_exit_clicked();

    void on_pushButton_UART_clicked();

    void on_pushButton1_3_clicked();

    void on_pushButton1_4_clicked();

   // void on_pushButton1_5_clicked();

    void on_digitButton_7_clicked();

private:
    Ui::mainWidget *ui;

    Form2 * subPage2;
    Form3 * subPage3;
    Form4 * subPage4;
    Form5 * subPage5;

    QTimer *tickTimer;


protected:
  //  void mousePressEvent(QMouseEvent *);


};

#endif // MAINWIDGET_H
