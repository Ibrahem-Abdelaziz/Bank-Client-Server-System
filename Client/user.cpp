#include "user.h"
#include <iostream>
#include<QDataStream>
#include<QDebug>

User::User()
{
    oStream.setDevice(&_socket);
    oStream.setVersion(QDataStream::Qt_6_6);
    iStream.setDevice(&_socket);
    iStream.setVersion(QDataStream::Qt_6_6);
    _role = "user";
    connect(&_socket,&QTcpSocket::connected,this,&User::connected);
    connect(&_socket,&QTcpSocket::disconnected,this,&User::disconnected);
    connect(&_socket,&QTcpSocket::stateChanged,this,&User::stateChanged);
    connect(&_socket,&QTcpSocket::readyRead,this,&User::readyRead);
    connect(&_socket,&QTcpSocket::errorOccurred,this,&User::error);
}


void User::TransAcc()
{
    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    outStream << "Please send the account number from which you will transfer:" << Qt::endl;
    QString fromAccountNumber = inStream.readLine();

    outStream << "Please send the account number to which you will transfer:" << Qt::endl;
    QString toAccountNumber = inStream.readLine();

    outStream << "Please send the transfer amount:" << Qt::endl;
    quint32 transferAmount = inStream.readLine().toUInt();

    // Send the data to the server to handle that
    outStream << fromAccountNumber << toAccountNumber << transferAmount;
    _socket.waitForBytesWritten();

    // Wait for the response from the server to view it to the client
    _socket.waitForReadyRead();
    outStream << "Transaction is: " << _serverresponse.toBool() <<Qt:: endl;
}
void User:: MakeTransaction()
{

    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    // Get account number from the user
    outStream << "Please send the account number:" << Qt::endl;
    QString accountNumber = inStream.readLine();

    // Get transaction amount from the user
    outStream << "Please send the transaction amount:" << Qt::endl;
    qint32 transactionAmount = inStream.readLine().toInt();

    // Send the data to the server to handle that
    outStream << accountNumber << transactionAmount;

    // Wait for the response from the server to view it on the client
    if (_socket.waitForReadyRead()) {
        outStream << "Transaction is: " << _serverresponse.toBool() << Qt::endl;
    } else {
        qWarning() << "Error during socket communication.";
    }

}
void User::ViewAccBalance()
{

    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    // Get account number from the user
    outStream << "Please send the account number:" << Qt::endl;
    QString accountNumber = inStream.readLine();

    // Send the account number to the server
    outStream << accountNumber;
    _socket.waitForBytesWritten();

    // Wait for the response from the server
    _socket.waitForReadyRead();

    // Read the server response and display it to the client
    qDebug() << "Your account money is: " << _serverresponse.toInt();
}

void User::GetAccNum()
{
    _socket.waitForReadyRead();
    qDebug()<<"Your account number is:"<<_serverresponse.toString();
}

void User::ViewTransHistory()
{
    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    // Get account number from the user
    outStream << "Please send the account number:" << Qt::endl;
    QString AccountNumber = inStream.readLine();
    // Get the number of transactions from the user
    outStream << "Please send the number of transactions:" << Qt::endl;
    quint32 count = inStream.readLine().toUInt();

    // Send the data to the server to handle that
    outStream << AccountNumber << count;
    _socket.waitForBytesWritten();

    // Wait for the response from the server to view it on the client
    _socket.waitForReadyRead();

    // Assuming m_serverrespond is a QVariant containing the transaction history
    QVariant transactionHistory = _serverresponse;

    // Print the transaction history
    qDebug() << transactionHistory;
}

void User::SendReqToServer()
{
    oStream<<_request<<_role;

    if(_request=="Transfer Account")
    {
        //call the method Transfer Account to handle this request
        TransAcc();
    }
    else if(_request=="View Account")
    {
        //call the method view Account to handle this request
        ViewAccBalance();
    }
    else if(_request=="Make Transaction")
    {
        MakeTransaction();
    }
    else if(_request=="Get Acc No")
    {
        GetAccNum();
    }
    else if(_request=="View Transaction History")
    {
        ViewTransHistory();
    }
    else
    {
        qFatal("The request message is not defined");
    }
}

