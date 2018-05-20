#include "room.h"

int Room::countConnections;
Room::Room(QObject *parent) : QObject(parent)
{
    countConnections++;
    currentRoomNumber = countConnections;
}

Room::Room(QTcpSocket *socket, int bet, QObject *parent) : QObject(parent)
{
    countConnections++;
    currentRoomNumber = countConnections;
    first = socket;
    second = nullptr;
    sack = new Sack();
    isFirstReady = isSecondReady = false;
    this->bet = bet;
    isGameFinished = false;
}

Room::~Room()
{
    if (first) delete first;
    if (second) delete second;
    delete sack;
}

Room *Room::createNewConnection(QTcpSocket *socket, int bet)
{
    return new Room(socket, bet);
}

bool Room::addUser(QTcpSocket *socket)
{
    if (hasFreePlace()) {
        if (first == nullptr) first = socket;
        else /*if (second == nullptr)*/ second = socket;
        bank = bet * 2;
        moneyOfWinPart = bank / WIN_PART;
        return true;
    }
    return false;
}

bool Room::hasFreePlace()
{
    return first == nullptr || second == nullptr;
}

bool Room::contains(QTcpSocket *socket)
{
    if (socket != nullptr)
        return (first && first->socketDescriptor() == socket->socketDescriptor())
                || (second && second->socketDescriptor() == socket->socketDescriptor());

    return false;
}

bool Room::isFirst(QTcpSocket *socket)
{
    return socket->socketDescriptor() == first->socketDescriptor();
}

bool Room::isSecond(QTcpSocket *socket)
{
    return socket->socketDescriptor() == second->socketDescriptor();
}

void Room::removeUser(QTcpSocket *socket)
{
    if (first && first->socketDescriptor() == socket->socketDescriptor()) {

        delete first;
        first = nullptr;
         qDebug() << "fir";
        return;
    }
    if (second && second->socketDescriptor() == socket->socketDescriptor()) {
        delete second;
        second = nullptr;
        qDebug() << "sec";
        return;
    }
    qDebug() << "MAGIC";
}

QTcpSocket *Room::getSecond(QTcpSocket *socket)
{
    if (this->first && this->first->socketDescriptor() == socket->socketDescriptor())
        return second;
    else if (second && second->socketDescriptor() == socket->socketDescriptor())
        return first;

    return nullptr;
}

void Room::takeWinPart()
{
    bank -= moneyOfWinPart;
}

Sack *Room::getSack() const
{
    return sack;
}

bool Room::isReadyToStartGame() const
{
    return isFirstReady && isSecondReady;
}

void Room::setReadyOfPlayer(QTcpSocket *socket)
{
    if (socket->socketDescriptor() == first->socketDescriptor()) {
        isFirstReady = true;
    }
    else if (socket->socketDescriptor() == second->socketDescriptor()) {
        isSecondReady = true;
    }
}

int Room::getCurrentRoomNumber() const
{
    return currentRoomNumber;
}

int Room::getBet() const
{
    return bet;
}

bool Room::getIsGameFinished() const
{
    return isGameFinished;
}

void Room::setIsGameFinished(bool value)
{
    isGameFinished = value;
}

int Room::getBank() const
{
    return bank;
}

int Room::getMoneyOfWinPart() const
{
    return moneyOfWinPart;
}
