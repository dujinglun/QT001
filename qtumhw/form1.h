#ifndef FORM1_H
#define FORM1_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QProcess>

#include "hex.h"

namespace Ui {
class Form1;
}

class Form1 : public QWidget
{
    Q_OBJECT
    

signals:

    void subClosedSig();

public:
    explicit Form1(QWidget *parent = 0);
    ~Form1();
    
    QTimer * cameraTimer;

private slots:
    void on_pushButton_clicked();
    void cameraTimerSlot();

private:
    Ui::Form1 *ui;

    QProcess*  process;
};

#endif // FORM1_H
