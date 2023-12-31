#include <QCoreApplication>
#include <QDebug>
#include "user.h"
#include "admin.h"
#include<iostream>
#include <windows.h>

void clearScreen()
{
    // ANSI escape code to clear the screen and move the cursor to the top-left corner
    //std::cout<< "\033[2J";
    //std::cout<<"\033[3J";
    system("cls");


}
void login(QString &username,QString &password)
{

    qInfo()<<"WELCOME!!";
    qInfo()<<"Username: ";
    std::string name;
    std::cin>>name;
    username=QString::fromStdString(name);
    qInfo()<<"Password: ";
    std::string pass;
    std::cin>>pass;
   password=QString::fromStdString(pass);
    clearScreen();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString role,username,password;


    qInfo()<<"Welcome to the bank system \nPlease choose \"user\"or\"admin\"or\"exit\"";
    std::string clientrole;
    std::cin>>clientrole;
    role = QString::fromStdString(clientrole);
    //connectToHost(QHostAddress::LocalHost, 1234);
   clearScreen();

     if(role.toUpper()=="USER")
     {
        User user;
         user.connectToHost("127.0.0.1", 1234);
            login(username,password);

            qInfo()<<"welcome user: "<<username;
            QString request;
            quint32 input=0;
            char in;
            while(true)
            {

            qInfo()<<"Choose from the list:\n1-View Account\n2-View Transaction History\n3-Get Account Number\n4-Transfer Account\n5-MakeTransaction\n6-exit";
            std::cin>>input;
            std::cin.ignore();
            clearScreen();
            switch(input)
             {
            case 1:
                request="View Account";
                user.sendrequesttoserver(request);
                qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
                std::cin>>in;
                if(in=='n'||in=='N')
                {
                    exit(122);
                    break;
                }
                else
                {
                    break;
                }

            case 4:
                  // qInfo()<<in;
                request="Transfer Account";
                user.sendrequesttoserver(request);
                qInfo()<<"if you have another request press 'y' if you want to exit press 'N':";
                std::cin>>in;
                if(in=='n'||in=='N')
                {
                     exit(122);
                    break;
                }
                else
                {
                    break;
                }
            case 6:
                qInfo()<<"exit";

                exit(122);
                break;
            default:
                qInfo()<<"default";
                break;
            }

            clearScreen();

     }
     }
     else if(role.toUpper()=="ADMIN")
     {
         Admin admin;
         admin.connectToHost("127.0.0.1", 1234);
            login(username,password);
         while(true)
         {
            qInfo()<<"welcome admin: "<<username;
         }
     }
     else if(role.toUpper()=="EXIT")
     {
         qInfo()<<"Thank you!....Sysyem is closing..";
     }
     else
     {
         qInfo()<<"Sorry you're entering invalid input";
     }

     qInfo()<<"jjddjgdjd";
    return a.exec();
}
