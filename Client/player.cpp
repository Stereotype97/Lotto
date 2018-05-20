#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    isReElectionable_ = true;
}

Player::Player(int id)
{
    IdentProfile = id;
    isReElectionable_ = true;
}


void Player::putKeg(int index, int value)
{
    cards[index]->setKeg(value);
    checkFillingOfCard(cards[index]);
}

QVector<int> Player::getAllNumbersFromPlayerByIndex(int index)
{
    return this->getCard(index)->getAllNumbers();
}

QVector<Card *> Player::getCards()
{
    return cards;
}

Card *Player::getCard(int index)
{
    //qDebug() << index << cards;
    return cards[index];
}

int Player::amountOfCards()
{
    return cards.size();
}

void Player::setCards(QVector<Card *> nCards)
{
    cards = nCards;
    qDebug() << cards;
}

bool Player::isReElectionable()
{
    return isReElectionable_;
}

void Player::setIsReElectionable(const bool &b)
{
    qDebug() << "!!!!!!!!!!!!!!";
    if (b == isReElectionable_)
        return;

    isReElectionable_ = b;
    emit isReElectionableChanged();
}

void Player::checkFillingOfCard(Card *card)
{
    if (card->isWinner()) {
        qDebug() << "STOP GAME! Player with id " << IdentProfile << "won11111111111111";
        emit playerWon();
        return;
    }
    auto fillingOfTwoLines = card->isNeedTakePartOfCush();
    if (fillingOfTwoLines.first){
        emit firstLineFilled();
        qDebug() << "FIRST LINE";
    }
    if (fillingOfTwoLines.second){
        emit secondLineFilled();
        qDebug() << "SECOND LINE";
    }

}
