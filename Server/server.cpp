#include "server.h"
#include "ui_server.h"
#include <QDateTime>


Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    m_nNextBlockSize = 0;
    server_status = 0;
    tcpServer = new QTcpServer(this);

    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newClient);
    //connect(ui->stopping,SIGNAL(clicked(bool)),this,SLOT(on_stoping_clicked()));
    connect(this, &Server::signalToSendAnswer,this, &Server::slotSendToClient);

    dis = new CardDistribution();

    //упрощаем себе запуск
    on_turnOn_clicked();
}

Server::~Server()
{
    if(server_status==1){
        on_disconnect_clicked();
        tcpServer->close();
        qDebug() << QString::fromUtf8("Сервер остановлен!");
        server_status = 0;
    }
    delete ui;
}

void Server::newClient()
{
    if(server_status == 1){

        tcpSocket = tcpServer->nextPendingConnection();
        ui->messages->append(QString::fromUtf8("У нас новое соединение!"));
        clients[tcpSocket] = "";

        slotSendToClient(tcpSocket, QString("You have connected!"));

        connect(tcpSocket, &QTcpSocket::readyRead, this, &Server::slotReadClient);
        //        connect(tcpSocket, &QAbstractSocket::disconnected, tcpSocket, &QAbstractSocket::deleteLater);
        connect(tcpSocket, &QAbstractSocket::disconnected, this, &Server::slotDisconnectClient);
    }
}

