#include <QApplication>
#include <QPixmap>
#include <QProcess>
#include <QPixmap>
#include <QProcess>
#include <QSplashScreen>
#include <QDateTime>
#include <QFont>

#include <QtCore/QCoreApplication>
#include <QSettings>

#include <QFile>

#include "mainwidget.h"

#include <QStringList>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QVariantList>
#include <QtSql>
#include <QTextCodec>


sbdiInfo_t  sbdiInfo;

gVarible_t   gVarible;
gVarible_t*  pgVarible;


QSqlDatabase gDatabase;
/******************************************************************************/

int main(int argc, char *argv[])
{
    QProcess  process;

    QApplication a(argc, argv);


    //   QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


    pgVarible = &gVarible;

    qApp->setFont(QFont("Microsoft YaHei", 12));

    QPixmap pixmap(":/new/prefix1/open");
    QSplashScreen screen(pixmap);
    screen.show();

    mainWidget w;

    screen.finish(&w);

    w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();



#define  DB_PATH  "/root/sign/myDatabase.db3"
    QString dbName( DB_PATH  );

    gDatabase = QSqlDatabase::addDatabase( "QSQLITE" );
    gDatabase.setDatabaseName( dbName );
    gDatabase.open();


    QSqlQuery query = QSqlQuery( gDatabase );


    qDebug()<<"程序开始--------";

    query.exec( "CREATE TABLE IF NOT EXISTS imgTable ( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, TradId INT, SbdIndex INT, SbdSN INT, FLAGNEW INT , SbdRawData BLOB )" );

    query.exec( "CREATE TABLE IF NOT EXISTS SendTable ( FLAG INT, TradId INT, SbdIndex INT,  SbdRawData BLOB )" );

    /* newtable 记录收到的最新邮件的ID */
    query.exec( "CREATE TABLE IF NOT EXISTS NewTable ( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, NEWCONSTRACTION INT , MailCnt INT , DEALED  INT )" );

    query.exec( "CREATE TABLE IF NOT EXISTS RECORDTable ( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, ADDRESS CHAR(35) , MONEY INT , SbdRawData BLOB   )" );

#if 0

    if( !query.exec(  "SELECT  count(*) from imgTable " )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    if(query.value(0 ).toInt() == 0){
        qDebug()<<"空表填充数据";

        QByteArray inByteArray;
        QString filename ="/root/sign/1.sbd" ;
        QFile filetemp(filename);

        if(!filetemp.open(QIODevice::ReadOnly)){
            qDebug()<<"here 123123";
        }else{
            inByteArray = filetemp.readAll();
            filetemp.close();
        }

        query.prepare( "INSERT INTO imgTable ( TradId, SbdIndex, FLAGNEW, SbdRawData) VALUES ( :TradId ,:SbdIndex, :FLAGNEW, :SbdRawData)" );
        query.bindValue( ":SbdRawData", inByteArray );
        query.bindValue( ":TradId", inByteArray.at(2)  );
        query.bindValue( ":SbdIndex", inByteArray.at(4)  );
        query.bindValue( ":FLAGNEW", 1);/* new mail */

        if( !query.exec() ){                                                        /* 执行写入*/
            qDebug() << "Error inserting image into table:\n" << query.lastError();
        }


        filename ="/root/sign/2.sbd" ;
        QFile filetemp2(filename);

        if(!filetemp2.open(QIODevice::ReadOnly)){
            qDebug()<<"here 123123";
        }else{
            inByteArray = filetemp2.readAll();
            filetemp2.close();
        }

        query.prepare( "INSERT INTO imgTable ( TradId, SbdIndex, FLAGNEW, SbdRawData) VALUES ( :TradId ,:SbdIndex, :FLAGNEW, :SbdRawData)" );
        query.bindValue( ":SbdRawData", inByteArray );
        query.bindValue( ":TradId", inByteArray.at(2)  );
        query.bindValue( ":SbdIndex", inByteArray.at(4)  );
        query.bindValue( ":FLAGNEW", 1);/* new mail */

        if( !query.exec() ){                                                        /* 执行写入*/
            qDebug() << "Error inserting image into table:\n" << query.lastError();
        }

    }

#endif
//    if( !query.exec( "DELETE  from imgTable WHERE TradId = 0" )){       /* 从数据库读取数据*/
//        qDebug() << "Error getting image from table:\n" << query.lastError();
//    }
//    if( !query.exec( "UPDATE imgTable SET FLAGNEW = 1 WHERE TradId = 51622" )){       /* 从数据库读取数据*/
//        qDebug() << "Error getting image from table:\n" << query.lastError();
//    }

    qDebug()<<"启动+++";
    if( !query.exec( "SELECT  ID , TradId , SbdIndex, SbdRawData from imgTable" )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }

    while(query.next())  {
        // qDebug()<< "blob data :"<< query.value( 3 ).toByteArray().toHex();
        qDebug()<< "blob size :"<< query.value( 3 ).toByteArray().size();
        qDebug()<< "ID       = "<< query.value( 0 ).toInt();
        qDebug()<< "TRAD_ID  = "<< query.value( 1 ).toInt();
        qDebug()<< "SbdIndex = "<< query.value( 2 ).toInt();
    }

    QString  chechCnt = "SELECT  count(*) from imgTable WHERE FLAGNEW == 1";
    if( !query.exec( chechCnt )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    qDebug()<< "count: "<< query.value(0 ).toInt();


    chechCnt = "SELECT  count(*) from NewTable ";
    if( !query.exec( chechCnt )){       /* 从数据库读取数据*/
        qDebug() << "Error getting image from table:\n" << query.lastError();
    }
    query.first();
    qDebug()<< "count: "<< query.value(0 ).toInt();
    if(query.value(0 ).toInt() == 0){
        if( !query.exec( "INSERT INTO NewTable ( NEWCONSTRACTION  , MailCnt ,DEALED ) VALUES (133, 2, 0 )" )){       /* 从数据库读取数据*/
            qDebug() << "Error getting image from table:\n" << query.lastError();
        }
        qDebug() << "newtable 插入一条记录";
    }else{
        if( !query.exec( "SELECT  NEWCONSTRACTION  , MailCnt ,DEALED FROM NewTable " )){       /* 从数据库读取数据*/
            qDebug() << "Error getting image from table:\n" << query.lastError();
        }

        query.first();
        qDebug()<< "NEWCONSTRACTION: "<< query.value(0 ).toInt();
        qDebug()<< "MailCnt        : "<< query.value(1 ).toInt();
        qDebug()<< "DEALED         : "<< query.value(2 ).toInt();

    }



    // db.close();


    return a.exec();
}
