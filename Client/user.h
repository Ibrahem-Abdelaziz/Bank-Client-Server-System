#ifndef USER_H
#define USER_H

#include "client.h"

class UserManager : public Client
{
public:
    explicit UserManager();
    void transferAccount();
    void makeTransaction();
    void viewAccount();
    void getAccountNumber();
    void viewTransactionHistory();
    void sendRequestToServer();
    bool login();
    quint8 userOptions();
    void start(bool &isLogged);

signals:
public slots:
    void connectToHost(QString host, quint16 port);
    void disconnect();

protected slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void stateChanged(QAbstractSocket::SocketState socketState);
    void readyRead();

private:
    QTcpSocket socket;
    QDataStream outStream;
    QDataStream inStream;
    QString userName;
    QString accountNumber;
};

#endif // USER_H
