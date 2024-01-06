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
    bool CheckLogin(QString ,QString);
    qint32 ViewAccBalance(QString);
    QString GetAccNum(QString);
    bool TransferAmount(QString,quint32);
	QString GetField(QString accountNumber, QString jsonField);
    bool UpdateField(QString,QString,QString);
    bool MakeTransaction(qint32);
    void SaveTransaction(QString,QString&);
    QString ViewTransactionHistory(QString,quint16);
    bool CheckAccNum(QString);
    bool CheckField(QString ,QString);
    bool UpdateUser(QString ,QVariantMap);
    bool CreateUser(QString,QString,QVariantMap);
    bool CheckUserName(QString);
    bool DeleteUser(QString);
    QString ViewBankDataBase();


    QString role() const;
    void setRole(const QString &newRole);

    QString username() const;

    QString accountnumber() const;

signals:
private:
    QFile database;
    QFile login_data;
    QString AccNum;
    QString UserName;
    QString _role;
    QJsonObject JsonObj;

};

#endif // DATABASE_H
