#include "room.h"

Room::Room(QObject *parent) : QObject(parent)
{

}

int Room::numberOfRoom()
{
    return numberOfRoom_;
}

int Room::bet()
{
    return bet_;
}

QString Room::description()
{
    return description_;
}

QString Room::nickname()
{
    return nickname_;
}
