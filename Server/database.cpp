#include "database.h"

Database::Database(QObject *parent) : QObject(parent)
{

}

Database::~Database()
{

}

void Database::connectToDataBase()
{
    //    qDebug() << "connect" << db.database("conn").isOpen();
    //    if(!db.database("conn").isOpen())
    //        restoreDataBase();
    //    else
    //        openDataBase();
}

bool Database::accrueWinnings(QString login, int bet, int experience)
{
    bool isSuccessful = false;
    QSqlDatabase db;
    //    qDebug() << "ISOPEN" << db.database(QSqlDatabase::defaultConnection).isOpen();

    if (db.database().isOpen()) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    if (!db.isOpen()){
        qDebug() << "!isOpen";
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("lotto");
        db.setUserName("root");
        db.setPassword("leagoo");
    }
    if(!db.open()){
        qDebug() << "Cannot open database: " << db.lastError();
        return false;
    }

    QSqlQuery query;
    QString str_t = "call accrue_winnings('%1', '%2', '%3');";
    QString str = str_t.arg(login)
            .arg(bet)
            .arg(experience);


    if(!query.exec(str)) {
        qDebug() << "error accrue winnings";
        qDebug() << query.lastError().text();
    } else {
        qDebug() << str;
        isSuccessful = true;
    }
    db.close();

    return isSuccessful;
}

bool Database::deductFunds(QString login, int bet, int experience)
{
    bool isSuccessful = false;
    QSqlDatabase db;
    if (db.database().isOpen()) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    if (!db.isOpen()){
        qDebug() << "!isOpen";
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("lotto");
        db.setUserName("root");
        db.setPassword("leagoo");
    }
    if(!db.open()){
        qDebug() << "Cannot open database: " << db.lastError();
        return false;
    }

    QSqlQuery query;
    QString str_t = "call deduct_funds('%1', '%2', '%3');";
    QString str = str_t.arg(login)
            .arg(bet)
            .arg(experience);

    if(!query.exec(str)) {
        qDebug() << "error deduct funds";
        qDebug() << query.lastError().text();
    } else {
        qDebug() << str;
        isSuccessful = true;
    }

    db.close();
    return isSuccessful;
}

bool Database::checkAccount(QString login, QString password)
{
    QSqlDatabase db;
    if (db.database().isOpen()) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    if (!db.isOpen()){
        qDebug() << "!isOpen";
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("lotto");
        db.setUserName("root");
        db.setPassword("leagoo");
    }
    if(!db.open()){
        qDebug() << "Cannot open database: " << db.lastError();
        return false;
    }

    QSqlQuery query;
    QString str_t = "select login, `password` from accounts "
                    "where login =  '%1';";
    QString str = str_t.arg(login);

    if(!query.exec(str)) {
        qDebug() << "error account";
        qDebug() << query.lastError().text();
        return false;
    } else {
        qDebug() << str;
    }

    QSqlRecord record = query.record();
    QString rightPassword;
    while(query.next()) {
        rightPassword = query.value(record.indexOf("password")).toString();
        qDebug() << "rigthPassword" << rightPassword << password;
        if (rightPassword == password) {

            return true;
        }
    }
    db.close();
    return false;
}

GameData *Database::getGameDataByLogin(QString login)
{
    QSqlDatabase db;
    if (db.database().isOpen()) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    if (!db.isOpen()){
        qDebug() << "!isOpen";
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setDatabaseName("lotto");
        db.setUserName("root");
        db.setPassword("leagoo");
    }
    if(!db.open()){
        qDebug() << "Cannot open database: " << db.lastError();
        return nullptr;
    }

    QSqlQuery query;
    QString str_t = "select * from game_data "
                    "where game_data_id = (select account_id from accounts "
                    "where login = '%1');";
    QString str = str_t.arg(login);

    if(!query.exec(str)) {
        qDebug() << "error game_data";
        qDebug() << query.lastError().text();
        return nullptr;
    } else {
        qDebug() << str;
    }

    QSqlRecord record = query.record();
    unsigned level;
    unsigned experience;
    unsigned money;
    unsigned winnings;
    unsigned losses;
    GameData *result;
    while(query.next()) {
        level = query.value(record.indexOf("level")).toUInt();
        experience = query.value(record.indexOf("experience")).toUInt();
        money = query.value(record.indexOf("money")).toUInt();
        winnings = query.value(record.indexOf("victories")).toUInt();
        losses = query.value(record.indexOf("losses")).toUInt();
        qDebug() << level << experience << money << winnings << losses;
        result = new GameData(level, experience, money, winnings, losses);
    }

    db.close();
    return result;
}

bool Database::removeRecord(QString table, const int id)
{
    // Удаление строки из базы данных будет производитсья с помощью SQL-запроса
    QSqlQuery query;
    qDebug() << id;
    // Удаление производим по id записи, который передается в качестве аргумента функции
    query.prepare("DELETE FROM " + table + " WHERE id = :ID ;");
    query.bindValue(":ID", id);
    qDebug() << query.executedQuery();
    // Выполняем удаление
    if(!query.exec()){
        qDebug() << "error delete row " << table;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}

bool Database::openDataBase()
{
    QSqlDatabase db;
    if (!db.isOpen()){
        qDebug() << "!isOpen";

        db.setDatabaseName("lotto");
        db.setUserName("root");
        db.setPassword("leagoo");
    }
    else {
        qDebug() << "DB is Open";
        return true;
    }
    if(!db.open()){
        qDebug() << "Cannot open database: " << db.lastError();
        return false;
    }


    return false;
}

bool Database::restoreDataBase()
{
    if(openDataBase()){
        qDebug() << "Восстановление базы данных";
    } else {
        qDebug() << "Не удалось восстановить базу данных";
        return false;
    }
    return false;
}
