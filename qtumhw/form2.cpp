/***
 * 串口操作
 *
 ***/


#include "form2.h"
#include "ui_form2.h"

#include <QDebug>
#include <QDateTime>
#include <QStringList>
#include <QSettings>
#include <QFile>


#include "scp_multi_sig.h"


Form2::Form2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form2)
{
    ui->setupUi(this);

    ui->sendlineEdit->setText(tr("12345"));

    myCom = new Posix_QextSerialPort("/dev/ttyS1",QextSerialBase::Polling);/* linux 下只能是polling*/

    myCom ->open(QIODevice::ReadWrite); //以读写方式打开串口
    myCom->setBaudRate(BAUD19200); //波特率设置，我们设置为9600
    myCom->setDataBits(DATA_8); //数据位设置，我们设置为8位数据位
    myCom->setParity(PAR_NONE); //奇偶校验设置，我们设置为无校验
    myCom->setStopBits(STOP_1); //停止位设置，我们设置为1位停止位
    myCom->setFlowControl(FLOW_OFF);    //为无数据流控制
    myCom->setTimeout(10);     //延时设置,10ms

    //   ui->textBrowser->setText(tr("ttyS1 OPENED 19200bps 8"));

    delayTimer = new QTimer(this);
    tickTimer  = new QTimer(this);

    tickTimer->start(1000); /* tickInc */
    connect(tickTimer,SIGNAL(timeout()),this,SLOT(tickSlot()));

    delayTimer->start(1000); /* tickInc */
    connect(delayTimer,SIGNAL(timeout()),this,SLOT(stateUpdate()));

    ui->sendlineEdit->setText(" Check New Mail First !");
    ui->sendlineEdit->setStyleSheet("background-color:rgba(0,128,0,128)");

}

Form2::~Form2()
{
    delete ui;
}


void Form2::tickSlot()
{
    tickInc += 1;
}


