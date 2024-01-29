#include "database.h"
#include <QDateTime>
#include <QDir>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    QDir dir("database");
    if (dir.exists() == false)
		dir.mkdir("database");

   database.setFileName(dir.absolutePath() + "/database.json");
   login_data.setFileName(dir.absolutePath() +  "/login_data.json");
}

bool DataBase::CheckLogin(QString username, QString password)
{
    // qDebug() << login_data.fileName();
    if (!login_data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }

    QByteArray jsonData = login_data.readAll();
    login_data.close();


    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data";
        return false;
    }



     JsonObj = jsonDoc.object();
    if(_role.toUpper()=="USER")
    {
        if(JsonObj.contains("users")&&JsonObj["users"].isArray())
        {
            QJsonArray usersArray = JsonObj["users"].toArray();
            for (const QJsonValue &userVal : usersArray)
            {
                QJsonObject userObject = userVal.toObject();
                if(userObject.contains(username))
                {
                    QJsonObject userData = userObject[username].toObject();
                    if (userData.contains("password") && userData["password"] .toString() == password)
                    {
                        qDebug() << "User authenticated: " <<username;
                        UserName=username;
                        AccNum=userData["accountnumber"].toString();

                        return true;
                    }

                }

            }

        }

    }
    else if(_role.toUpper()=="ADMIN")
    {
        if(JsonObj.contains("admins")&&JsonObj["admins"].isArray())
        {
            QJsonArray usersArray = JsonObj["admins"].toArray();
            for (const QJsonValue &userVal : usersArray)
            {
                if(!userVal.isObject())
                    return false;
                QJsonObject userObject = userVal.toObject();
                if(userObject.contains(username))
                {
                    QJsonObject userData = userObject[username].toObject();
                    if (userData.contains("password") && userData["password"] .toString() == password)
                    {
                        qDebug() << "User authenticated: " <<username;
                        UserName=username;
                        return true;
                    }

                }

            }

        }
    }
    else
    {
        return false;
    }
    return false;

}

qint32 DataBase::ViewAccBalance(QString accountNumber)
{
	QString AccountBalance = GetField(accountNumber,"Balance");
	return AccountBalance.toInt();
}

QString DataBase::GetAccNum(QString username)
{
    QString error ="Username is not found";

    QJsonArray usersArray = JsonObj["users"].toArray();
        for (const QJsonValue &userVal : usersArray)
        {
            QJsonObject userObject = userVal.toObject();
            if(userObject.contains(username))
            {QJsonObject userData = userObject[username].toObject();
                return (userData["accountnumber"].toString());
            }
        }

        return error;
}

bool DataBase::TransferAmount( QString toAccNo, quint32 transferAmount)
{
    QString mybalance,tobalance;
    quint32 Mybalance,Tobalance;
    mybalance = GetField(AccNum,"Balance");
	Mybalance=mybalance.toUInt();
	if(Mybalance==0 && Mybalance < transferAmount)
	{
		qDebug()<<"Sorry ,Your balance is not enough";
		return false;
	}

	Mybalance -= transferAmount;
    bool ok = UpdateField(AccNum,"Balance",QString::number(Mybalance));
	if(!ok)
	{
		return false;
	}
	tobalance =GetField(toAccNo,"Balance");
    if(ok)
    {
        Tobalance=tobalance.toUInt();
        Tobalance += transferAmount;
        ok=UpdateField(toAccNo,"Balance",QString::number(Tobalance));
        QString details_from = QString::number(transferAmount)+" to "+toAccNo;
        QString details_to = QString::number(transferAmount)+" from "+AccNum;
        SaveTransaction(AccNum,details_from);
        SaveTransaction(toAccNo,details_to);
        return ok;
    }
    return false;
}

QString DataBase::GetField(QString accountnumber, QString field)
{
    if (!database.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Can't open the DataBase file";
		return "";
    }

    QByteArray jsonData = database.readAll();
    database.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from get";
		return "";
    }

    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
		return "";
    }


    QJsonObject m_Users = jsonDoc.object();

    if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
    {
        QJsonObject userObject = m_Users[accountnumber].toObject();
		return userObject[field].toString();
    }
	return "";

}

