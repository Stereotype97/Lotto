#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{

}

Player::Player(int id)
{
    IdentProfile = id;
}

void Player::putKeg(int index, int value)
{
    cards[index]->setKeg(value);
    checkFillingOfCard(cards[index]);
}

QVector<Card *> Player::getCards()
{
    return cards;
}

Card *Player::getCard(int index)
{
    return cards[index];
}

int Player::amountOfCards()
{
    return cards.size();
}

void Player::setCards(QVector<Card *> nCards)
{
    cards = nCards;
}

void Player::checkFillingOfCard(Card *card)
{

    if (card->isWinner()) {
        qDebug() << "STOP GAME! Player with id " << IdentProfile << "won";
        emit playerWon();
        return;
    }
    auto fillingOfTwoLines = card->isNeedTakePartOfCush();

    if (fillingOfTwoLines.first){
        qDebug() << "FIRST LINE";
    }
    if (fillingOfTwoLines.second){
        qDebug() << "SECOND LINE";
    }

}
