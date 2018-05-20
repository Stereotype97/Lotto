#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMap>
#include <QVector>
#include <room.h>
#include <carddistribution.h>
#include <card.h>
#include <player.h>
#include <database.h>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();
    enum class Experience{DEFAULT_ADDED = 10, DEFAULT_TAKEN_LOSS = 3,
                          DEFAULT_TAKEN_DRAW = 1, DEFAULT_PER_LEVEL = 50};

    QTcpSocket *getSecondUser(QTcpSocket *socket);
    void sendMyCardsToClient(QTcpSocket *socket, QVector<QVector<QVector<int> > > first);
    void sendEnemyCardsToClient(QTcpSocket *socket, QVector<QVector<QVector<int> > > second);
    void sendConsent(QTcpSocket *socket, QString str, bool isPositive = true);
    void sendTwoNumbersToClient(QTcpSocket *socket, int first, int second);
    void sendAvailableRooms(QTcpSocket *socket);
    void sendMyGameDataToClient(QTcpSocket *socket);
    void sendOpponentGameDataToClient(QTcpSocket *socket);
    void sendCurrentBankToClient(QTcpSocket *socket);

    Room *getRoomBySocket(QTcpSocket *socket);
    bool removeRoomBySocket(QTcpSocket *socket);
    void setFinishForRoom(QTcpSocket *socket);
    int getLevelByExperience(int& experience);
    void accrueDraw(QTcpSocket *socket);
signals:
    void signalToSendAnswer(QTcpSocket *socket, QString str);


public slots:
    void newClient();
    void slotReadClient();
    void slotSendToClient(QTcpSocket *socket, QString str);
    void slotDisconnectClient();
    void on_sendInfo_clicked();

private slots:
    void on_disconnect_clicked();

    void on_turnOn_clicked();

private:
    Ui::Server *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    quint16 m_nNextBlockSize;
    int server_status;

    QMap<QTcpSocket *, QString> clients;
    QVector<Room *> rooms;

    CardDistribution *dis;

    Database db;
};

#endif // SERVER_H
