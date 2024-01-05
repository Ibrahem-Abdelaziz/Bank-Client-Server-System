#include "admin.h"
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
Admin::Admin()
{
    oStream.setDevice(&_socket);
    oStream.setVersion(QDataStream::Qt_6_6);
    iStream.setDevice(&_socket);
    iStream.setVersion(QDataStream::Qt_6_6);
    _role = "admin";
    connect(&_socket,&QTcpSocket::connected,this,&Admin::connected);
    connect(&_socket,&QTcpSocket::disconnected,this,&Admin::disconnected);
    connect(&_socket,&QTcpSocket::stateChanged,this,&Admin::stateChanged);
    connect(&_socket,&QTcpSocket::readyRead,this,&Admin::readyRead);
    connect(&_socket,&QTcpSocket::errorOccurred,this,&Admin::error);

}

void Admin::ViewAccBalance()
{
    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    // Get account number from the user
    outStream << "Please send the account number:" << Qt:: endl;
    QString accountNumber = inStream.readLine();

    // Send the account number to the server
    outStream << accountNumber;
    _socket.waitForBytesWritten();

    // Wait for the response from the server
    _socket.waitForReadyRead();

    // Read the server response and display it to the client
    qDebug() << "Your account balance is: " << _serverresponse.toInt();

}

void Admin::GetAccNum()
{
    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    outStream << "Please Enter the username:" << Qt::endl;
    QString username;
    inStream >> username;

    outStream << username;
    _socket.waitForBytesWritten();

    // Wait for the response from the server
    _socket.waitForReadyRead();

    qDebug() << "Account Number is: " << _serverresponse.toString();
}

void Admin::ViewDatabase()
{
    _socket.waitForReadyRead();
    qDebug()<<_serverresponse.toString();
}

void Admin::CreatNewUser()
{
    QVariantMap userMap;
    QString username, password, fullName;
    quint32 age = 0;
    qint32 balance = 0;
    qint16 counter = 0 ;
    QTextStream inStream(stdin);
    bool ok = false;
    reqflag = "CreateUser";
    QString flag = "check";

    do
    {
        if (counter > 0)
        {
            qDebug() << "User name is already existed,Enter a another name:";
        }
        else
        {
            qDebug() << "Username:";
        }

        inStream >> username;

        oStream << reqflag << flag << username;
        counter++;

        _socket.waitForReadyRead();
        ok = _serverresponse.toBool();

    } while (!ok);

    flag = "update";

    qDebug() << "Full Name:";
    fullName = inStream.readLine();

    qDebug() << "Age:";
    inStream >> age;

    qDebug() << "Balance:";
    inStream >> balance;

    qDebug() << "Password:";
    inStream >> password;

    userMap["Fullname"] = fullName;
    userMap["Age"] = age;
    userMap["Balance"] = balance;
    userMap["Username"] = username;

    SendUserCreationRequest(username, password, userMap);
}

void Admin::SendUserCreationRequest(const QString& username, const QString& password, const QVariantMap& userMap)
{
    QString flag = "update";
    QString Password = password;

    oStream << reqflag << flag << username << Password << userMap;
    _socket.waitForBytesWritten();
    _socket.waitForReadyRead();
    reqflag = "General";

    if (_serverresponse.toBool())
    {
        qDebug() << "User is created";
    }
    else
    {
        qDebug() << "User is not created";
    }
}

void Admin::DeleteUser()
{
    qDebug() << "Please send the account number:";
    QString accountNumber;
    iStream >> accountNumber;
    oStream << accountNumber;
    _socket.waitForBytesWritten();

    // Wait for the response from the server to view it on the client
    _socket.waitForReadyRead();

    if (_serverresponse.toBool())
    {
        qDebug() << "User is deleted";
    }
    else
    {
        qDebug() << "User is not deleted!!";
    }

}

void Admin::UpdateUser()
{
    QVariantMap map;
    QString accountNumber, fullname, password;
    quint8 age = 0;
    quint8 in = 0;
    char key = 'y';
    quint8 counter = 0;
    qint32 balance = 0;
    bool ok = false;

    do
    {
        if (counter > 0)
        {
            qDebug() << "Account Number is not existed!!\nPlease Enter a valid one:";
            iStream >> accountNumber;
            oStream << accountNumber;
        }
        else
        {
            qDebug() << "Account Number:";
            iStream >> accountNumber;
            oStream << accountNumber;
        }
        counter++;
        _socket.waitForReadyRead();
        ok = _serverresponse.toBool();
    } while (!ok);

    do
    {
        qDebug() << "Choose the field you want to update:";
        qDebug() << "1-Full Name\n2-Age\n3-Balance\n4-Password";
        iStream >> in;

        switch (in)
        {
        case 1:
            qDebug() << "New Full Name:";
            iStream >> fullname;
            map["Full Name"] = fullname;
            break;
        case 2:
            qDebug() << "New Age:";
            iStream >> age;
            map["Age"] = age;
            break;
        case 3:
            qDebug() << "New Balance:";
            iStream >> balance;
            map["Balance"] = balance;
            break;
        case 4:
            qDebug() << "New Password:";
            iStream >> password;
            map["Password"] = password;
            break;
        default:
            qDebug() << "Please enter a valid choice";
            break;
        }

        qDebug() << "----------------------------------------------------------";
        qDebug() << "If you want to update another field press 'y'\nIf you want to save and exit press 'n'";
        iStream >> key;
    } while (key != 'n' && key != 'N');

    oStream << map;
    _socket.waitForBytesWritten();
    _socket.waitForReadyRead();

    if (_serverresponse.toBool())
    {
        qDebug() << "User Data is updated Successfully";
    }
    else
    {
        qDebug() << "User Data is not updated!!";
    }

}

