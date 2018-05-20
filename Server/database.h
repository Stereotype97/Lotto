#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <gamedata.h>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    void connectToDataBase();

    //merge into one function with bool
    bool accrueWinnings(QString login, int bet, int experience);
    bool deductFunds(QString login, int bet, int experience);

    bool checkAccount(QString login, QString password);
    GameData *getGameDataByLogin(QString login);

    bool removeRecord(QString table, const int id); // Удаление записи из таблицы по её id

private:
//    QSqlDatabase db;

    bool openDataBase();
    bool restoreDataBase();
};

#endif // DATABASE_H
