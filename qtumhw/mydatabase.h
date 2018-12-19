#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QVariantList>
#include <QtSql>



class mydatabase
{


public:
    explicit mydatabase();


signals:
    void openSubSig(int);
    void subClosedSig();

private:


private slots:



};

#endif // MYDATABASE_H
