#include <QCoreApplication>
#include <QDebug>
#include "user.h"
#include "admin.h"
#include <QTextStream>
#include <QCoreApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream outStream(stdout);
    QTextStream inStream(stdin);

    QString role;

    qDebug() << "Welcome to the bank system\ncan you choose your role  \"user\", \"admin\", or you can exit: ";
    QString clientRole;
    inStream >> clientRole;
    role = clientRole.toStdString().c_str();

    if (role.toUpper() == "USER")
    {
        User user;
        user.connectToHost("127.0.0.1", 1234);
        user.StartNew();
        bool isLogged = user.Login();
        user.StartNew();
        outStream << "Welcome ";
        while (isLogged)
        {
            user.Start(isLogged);
        }
    }
    else if (role.toUpper() == "ADMIN")
    {
        Admin admin;
        admin.connectToHost("127.0.0.1", 1234);
        bool isLogged = admin.Login();
        admin.StartNew();
        outStream << "Welcome ";
        while (isLogged)
        {
            admin.Start(isLogged);
        }
    }
    else if (role.toUpper() == "EXIT")
    {
        outStream << "Thank you for using our Bank ... See you soon" <<Qt:: endl;
        return a.exec();
    }
    else
    {
        outStream << "Sorry, you've entered invalid input" <<Qt:: endl;
    }

    outStream << "Thank you for using our Bank ... See you soon " << Qt::endl;

    return a.exec();
}