bool DataBase::UpdateField(QString accountnumber, QString field, QString fieldValue)
{


    if (!database.open(QIODevice::ReadWrite | QIODevice::Text )) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }
    QByteArray jsonData = database.readAll();

    // Parse JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from update";
        database.close();
        return false;
    }
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        database.close();
        return false;
    }
    QJsonObject m_Users = jsonDoc.object();
    if(field=="password")
    {
        QString username;
        if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
        {
             QJsonObject userObject = m_Users[accountnumber].toObject();
             username=userObject["Username"].toString();
        }
        if (!login_data.open(QIODevice::ReadWrite| QIODevice::Text)) {
            qDebug() << "Error: Can't open the login_data file";
            database.close();
            return false;
        }
        QByteArray loginData = login_data.readAll();
        QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);

        if (loginDoc.isObject()) {
            QJsonObject JsonObj = loginDoc.object();
            QJsonArray usersArray = JsonObj["users"].toArray();
            for(int i=0;i<usersArray.size();++i)
            {
                if (!usersArray[i].isObject()) {
                    qDebug() << "Error: User entry is not an object";
                    database.close();
                    login_data.close();
                    return false;
                }

                QJsonObject userObject = usersArray[i].toObject();
                if(userObject.contains(username))
                {
                    QJsonObject user=userObject[username].toObject();
                    user["password"]=fieldValue;
                    userObject[username]=user;
                    usersArray.replace(i,userObject);
                   JsonObj["users"]=usersArray;
                   login_data.seek(0);
                   login_data.write(QJsonDocument(JsonObj).toJson(QJsonDocument::Indented));
                   login_data.close();
                   database.close();
                   return true;

            }


        }
    }
}

    else
    {

        if(m_Users.contains(accountnumber)&&m_Users[accountnumber].isObject())
        {
            QJsonObject userObject = m_Users[accountnumber].toObject();
            userObject[field]=fieldValue;
            m_Users[accountnumber] = userObject;
            database.seek(0);
            database.write(QJsonDocument(m_Users).toJson());
            database.close();
            return true;
        }
    }
       database.close();
        return false;
}

bool DataBase::MakeTransaction( qint32 TransactionAmount)
{
    QString oldBalance,Balance,details;
    oldBalance = GetField(AccNum,"Balance");
	qint32 newBalance=oldBalance.toInt();
    newBalance -= TransactionAmount;
	Balance = QString::number(newBalance);
	if(TransactionAmount>0)
	{
     details = "+"+QString::number(TransactionAmount);
	}
	else
	{
	  details = QString::number(TransactionAmount);
	}
    SaveTransaction(AccNum,details);

    bool ok=UpdateField(AccNum,"Balance",Balance);
    return ok;
}

void DataBase::SaveTransaction( QString accountnumber,QString &TransactionDetails)
{

    database.open(QIODevice::ReadWrite | QIODevice::Text);

    QByteArray jsonData = database.readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);


    QJsonObject usersObject = jsonDoc.object();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    QJsonObject transactionObject;
    transactionObject["datetime"] = dateTimeString;
    transactionObject["details"] = TransactionDetails;
    if(usersObject.contains(accountnumber)&&usersObject[accountnumber].isObject())
    {
      QJsonObject  userObject = usersObject[accountnumber].toObject();

    if (!userObject.contains("Transaction History")) {
        userObject["Transaction History"] = QJsonArray();
    }

    QJsonArray transactionHistoryArray = userObject["Transaction History"].toArray();

    transactionHistoryArray.prepend(transactionObject);
    userObject["Transaction History"] = transactionHistoryArray;
    usersObject[accountnumber]=userObject;
    database.seek(0);
    database.write(QJsonDocument(usersObject).toJson());
}
    database.close();
}

QString DataBase::ViewTransactionHistory(QString accountnumber,quint16 count)
{
    QString error="This account doesn't have a Transaction Hisstory";
    QString transactions;

    database.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray jsonData = database.readAll();
    database.close();


    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

      QJsonObject usersObject = jsonDoc.object();

    if(usersObject.contains(accountnumber)&&usersObject[accountnumber].isObject())
    {

        QJsonObject  userObject = usersObject[accountnumber].toObject();
        if(!userObject.contains("Transaction History"))
            return error;
         QJsonArray transactionHistoryArray = userObject["Transaction History"].toArray();

        int counter = 0;
        for (const QJsonValue& value: transactionHistoryArray)
        {
            if (counter == count)
                break;
            
            transactions += QJsonDocument(value.toObject()).toJson(QJsonDocument::Indented);
            counter++;
        }
        return transactions;

}
    return error;
}

bool DataBase::CheckAccNum(QString accountnumber)
{

    database.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray jsonData1 = database.readAll();
    database.close();

    QJsonDocument jsonDoc1 = QJsonDocument::fromJson(jsonData1);

    QJsonObject usersObject = jsonDoc1.object();

    return usersObject.contains(accountnumber) && usersObject[accountnumber].isObject();
}

bool DataBase::CheckField(QString accountnumber, QString field)
{

    database.open(QIODevice::ReadOnly | QIODevice::Text);
    // Read JSON data
    QJsonObject usersObject = QJsonDocument::fromJson(database.readAll()).object();
    database.close();
    // Parse JSON
    QJsonObject  userObject = usersObject[accountnumber].toObject();
    // if(userObject.contains(field))
    //     return true;
    return userObject.contains(field);

}

bool DataBase::UpdateUser(QString accountnumber, QVariantMap map)
{
    bool ok=false;

    database.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData = database.readAll();
    database.close();

    qInfo()<<"okkkk";
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject usersObject = jsonDoc.object();
    QJsonObject  userObject = usersObject[accountnumber].toObject();
    foreach (QString key, map.keys()) {
        if(key != "password")
        {
            qInfo()<<"not pass";
            ok =CheckField(accountnumber,key);

        }
        else
        {
            ok=true;
        }

        if(ok)
        {
            QString val=map[key].toString();
            qInfo()<<val;
            ok=UpdateField(accountnumber,key,val);
        }
    }
    return ok;
}

