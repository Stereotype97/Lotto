#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    blockSize = 0;
    name = "";
    iPlayer_ = enemyPlayer_ = nullptr;

    tcpSocket = new QTcpSocket(this);
    //realloc data failure
    rooms.reserve(5);
}

Player *Client::iPlayer()
{
    qDebug() << iPlayer_->amountOfCards() << iPlayer_;
    return iPlayer_;
}

Player *Client::enemyPlayer()
{
    return enemyPlayer_;
}

Room *Client::getRoom(int index)
{
    return rooms[index];
}

void Client::up()
{
    blockSize = 0;
    name = "";
    iPlayer_ = enemyPlayer_ = nullptr;
    tcpSocket = new QTcpSocket(this);
    //realloc data failure
    rooms.reserve(5);
}

QVector<int> Client::getAllNumbersFromIPlayerByIndex(int index)
{
    return iPlayer_->getCard(index)->getAllNumbers();
}

QVector<int> Client::getAllNumbersFromEnemyByIndex(int index)
{
    return enemyPlayer_->getCard(index)->getAllNumbers();
}

void Client::connectToServer(QString login, QString password)
{
    if (tcpSocket->isOpen()) {
        tcpSocket->disconnectFromHost();
    }
    iPlayer_ = enemyPlayer_ = nullptr;

    qDebug() << "connect";
    tcpSocket->connectToHost("77.47.223.72", 88);
//    tcpSocket->connectToHost("100.100.14.116", 88);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::slotReadyRead);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketDisplayError(QAbstractSocket::SocketError)));
    slotSendAccountData(login, password);
}

void Client::onSocketDisplayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "Error" << "The host was not found";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "Error" << "The connection was refused by the peer.";
        break;
    default:
        qDebug() << "Error" << "The following error occurred: " << tcpSocket->errorString();
    }
}

void Client::disconnectFromServer()
{
    tcpSocket->disconnectFromHost();
}


void Client::slotConnected()
{
    qDebug() << "Received the connected() signal";
}

