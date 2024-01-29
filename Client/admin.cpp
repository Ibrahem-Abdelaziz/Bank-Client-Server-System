#include "admin.h"
#include <iostream>
#include <QVariant>
#include <QVariantMap>
#include <conio.h>

AdminManager::AdminManager() : Client()
{
    outStream.setDevice(&socket);
    outStream.setVersion(QDataStream::Qt_6_6);
    inStream.setDevice(&socket);
    inStream.setVersion(QDataStream::Qt_6_6);
    role = "admin";
    connect(&socket, &QTcpSocket::connected, this, &AdminManager::connected);
    connect(&socket, &QTcpSocket::disconnected, this, &AdminManager::disconnected);
    connect(&socket, &QTcpSocket::stateChanged, this, &AdminManager::stateChanged);
    connect(&socket, &QTcpSocket::readyRead, this, &AdminManager::readyRead);
    connect(&socket, &QTcpSocket::errorOccurred, this, &AdminManager::error);
}

void AdminManager::viewAccount()
{
    std::string accountNumber;
    QString accountNumberStr;

    clearScreen();
    qInfo() << "Please Enter the account number";
    std::cin >> accountNumber;
    accountNumberStr = QString::fromStdString(accountNumber);
    outStream << accountNumberStr;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo() << "Admin account balance is: " << serverResponse.toInt();
}

void AdminManager::getAccountNumber()
{
    qInfo() << "Please Enter the username:";
    std::string username;
    std::cin >> username;
    QString userName = QString::fromStdString(username);
    outStream << userName;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo() << "Account Number is: " << serverResponse.toString();
}

void AdminManager::viewBankDatabase()
{
    socket.waitForReadyRead();
    qInfo().noquote() << serverResponse.toString();
}

void AdminManager::createNewUser()
{
    QVariantMap map;
    std::string username, password;
    QString userName, fullName, flag = "check";
    quint32 age = 0;
    quint16 counter = 0;
    qint32 balance = 0;
    bool ok = false;
    QTextStream cin(stdin);
    requestFlag = "CreateUser";

    do
    {
        if (counter > 0)
        {
            qInfo() << "User name is already existed!!\nPlease Enter a different one:";
            std::cin >> username;
            userName = QString::fromStdString(username);
            outStream << requestFlag << flag << userName;
        }
        else
        {
            qInfo() << "Username:";
            std::cin >> username;
            userName = QString::fromStdString(username);
            outStream << flag << userName;
        }

        counter++;
        socket.waitForReadyRead();
        ok = serverResponse.toBool();
    } while (!ok);

    flag = "update";

    qInfo() << "Full Name:";
    fullName = cin.readLine();

    qInfo() << "Age:";
    std::cin >> age;

    qInfo() << "Balance:";
    std::cin >> balance;

    qInfo() << "Password:";
    std::cin >> password;

    map["Fullname"] = fullName;
    map["Age"] = age;
    map["Balance"] = balance;
    map["Username"] = userName;

    QString passwordStr = QString::fromStdString(password);
    outStream << requestFlag << flag << userName << passwordStr << map;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    requestFlag = "General";

    if (serverResponse.toBool() == true)
    {
        qInfo() << "User is created Successfully";
    }
    else
    {
        qInfo() << "User is not created!!";
    }
}

void AdminManager::deleteUser()
{
    qInfo() << "Please send the account number:";
    std::string accountNumber;
    std::cin >> accountNumber;
    QString accountNumberStr = QString::fromStdString(accountNumber);
    outStream << accountNumberStr;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    if (serverResponse.toBool() == true)
    {
        qInfo() << "User is deleted";
    }
    else
    {
        qInfo() << "User is not deleted!!\nAccount number is not existed!!";
    }
}

