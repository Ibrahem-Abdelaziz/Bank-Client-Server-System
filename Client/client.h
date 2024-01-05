#ifndef CLIENT_H
#define CLIENT_H

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
    virtual void GetAccNum()=0;
    virtual void ViewAccBalance()=0;
    virtual void ViewTransHistory()=0;
    virtual void SendReqToServer()=0;
    virtual bool Login () = 0 ;
    void StartNew();

 protected:
    QString _role ;
    QString _request;
    QVariant _serverresponse;
    QString reqflag;


};

#endif // CLIENT_H
