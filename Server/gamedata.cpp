#include "gamedata.h"

//GameData::GameData(QObject *parent) : QObject(parent)
//{

//}

unsigned GameData::getLosses() const
{
    return losses;
}

unsigned GameData::getWinnings() const
{
    return winnings;
}

unsigned GameData::getMoney() const
{
    return money;
}

unsigned GameData::getExperience() const
{
    return experience;
}

unsigned GameData::getLevel() const
{
    return level;
}

GameData::GameData(unsigned level, unsigned experience, unsigned money, unsigned winnings, unsigned losses, QObject *parent)
    : QObject(parent), level(level), experience(experience), money(money), winnings(winnings), losses(losses)
{

}
