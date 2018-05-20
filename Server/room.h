#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <QTcpSocket>
#include <sack.h>
#define WIN_PART 10

class Room : public QObject
{
    Q_OBJECT
public:
    explicit Room(QObject *parent = nullptr);
    Room(QTcpSocket *socket, int bet, QObject *parent = nullptr);
    ~Room();
    static Room *createNewConnection(QTcpSocket *socket, int bet);
    bool addUser(QTcpSocket *socket);
    bool hasFreePlace();
    bool contains(QTcpSocket *socket);
    bool isFirst(QTcpSocket *socket);
    bool isSecond(QTcpSocket *socket);
    void removeUser(QTcpSocket *socket);
    QTcpSocket *getSecond(QTcpSocket *socket);

    void takeWinPart();

    QTcpSocket *first;
    QTcpSocket *second;

    Sack *sack;

    Sack *getSack() const;
    bool isReadyToStartGame() const;
    void setReadyOfPlayer(QTcpSocket *socket); //проверка на наличие этого сокета производится на вызывающей стороне


    int getCurrentRoomNumber() const;
    int getBet() const;
    bool getIsGameFinished() const;

    void setIsGameFinished(bool value);

    int getBank() const;
    int getMoneyOfWinPart() const;

signals:

public slots:

private:
    static int countConnections;
    int currentRoomNumber;
    int bet;
    int bank;
    int moneyOfWinPart;

    bool isFirstReady;
    bool isSecondReady;

    bool isGameFinished;
};

#endif // ROOM_H
