#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>

class GameData : public QObject
{
    Q_OBJECT

    unsigned level;
    unsigned experience;
    unsigned money;
    unsigned winnings;
    unsigned losses;

public:
//    explicit GameData(QObject *parent = nullptr);
    explicit GameData(unsigned level = 1, unsigned experience = 0, unsigned money = 1000,
             unsigned winnings = 0, unsigned losses = 0, QObject *parent = nullptr);

    unsigned getLosses() const;

    unsigned getWinnings() const;

    unsigned getMoney() const;

    unsigned getExperience() const;

    unsigned getLevel() const;

signals:

public slots:
};

#endif // GAMEDATA_H