void Form2::stateUpdate()
{
    QSqlQuery query = QSqlQuery( gDatabase );
    query.prepare( "SELECT  count(*)  from SendTable WHERE FLAG = 1" );
    if( !query.exec( )){                     /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    ui->sendlineEdit_ToSendNum->setText( "ToSend:" + QString::number(query.value(0 ).toInt()));


    query.prepare( "SELECT  count(*)  from imgTable WHERE FLAGNEW = 1" );
    if( !query.exec( )){                     /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    ui->sendlineEdit_RcvedMail->setText( "New Mail:" + QString::number(query.value(0 ).toInt()));

    //  qDebug( ) << "remind:"+query.value(0 ).toString();
}

/******************************************************************************/

void Form2::sbdButtonLock()
{
    flagUartBusy = TRUE;/* 资源互斥锁*/
    ui->pushButton_csq->blockSignals(true);
    ui->pushButton_sbdi->blockSignals(true);
    // ui->pushButton_wb->blockSignals(true);
    ui->pushButton_sbdrb->blockSignals(true);

    ui->sendlineEdit->setStyleSheet("background-color:rgba(0,255,0,255)");

}
void Form2::sbdButtonUnlock()
{
    flagUartBusy = FALSE;
    ui->pushButton_csq->blockSignals(false);
    ui->pushButton_sbdi->blockSignals(false);
    // ui->pushButton_wb->blockSignals(false);
    ui->pushButton_sbdrb->blockSignals(false);

    ui->sendlineEdit->setStyleSheet("background-color: rgb(170, 85, 255);");

}


/******************************************************************************/
void Form2::on_pushButton_clicked()
{
    myCom->write(ui->sendlineEdit->text().toAscii()); //以ASCII码形式将数据写入串口
}

void Form2::on_pushButton_exit_clicked()
{
    emit subClosedSig();
    this->hide();
}

/******************************************************************************/

void Form2::on_pushButton_wb_clicked()
{
    static int sum;
    int i,j;
    QByteArray ba;

    sbdButtonLock();

    QTime _Timer ;/* 延时 */

    /**********************************************/
    ba = "at+sbdd0\r";/* 清空发件箱*/
    myCom->flush();
    myCom->write(ba);

    _Timer = QTime::currentTime().addMSecs(500);   /* 等待清除sbd缓存*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    }while( QTime::currentTime() < _Timer );
    /*********************************************/
    sum = 0;
    j = pgVarible->sbdSendBuf.size();

    if(j > SBD_SENDBUF_MAX){ /* 写入数据szie最大340*/
        qDebug() << "size error ";
        sbdButtonUnlock();/* 按键恢复*/
        return;
    }

    if(j == 0){
        qDebug()<< "nothing to write";
        sbdButtonUnlock();/* 按键恢复*/
        return;
    }

    for(i=0;i<j;i++){
        sum += pgVarible->sbdSendBuf.data()[i];/* 校验和*/
    }

    ba = "at+sbdwb=";
    ba += QString("%1\r").arg(j);        /* 注意加  \r*/

    myCom->write(ba);

  //  ui->sendlineEdit->setText(ba);/* 写入数据*/
    ui->sendlineEdit->setText( QString("Send Mail Size:%1").arg(j) );/* 写入数据*/

    qDebug()<<"at+sbdwb= start"<< QTime::currentTime();

    _Timer = QTime::currentTime().addMSecs(500);
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if(myCom->bytesAvailable() < sizeof("at+xx")){/* 接收数据量小，判定未连接*/
            ui->sendlineEdit->setText("SBD UNCONNECT");
            break;
        }
    }while( QTime::currentTime() < _Timer );
    /*********************************************/
    QByteArray baCheckSum;
    baCheckSum.resize(2);

    baCheckSum.data()[0] = (unsigned char)((sum & 0xff00)>>8);
    baCheckSum.data()[1] = (unsigned char)((sum & 0xff));

    myCom->write(pgVarible->sbdSendBuf + baCheckSum);

    _Timer = QTime::currentTime().addMSecs(1000);   /* 等待清除sbd缓存*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }while( QTime::currentTime() < _Timer );
    /*********************************************/

    ba = myCom->readAll();
    _Timer = QTime::currentTime().addMSecs(500);   /* 等待清除sbd缓存*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    }while( QTime::currentTime() < _Timer );
    /*********************************************/

    qDebug()<<"delay end"<< _Timer;
    sbdButtonUnlock();
}

/******************************************************************************/
/*
 *修改串口状态机，为阻塞式
*/
void Form2::on_pushButton_csq_clicked()
{
    static int tickCnt;
    sbdButtonLock();

    QTime _Timer = QTime::currentTime().addSecs(60);/* 延时最大60秒*/
    qDebug()<<"delay start"<< QTime::currentTime();

    QString qstr;
    int index;
    QByteArray ba;

    pgVarible->sSbdStatus.csq = 0;

    ba = "at+csq\r";
    myCom->flush();/* 这个可能失效*/
    myCom->readAll();/* 所以用这个清空*/

    myCom->write(ba);
    ui->sendlineEdit->setText(ba);

    tickInc = -1;/* 心跳计数多1秒，避免SBD UNCONNECT*/

    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        if(tickInc > 0){
            tickInc = 0;
            tickCnt += 1;

            /***/
            if(myCom->bytesAvailable() < sizeof("at+xx")){/* 接收数据量小，判定未连接*/
                ui->sendlineEdit->setText("SBD UNCONNECT");
                break;
            }

            /***/
            if(myCom->bytesAvailable() >= sizeof("at+csq\rcsq:0\rok\r")){

                qstr = myCom->readAll();
                index = qstr.indexOf("CSQ:");/* 截取数据*/
                if(index < 0){
                    ui->sendlineEdit->setText("Error data return");
                }else{
                    qstr = qstr.mid(index+sizeof("CSQ:")-1, 1);/* 截取一个字*/
                    pgVarible->sSbdStatus.csq = qstr.toInt();/* 记录信号强度*/
                    qDebug()<< "CSQ:"<<qstr;

                    ui->sendlineEdit->setText("CSQ:"+qstr);
                    ui->label_csq->display(pgVarible->sSbdStatus.csq);/* 数码管字体*/
                }
                break;/* 终止循环*/
            }else{

                qDebug()<< QString::number(tickCnt);

                ui->sendlineEdit->setText("Checking Signal Quarlity -"+ QString::number( tickCnt));

                if(tickCnt > CSQ_WAIT_MAX){              /* 如果超时，退出，返回错误*/
                    qDebug()<< "csq over time";
                    ui->sendlineEdit->setText("Error.Time Over-csq");
                    break; /* 终止读取等待循环*/
                }
            }
        }
    }while( QTime::currentTime() < _Timer );

    qDebug()<<"delay end"<< _Timer;
    tickCnt = 0;

    sbdButtonUnlock();
}

