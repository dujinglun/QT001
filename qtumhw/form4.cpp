#include "form4.h"
#include "ui_form4.h"

#include <QProcess>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

#include <QStandardItemModel>
#include <QPoint>
#include <QFile>
#include <QMessageBox>
#include <QTimer>


#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

#include "hex.h"

/******************************************************************************/

Form4::Form4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form4)
{
    ui->setupUi(this);


    connect(this, SIGNAL(waitCloseSig()),this, SLOT(dialogClose()));
}

Form4::~Form4()
{
    emit subClosedSig();
    delete ui;
}
/******************************************************************************/

void Form4::reshow()
{
    this->show();
}
/******************************************************************************/

void Form4::on_pushButton_payto_clicked()
{
    QByteArray messageHead;
    messageHead.resize(3);
    messageHead[0] = 's';                 /* message kind */
    messageHead[1] = 20;
    messageHead[2] = 20;

    pgVarible->sbdSendBuf = messageHead + pgVarible->SendBufPayto;

    qDebug()<<pgVarible->sbdSendBuf ;

    ui->lineEdit->setText(QString::number( pgVarible->sbdSendBuf.size()  ));
    emit  openSubSig(2);

}

/******************************************************************************/

void Form4::on_pushButton_exit_clicked()
{
    emit subClosedSig();
    this->hide();
}

/******************************************************************************/
/**
 * @brief Form4::on_pushButton_sign_clicked
 * sign the transaction
 */
void Form4::on_pushButton_sign_clicked()
{
    QProcess     process;
    QByteArray   byteArray;
    QString      str;
    QString      cmd ;

    static bool currentBusy=false;

    if(currentBusy){
        return;
    }else{
        currentBusy=true;
        waitPage->show();

    }
    waitPage->show();

    QString rcvStr;


    qDebug()<<"##############################";

    rcvStr = pgVarible->sPackRcved.sbdRcvHexBuf.mid(6);
    if(rcvStr.isEmpty() ){
        return;
    }

    qDebug()<< rcvStr;
    qDebug()<<"kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk";

    cmd = "run_qtum_electrum --testnet ";
    cmd = cmd + " signtransaction " + rcvStr ;

    process.start(cmd);
    process.waitForFinished(40000);/*waite for long time */

    byteArray = process.readAllStandardOutput();/* 读取签名输出*/
    process.close();

    qDebug()<< byteArray;
    qDebug()<<"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC";

    /*************/
    bool ok;
    JsonObject result = QtJson::parse( QString(byteArray), ok).toMap();
    if (!ok) {
        qFatal("An error occurred during parsing");
        //return 1;
    }

    str = result["hex"].toString();
    StringToHex(str, pgVarible->finalHex);
    // pgVarible->unsignTransactionHex = str.toAscii();


    qDebug() << "hex:" << result["hex"].toString();
    qDebug() << "complete:"<< result["complete"].toBool() ;

    qDebug()<< "++++++++++++++++++++++++++++++++++++++";


    currentBusy=false;

    emit waitCloseSig();

    // waitPage->close();
}

/******************************************************************************/

void Form4::on_pushButton_broadcast_clicked()
{
    waitPage->show();
    QTimer::singleShot(3000, this, SLOT(dialogClose()));

}

void Form4::dialogClose()
{
    waitPage->close();
}

/******************************************************************************/
/**
 * @brief Form4::on_pushButton_check_clicked
 * check unsigned transaction
 */
void Form4::on_pushButton_check_clicked()
{
    QProcess     process;
    QByteArray   byteArray;
    QString      str;
    QString      cmd ;


    ui->pushButton_check->blockSignals(true);


    QString rcvStr;
    rcvStr =  pgVarible->rcvUsignedTransEmai.mid(3).toHex();


    rcvStr = pgVarible->sPackRcved.sbdRcvHexBuf.mid(6);

    if(rcvStr.isEmpty() ){
        return;
    }

    cmd = "run_qtum_electrum --testnet ";
    cmd = cmd + " deserialize " + rcvStr ; /*  读取签名请求的内容*/


    qDebug()<< "hello here";
    qDebug()<< cmd;


    process.start(cmd);
    process.waitForFinished();

    byteArray = process.readAllStandardOutput();

    process.close();

    qDebug()<< "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    qDebug()<< byteArray;
    qDebug()<< "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&";


    bool ok;
    JsonObject result = QtJson::parse( QString(byteArray), ok).toMap();
    if (!ok) {
        qFatal("An error occurred during parsing");
        //return 1;
    }

    qDebug() << "lockTime:" << result["lockTime"].toInt();
    qDebug() << "version:"<< result["version"].toInt() ;
    qDebug() << "segwit_ser:"<< result["segwit_ser"].toBool() ;
    qDebug() << "partial:"<< result["partial"].toBool() ;

    qDebug()<< "++++++++++++++++++++++++++++++++++++++";

    JsonArray plugins = result["outputs"].toList();     /* out array */

    JsonObject output = plugins[0].toMap();

    qDebug()<< "address:"<<output["address"].toString();/* the payee*/
    qDebug()<< "value:" <<output["value"].toString();

    qDebug()<< "++++++++++++++++++++++++++++++++++++++";

    ui->pushButton_check->blockSignals(false);

}
