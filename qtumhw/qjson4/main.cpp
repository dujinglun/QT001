/*
Copyright (C) 2014 - 2016 Evan Teran
                          evan.teran@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "QJsonValue.h"
#include "QJsonDocument.h"
#include "QJsonArray.h"
#include "QJsonObject.h"
#include "QJsonParseError.h"
#include <QFile>
#include <QCoreApplication>
#include <QTextCodec>
#include <QtDebug>

int main(int argc, char *argv[]) {

	QCoreApplication app(argc, argv);

#if 0
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
        if (doucment.isObject()) { // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("Name")) {  // 包含指定的 key
                QJsonValue value = object.value("Name");  // 获取指定 key 对应的 value
                if (value.isString()) {  // 判断 value 是否为字符串
                    QString strName = value.toString();  // 将 value 转化为字符串
                    qDebug() << "Name : " << strName;
                }
            }
            if (object.contains("From")) {
                QJsonValue value = object.value("From");
                if (value.isDouble()) {
                    int nFrom = value.toVariant().toInt();
                    qDebug() << "From : " << nFrom;
                }
            }
            if (object.contains("Cross Platform")) {
                QJsonValue value = object.value("Cross Platform");
                if (value.isBool()) {
                    bool bCrossPlatform = value.toBool();
                    qDebug() << "CrossPlatform : " << bCrossPlatform;
                }
            }
        }
    }


#endif




    QFile f("test.json");
	if(f.open(QIODevice::ReadOnly)) {
		QJsonParseError e;
		QJsonDocument d = QJsonDocument::fromJson(f.readAll(), &e);

        if(!d.isNull() && e.error == QJsonParseError::NoError) {
            qDebug() << QString::fromUtf8(d.toJson(QJsonDocument::Compact));
            qDebug()<<"123" <<  QString::fromUtf8(d.fromJson("u").toBinaryData());

            if (d.isObject()) { // JSON 文档为对象
                QJsonObject object = d.object();  // 转化为对象

                QString nFrom = object.take("test\nreturn").toString();  //value.toVariant().toInt();
                qDebug() << "From : " << nFrom;


            }




		} else {
			qDebug() << e.errorString();
		}

	}

	qDebug() << "----------------------------------";

	// Regression test for issue #3
	QVariantMap map;
	map.insert("int", INT_MAX);
	map.insert("double", 3.1415926);
	map.insert("name", "hell");
	QJsonDocument doc = QJsonDocument::fromVariant(map);
	qDebug() << QString::fromUtf8(doc.toJson());


        QVariant value = doc.fromJson("Name").toVariant();  // 获取指定 key 对应的 value


            QString strName = value.toString();  // 将 value 转化为字符串
            qDebug() << "Name : " << strName;







}
