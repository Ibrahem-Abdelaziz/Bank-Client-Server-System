#ifndef DATABASE_H
#define DATABASE_H
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QVariantMap>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    bool checkLogin(QString ,QString);
    qint32 ViewAccBalance(QString);
    QString GetAccNum(QString);
    bool TransAcc(QString,quint32);
    bool GetField(QString,QString,QString&);
    bool UpdateField(QString,QString,QString);
    bool MakeTransaction(qint32);
    void SaveTransaction(QString,QString&);
    QString ViewTransHistory(QString,quint16);
    void checkDataBase();
    bool checkField(QString);

    QString role() const;
    void setRole(const QString &newRole);

    QString username() const;

    QString accountnumber() const;

signals:
private:
    QFile Login_DB;
    QFile Users_DB;
    QString m_accountnumber;
    QString m_username;
    QString m_role;
    QJsonObject m_login;
    //QJsonObject userObject;

};

#endif // DATABASE_H