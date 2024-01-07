#include "user.h"
#include <iostream>
#include <QVariant>
#include <conio.h>

UserManager::UserManager() : Client()
{
    outStream.setDevice(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    inStream.setDevice(&socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    role = "user";
    request = "Login";
    requestFlag = "General";
    connect(&socket, &QTcpSocket::connected, this, &UserManager::connected);
    connect(&socket, &QTcpSocket::disconnected, this, &UserManager::disconnected);
    connect(&socket, &QTcpSocket::stateChanged, this, &UserManager::stateChanged);
    connect(&socket, &QTcpSocket::readyRead, this, &UserManager::readyRead);
    connect(&socket, &QTcpSocket::errorOccurred, this, &UserManager::error);
}

void UserManager::transferAccount()
{
    qInfo() << "Please send the account number to which you will transfer:";
    std::string toAccountNumber;
    std::cin >> toAccountNumber;
    qInfo() << "Please send the transfer amount:";
    quint32 transferAmount;
    std::cin >> transferAmount;
    QString toAccountNumberStr = QString::fromStdString(toAccountNumber);
    outStream << toAccountNumberStr << transferAmount;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo() << "Transaction is: " << serverResponse.toBool();
}

void UserManager::makeTransaction()
{
    qint32 transactionAmount = 0;
    qInfo() << "Please send the transaction amount:";
    std::cin >> transactionAmount;
    outStream << transactionAmount;
    socket.waitForReadyRead();
    qInfo() << "Transaction is: " << serverResponse.toBool();
}

void UserManager::viewAccount()
{
    socket.waitForReadyRead();
    qInfo() << "Your account money is: " << serverResponse.toInt();
}

void UserManager::getAccountNumber()
{
    outStream << userName;
    socket.waitForReadyRead();
    qInfo() << "Your account number is: " << serverResponse.toString();
}

void UserManager::viewTransactionHistory()
{
    qInfo() << "Please send the number of transactions:";
    quint16 count;
    std::cin >> count;
    outStream << count;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo().noquote() << serverResponse.toString();
}

void UserManager::sendRequestToServer()
{
    outStream << requestFlag << request << role;
}

bool UserManager::login()
{
    qInfo() << "WELCOME!!";
    qInfo() << "Username: ";
    request = "Login";
    outStream << requestFlag << request << role;
    QString password;
    std::string name;
    std::cin >> name;
    userName = QString::fromStdString(name);
    qInfo() << "Password: ";

    char ch;
    std::string pass;

    while (true)
    {
        ch = _getch();

        if (ch == 13) // Enter key
            break;

        std::cout << '*' ;
        pass += ch;
    }
       std::cout<< std::endl;

    password = QString::fromStdString(pass);
    bool ok = false;
    quint8 count = 0;
    while (count < 3)
    {
        count++;
        if (!userName.isEmpty() && !password.isEmpty())
        {
            outStream << userName << password;
            socket.waitForBytesWritten();
            socket.waitForReadyRead();
            ok = serverResponse.toBool();
            break;
        }
    }
    clearScreen();
    return ok;
}

void UserManager::start(bool &isLogged)
{
    std::string stInput;
    char in;
    qInfo() << "Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Transfer Account\n5-MakeTransaction\n6-exit";
    std::cin >> stInput;
    QString input = QString::fromStdString(stInput);
    std::cin.ignore();
    clearScreen();
    switch (input.toUInt())
    {
    case 1:
        request = "View Account";
        sendRequestToServer();
        viewAccount();
        break;
    case 2:
        request = "View Transaction History";
        sendRequestToServer();
        viewTransactionHistory();
        break;
    case 3:
        request = "GetAccNo";
        sendRequestToServer();
        getAccountNumber();
        break;
    case 4:
        request = "Transfer Account";
        sendRequestToServer();
        transferAccount();
        break;
    case 5:
        request = "Make Transaction";
        sendRequestToServer();
        makeTransaction();
        break;
    case 6:
        isLogged = false;
        break;
    default:
        qInfo() << "Your choice is invalid";
        break;
    }
    if (input.toUInt() == 1 || input.toUInt() == 2 || input.toUInt() == 3 || input.toUInt() == 4 || input.toUInt() == 5)
    {
        qInfo() << "if you have another request press 'y' if you want to exit press 'N':";
        std::cin >> in;
        if (in == 'n' || in == 'N')
        {
            isLogged = false;
        }
        clearScreen();
    }
}

void UserManager::connectToHost(QString host, quint16 port)
{
    if (socket.isOpen())
        disconnect();
    socket.connectToHost(host, port);
}

void UserManager::disconnect()
{
    socket.close();
    socket.disconnectFromHost();
    socket.waitForDisconnected();
}

void UserManager::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    socket.write(userInput.toUtf8());
}

void UserManager::disconnected()
{
    qInfo() << "Thank you!...system is closing";
    qInfo() << "Disconnected";
}

void UserManager::error(QAbstractSocket::SocketError socketError)
{
    qInfo() << "Error:" << socketError << socket.errorString();
}

void UserManager::stateChanged(QAbstractSocket::SocketState socketState)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str = metaEnum.valueToKey(socketState);
}

void UserManager::readyRead()
{
    if (request == "View Account")
    {
        quint32 accountMoney;
        inStream >> accountMoney;
        serverResponse.setValue(accountMoney);
    }
    else if (request == "Transfer Account" || request == "Make Transaction" || request == "Login")
    {
        bool respond;
        inStream >> respond;
        serverResponse.setValue(respond);
    }
    else if (request == "GetAccNo")
    {
        QString accNo;
        inStream >> accNo;
        serverResponse.setValue(accNo);
    }
    else if (request == "View Transaction History")
    {
        QString history;
        inStream >> history;
        serverResponse.setValue(history);
    }
}
