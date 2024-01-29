#include "server.h"

Server::Server(DataBase *parent)
    : DataBase{parent}
{
    connect(&server,&QTcpServer::newConnection,this,&Server::newConnection);
}

void Server::start()
{
    if(!server.listen(QHostAddress::Any,1234))
    {
        qWarning()<<server.errorString();
        return;
    }
    qInfo()<<"Listening";
}

void Server::quit()
{
    server.close();
}

void Server::newConnection()
{
    QTcpSocket* socket=server.nextPendingConnection();
    connect(socket,&QTcpSocket::disconnected,this,&Server::disconnected);
    connect(socket,&QTcpSocket::readyRead,this,&Server::readyRead);
    qInfo()<<"connected to"<<socket;
}

void Server::disconnected()
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    qInfo()<<"Disconnected"<<socket;
}

void Server::readyRead()
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    QDataStream inStream(socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    QString request,role,requestflag;
    inStream>>requestflag;
    if(requestflag=="CreateUser")
    {
        role="admin";
        request="Create User";
    }
    else if(requestflag=="UpdateUser")
    {
        role="admin";
        request="Update User";
    }
    else if(requestflag=="General")
    {
    inStream>>request>>role;
    qDebug()<<request<<role;
    }
    setRole(role);
    Handlerequest(request,role);
}

void Server::Handlerequest(QString request,QString role)
{
    QTcpSocket* socket=qobject_cast<QTcpSocket*>(sender());
    QDataStream inStream(socket);
    QDataStream outStream(socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    outStream.setVersion(QDataStream::Qt_6_6);
    if(role.toUpper()=="USER")
    {
    if(request=="Transfer Account")
    {
        bool ok = true;
        QString totransferaccount;
        quint32 transferamount;
        inStream >> totransferaccount >> transferamount;
        qDebug() << "Received from client:"<<totransferaccount<<" "<<transferamount ;
        ok=TransferAmount(totransferaccount,transferamount);
        outStream<<ok;

    }
    else if(request=="View Account")
    {
        QString m_accountnumber=accountnumber();
        qint32 Balance =ViewAccBalance(m_accountnumber);
        outStream<<Balance;
    }
    else if(request=="Make Transaction")
    {
         bool ok = true;
        qint32 TransactionAmount=0;
        inStream >> TransactionAmount;
        ok=MakeTransaction(TransactionAmount);
        outStream<<ok;
    }
    else if(request=="Login")
    {
        QString username , password;
        bool ok;
        inStream>>username>>password;
        qDebug() << "Received from client:" << username<<" "<<password ;
        ok=CheckLogin(username,password);
         outStream<<ok;

    }
    else if(request=="GetAccNo")
    {
        QString username,accountnumber;
        inStream>>username;
        accountnumber = GetAccNum(username);
        outStream<<accountnumber;

    }
    else if(request =="View Transaction History")
    {
        quint16 count;
        QString m_accountnumber=accountnumber();
        inStream>>count;
        QString data=ViewTransactionHistory(m_accountnumber,count);
        outStream<<data;
    }
    }
    else if (role.toUpper()=="ADMIN")
    {
        if(request=="Login")
        {
            QString username , password;
            bool ok;
            inStream>>username>>password;
            qDebug() << "Received from client:" << username<<" "<<password ;
            ok=CheckLogin(username,password);
            outStream<<ok;

        }
        else if(request=="View Account")
        {
            QString accountnumber;
            inStream >>accountnumber;
            qint32 Balance =ViewAccBalance(accountnumber);
            outStream<<Balance;
        }
        else if(request=="GetAccNo")
        {
            QString username,accountnumber;
            inStream>>username;
            accountnumber = GetAccNum(username);
            outStream<<accountnumber;

        }
        else if(request=="Update User")
        {
            QString flag;
            inStream>>flag;
            if(flag=="check")
            {
             QString accountnumber;
             inStream>>accountnumber;
             qInfo()<<accountnumber;
             bool ok=CheckAccNum(accountnumber);
             outStream<<ok;

            }
            else if(flag=="update")
            {
                QString accountnumber;
                QVariantMap map;
                inStream>>accountnumber>>map;
                qInfo()<<accountnumber;
                bool ok=UpdateUser(accountnumber,map);
                outStream<<ok;
            }

        }
        else if(request=="Create User")
        {
            QString flag;
            inStream>>flag;
            if(flag=="check")
            {
                 QString username;
                inStream>>username;
                bool ok=CheckUserName(username);
                outStream<<ok;

            }
            else if(flag=="update")
            {
                QString username,password;
                QVariantMap map;
                inStream>>username>>password>>map;
                qInfo()<<username<<password;
                bool ok=CreateUser(username,password,map);
                outStream<<ok;

            }

        }
        else if(request=="View Transaction History")
        {
            QString accountnumber;
            quint16 count;
            inStream>>accountnumber>>count;
            QString data=ViewTransactionHistory(accountnumber,count);
            outStream<<data;

        }
        else if(request=="Delete User")
        {
            QString accountnumber;
            inStream>>accountnumber;
            bool ok=DeleteUser(accountnumber);
            outStream<<ok;

        }
        else if(request=="View Bank DataBase")
        {
            QString Database=ViewBankDataBase();
            outStream<<Database;

        }
    }

}