bool DataBase::CreateUser(QString username,QString password,QVariantMap map)
{

    database.open(QIODevice::ReadWrite| QIODevice::Text);
    QByteArray jsonData2 =database.readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData2);

    QJsonObject usersObject  = jsonDoc.object();
    

    QString accountnumber ="9998";
    QJsonObject userObject;
    userObject["Accountnumber"]=accountnumber;
    foreach (QString key, map.keys()) {
        userObject[key]=map[key].toString();
    }
    usersObject[accountnumber]=userObject;
    jsonDoc.setObject(usersObject);
    database.seek(0);
    database.write(jsonDoc.toJson(QJsonDocument::Indented));
    database.close();

    login_data.open(QIODevice::ReadWrite| QIODevice::Text);
    QByteArray loginData = login_data.readAll();
    QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);


    QJsonObject JsonObj = loginDoc.object();
    QJsonArray usersArray = JsonObj["users"].toArray();


    QJsonObject newUser;
    newUser["accountnumber"] = accountnumber;
    newUser["password"] = password;

    QJsonObject newUserArrayElement;
    newUserArrayElement[username] = newUser;


    usersArray.append(newUserArrayElement);

    JsonObj["users"]=usersArray;


    login_data.seek(0);
    login_data.write(QJsonDocument(JsonObj).toJson(QJsonDocument::Indented));


    login_data.close();

    return true;

}

bool DataBase::CheckUserName(QString username)
{

    login_data.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray jsonData = login_data.readAll();
    login_data.close();
    qInfo() << "Checking username";

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);


    QJsonObject JsonObj = jsonDoc.object();
    if (JsonObj.contains("users") && JsonObj["users"].isArray()) {
        QJsonArray usersArray = JsonObj["users"].toArray();

        for (const QJsonValue &userVal : usersArray) {
            if (!userVal.isObject()) {
                qDebug() << "Error: User entry is not an object";
                return false;
            }

            QJsonObject userObject = userVal.toObject();
            if (userObject.contains(username)) {
                qInfo() << "Username already exists";
                return false;  // Username found in the database
            }
        }
    } else {
        qDebug() << "Error: 'users' key not found or not an array in login_data";
        return false;
    }

    qInfo() << "Username is available";
    return true;
}

bool DataBase::DeleteUser(QString accountnumber)
{
    bool ok=CheckAccNum(accountnumber);
    if(!ok)
    {
        return ok;
    }
    QString username;
	username =GetField(accountnumber,"Username");

    login_data.open(QIODevice::ReadWrite| QIODevice::Text);
    QByteArray loginData = login_data.readAll();
    QJsonDocument loginDoc = QJsonDocument::fromJson(loginData);

    if (loginDoc.isObject()) {
        QJsonObject JsonObj = loginDoc.object();
        QJsonArray usersArray = JsonObj["users"].toArray();
        for(int i=0;i<usersArray.size();++i)
        {
            if (!usersArray[i].isObject()) {
                qDebug() << "Error: User entry is not an object";
                login_data.close();
                return false;
            }

            QJsonObject userObject = usersArray[i].toObject();
            if(userObject.contains(username))
            {
                usersArray.removeAt(i);
                JsonObj["users"]=usersArray;
                break;
            }

        }
        login_data.seek(0);
        login_data.resize(0);
        login_data.write(QJsonDocument(JsonObj).toJson(QJsonDocument::Indented));
        login_data.close();
    }
    else
    {
        qDebug()<<"loginDoc is not object";
        login_data.close();
        return false;
    }
    if (!database.open(QIODevice::ReadWrite | QIODevice::Text )) {
        qDebug() << "Error: Can't open the DataBase file";
        return false;
    }

    QByteArray jsonData = database.readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qDebug() << "Error: Failed to parse JSON data from update";
        database.close();
        return false;
    }
    if (!jsonDoc.isObject()) {
        qDebug() << "Error: JSON data is not an object";
        database.close();
        return false;
    }
    QJsonObject m_Users = jsonDoc.object();
    m_Users.remove(accountnumber);
    database.seek(0);
    database.resize(0);
    database.write(QJsonDocument(m_Users).toJson(QJsonDocument::Indented));
    database.close();
    return true;

}

QString DataBase::ViewBankDataBase()
{

    database.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray jsonData = database.readAll();
    database.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);



    QJsonObject bankObject = jsonDoc.object();

    QJsonArray bankArray;
    for (const QString& accountnumber : bankObject.keys()) {
        QJsonObject userObject = bankObject[accountnumber].toObject();
        bankArray.append(userObject);
    }

    return QJsonDocument(bankArray).toJson(QJsonDocument::Indented);
}




QString DataBase::role() const
{
    return _role;
}

void DataBase::setRole(const QString &newRole)
{
    _role = newRole;
}

QString DataBase::username() const
{
    return UserName;
}

QString DataBase::accountnumber() const
{
    return AccNum;
}


