#include "client.h"
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include<QProcess>
#include<QSysInfo>

Client::Client(QObject *parent)
    : QObject(parent)
{
    requestFlag = "General";
}

void Client::clearScreen()
{
    // if (QSysInfo::productType() == "windows")
    //     QProcess::execute("cls");
    // else
    //     QProcess::execute("clear");
    system("cls");
}