void Server::slotReadClient()
{
    qDebug() << "size" << rooms.size();
    if (rooms.size() > 0)
        for(int i(0); i < rooms.size(); i++)
            qDebug() << i << rooms[i]->first << rooms[i]->second;
    tcpSocket = (QTcpSocket*)sender();
    QTcpSocket *second = getSecondUser(tcpSocket);

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for (;;) {
        m_nNextBlockSize = 0;
        if (!m_nNextBlockSize) {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (tcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QString target;
        in >> target;


        if (target.compare("RequestGetCurrentKeg") == 0) {
            if (getRoomBySocket(tcpSocket) != nullptr) {
                int currentKeg = getRoomBySocket(tcpSocket)->getSack()->nextKeg();
                sendTwoNumbersToClient(tcpSocket, -1, currentKeg);
                ui->messages->append("Player " + clients[tcpSocket] + " got " + QString::number(currentKeg));
            }
            else {
                qDebug() << "MAGIC from " << tcpSocket;
            }
        }

        else if (target.compare("RequestPutTheKeg") == 0){
            int index;
            int value;
            in >> index >> value;
            if (second && second->socketDescriptor() != -1) sendTwoNumbersToClient(second, index, value);
            ui->messages->append("Player " + clients[tcpSocket] + " put " + QString::number(value)
                                 + " in line " + QString::number(index));
        }

        else if (target.compare("RequestAccount") == 0) {
            qDebug() << "request account";
            QString login, password;
            in >> login >> password;
            bool isAccountCorrect = db.checkAccount(login, password);
            sendConsent(tcpSocket, "IsAccountCorrect", isAccountCorrect);
            if (isAccountCorrect) {
                clients[tcpSocket] = login;
                ui->messages->append("Player " +  clients[tcpSocket] + " connected");
            }
        }

        else if (target.compare("RequestCreateRoom") == 0) {
            int bet;
            in >> bet;

            rooms.push_back(Room::createNewConnection(tcpSocket, bet));
            slotSendToClient(tcpSocket, "You have created new game!");
        }

        else if (target.compare("RequestJoinToRoom") == 0) {
            int index;
            in >> index;

            bool isCheck = false;
            for(Room * con : rooms) {
                if (con->getCurrentRoomNumber() == index) {
                    isCheck = true;
                    second = (con->first != nullptr) ? con->first : con->second;
                    con->addUser(tcpSocket);
                    slotSendToClient(second, "New player - " + clients[tcpSocket]);;
                    break;
                }
            }
            if (isCheck) {
                sendConsent(tcpSocket, "StartDistribution");
                sendConsent(second, "StartDistribution");
            }
        }

        else if (target.compare("RequestRemoveRoom") == 0) {
            removeRoomBySocket(tcpSocket);
        }

        else if (target.compare("RequestGetCards") == 0) {
            Player p;
            dis->appointCardsToPlayer(&p);
            QVector<Card *> cards = p.getCards();
            QVector<QVector<QVector<int> > > outputCards;
            for(int i(0); i < cards.size(); i++) {
                outputCards.push_back(cards[i]->getNumbers());
            }
            sendMyCardsToClient(tcpSocket, outputCards);
        }

        else if (target.compare("RequestGetMyGameData") == 0) {
            sendMyGameDataToClient(tcpSocket);
        }

        else if (target.compare("RequestGetOpponentGameData") == 0) {
            sendOpponentGameDataToClient(tcpSocket);
        }

        else if (target.compare("RequestGetAvailableRooms") == 0) {
            sendAvailableRooms(tcpSocket);
        }

        else if (target.compare("RequestReadyToPlay") == 0) {
            qDebug() << "ReadyToPlay";

            QVector<QVector<QVector<int> > > inputCards;
            in >> inputCards;
            //qDebug() << inputCards;
            QVector<Card *> cards;
            for(int i(0); i < inputCards.size(); i++) {
                cards.push_back(new Card());
                cards[i]->setNumbers(inputCards[i]);
            }
            sendEnemyCardsToClient(second, inputCards);
            getRoomBySocket(tcpSocket)->setReadyOfPlayer(tcpSocket);

            if (getRoomBySocket(tcpSocket)->isReadyToStartGame()) {
                qDebug() << "READY TO START GAME";
                sendConsent(tcpSocket, "StartGame");
                sendConsent(second, "StartGame");
            }
        }

        else if (target.compare("RequestCurrentBank") == 0) {

            sendCurrentBankToClient(tcpSocket);
        }

        else if (target.compare("RequestTakePartOfBank") == 0) {

            QTcpSocket *second = getSecondUser(tcpSocket);
            Room *currentRoom = getRoomBySocket(tcpSocket);
            slotSendToClient(second, "OpponentFilledFirstLine");
            db.accrueWinnings(clients[tcpSocket], currentRoom->getMoneyOfWinPart(), 1);
            currentRoom->takeWinPart();
            sendCurrentBankToClient(tcpSocket);
            sendCurrentBankToClient(second);
        }

        else if (target.compare("RequestBlockEnemyTurn") == 0) {
            qDebug() << "reques block enemy";
            QTcpSocket *second = getSecondUser(tcpSocket);
            slotSendToClient(second, "OpponentFilledSecondLine");

        }

        else if (target.compare("RequestExit") == 0) {

            QTcpSocket *second = getSecondUser(tcpSocket); //winner
            Room *currentRoom = getRoomBySocket(tcpSocket);
            slotSendToClient(second, "OpponentHasGone");
            db.accrueWinnings(clients[tcpSocket], currentRoom->getBet(), static_cast<int>(Experience::DEFAULT_ADDED));
            db.deductFunds(clients[second], currentRoom->getBet(), static_cast<int>(Experience::DEFAULT_TAKEN_LOSS));
            removeRoomBySocket(tcpSocket);
        }

        else if (target.compare("RequestGetVictory") == 0) {

            QTcpSocket *second = getSecondUser(tcpSocket); //winner
            Room *currentRoom = getRoomBySocket(tcpSocket);
            //Сбор ставок
            db.deductFunds(clients[tcpSocket], currentRoom->getBet(), 0);
            db.deductFunds(clients[second], currentRoom->getBet(), static_cast<int>(Experience::DEFAULT_TAKEN_LOSS));

            db.accrueWinnings(clients[tcpSocket], currentRoom->getBank(), static_cast<int>(Experience::DEFAULT_ADDED));
            removeRoomBySocket(tcpSocket);
        }

        else if (target.compare("RequestGetDraw") == 0) {
            qDebug() << "request draw";
            accrueDraw(tcpSocket);
        }

        else if (target.compare("Message") == 0){
            QString message;
            in >> message;
            if (second && second->socketDescriptor() != -1) emit signalToSendAnswer(second, message);
            ui->messages->append("Player " + QString::number(tcpSocket->socketDescriptor())
                                 + " requested " + message);
        }

        m_nNextBlockSize = 0;
    }
}

void Server::slotSendToClient(QTcpSocket *socket, QString str)
{
    qDebug() << "slotSend" << str;
    if (socket && socket->socketDescriptor() != -1) {
        qDebug() << "size" << rooms.size();
        if (rooms.size() > 0)
            for(int i(0); i < rooms.size(); i++)
                qDebug() << i << rooms[i]->first << rooms[i]->second;
        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        serverSendStream << quint16(0) << str;

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendMyCardsToClient(QTcpSocket *socket, QVector<QVector<QVector<int> > > first)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        serverSendStream << quint16(0) << QString("MyCards") << first;

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendEnemyCardsToClient(QTcpSocket *socket, QVector<QVector<QVector<int> > > second)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        serverSendStream << quint16(0) << QString("EnemyCards") << second;

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendConsent(QTcpSocket *socket, QString str, bool isPositive)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        serverSendStream << quint16(0) << str << isPositive;

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendTwoNumbersToClient(QTcpSocket *socket, int first, int second)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        serverSendStream << quint16(0) << QString("CardIndex_CurrentKeg") << first << second;

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendAvailableRooms(QTcpSocket *socket)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        QVector<Room *> local;
        for(int i(0); i < rooms.size(); i++) {
            if (rooms[i]->hasFreePlace()) {
                local.push_back(rooms[i]);
            }
        }

        serverSendStream << quint16(0) << QString("AvailableRooms") << local.size();
        for (int i(0); i < local.size(); i++) {
            serverSendStream << local[i]->getCurrentRoomNumber() << clients[local[i]->first] << local[i]->getBet();
        }

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendMyGameDataToClient(QTcpSocket *socket)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        GameData *gameData = db.getGameDataByLogin(clients[tcpSocket]);
        int experience = gameData->getExperience();
        int confirmedLevel = getLevelByExperience(experience);
        serverSendStream << quint16(0) << QString("MyGameData") << clients[tcpSocket] << confirmedLevel << experience
                         << gameData->getMoney() << gameData->getWinnings() << gameData->getLosses();

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendOpponentGameDataToClient(QTcpSocket *socket)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        QTcpSocket *opponent = getSecondUser(tcpSocket);
        GameData *gameData = db.getGameDataByLogin(clients[opponent]);

        int experience = gameData->getExperience();
        int confirmedLevel = getLevelByExperience(experience);
        serverSendStream << quint16(0) << QString("OpponentGameData") << clients[opponent] << confirmedLevel << experience
                         << gameData->getMoney() << gameData->getWinnings() << gameData->getLosses();

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

void Server::sendCurrentBankToClient(QTcpSocket *socket)
{
    if (socket && socket->socketDescriptor() != -1) {

        QByteArray arrBlock;
        QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);

        Room *currentRoom = getRoomBySocket(socket);
        serverSendStream << quint16(0) << QString("CurrentBank") << currentRoom->getBank();

        serverSendStream.device()->seek(0);
        serverSendStream << quint16(arrBlock.size() - sizeof(quint16));
        socket->write(arrBlock);
    }
}

Room *Server::getRoomBySocket(QTcpSocket *socket)
{
    for (int i(0); i < rooms.size(); i++) {
        if (rooms[i]->contains(socket)) {
            return rooms[i];
        }
    }
    return nullptr;
}

bool Server::removeRoomBySocket(QTcpSocket *socket)
{
    for (int i(0); i < rooms.size(); i++) {
        if (rooms[i]->contains(socket)) {
            rooms.removeAt(i);
            return true;
        }
    }
    return false;
}

void Server::setFinishForRoom(QTcpSocket *socket)
{
    for (int i(0); i < rooms.size(); i++) {
        if (rooms[i]->contains(socket)) {
            rooms[i]->setIsGameFinished(true);
            break;
        }
    }
}

int Server::getLevelByExperience(int &experience)
{
    int level = (experience / static_cast<int> (Experience::DEFAULT_PER_LEVEL)) + 1;
    experience %= static_cast<int> (Experience::DEFAULT_PER_LEVEL);
    return level;
}

void Server::accrueDraw(QTcpSocket *socket)
{
    Room *currentRoom = getRoomBySocket(socket);
    //Сбор ставок
    db.deductFunds(clients[socket], currentRoom->getBet(), static_cast<int>(Experience::DEFAULT_TAKEN_DRAW));

    db.accrueWinnings(clients[socket], currentRoom->getBank() / 2, 0);

    if (currentRoom->getIsGameFinished()) {
        qDebug() << "draw - game finished";
        removeRoomBySocket(socket);
    }
    else {
        qDebug() << "draw - first@";
        currentRoom->setIsGameFinished(true);
    }
}

void Server::slotDisconnectClient()
{
    qDebug() << "disconnect" << sender();
    if (rooms.size() > 0)
        for(int i(0); i < rooms.size(); i++)
            qDebug() << i << rooms[i]->first << rooms[i]->second;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
    //    qDebug() << tcpSocket->socketDescriptor();

    QTcpSocket *second = getSecondUser(tcpSocket);//winner
    for (int i(0); i < rooms.size(); i++) {
        if (rooms[i]->contains(tcpSocket)) {
            if (getSecondUser(tcpSocket) != nullptr) {
                if(!rooms[i]->getIsGameFinished() && rooms[i]->isReadyToStartGame()){
                    slotSendToClient(second, "OpponentHasGone");
                    db.accrueWinnings(clients[second], rooms[i]->getBet(), static_cast<int>(Experience::DEFAULT_ADDED));
                    db.deductFunds(clients[tcpSocket], rooms[i]->getBet(), static_cast<int>(Experience::DEFAULT_TAKEN_LOSS));
                }
                else if (!rooms[i]->isReadyToStartGame()) {
                    //игра не началась а противник ушел
                    slotSendToClient(second, "Opponent has gone");
                }
            }


            //if (disconnected == first)
            rooms.removeAt(i);
            break;
        }
    }

}

QTcpSocket *Server::getSecondUser(QTcpSocket *socket)
{
    if (socket != nullptr) {
        for (int i(0); i < rooms.size(); i++) {
            if (rooms[i]->contains(socket)) {
                return rooms[i]->getSecond(socket);
            }
        }
    }
    return nullptr;
}

void Server::on_sendInfo_clicked()
{
    for (auto socket : clients.keys()) {
        slotSendToClient(socket, ui->sending->text());
    }
}

void Server::on_turnOn_clicked()
{
    if (server_status == 0 && !tcpServer->listen(QHostAddress::Any, 88)) {
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(tcpServer->errorString());
        //ui->textinfo->append(tcpServer->errorString());
    }
    else if (server_status == 1){
        ui->messages->append("Сервер уже запущен");
    }
    else {
        server_status = 1;
        qDebug() << tcpServer->isListening() << "TCPSocket listen on port";
        ui->messages->append(QString::fromUtf8("Сервер запущен!"));
    }
}
void Server::on_disconnect_clicked()
{
    if(server_status == 1){
        for(auto it = clients.begin(); it != clients.end(); it++){
            if (it.key() != nullptr) {
                slotSendToClient(it.key(), "Server is temporarily unavailable");
                it.key()->close();
            }
        }
        rooms.clear();
        tcpServer->close();
        ui->messages->append(QString::fromUtf8("Сервер остановлен!"));
        server_status = 0;
    }
}

