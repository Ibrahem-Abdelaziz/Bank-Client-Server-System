#ifndef ADMIN_H
#define ADMIN_H

#include "client.h"

class Admin : public Client
{
public:
    explicit Admin();
    void GetAccNum();
    void ViewAccBalance();
    void ViewTransHistory();
    void SendReqToServer();
    bool Login ();
    void Start(bool&);
    void ViewDatabase();
    void CreatNewUser();
    void DeleteUser();
    void UpdateUser();
     void SendUserCreationRequest(const QString&, const QString&, const QVariantMap&);

signals:
public slots:
    void connectToHost(QString host , quint32 port );
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
    QString _admin;

};

#endif // ADMIN_H
