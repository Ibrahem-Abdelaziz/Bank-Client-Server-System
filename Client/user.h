#ifndef USER_H
#define USER_H

#include "client.h"

class User : public Client
{
public:
    explicit User();
    void GetAccNum() ;
    void ViewAccBalance();
    void ViewTransHistory();
    void SendReqToServer();
    bool Login ();
    void TransAcc();
    void MakeTransaction();
    // quint8 UserOption();
    void Start(bool&);
signals:
public slots:
    void connectToHost(QString host , quint16 port );
    void disconnect();
protected slots:
    void connected();
    void disconnected();
    void error (QAbstractSocket::SocketError socketerror);
    void stateChanged(QAbstractSocket::SocketState socketstate);
    void readyRead();
private:
    QTcpSocket _socket;
    QDataStream oStream;
    QDataStream iStream;
    QString _userName;
    QString _accNumber;

};

#endif // USER_H
