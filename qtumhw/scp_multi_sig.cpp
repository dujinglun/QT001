/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: spc_slave.c
**
** 创   建   人: Zhang.ZhaoGe (张兆阁)
**
** 文件创建日期: 2018 年 07 月 15 日
**
** 描        述: spc 从机端代码 (模拟了载荷端功能)
*********************************************************************************************************/

#include "scp_multi_sig.h"

#include <QDebug>
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;


/*********************************************************************************************************
  数据类型定义
  [start flag 0x5A][2B long][xB data][end flag 0xA5][...]
*********************************************************************************************************/


/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/
static unsigned char _G_ucAllSendBuf[ALLSENDBUFLEN]; /*  发送数据总缓冲区            */
static unsigned char _G_ucAllRcvBuf[ALLRCVBUFLEN]; /*  接收数据总缓冲区            */
static int _G_uiAllRcvDataIndex = 0;               /*  接收数据总缓冲索引          */


/*********************************************************************************************************
** 函数名称: spcStringStrip
** 功能描述: 去除缓冲区中的回车和空格
** 输　入  : cString ：需要格式化的缓冲区
**         : iLen    ：字符串长度
** 输　出  : 格式化后的字符串首地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/

/*********************************************************************************************************
** 函数名称: spccharToHex
** 功能描述: 把字符转成二进制数
** 输　入  : cIn
** 输　出  : 结果
** 全局变量:
** 调用模块:
*********************************************************************************************************/
unsigned char spccharToHex(char cIn)
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
** 函数名称: spcHexTochar
** 功能描述: 把字符转成二进制数
** 输　入  : cIn
** 输　出  : 结果
** 全局变量:
** 调用模块:
*********************************************************************************************************/
char spcHexTochar(unsigned char ucIn)
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
/*********************************************************************************************************
** 函数名称: spcStringToHex
** 功能描述: 把 hash 字符串转换成二进制数据格式（"0123456789abcdef" --- 0x0123456789abcdef）
** 输　入  : cString    ：字符串地址
**         : iLen       ：字符串长度
**         : ucOutData  ：转换结果
** 输　出  : 成功转换的字符个数
** 补充信息: 需要注意输入字符串字符个数是奇数的情况以及 ucOutData 在调用的时候需要清空一下
** 全局变量:
** 调用模块:
*********************************************************************************************************/
int spcStringToHex(unsigned char *ucOutData, char *cString, int iLen)
{
    char *cData = cString;
    int iInIndex = 0;
    int iOutIndex = 0;

    for (iInIndex = 0; iInIndex < iLen; iInIndex++)
    {
        if ((iInIndex % 2) == 0)
        {
            /*
             * 奇数，放一个字节的高四位
             */
            *(ucOutData + iOutIndex) |= (spccharToHex(*(cData + iInIndex)) << 4);
        }
        else
        {
            /*
             * 偶数，放一个字节的低四位
             */
            *(ucOutData + iOutIndex) |= (spccharToHex(*(cData + iInIndex)));
            iOutIndex++;
        }
    }

    return (iInIndex);
}
/*********************************************************************************************************
** 函数名称: spcHexToString
** 功能描述: 把二进制数据格式转换成 hash 字符串（0x0123456789abcdef --- "0123456789abcdef"）
** 输　入  : cString     ：二进制数据地址
**         : iLen        ：有效的字节个数
**         : bIs         ：ucInData 最后一个字节数据是否只有一半有效（输出字符个数是否为奇数个）
**         : cOutString  ：转换结果
** 输　出  : 字符串长度
** 全局变量:
** 调用模块:
*********************************************************************************************************/
int spcHexToString(char *cOutString, unsigned char *ucInData, int iLen, bool bIs)
{
    unsigned char *ucData = ucInData;
    int iInIndex = 0;
    int iOutIndex = 0;

    for (iInIndex = 0; iInIndex < iLen; iInIndex++)
    {
        *(cOutString + iOutIndex) = spcHexTochar(((*(ucData + iInIndex)) & 0xF0) >> 4);
        iOutIndex++;
        *(cOutString + iOutIndex) = spcHexTochar((*(ucData + iInIndex)) & 0x0F);
        iOutIndex++;
    }

    if (bIs)
    {
        iOutIndex--;
    }
    *(cOutString + iOutIndex) = '\0';

    return (iOutIndex);
}
/*********************************************************************************************************
** 函数名称: spcRcvDataToCmdString
** 功能描述: 把接收到的压缩数据转换成签名时执行的字符串命令
** 输　入  : ucDest：存储结果的缓冲区
**         : ucSrc ：接收的压缩数据
**         : iLen  ：ucDest 缓冲区长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
int spcRcvDataToCmdString(char *ucDest, int iLen, unsigned char *ucSrc)
{
    int ret;
    int iIndex = 0;
    int iTxidIndex = 2;
    ProtocolDataHead *pData = (ProtocolDataHead *)ucSrc;


    memset(ucDest, '\0', iLen);/* 初始化数组为0000*/


    if (pData->ucStartFlag == 0xba) /*  交易编码数据                */
    {
        memcpy(ucDest + iIndex, SIGCMDSTR, strlen(SIGCMDSTR));/* 命令头*/
        iIndex += strlen(SIGCMDSTR);

        ret = spcHexToString(ucDest + iIndex, &pData->ucData[0],
                             pData->usDataLong & 0x7FFF, pData->usDataLong & 0x8000);
        iIndex += ret;
        pData = (ProtocolDataHead *)((unsigned char *)pData + (3 + (pData->usDataLong & 0x7FFF)));
    }


    if (pData->ucStartFlag == 0xca) /*  txid 数组数据               */
    {
        unsigned char i;

        char cPrivKeyIndex[6 + strlen(PRIVKEYFILEDIC)] = {0};

        iTxidIndex = 1;
        memcpy(ucDest + iIndex, STARTSTR, strlen(STARTSTR));
        iIndex += strlen(STARTSTR);

        for (i = 0; i < pData->ucData[0]; i++)
        {
            /*
             * TXID
             */
            memcpy(ucDest + iIndex, TXIDSTR, strlen(TXIDSTR));
            iIndex += strlen(TXIDSTR);
            ret = spcHexToString(ucDest + iIndex, &pData->ucData[iTxidIndex + 1],
                                 pData->ucData[iTxidIndex] & 0x7F, pData->ucData[iTxidIndex] & 0x80);
            iIndex += ret;
            iTxidIndex += (1 + (pData->ucData[iTxidIndex] & 0x7F));

            /*
             * VOUT
             */
            memcpy(ucDest + iIndex, VOUTSTR, strlen(VOUTSTR));
            iIndex += strlen(VOUTSTR);

            sprintf(ucDest + iIndex, "%d", pData->ucData[iTxidIndex]);
            do
            {
                iIndex += 1;
                pData->ucData[iTxidIndex] /= 10;

            } while (pData->ucData[iTxidIndex] > 0);

            iTxidIndex += 1;

            /*
             * scriptPubKey
             */
            memcpy(ucDest + iIndex, PUBKEYSTR, strlen(PUBKEYSTR));
            iIndex += strlen(PUBKEYSTR);
            ret = spcHexToString(ucDest + iIndex, &pData->ucData[iTxidIndex + 2],
                                 (pData->ucData[iTxidIndex] | (pData->ucData[iTxidIndex + 1] << 8)) & 0x7FFF,
                                 pData->ucData[iTxidIndex + 1] & 0x80);
            iIndex += ret;
            iTxidIndex += (2 + ((pData->ucData[iTxidIndex] | (pData->ucData[iTxidIndex + 1] << 8)) & 0x7FFF));

            /*
             * redeemScript
             */
            memcpy(ucDest + iIndex, REDEEMSCRIPTSTR, strlen(REDEEMSCRIPTSTR));
            iIndex += strlen(REDEEMSCRIPTSTR);
            ret = spcHexToString(ucDest + iIndex, &pData->ucData[iTxidIndex + 2],
                                 (pData->ucData[iTxidIndex] | (pData->ucData[iTxidIndex + 1] << 8)) & 0x7FFF,
                                 pData->ucData[iTxidIndex + 1] & 0x80);
            iIndex += ret;
            iTxidIndex += (2 + ((pData->ucData[iTxidIndex] | (pData->ucData[iTxidIndex + 1] << 8)) & 0x7FFF));

            /*
             * ENDENDSTR
             */
            memcpy(ucDest + iIndex, ENDSTR, strlen(ENDSTR));
            iIndex += strlen(ENDSTR);
        }

        iIndex -= 1; /*  去掉 ENDSTR 中的 ','        */
        memcpy(ucDest + iIndex, ENDENDSTR, strlen(ENDENDSTR));
        iIndex += strlen(ENDENDSTR);

        memcpy(ucDest + iIndex, PRIVKEYSTARTSTR, strlen(PRIVKEYSTARTSTR));
        iIndex += strlen(PRIVKEYSTARTSTR);

        /*
         * 读取索引值，然后到文件中找到相应的秘钥，拼接到命令中
         */
        memcpy(cPrivKeyIndex, PRIVKEYFILEDIC, strlen(PRIVKEYFILEDIC));
        sprintf(cPrivKeyIndex + strlen(PRIVKEYFILEDIC), "%d", pData->ucData[iTxidIndex] | (pData->ucData[iTxidIndex + 1] << 8));

      //  qDebug(cPrivKeyIndex);

        FILE* iFd;
        size_t  ssReadNum;

        iFd = fopen(cPrivKeyIndex, "r");
        if (NULL == iFd) {
            qDebug("failed to open key %s!\n", cPrivKeyIndex);
            return (-1);
        } else {
            qDebug("sucess to open key %s!\n", cPrivKeyIndex);
        }


        fseek(iFd, 0, SEEK_SET);
        ssReadNum = fread( ucDest + iIndex, 1 , PRIVKEYLEN, iFd);
        if (ssReadNum != PRIVKEYLEN)
        {
            printf("read private key %s file failed\n", cPrivKeyIndex);
            return (-1);
        }

        fclose(iFd);

        iIndex += PRIVKEYLEN;
        iTxidIndex += 2;
        memcpy(ucDest + iIndex, PRIVKEYENDSTR, strlen(PRIVKEYENDSTR));
    }


    qDebug()<< pData->ucData[iTxidIndex];

    return ((pData->ucData[iTxidIndex] == 0x1a) ? 0 : -1);
}

