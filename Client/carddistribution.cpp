#include "carddistribution.h"
#include <QTime>
#include <qthread.h>


CardDistribution::CardDistribution(QObject *parent) : QObject(parent)
{
    amountsOfCards = 3;
    qDebug() << "CONSTRUCTOR OF CARD DISTRIBYTION";
}

void CardDistribution::appointCardsToPlayer(Player *player)
{
    QVector<Card *> vec;

    for(int i(0); i < amountsOfCards; i++) {
        vec.push_back(cardGeneration.getNewCard());
    }

    player->setCards(vec);
}
