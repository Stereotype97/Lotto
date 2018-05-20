#ifndef CLIENT_H
#define CLIENT_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QAbstractSocket>
#include <QVector>
#include <player.h>
#include <card.h>
#include <room.h>
#include <QTime>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    Player *iPlayer();
    Player *enemyPlayer();
    Room *getRoom(int index);

public slots:

    void up();
    QVector<int> getAllNumbersFromIPlayerByIndex(int index);
    QVector<int> getAllNumbersFromEnemyByIndex(int index);
    void onSocketDisplayError(QAbstractSocket::SocketError socketError);
    void slotConnected();
    void slotSendAccountData(QString login, QString password);
    void slotReadyRead();
    void disconnectFromServer();
    void connectToServer(QString login, QString password);
    void createRoom(int bet);
    void joinToRoom(int indexOfRoom);
    void sendMessage(const QString &msg);
    void getCards();
    void readyToPlay();
    void getCurrentKeg();
    void putTheKeg(int index, int value);
    void getAvailableRooms();
    int getCountOfAvailableRooms();
    void removeRoom();
    void getVictory();
    void getMyGameData();
    void getOpponentGameData();
    void getCurrentBank();
    void takePartOfBank();
    void blockEnemyTurn();
    void getDraw();
    void exit();

private slots:

signals:
    void isAccountCorrect(const bool &isAccountCorrect);
    void iPlayerChanged();
    void enemyPlayerChanged();
    void roomsChanged();
    void myCardsSet();
    void enemyCardsSet();
    void startDistribution();
    void startGame();
    void newCurrentKeg(const int &newCurrentKeg);
    void isNeedToPutKeg(const int &indexOfCards, const int &newCurrentKeg);
    void isAvailableRooms(int number, QString nickname, int bet);
    void opponentHasGone();
    void isMyGameData(const QString &nickname, const unsigned &level, const unsigned &experience, const unsigned &money,
                      const unsigned &winnings, const unsigned &losses, const unsigned &draws);
    void isOpponentGameData(const QString &nickname, const unsigned &level, const unsigned &experience,
                            const unsigned &money, const unsigned &winnings, const unsigned &losses, const unsigned &draws);


    void bankChanged(int bank);
    void iWon();
    void enemyWon();
    void skipTurn();

private:

    QTcpSocket *tcpSocket;

    QString name;
    quint16 blockSize;

    Player *iPlayer_;
    Player *enemyPlayer_;
    QVector<Room *> rooms;
};

#endif // CLIENT_H