/*********************************************************************************************************
** 函数名称: spcImportPrivKey
** 功能描述: 导入冷钱包私钥
** 输　入  : cPrivKey  ：私钥字符串地址
**         : ucPrivNum ：私钥个数
** 输　出  : 操作结果字符串长度（含'\0'），结果存储在了 _G_ucAllSendBuf 中
** 全局变量:
** 调用模块:
*********************************************************************************************************/
int spcImportPrivKey(char *cPrivKey, unsigned char ucPrivNum)
{
    unsigned char ucIndex;
    char cKey[IMPORTPRIVKEYLEN + PRIVKEYLEN + 1];
    FILE *sigResult;
    size_t sSigDataNum = 0;

    cKey[IMPORTPRIVKEYLEN + PRIVKEYLEN] = '\0';
    memcpy(cKey, IMPORTPRIVKEYSTR, IMPORTPRIVKEYLEN);

    for (ucIndex = 0; ucIndex < ucPrivNum; ucIndex++)
    {
        memcpy(cKey + IMPORTPRIVKEYLEN, cPrivKey + (ucIndex * PRIVKEYLEN), PRIVKEYLEN);

        printf("\n%s\n", cKey);

        sigResult = popen((const char *)cKey, "r");
        if (sigResult == NULL)
        {
            return (-1);
        }

        if ((sizeof(_G_ucAllSendBuf) - sSigDataNum) > 0)
        {
            sSigDataNum += fread(_G_ucAllSendBuf + sSigDataNum, sizeof(char),
                                 sizeof(_G_ucAllSendBuf) - sSigDataNum, sigResult);
            /*  导入私钥操作结果预留 100B   */
        }

        pclose(sigResult);
        //        sleep(1);                                                       /*  每导入一个密钥等待1秒       */
    }

    return (sSigDataNum + ucIndex);
}