bool User::Login()
{
    QTextStream inputStream(stdin);
    QTextStream outputStream(stdout);

    qDebug() << "WELCOME" << Qt::endl;
    qDebug()<< "Username: "<< Qt::endl;;
    _request = "Login";

    QString userName;
    QString password;

    inputStream >> userName;

    _userName = userName;

    qDebug() << "Password: "<<Qt::endl;

    inputStream >> password;

    bool ok = false;
    quint8 count = 0;

    while (count < 3)
    {
        count++;

        if (!userName.isEmpty() && !password.isEmpty())
        {
            outputStream << userName << password;
            _socket.waitForBytesWritten();
            _socket.waitForReadyRead();
            ok = _serverresponse.toBool();
            break;
        }
    }

    StartNew();
    // qDebug()<<"asdasd";
    return true ;
}

void User::Start(bool& isLogged)
{
    quint16 input;
    char in;

    qDebug() << "Choose from the list:";
    qDebug() << "1- View Account";
    qDebug() << "2- View Transaction History";
    qDebug() << "3- Get Account Number";
    qDebug() << "4- Transfer Account";
    qDebug() << "5- Make Transaction";
    qDebug() << "6- Exit";

    QTextStream inStream(stdin);
    inStream >> input;
    inStream.flush(); // Flush the input buffer

    StartNew();

    switch(input)
    {
    case 1:
        _request = "View Account";
        SendReqToServer();

        break;
    case 2:
        _request = "View Transaction History";
        SendReqToServer();

        break;
    case 3:
        _request = "Get Account Number";
        SendReqToServer();

        break;
    case 4:
        _request = "Transfer Account";
        SendReqToServer();

        break;
    case 5:
        _request = "Make Transaction";
        SendReqToServer();

        break;
    case 6:
        isLogged = false;
        break;
    default:
        qDebug() << "Your choice is invalid";
        break;
    }

    if (input != 6)
    {
        SendReqToServer();
        qDebug() << "If you have another request, press 'y'; if you want to exit, press 'N':";
        inStream >> in;

        if (in == 'n' || in == 'N')
        {
            isLogged = false;
        }

        qDebug() << "----------------------------------------------------------------------------";
    }

}




void User::connectToHost(QString host, quint16 port)
{
    if(_socket.isOpen()) disconnect();
    //qDebug()<<"connecting to host"<<host<<"on port:"<<port;
    _socket.connectToHost(host,port);
}

void User::disconnect()
{
    _socket.close();
    _socket.disconnectFromHost();
    _socket.waitForDisconnected();
}

void User::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    _socket.write(userInput.toUtf8());
}

void User::disconnected()
{
    qDebug()<<"Thank you!...system is closing";
    qDebug()<<"Disconnected";
}

void User::error(QAbstractSocket::SocketError socketerror)
{
    qDebug()<<"Error:"<<socketerror<<_socket.errorString();
}

void User::stateChanged(QAbstractSocket::SocketState socketstate)
{
    QMetaEnum metaenum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str= metaenum.valueToKey(socketstate);
}

void User::readyRead()
{
    if (_request=="View Account")
    {
        quint32 AccountMoney;
        //take the money in the account from the server and save it in the server respond
        iStream>>AccountMoney;
        _serverresponse.setValue(AccountMoney);
    }
    else if(_request=="Transfer Account"||_request=="Make Transaction"||_request=="Login")
    {
        bool respond;
        //know if the transaction succeed or not from the server and save it in the server respond
        iStream>>respond;
        _serverresponse.setValue(respond);
    }
    else if(_request=="Get Acc No")
    {
        QString AccNo;
        iStream>>AccNo;
        _serverresponse.setValue(AccNo);
    }
    else if(_request=="View Transaction History")
    {
        QString History;
        iStream>>History;
        _serverresponse.setValue(History);
    }
}