/******************************************************************************/
#define SBDI_WAIT_MAX 50   /* 状态机 等待时间50秒*/
void Form2::on_pushButton_sbdi_clicked()
{

    on_pushButton_csq_clicked();

    if(pgVarible->sSbdStatus.csq <  CSQ_MIN){ /* 最小可用信号强度*/
        return;
    }


    sbdButtonLock();/* 需要重启按键屏蔽*/

    QTime _Timer = QTime::currentTime().addSecs(60);/* 延时最大60秒*/

    static int tickCnt;
    QString qstr;
    int index;
    QByteArray ba;
    QStringList list1;

    ba = "at+sbdi\r";
    myCom->flush();
    myCom->write(ba);
    ui->sendlineEdit->setText(ba);

    tickInc = -1;/* 心跳计数多1秒，避免SBD UNCONNECT*/

    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);/* 事件响应*/

        if(tickInc > 0){/* 每个心跳周期，处理串口事物*/
            tickInc = 0;
            tickCnt += 1;
            ui->sendlineEdit->setText( QString::number(tickCnt) );

            /***/
            if(myCom->bytesAvailable() < sizeof("at+xx")){/* 接收数据量小，判定未连接*/
                ui->sendlineEdit->setText("SBD UNCONNECT");
                break;
            }

            /***/
            if(myCom->bytesAvailable() > sizeof("at+sbdi\r+SBDI: 2, 8, 2, 0, 0, 0")){

                qstr = myCom->readAll();
                qstr = qstr.simplified();/* 去除空格和回车*/
                index = qstr.indexOf("+SBDI:");/* 截取数据*/
                index += sizeof("+SBDI:")-sizeof('\0');


                if(index > 0){
                    ui->sendlineEdit->setText("SBDI OK"); /*显示数据接收完成*/

                    qstr = qstr.mid(index);
                    qstr = qstr.remove("OK");

                    list1 = qstr.split(",");
                    pgVarible->sSbdStatus.sbdiSendState = list1.at(0).toInt();
                    pgVarible->sSbdStatus.sbdiSendSN = list1.at(1).toInt();
                    pgVarible->sSbdStatus.sbdiRcvState = list1.at(2).toInt();
                    pgVarible->sSbdStatus.sbdiRcvSN = list1.at(3).toInt();
                    pgVarible->sSbdStatus.sbdiRcvSize = list1.at(4).toInt();
                    pgVarible->sSbdStatus.sbdiUnReadNum = list1.at(5).toInt();

                    qDebug()<< " count      " << list1.count();
                    qDebug()<< " <MO status>" << list1.at(0);
                    qDebug()<< " <MOMSN>    " << list1.at(1);
                    qDebug()<< " <MT status>" << list1.at(2);
                    qDebug()<< " <MTMSN>    " << list1.at(3);
                    qDebug()<< " <MT length>" << list1.at(4);
                    qDebug()<< " <MT queued>" << list1.at(5);



                    if(pgVarible->sSbdStatus.sbdiRcvState == 0){       /* 如没有邮件*/
                        ui->sendlineEdit->setText("No new Mail");
                    }
                    if(pgVarible->sSbdStatus.sbdiRcvState == 1){       /* 如果有邮件*/
                        ui->sendlineEdit->setText("Get new Mail");

                        atCmdRb();/* 启动读取SBD邮件*/
                    }
                    if(pgVarible->sSbdStatus.sbdiRcvState == 2){       /* 出错*/
                        ui->sendlineEdit->setText("Signal weak");
                    }

                    if(pgVarible->sSbdStatus.sbdiUnReadNum > 0){       /* 卫星上还有未读取的邮件*/
                        qDebug()<< "retry  +++";
                        delayTimer-> singleShot(2000, this, SLOT(on_pushButton_sbdi_clicked()));/* 等待重入*/
                    }

                } else {
                    ui->sendlineEdit->setText("Error data return");    /* 有可能错误*/
                }

                break;/* 终止循环*/

            }else{
                qDebug()<<"SBDI 通信等待时间"<< QString::number(tickCnt);

                ui->sendlineEdit->setText("Communicating - "+ QString::number( tickCnt));

                if(tickCnt > CSQ_WAIT_MAX){              /* 如果超时，退出，返回错误*/

                    ui->sendlineEdit->setText("Error.Time Over - sbdi");
                    break; /* 终止读取等待循环*/
                }
            }
        }
    }while( QTime::currentTime() < _Timer );

    tickCnt = 0;

    sbdButtonUnlock();
}