/*********************************************************************************************************
** 函数名称: spcProcessfile
** 功能描述: 从文件读取签名请求数据包
** 输　入  : 无
** 输　出  : 无
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#define ERR_0   0  /* 返回错误*/
#define ERR_OK  1
int spcProcessfile()
{
    qDebug()<< "enter scpProcess";

    _G_uiAllRcvDataIndex = pgVarible->rcvUsignedTransEmai.size();
    memcpy( _G_ucAllRcvBuf, pgVarible->rcvUsignedTransEmai,  _G_uiAllRcvDataIndex );/* 全局变量传递数据*/

    qDebug() << _G_uiAllRcvDataIndex;/* 打印数据长度*/


    if (_G_uiAllRcvDataIndex >= ALLRCVBUFLEN)
    { /*  接受数据达到了 500 字节     */
        ProtocolDataHead *data = (ProtocolDataHead *)_G_ucAllRcvBuf;

        _G_uiAllRcvDataIndex = 0;

        qDebug("data->ucStartFlag : %x \n", data->ucStartFlag);
        qDebug("data->usDataLong : %x \n", data->usDataLong );
        qDebug("data->ucData[data->usDataLong - 3] : %x \n", data->ucData[data->usDataLong - 3]);


        if ((data->ucStartFlag == 0xa1) && (data->ucData[data->usDataLong - 3] == 0x1a))
        {
            /*  数据合法计算并保存签名结果  */
            FILE *sigResult;
            size_t sSigDataNum;

            spcRcvDataToCmdString((char *)_G_ucAllSendBuf, ALLSENDBUFLEN, data->ucData);

            qDebug((const char *)_G_ucAllSendBuf);

            /*  处理接收到的500字节         */
            sigResult = popen((const char *)_G_ucAllSendBuf, "r");
            if (sigResult == NULL)
            {
                qDebug("\nsign error\n");
                return (NULL);
            }

            memset(_G_ucAllSendBuf, '\0', ALLSENDBUFLEN);

            sSigDataNum = fread(_G_ucAllSendBuf, sizeof(char), sizeof(_G_ucAllSendBuf), sigResult);

            pgVarible->bJsonResult.resize(sSigDataNum);/* 重置bytearry大小 */
            memcpy(pgVarible->bJsonResult.data(), _G_ucAllSendBuf, sSigDataNum);/* copy数据*/

            /*  获取json格式的操作结果      */
            pclose(sigResult);


            bool ok;
            JsonObject result = QtJson::parse( QString(pgVarible->bJsonResult), ok).toMap();
            if (!ok) {
                qFatal("An error occurred during parsing");
                return (ERR_0); /* */
            }

            pgVarible->gSMultiSignResult = result["hex"].toString();  /* 全局变量传递参数*/

            qDebug() << pgVarible->gSMultiSignResult;

            StringToHex(result["hex"].toString(),  pgVarible->baMultiSignResultCompressed);  /* 压缩hex compress*/

            qDebug() << "compressed size:"<< pgVarible->baMultiSignResultCompressed.size();


        }
    }

    qDebug("spcProcessfile end \n");
    return (ERR_OK);
}

/*********************************************************************************************************
  END
*********************************************************************************************************/
