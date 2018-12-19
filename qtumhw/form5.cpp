#include "form5.h"
#include "ui_form5.h"
#include <QProcess>
#include <QDebug>
#include <QDateTime>
#include <QStringList>

#include <QStandardItemModel>
#include <QPoint>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

#include <QTextStream>
#include <QDebug>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

#include "hex.h"


Form5::Form5(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form5)
{
    ui->setupUi(this);
}

Form5::~Form5()
{
    delete ui;
}


/******************************************************************************/


void Form5::showEvent(QShowEvent *event)
{
    ui->textBrowser->clear();
    qtumDecodeInfo();/* 打开页面，执行一次*/
}


void Form5::qtumDecodeInfo()
{
    QProcess     process;
    QByteArray   byteArray;
    QString      cmd  ;
    QString      strTemp  ;


    cmd = CMDSTR_DECODE + pgVarible->gSMultiSignResult ;

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
        return ;
    }


    JsonArray plugins = result["vout"].toList();     /* out array */
    JsonObject output = plugins[0].toMap();/* choice 1st*/

    JsonObject output2 = output["scriptPubKey"].toMap();/* choice 1st*/
    JsonArray address = output2["addresses"].toList();     /* out array */


    ui->textBrowser->clear();

    strTemp = "ADDRESS: " + address[0].toString();/* the payee*/
    ui->textBrowser->append(strTemp);
    ui->textBrowser->append("####################");
    strTemp = "VALUE: " + output["value"].toString();
    ui->textBrowser->append(strTemp);
    ui->textBrowser->append("####################");

}


void Form5::on_pushButton_exit_clicked()
{
   // emit subClosedSig();
    emit openSubSig(2);
    this->close();
}

void Form5::on_pushButton_sign_2_clicked()
{
    qtumDecodeInfo();
}



