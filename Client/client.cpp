#include "client.h"
#include <QTextStream>
#include<QDataStream>
#include<QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
{

}


void Client::StartNew()
{
    system("cls");
}