void Client::slotSendAccountData(QString login, QString password)
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    serverSendStream << quint16(0) << QString("RequestAccount") << login << password;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::slotReadyRead()
{
//    qDebug() << "startIter";
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for (;;) {
        if (!blockSize) {
            if (tcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> blockSize;
        }

        if (tcpSocket->bytesAvailable() < blockSize) {
            break;
        }
        QString str;
        in >> str;
        if (str.compare("CardIndex_CurrentKeg") == 0) {
//            qDebug() << "CardIndex_CurrentKegStart";
            int indexOfCards, keg;
            in >> indexOfCards >> keg;

            if (indexOfCards == -1) {
//                qDebug() << "newCurrentKegStart";
                emit newCurrentKeg(keg);
            }
            else {
//                qDebug() << "isNeedToPutKegStart";
                emit isNeedToPutKeg(indexOfCards, keg);
            }
//            qDebug() << "CardIndex_CurrentKegEnd";
        }

        else if (str.compare("OpponentFilledFirstLine") == 0) {

        }

        else if (str.compare("OpponentFilledSecondLine") == 0) {
            emit skipTurn();
        }

        else if (str.compare("CurrentBank") == 0) {
            int bank = 0;
            in >> bank;
            emit bankChanged(bank);
            qDebug() << "BANK CHANGED " << bank;
        }

        else if (str.compare("MyCards") == 0) {
            in.startTransaction();
            QVector<QVector<QVector<int> > > inputCards;
            in >> inputCards;
            if (!in.commitTransaction()){
                qDebug()<<"TransError";
            }
            //qDebug() << inputCards;
            QVector<Card *> cards;
            for(int i(0); i < inputCards.size(); i++) {
                cards.push_back(new Card());
                cards[i]->setNumbers(inputCards[i]);
            }
            //qDebug() << "set game my cards";
//            if (iPlayer_ != nullptr)
//                delete iPlayer_;
            iPlayer_ = new Player(1);
            iPlayer_->setCards(cards);

            emit myCardsSet();

            connect(iPlayer_, &Player::firstLineFilled, this, &Client::takePartOfBank);
            connect(iPlayer_, &Player::secondLineFilled, this, &Client::blockEnemyTurn);
            qDebug() << "My cards";
            for (int i(0); i < iPlayer_->getCards().size(); i++) {
                qDebug() << iPlayer_->getCard(i)->toString();
            }
        }

        else if (str.compare("EnemyCards") == 0) {
            QVector<QVector<QVector<int> > > inputCards;
            in >> inputCards;
            if (!in.commitTransaction()){
                qDebug()<<"TransError";
            }
            //qDebug() << inputCards;
            QVector<Card *> cards;
            for(int i(0); i < inputCards.size(); i++) {
                cards.push_back(new Card());
                cards[i]->setNumbers(inputCards[i]);
            }
            //qDebug() << "set game enemy cards";
//            if (enemyPlayer_ != nullptr)
//                delete enemyPlayer_;
            enemyPlayer_ = new Player(2);
            enemyPlayer_->setCards(cards);
            emit enemyCardsSet();
            qDebug() << "Enemy cards";
            for (int i(0); i < enemyPlayer_->getCards().size(); i++) {
                qDebug() << enemyPlayer_->getCard(i)->toString();
            }
        }

        else if (str.compare("OpponentFilledSecondLine") == 0) {
            qDebug() << "Opponent second line";
            emit skipTurn();
        }

        else if (str.compare("StartDistribution") == 0) {
            bool isConsentCreateGame;
            in >> isConsentCreateGame;
            if (isConsentCreateGame) {
                emit startDistribution();
                //qDebug() << "Распределение карточек" << QTime::currentTime().toString() << ":" << QString::number(QTime::currentTime().msec());
            }
        }

        else if (str.compare("StartGame") == 0) {
            bool isConsentStartGame;
            in >> isConsentStartGame;
            if (isConsentStartGame) {
                connect(iPlayer_, &Player::playerWon, this, &Client::iWon);
                connect(enemyPlayer_, &Player::playerWon, this, &Client::enemyWon);
                emit startGame();
                //qDebug() << "Старт игры";
            }
        }
        else if (str.compare("AvailableRooms") == 0) {
//            for (int i = 0; i < rooms.size(); i++) {
//                delete rooms[i];
//            }
            rooms.clear();
            int amountAvailableRooms;
            in >> amountAvailableRooms;
            qDebug() << "amount" << amountAvailableRooms;
            for (int i = 0; i < amountAvailableRooms; i++) {
                int roomNumber, bet;
                QString nickname;
                in >> roomNumber >> nickname >> bet;
                qDebug() << roomNumber << nickname << bet;
                Room *room = new Room(roomNumber, "", nickname, bet);
                rooms.push_back(room);
                //QVariantList temp;
                //temp.push_back(QVariant(roomNumber));
                //, "", nickname, bet);
                emit isAvailableRooms(roomNumber, nickname, bet);
            }
//            emit isAvailableRooms();
            //qDebug() << rooms_.size();
        }
        else if (str.compare("OpponentHasGone") == 0) {
            emit opponentHasGone();
        }
        else if (str.compare("IsAccountCorrect") == 0) {
            bool check;
            in >> check;
            qDebug() << check;
            emit isAccountCorrect(check);
        }
        else if (str.compare("MyGameData") == 0) {
            QString nickname;
            unsigned level, experience, money, winnings, losses;
            in >> nickname >> level >> experience >> money >> winnings >> losses;
            //qDebug() << nickname << level << experience << money << winnings << losses;
            emit isMyGameData(nickname, level, experience, money, winnings, losses, 0);
        }
        else if (str.compare("OpponentGameData") == 0) {
            QString nickname;
            unsigned level, experience, money, winnings, losses;
            in >> nickname >> level >> experience >> money >> winnings >> losses;
            //qDebug() << nickname << level << experience << money << winnings << losses;
            emit isOpponentGameData(nickname, level, experience, money, winnings, losses, 0);
        }
        //        else if (str.compare("Message") == 0) {
        //            QString str;
        //            qDebug() << "Message: " << str;
        //        }
        else {
            qDebug() << "Server: " << str;
        }

        blockSize = 0;
//        qDebug() << "end iter";
    }
}



void Client::createRoom(int bet)
{
    //qDebug() << "new game";
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    serverSendStream << quint16(0) << QString("RequestCreateRoom") << bet;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::joinToRoom(int indexOfRoom)
{
    //qDebug() << "join to room";
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    serverSendStream << quint16(0) << QString("RequestJoinToRoom") << indexOfRoom;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::sendMessage(QString const &msg)
{
    //qDebug() << "send";
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    serverSendStream << quint16(0) << QString("Message") << msg;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getCards()
{

    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    serverSendStream << quint16(0) << QString("RequestGetCards");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::readyToPlay()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);
    //convert to vec<vec<int> >
    QVector<QVector<QVector<int> > > myCards;
    for(int i(0); i < iPlayer_->getCards().size(); i++) {
        myCards.push_back(iPlayer_->getCards()[i]->getNumbers());
    }
    serverSendStream << quint16(0) << QString("RequestReadyToPlay") << myCards;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getCurrentKeg()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetCurrentKeg");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::putTheKeg(int index, int value)
{
    qDebug() << "putTheKegStart";
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestPutTheKeg") << index << value;

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
    qDebug() << "putTheKegEnd";
}

void Client::getAvailableRooms()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetAvailableRooms");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

int Client::getCountOfAvailableRooms()
{
    return rooms.size();
}

void Client::removeRoom()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestRemoveRoom");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getVictory()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetVictory");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getMyGameData()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetMyGameData");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getOpponentGameData()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetOpponentGameData");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getCurrentBank()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestCurrentBank");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::takePartOfBank()
{
    qDebug() << "take Part";
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestTakePartOfBank");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::blockEnemyTurn()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestBlockEnemyTurn");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::getDraw()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestGetDraw");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}

void Client::exit()
{
    QByteArray arrBlock;
    QDataStream serverSendStream(&arrBlock, QIODevice::WriteOnly);
    serverSendStream.setVersion(QDataStream::Qt_5_9);

    serverSendStream << quint16(0) << QString("RequestExit");

    serverSendStream.device()->seek(0);
    serverSendStream << quint16(arrBlock.size() - sizeof(quint16));

    tcpSocket->write(arrBlock);
}