void AdminManager::updateUser()
{
    QTextStream cin(stdin);
    QString flag = "check";
    QString fullName;
    QVariantMap map;
    QString accountNumberStr;
    std::string accountNumber, password;
    quint16 age = 0;
    quint16 in = 0;
    std::string key;
    bool chosenFlag = true;
    qint32 balance = 0;
    quint16 counter = 0;
    bool ok = false;
    requestFlag = "UpdateUser";

    do
    {
        if (counter > 0)
        {
            qInfo() << "Account Number is not existed!!\nplease Enter a valid one:";
            std::cin >> accountNumber;
            accountNumberStr = QString::fromStdString(accountNumber);
            outStream << requestFlag << flag << accountNumberStr;
        }
        else
        {
            qInfo() << "Account Number:";
            std::cin >> accountNumber;
            accountNumberStr = QString::fromStdString(accountNumber);
            outStream << flag << accountNumberStr;
        }
        counter++;
        socket.waitForReadyRead();
        ok = serverResponse.toBool();
    } while (!ok);

    flag = "update";

    do
    {
        qInfo() << "Choose the field you want to update:";
        qInfo() << "1-Full Name\n2-Age\n3-Balance\n4-Password";
        std::cin >> in;
        switch (in)
        {
        case 1:
            qInfo() << "New Full Name:";
            fullName = cin.readLine();
            map["Fullname"] = fullName;
            qInfo() << fullName;
            break;
        case 2:
            qInfo() << "New Age:";
            std::cin >> age;
            map["Age"] = age;
            break;
        case 3:
            qInfo() << "New Balance:";
            std::cin >> balance;
            map["Balance"] = balance;
            break;
        case 4:
            qInfo() << "New Password:";
            std::cin >> password;
            map["password"] = QString::fromStdString(password);
            break;
        default:
            qInfo() << "Please enter a valid choice";
            break;
        }
        qInfo() << "----------------------------------------------------------";
        qInfo() << "If you want to update another field press 'y'\nIf you want to save and exit press'n'";
        std::cin >> key;
        if (key != "y")
        {
            chosenFlag = false;
        }
    } while (chosenFlag);

    outStream << requestFlag << flag << accountNumberStr << map;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    requestFlag = "General";

    if (serverResponse.toBool() == true)
    {
        qInfo() << "User Data is updated Successfully";
    }
    else
    {
        qInfo() << "User Data is not updated!!";
    }
}

void AdminManager::viewTransactionHistory()
{
    qInfo() << "Please send the account number:";
    std::string accountNumber;
    std::cin >> accountNumber;
    QString accountNumberStr = QString::fromStdString(accountNumber);
    qInfo() << "Please send the number of transactions:";
    quint16 count;
    std::cin >> count;
    outStream << accountNumberStr << count;
    socket.waitForBytesWritten();
    socket.waitForReadyRead();
    qInfo().noquote() << serverResponse.toString();
}

void AdminManager::sendRequestToServer()
{
    outStream << requestFlag << request << role;
}

bool AdminManager::login()
{
    qInfo() << "WELCOME!!";
    qInfo() << "Username: ";
    request = "Login";
    outStream << requestFlag << request << role;
    QString adminName, password;
    std::string name;
    std::cin >> name;
    adminName = QString::fromStdString(name);
    qInfo() << "Password: ";


    char ch;
    std::string pass;

    while (true)
    {
        ch = _getch();

        if (ch == 13)
            break;

        std::cout << '*';
        pass += ch;
    }

    std::cout<< std::endl;
    password = QString::fromStdString(pass);



    bool ok = false;


    if (!adminName.isEmpty() && !password.isEmpty())
    {
        outStream << adminName << password;
        socket.waitForBytesWritten();
        socket.waitForReadyRead();
        ok = serverResponse.toBool();
        this->adminName = adminName;

    }

    clearScreen();
    return ok;
}

void AdminManager::start(bool &isLogged)
{
    quint16 input;
    char in;
    qInfo() << "Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Create New User"
               "\n5-Update User Data\n6-Delete User\n7-View Bank DataBase\n8-exit";
    std::cin >> input;

    std::cin.ignore();
    clearScreen();
    switch (input)
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
        request = "Create User";
        sendRequestToServer();
        createNewUser();
        break;
    case 5:
        request = "Update User";
        sendRequestToServer();
        updateUser();
        break;
    case 6:
        request = "Delete User";
        sendRequestToServer();
        deleteUser();
        break;
    case 7:
        request = "View Bank DataBase";
        sendRequestToServer();
        viewBankDatabase();
        break;
    case 8:
        isLogged = false;
        break;
    default:
        qInfo() << "Your choice is invalid";
        break;
    }
    if (input != 8)
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

void AdminManager::connectToHost(QString host, quint16 port)
{
    if (socket.isOpen())
        disconnect();
    socket.connectToHost(host, port);
}

void AdminManager::disconnect()
{
    socket.close();
    socket.waitForDisconnected();
}

void AdminManager::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    socket.write(userInput.toUtf8());
}

void AdminManager::disconnected()
{
    qInfo() << "Disconnected";
}

void AdminManager::error(QAbstractSocket::SocketError socketError)
{
    qInfo() << "Error:" << socketError << socket.errorString();
}

void AdminManager::stateChanged(QAbstractSocket::SocketState socketState)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str = metaEnum.valueToKey(socketState);
}

void AdminManager::readyRead()
{
    if (request == "View Bank DataBase")
    {
        QString database;
        inStream >> database;
        serverResponse.setValue(database);
    }
    else if (request == "View Account")
    {
        quint32 accountMoney;
        inStream >> accountMoney;
        serverResponse.setValue(accountMoney);
    }
    else if (request == "Create User" || (request == "Delete User") || (request == "Update User") || (request == "Login"))
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
