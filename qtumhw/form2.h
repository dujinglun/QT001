#ifndef FORM2_H
#define FORM2_H

#include <QWidget>
#include <QTimer>
#include "qextserial/posix_qextserialport.h"

#include "hex.h"

namespace Ui {
class Form2;
}

class Form2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form2(QWidget *parent = 0);
    ~Form2();


    bool flagUartBusy;

signals:
    void openSubSig(int);
    void subClosedSig();

private:
    Ui::Form2 *ui;

    Posix_QextSerialPort *myCom;
    QTimer *tickTimer;
    QTimer *delayTimer;

    int SbdRepeatCnt;
    // int statusUartCmd;

    int tickInc;/* 心跳增量计数器*/

    enum sbd_status{ SBD_IDLE,SBD_BUSY,SBD_CSQ,SBD_SBDI,SBD_ERR  }statusUartCmd;

    void sbdButtonLock();
    void sbdButtonUnlock();

    int getMailtoSignData(QByteArray * baResult);
    int getRecvedMailNumber( );

    int  atCmdCsq();
    void atCmdWb(QByteArray * baToSend);
    int atCmdRb();

    void on_pushButton_wb_clicked();

private slots:
    void on_pushButton_clicked(); //”发送数据”按钮槽函数

    void on_pushButton_exit_clicked();

    void tickSlot();
    void stateUpdate();

    void on_pushButton_csq_clicked();

    void on_pushButton_sbdi_clicked();

    void on_pushButton_sbdrb_clicked();

    void on_pushButton_wb_3_clicked();

    void on_pushButtonSign_clicked();


    void on_pushButton_sendMail_clicked();
    void on_pushButtonSignCheck_clicked();
};

#endif // FORM2_H
