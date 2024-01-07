#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMetaEnum>
#include <windows.h>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    virtual void getAccountNumber() = 0;
    virtual void viewAccount() = 0;
    virtual void viewTransactionHistory() = 0;
    virtual void sendRequestToServer() = 0;
    virtual bool login() = 0;
    void clearScreen();

protected:
    QString role;
    QString request;
    QVariant serverResponse;
    QString requestFlag;
};

#endif // CLIENT_H_INCLUDED
