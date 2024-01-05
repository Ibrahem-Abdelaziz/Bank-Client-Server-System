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

    qInfo() << "Welcome to the bank system\nPlease choose \"user\", \"admin\", or \"exit\": ";
    QString clientRole;
    inStream >> clientRole;

    std::string roleStdString = clientRole.toStdString();
    role = QString::fromStdString(roleStdString);

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
        outStream << "Thank you! The system is closing..." <<Qt:: endl;
        // No need for exit(0); just let the main function return
        return a.exec();
    }
    else
    {
        outStream << "Sorry, you've entered invalid input." <<Qt:: endl;
    }

    outStream << "BYE...BYE" << Qt::endl;

    return a.exec();
}