/******************************************************************************/

void Form2::on_pushButton_sbdrb_clicked()
{
    sbdButtonLock();
    qDebug() << "getMailtoSignData ++++++++++++++++++++++++++++++++++++++++";

    int getsize =  getMailtoSignData( & pgVarible->rcvUsignedTransEmai );/* 每次读取都检查能否合成新请求包*/

    if(getsize == 0){
        ui->sendlineEdit->setText("no new mail");
    }
    if(getsize < RCV_SIGN_REQUEST_SIZE && getsize > 0){
        ui->sendlineEdit->setText("need another mail");
    }
    if(getsize == RCV_SIGN_REQUEST_SIZE){
        ui->sendlineEdit->setText("Get New Mult-Sign Request");
        /* 多签名文件准备好*/
        qDebug() << "500 byte 多签名文件准备好";

    }

    sbdButtonUnlock();
}


/******************************************************************************/
void Form2::on_pushButton_wb_3_clicked()/*  debug button */
{
    /* Head	Type	ID	Num	Index	Reserved	Data*/

    QSqlQuery query = QSqlQuery( gDatabase );

    pgVarible->sbdSendBuf.resize(1);
    pgVarible->sbdSendBuf[0] = 0xaa;
    pgVarible->sbdSendBuf[1] = 0x04;
    pgVarible->sbdSendBuf[2] = 0x01;
    pgVarible->sbdSendBuf[3] = 0x02;
    pgVarible->sbdSendBuf[4] = 0x01;
    pgVarible->sbdSendBuf[5] = 0x00;

    pgVarible->sbdSendBuf[6] = 0xa5;
    pgVarible->sbdSendBuf[7] = 0xa5;
    pgVarible->sbdSendBuf[8] = 0xa5;
    pgVarible->sbdSendBuf[9] = 0xa5;



    query.prepare( "SELECT  count(*)  from SendTable " );
    if( !query.exec( )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    qDebug()<< "SendTable size:"<<query.value(0 ).toInt() ;


    if( !query.exec( "SELECT TradId , SbdIndex , SbdRawData , FLAG from SendTable" )){
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    while(query.next()){

        qDebug()<< "TRAD_ID  = "<< query.value( 0 ).toInt();
        qDebug()<< "SbdIndex = "<< query.value( 1 ).toInt();
        qDebug()<< "blob data :"<< query.value( 2 ).toByteArray().size();
        qDebug()<< "FLAG     = "<< query.value( 3 ).toInt();
    } ;

    qDebug() << "测试读取最新邮件的constract ID";
    qDebug()<< "获得的签名请求文件长度"<< getMailtoSignData( & pgVarible->rcvUsignedTransEmai );
    qDebug() << pgVarible->rcvUsignedTransEmai.toHex();

    atCmdWb( & pgVarible->sbdSendBuf);/* 测试sbd写入命令*/
}

/******************************************************************************/

void Form2::on_pushButtonSign_clicked()
{
    int i,j,err;

    QByteArray baHead;
    QByteArray baTemp;
    sbdButtonLock();

    QSqlQuery query = QSqlQuery( gDatabase );

    /* Head	Type	ID	Num	Index	Reserved	Data*/
    baHead.resize(6);
    baHead[0]=0xaa;/* head*/
    baHead[1]=0x04;/* type*/
    baHead[2]=0x00;/* ID*/
    baHead[3]=0x00;/* num*/
    baHead[4]=0x01;/* index*/
    baHead[5]=0x00;

    //   QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    err =  spcProcessfile();    /* 多签程序，必须确保qtumd已经daemon运行*/

    if(err > 0){
        ui->sendlineEdit->setText("Sign Correct");

        this->hide();
        emit openSubSig(5);

        /******************************************/
        i = pgVarible->baMultiSignResultCompressed.size();

        if(i%SEND_PAYLOAD_MAX > 0){
            j = i/SEND_PAYLOAD_MAX + 1;
        }else{
            j = i/SEND_PAYLOAD_MAX ;
        }




        query.prepare( "SELECT NEWCONSTRACTION FROM NewTable " );
        if(!query.exec()) {
            qDebug() << query.lastError();
        }
        query.first();
        i = query.value(0).toInt();
        baHead[2]= (qint8)(i & 0xff);/* 更新交易ID*/
        baHead[5]= (qint8)(i>>8 & 0xff);/* 更新交易ID*/


        query.prepare( "DELETE FROM SendTable " );/* 删除整个数据表*/
        if( !query.exec()){       /* 从数据库读取数据*/
            qDebug() << "Error getting image from table:\n" << query.lastError();
        }

        for(i = 1;i <= j; i++){
            baHead[3]= j;/* num*/
            baHead[4]= i;/* index*/

            baTemp = baHead + pgVarible->baMultiSignResultCompressed.mid((SEND_PAYLOAD_MAX*(i-1)), SEND_PAYLOAD_MAX);

            qDebug() << "baTemp.size() = " << baTemp.size();
            qDebug() << "baTemp.hex = " << baTemp.toHex();

            query.prepare( "INSERT INTO SendTable ( FLAG, TradId, SbdIndex, SbdRawData) VALUES ( :FLAG , :TradId ,:SbdIndex, :SbdRawData)" );
           // query.bindValue( ":SbdRawData", baTemp.mid(0, 10) );
            query.bindValue( ":SbdRawData", baTemp );
            query.bindValue( ":TradId", baTemp.at(2)  );
            query.bindValue( ":SbdIndex", baTemp.at(4)  );
            query.bindValue( ":FLAG", 1  );

            if( !query.exec()){       /* 从数据库读取数据*/
                qDebug() << "Error getting image from table:\n" << query.lastError();
            }
            qDebug()<< "insert" << i <<  " mail to send";
        }



    }else{
        ui->sendlineEdit->setText("Sign Error");
    }

    sbdButtonUnlock();
}



/******************************************************************************/

void Form2::on_pushButton_sendMail_clicked()/* 自动发送所有待发送短信*/
{
    sbdButtonLock();

    QSqlQuery query = QSqlQuery( gDatabase );
    QSqlQuery query1 = QSqlQuery( gDatabase );/* 必须新建一个query */

    query.prepare( "SELECT SbdIndex, SbdRawData from SendTable WHERE FLAG == 1;" );
    if( !query.exec( )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    while(query.next()){
        qDebug()<< "blob size :"<< query.value( 1 ).toByteArray().size();
        qDebug()<< "SbdIndex  = "<< query.value( 0 ).toInt();

        pgVarible->sbdSendBuf = query.value( 1 ).toByteArray();/* 传递参数*/

        on_pushButton_wb_clicked();/* 写入SBD模块*/
        on_pushButton_sbdi_clicked();/* 启动卫星连接，发送数据*/

        if(pgVarible->sSbdStatus.sbdiSendState ==1){           /* 发送成功*/

            QString strdele = QString("UPDATE SendTable SET FLAG = 0 WHERE SbdIndex == %1 ;")
                    .arg(query.value( 0 ).toInt());

            if( !query1.exec( strdele )){       /* 从数据库读取数据*/
                qDebug() << "Error getting image from table:\n" << query.lastError();
            }

            qDebug() << "发送成功一条";
        }
    };



    query.prepare( "SELECT  count(*)  from SendTable WHERE FLAG = 1" );
    if( !query.exec( )){                     /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    if(query.value(0 ).toInt() == 0){/* 全部发送成功，执行接收邮件标注工作*/

        qDebug() << "全部发送成功，执行接收邮件标注工作";


        if( !query.exec( "SELECT  NEWCONSTRACTION  , MailCnt ,DEALED FROM NewTable " )){       /* 从数据库读取数据*/
            qDebug() << "Error getting image from table:\n" << query.lastError();
        }
        query.first();
        qDebug()<< "NEWCONSTRACTION: "<< query.value(0 ).toInt();
        qDebug()<< "MailCnt        : "<< query.value(1 ).toInt();
        qDebug()<< "DEALED         : "<< query.value(2 ).toInt();
        query.first();
        int  constrctID = query.value(0).toInt();/* 交易ID*/
        int  mailCnt = query.value(1).toInt();/* 邮件数量*/

        qDebug() << "constrctID=" <<constrctID;
        qDebug() << "mailCnt   =" << mailCnt;

        /* 把已经完成的交易标注flagNew = 0*/
        for(int i=1; i<=mailCnt; i++ ){
            QString strdele = QString("UPDATE imgTable SET FLAGNEW = 0 WHERE SbdIndex == %1 AND TradId == %2 ")
                    .arg(i)
                    .arg(constrctID);

            if( !query.exec( strdele )){
                qDebug() << "Error getting image from table:\n" << query.lastError();
            }
        }
    }

    sbdButtonUnlock();
}

/******************************************************************************/
void Form2::on_pushButtonSignCheck_clicked()
{
    this->hide();
    emit openSubSig(5);
}

/******************************************************************************/
int Form2:: getMailtoSignData(QByteArray * baResult)
{
    QSqlQuery query = QSqlQuery( gDatabase );
    QByteArray baTemp;

    /*= 第一步，获取最大ID，也就是最新接收的短信=*/
    QString  strGetMaxID = QString("SELECT  MAX(ID) from imgTable WHERE FLAGNEW == 1 ") ;
    if( !query.exec( strGetMaxID)){            /* 从数据库读取最新 未签名成功的邮件的ID*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    int maxIdVal = query.value( 0 ).toInt();   /* 获取最大ID，最新的mail*/

    /*= 第二部步，读取最大ID对应的短信内容=*/
    QString  strGetConstractionId = QString("SELECT TradId, SbdRawData  from imgTable WHERE ID == %1 AND FLAGNEW = 1;").arg(maxIdVal) ;
    if( !query.exec( strGetConstractionId)){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    int newConstraId = query.value( 0 ).toInt();/* 最新mail里面的交易编号*/
    baTemp = query.value( 1 ).toByteArray();

    /*= 第三步，更新newTable的 交易编号 分包数量 =*/
    query.prepare( "UPDATE NewTable SET NEWCONSTRACTION = :newid , MailCnt = :mailCnt" );
    query.bindValue( ":newid", newConstraId );/* 交易编号更新*/
    query.bindValue( ":mailCnt", baTemp.at(3) );/* 交易分包数量*/
    if(!query.exec()) {
        qDebug() << query.lastError();
    }

    /*= 第四步，查询目标交易编号短信数量，对比分包数量=*/
    QString  chechCnt = QString("SELECT  count(*) from imgTable WHERE TradId == %1 ").arg(newConstraId) ;
    if( !query.exec( chechCnt)){                 /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    int rcvedCnt = query.value( 0 ).toInt();     /* 最新ID下的邮件数量*/

    qDebug() << " 邮件数量 rcvedCnt =" << rcvedCnt ;
    qDebug() << " 邮件ID newConstraId =" << newConstraId ;

    QString  getNewRequest;

    baResult->clear();/* 未签名交易缓存*/

    if( rcvedCnt >= baTemp.at(3) ){/* 对比分包数量*/
        /*= 第五步，如果邮件数量足够，进行合包操作*/

        for(int i =1; i <= rcvedCnt; i++){      /* 错误 index从1开始计数*/
            getNewRequest = QString("SELECT  SbdRawData from imgTable WHERE TradId == %1 AND SbdIndex == %2")
                    .arg(newConstraId)
                    .arg(i);

            qDebug() << getNewRequest;

            if( !query.exec( getNewRequest )){        /* 从数据库读取数据*/
                qDebug() << "Error getting image from table:\n" << query.lastError();
                break;       /* 退出循环*/
            }

            query.first();
            baResult->append(query.value( 0 ).toByteArray().mid(6) );/* 截取数据*/

            qDebug() << baResult->size();
            if(baResult->size() == RCV_SIGN_REQUEST_SIZE){
                break;
            }
        }
        return (baResult->size());/* 返回长度*/
    }else{

        // ui->sendlineEdit->setText("need another mail");/* 显示消息*/
        return 0;
    }


}


int Form2:: getRecvedMailNumber( )
{
    qDebug()<<"getRecvedMailNumber++++++++++++++++++++++++++++++++++++++++++++++++++++";

    QSqlQuery query = QSqlQuery( gDatabase );


    QString  chechCnt = QString("SELECT  count(*) from imgTable WHERE FLAGNEW == 1 ") ;
    if( !query.exec( chechCnt)){                 /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    int rcvedCnt = query.value( 0 ).toInt();     /* 最新ID下的邮件数量*/

    qDebug() << " 邮件数量 rcvedCnt =" << rcvedCnt ;

    return rcvedCnt;
}

int Form2::atCmdCsq()
{

    static int tickDelaySecondCnt = 0;
    int csqVal = -1;

    QTime _Timer = QTime::currentTime().addSecs(60);/* 延时最大60秒*/

    QString qstr;
    int index;
    QByteArray ba;

    ba = "at+csq\r";
    myCom->flush();                 /* 这个可能失效*/
    myCom->readAll();               /* 所以用这个清空*/
    myCom->write(ba);
    ui->sendlineEdit->setText(ba);

    tickInc = -1;                   /* 心跳计数多1秒，避免SBD UNCONNECT*/

    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        if(tickInc > 0){/* 心跳边沿*/
            tickInc = 0;

            tickDelaySecondCnt += 1;

            /***/
            if(myCom->bytesAvailable() < sizeof("at+xx")){/* 接收数据量小，判定未连接*/
                ui->sendlineEdit->setText("SBD UNCONNECT");
                break;
            }

            /***/
            if(myCom->bytesAvailable() >= sizeof("at+csq\rcsq:0\rok\r")){

                qstr = myCom->readAll();
                index = qstr.indexOf("CSQ:");/* 截取数据*/
                if(index < 0){
                    ui->sendlineEdit->setText("Error data return");
                }else{
                    qstr = qstr.mid(index+sizeof("CSQ:")-1, 1);/* 截取信号强度*/
                    csqVal = qstr.toInt();  /* 记录信号强度*/
                }
                break;/* 终止循环*/
            }else{

                qDebug()<< QString::number(tickDelaySecondCnt);

                if(tickDelaySecondCnt > CSQ_WAIT_MAX){              /* 如果超时，退出，返回错误*/
                    qDebug()<< "csq over time";
                    ui->sendlineEdit->setText("csq over time");
                    break; /* 终止读取等待循环*/
                }
            }
        }
    }while( QTime::currentTime() < _Timer );

    tickDelaySecondCnt = 0;

    return  csqVal;
}

void Form2::atCmdWb(QByteArray * baToSend)
{
    static int sum;
    int i,j;
    QByteArray ba;

    sbdButtonLock();

    QTime _Timer ;/* 延时 */

    /**********************************************/
    ba = "at+sbdd0\r";/* 清空发件箱*/
    myCom->flush();
    myCom->write(ba);

    _Timer = QTime::currentTime().addMSecs(500);   /* 等待清除sbd缓存*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    }while( QTime::currentTime() < _Timer );
    /*********************************************/
    sum = 0;
    j =  baToSend->size() ;

    if(j > SBD_SENDBUF_MAX){ /* 写入数据szie最大340*/
        qDebug() << "size error ";
        sbdButtonUnlock();/* 按键恢复*/
        return;
    }

    if(j == 0){
        qDebug()<< "nothing to write";
        sbdButtonUnlock();/* 按键恢复*/
        return;
    }

    for(i=0;i<j;i++){
        sum += baToSend->data()[i];   /* 校验和*/
    }

    /*********************************************/
    ba  = QString("at+sbdwb=%1\r").arg(j).toLatin1();        /* 注意加  \r*/
    myCom->write(ba);

    _Timer = QTime::currentTime().addMSecs(500);
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        if(myCom->bytesAvailable() < sizeof("at+xx")){  /* 接收数据量小，判定未连接*/
            ui->sendlineEdit->setText("SBD UNCONNECT");
            break;
        }
    }while( QTime::currentTime() < _Timer );
    /*********************************************/
    QByteArray baCheckSum;
    baCheckSum.resize(2);
    baCheckSum.data()[0] = (unsigned char)((sum & 0xff00)>>8);
    baCheckSum.data()[1] = (unsigned char)((sum & 0xff));

    baToSend->append(baCheckSum);/* 加上校验和后缀*/
    myCom->write(*baToSend );

    _Timer = QTime::currentTime().addMSecs(1000);   /*  延时1000ms*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }while( QTime::currentTime() < _Timer );
    /*********************************************/

    ba = myCom->readAll();/* 执行一次串口清空*/

    qDebug() << ba;

    _Timer = QTime::currentTime().addMSecs(500);   /*  延时500ms*/
    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }while( QTime::currentTime() < _Timer );

}

/**
 * AT命令，读取接收缓存
*/

int Form2::atCmdRb( )
{
    static int tickCnt;
    int newMailNumber = 0;

    QTime _Timer = QTime::currentTime().addSecs(60);/* 延时最大60秒*/

    QSqlQuery query = QSqlQuery( gDatabase );
    QByteArray ba;

    ba = "at+sbdrb\r";
    myCom->flush();
    myCom->readAll();
    myCom->write(ba);
    ui->sendlineEdit->setText(ba);

    tickInc = -1;/* 心跳计数多1秒，避免SBD UNCONNECT*/

    do{
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        if(tickInc > 0){
            tickInc = 0;
            tickCnt += 1;
            ui->sendlineEdit->setText( QString::number(tickCnt) );

            /***/
            if(myCom->bytesAvailable() < sizeof("at+xx")){   /* 接收数据量小，判定未连接*/
                ui->sendlineEdit->setText("SBD HardWare Error");
                break;
            }

            /***/
            if(myCom->bytesAvailable() >= sizeof("at+csq\rcsq:0\rok\r")){
                ba = myCom->readAll();

                int rcvSize;
                int constraID;
                rcvSize = ba.at(9) *256;   /* 解析数据长度*/
                rcvSize += ba.at(10);
                ba = ba.mid(11, rcvSize);  /* 盲解析返回值*/
                constraID = ba.at(2) + ba[5]*256;

                qDebug()<< "rcv    :" << ba;
                qDebug()<< "rcvSize:"<< rcvSize;
                qDebug()<< "constraID:"<< constraID;

                if(rcvSize == 0){
                    newMailNumber = 0;
                    break;/* 读取数据为空，终止循环*/
                }else{
                    query.prepare( "INSERT INTO imgTable ( TradId, SbdIndex, SbdRawData, FLAGNEW) VALUES ( :TradId ,:SbdIndex, :SbdRawData, :FLAGNEW)" );
                    query.bindValue( ":SbdRawData", ba  );
                    query.bindValue( ":TradId", constraID ); /* [2]+[5]*256*/
                    query.bindValue( ":SbdIndex", ba.at(4) );/* [4] 一个交易包组合的序号*/
                    query.bindValue( ":FLAGNEW", 1 ); /* 未完成标志*/

                    if(!query.exec()) {
                        qDebug() << query.lastError();
                    }else{

                        newMailNumber = 1;/* 返回值，新邮件数量*/

                        ba = "at+sbdd1\r";/* 清空收件箱*/
                        myCom->write(ba);
                    }
                }
                break;/* 终止循环*/

            }else{
                qDebug()<< QString::number(tickCnt);
                ui->sendlineEdit->setText("waiting -"+ QString::number( tickCnt));
                if(tickCnt > CSQ_WAIT_MAX){              /* 如果超时，退出，返回错误*/
                    qDebug()<< "csq over time";
                    ui->sendlineEdit->setText("Check Signal Quarlet timeOver");
                    break; /* 终止读取等待循环*/
                }
            }
        }
    }while( QTime::currentTime() < _Timer );

    tickCnt = 0;

    return newMailNumber;


}
