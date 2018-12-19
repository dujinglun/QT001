#include "hex.h"


/*********************************************************************************************************
** 函数名称: spcCharToHex
** 功能描述: 把字符转成二进制数
** 输　入  : cIn
** 输　出  : 结果
** 全局变量:
** 调用模块:
*********************************************************************************************************/
unsigned char spcCharToHex(char cIn)
{
    if ((cIn >= '0') && (cIn <= '9'))
    {
        return (cIn - '0');
    }
    else if ((cIn >= 'a') && (cIn <= 'f'))
    {
        return (cIn - 'a' + 10);
    }
    else if ((cIn >= 'A') && (cIn <= 'F'))
    {
        return (cIn - 'A' + 10);
    }
    else
    {
        return (-1); /*  NOP                         */
    }
}
/*********************************************************************************************************
** 函数名称: spcHexToChar
** 功能描述: 把字符转成二进制数
** 输　入  : cIn
** 输　出  : 结果
** 全局变量:
** 调用模块:
*********************************************************************************************************/
char spcHexToChar(unsigned char  ucIn)
{
    if ((ucIn >= 0) && (ucIn <= 9))
    {
        return (ucIn + '0');
    }
    else if ((ucIn >= 10) && (ucIn <= 15))
    {
        return (ucIn - 10 + 'a');
    }
    else
    {
        return (-1); /*  NOP                         */
    }
}


#if  0
char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}


void StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();

    char lstr,hstr;

    if((len & 0x0001) > 0){
        senddata.resize(len/2+1);
    }else{
        senddata.resize(len/2);
    }

    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toAscii();
        if(hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if(i >= len)
            break;

        lstr = str[i].toAscii();

        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);

        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

#else

void StringToHex(QString str, QByteArray &senddata  )
{
    int iInIndex = 0;
    int iOutIndex = 0;

    unsigned char temp;

    int iLen = str.length();


    if((iLen & 0x0001) > 0){
        senddata.resize(iLen/2+1);
    }else{
        senddata.resize(iLen/2);
    }


    for (iInIndex = 1,iOutIndex=0; iInIndex < iLen; iInIndex+=2,iOutIndex+=1)
    {
        senddata[iOutIndex]  = (spcCharToHex(  str[iInIndex].toAscii()  ));
    }

    for (iInIndex = 0,iOutIndex=0; iInIndex < iLen; iInIndex+=2,iOutIndex+=1)
    {
        temp = senddata[iOutIndex]  ;
        temp += (spcCharToHex(str[iInIndex].toAscii())<<4);
        senddata[iOutIndex] =temp;

    }

    // return (iInIndex);
}






//bool saveFile(const QString &fileFullName)
//{
//    if(fileFullName.isEmpty())
//        return false;

//    QFile     fileOut;
//    QTextStream      outStream;

//    fileOut.setFileName(fileFullName);
//    if(!fileOut.open(QIODevice::WriteOnly))
//    {
//        qDebug("文件:%s写入失败!\n",qPrintable(fileFullName));
//        return false;
//    }
//    outStream.setDevice(&fileOut);
//    outStream << QObject::tr("[Config]\r\n");
//    outStream << QObject::tr("Abstract=testName\r\n"));
//    outStream << QObject::tr("Directory=%1\r\n").arg(QCoreApplication::applicationDirPath());
//    fileOut.close();
//    return true;
//}


//bool loadFile(const QString &fileFullName)
//{
//    if(fileFullName.isEmpty())
//        return false;

//    QFileInfo fileInfo(fileFullName);
//    if(!fileInfo.exists())//判断文件是否存在
//    {
//        qDebug("配置文件不存在,读取失败!\n");
//        return false;
//    }
//    QFile        fileIn;
//    QTextStream        *inStream;
//    QByteArray          fileByteArray;
//    int             fileSize;
//    char                fileBuf[CONFIG_FILE_MAX_SIZE];

//    fileIn.setFileName(fileFullName);
//    if(!fileIn.open(QIODevice::ReadOnly))
//    {
//        qDebug("文件:%s读取失败!\n",qPrintable(fileFullName));
//        return false;
//    }

//    fileSize = fileIn.read(fileBuf, CONFIG_FILE_MAX_SIZE);
//    fileIn.close();

//    /* 读取到类对象中 */
//    fileByteArray = QByteArray::fromRawData(fileBuf, fileSize);
//    inStream = new QTextStream(fileByteArray, QIODevice::ReadOnly);
//    QString        tempString;
//    tempString = inStream->readLine();
//    if(tempString != tr("[Config]"))
//    {
//        qDebug("配置文件组名错误!\n");
//        return false;
//    }
//    while (!(inStream->atEnd()))
//    {
//        tempString = inStream->readLine();
//        if(tempString.startsWith(tr("Abstract=")))
//            QString testName = tempString.mid(9);
//        else if(tempString.startsWith(tr("Directory=")))
//            QString Directory = tempString.mid(10);
//        else
//        {
//            qDebug("无效项!\n");
//        }
//    }
//    delete inStream;
//    fileIn.close();
//    return true;
//}














#endif

