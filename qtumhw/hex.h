#ifndef HEX_H
#define HEX_H

#include <QProcess>
#include <QDebug>
#include <QString>


#include <QStringList>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QVariantList>
#include <QtSql>

#define  USE_TESTNET

#ifdef USE_TESTNET

#define CMDSTR_DECODE "/root/qtum/qtum-cli -testnet decoderawtransaction "
#define SIGCMDSTR "/root/qtum/qtum-cli -testnet signrawtransaction "

#define  CMD_QTUMD_START     "/root/qtum/qtumd  -testnet -daemon  "

#else

#define CMDSTR_DECODE "/root/qtum/qtum-cli   decoderawtransaction "
#define SIGCMDSTR "/root/qtum/qtum-cli  signrawtransaction "

#endif

#define RCV_SIGN_REQUEST_SIZE  500
#define SEND_MAX 340
#define SEND_PAYLOAD_MAX (SEND_MAX - 6)
#define SBD_SENDBUF_MAX 340

#define CSQ_WAIT_MAX 40/* csq等待40秒*/
#define CSQ_MIN  4     /* 要求最小的信号强度*/



typedef struct
{
    int packlength;
    QByteArray head;
    QByteArray sbdSendBuf;
    QByteArray payload;

}sPackToSend_t;

typedef struct
{
    int packlength;
    QByteArray head;
    QByteArray payload;
    QByteArray sbdRcvHexBuf;

}sPackRcved_t;

typedef struct
{
    int csq;

    int sbdiSendState;
    int sbdiSendSN;

    int sbdiRcvState;
    int sbdiRcvSN;

    int sbdiRcvSize;
    int sbdiUnReadNum;

}sSbdStatus_t;



typedef struct
{
    QString   strQrcode;

    QByteArray SendBufPayto;

    QByteArray  finalHex;
    QByteArray  rcvUsignedTransEmai;
    bool  flagRequestReady;

    sPackToSend_t sPackToSend;
    sPackRcved_t  sPackRcved;

    sSbdStatus_t sSbdStatus;

    int packlength;
    QByteArray sbdSendBuf;
    QByteArray bJsonResult;


    QByteArray baMultiSignResultCompressed;/* 压缩的最终hex*/
    QString   gSMultiSignResult;/* 最终签名hex，未压缩*/

}gVarible_t;




typedef struct {
    int sendState;
    int sendSN;

    int rcvState;
    int rcvSN;

    int rcvSize;
    int unReadNum;

}sbdiInfo_t;


extern sbdiInfo_t sbdiInfo;
extern gVarible_t gVarible;
extern gVarible_t* pgVarible;



extern QSqlDatabase gDatabase;
void StringToHex(QString str, QByteArray &senddata);


#endif // HEX_H