void Admin::connectToHost(QString host, quint32 port)
{
    if(_socket.isOpen()) disconnect();
    _socket.connectToHost(host,port);
}

void Admin::disconnect()
{
    _socket.close();
    _socket.disconnectFromHost();
    _socket.waitForDisconnected();
}

void Admin::ViewTransHistory()
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

void Admin::SendReqToServer()
{
    oStream<<_request<<_role;

    if(_request=="View Bank DataBase")
    {
        //call the method View Bank DataBase to handle this request
        ViewDatabase();
    }
    else if(_request=="View Account")
    {
        //call the method view Account to handle this request
        ViewAccBalance();
    }
    else if(_request=="Create New User")
    {
        CreatNewUser();
    }
    else if(_request=="Get Acc No")
    {
        GetAccNum();
    }
    else if(_request=="View Transaction History")
    {
        ViewTransHistory();
    }
    else if(_request=="Update User")
    {
        UpdateUser();
    }
    else if(_request=="Delete User")
    {
        DeleteUser();
    }
    else
    {
        qFatal("The request message is not defined");
    }
}

bool Admin::Login()
{
    QTextStream inStream(stdin);
    QTextStream outStream(stdout);

    qDebug() << "WELCOME"<<Qt::endl;
    qDebug() << "Username: "<<Qt::endl;
    _request="Login";

    QString adminName, password;

    inStream >> adminName;
    _admin = adminName;

    qDebug() << "Password: "<<Qt::endl;

    inStream >> password;

    bool ok = false;
    quint8 count = 0;

    while (count < 3)
    {
        count++;

        if (!adminName.isEmpty() && !password.isEmpty())
        {
            outStream << adminName << password;
            _socket.waitForBytesWritten();
            _socket.waitForReadyRead();
            ok = _serverresponse.toBool();
            break;
        }
    }

    StartNew();
    return ok;
}

void Admin::Start(bool &isLogged)
{
    quint16 input;
    char in;

    qDebug() << "Choose from the list:";
    qDebug() << "1- View Account";
    qDebug() << "2- View Transaction History";
    qDebug() << "3- Get Account Number";
    qDebug() << "4- Create New User";
    qDebug() << "5- Update User Data";
    qDebug() << "6- Delete User";
    qDebug() << "7- View Bank DataBase";
    qDebug() << "8- Exit";

    QTextStream inStream(stdin);
    inStream >> input;

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
        _request = "Create New User";
        SendReqToServer();
        break;
    case 5:
        _request = "Update User Data";
        SendReqToServer();
        break;
    case 6:
        _request = "Delete User";
        SendReqToServer();
        break;
    case 7:
        _request = "View Bank DataBase";
        SendReqToServer();
        break;
    case 8:
        isLogged = false;
        break;
    default:
        qDebug() << "Your choice is invalid";
        break;
    }

    if (input != 8)
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

void Admin::connected()
{
    QTextStream input(stdin, QIODevice::ReadOnly);
    QString userInput = input.readLine().trimmed();
    _socket.write(userInput.toUtf8());
}

void Admin::disconnected()
{
    _socket.close();
    _socket.waitForDisconnected();
    qDebug() << "Disconnected";
}

void Admin::error(QAbstractSocket::SocketError socketerror)
{
    qDebug()<<"Error:"<<socketerror<<_socket.errorString();
}

void Admin::stateChanged(QAbstractSocket::SocketState socketstate)
{
    QMetaEnum metaenum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString str= metaenum.valueToKey(socketstate);
}

void Admin::readyRead()
{

    if(_request=="View Bank DataBase")
    {
        QString Database;
        iStream>>Database;
        _serverresponse.setValue(Database);

    }
    else if(_request=="View Account")
    {
        quint32 AccountMoney;
        //take the money in the account from the server and save it in the server respond
        iStream>>AccountMoney;
        _serverresponse.setValue(AccountMoney);
    }
    else if(_request=="Create New User"||(_request=="Delete User")||(_request=="Update User Data")||(_request=="Login"))
    {
        bool respond;
        iStream>>respond;
        _serverresponse.setValue(respond);

    }
    else if(_request=="Get Account Number")
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



