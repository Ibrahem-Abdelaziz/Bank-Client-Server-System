#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include "client.h"

class AdminManager : public Client
{
    Q_OBJECT

public:
    explicit AdminManager();
    void viewAccount();
    void getAccountNumber();
    void viewBankDatabase();
    void createNewUser();
    void deleteUser();
    void updateUser();
    void viewTransactionHistory();
    void sendRequestToServer();
    bool login();
    void start(bool&);

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
    QString adminName;
};

#endif // ADMIN_MANAGER_H
