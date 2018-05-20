#include "carddistribution.h"
#include <QTime>
#include <qthread.h>


CardDistribution::CardDistribution(QObject *parent) : QObject(parent)
{
    amountsOfCards = 3;

//    fillBank(6);
//    qDebug() << "Bank size" << bank.size();

//    for(Card *card : bank) {
//        CardGeneration::showCard(card);
//        qDebug() << "\n";
//    }
}

//void CardDistribution::fillBank(int amount)
//{
//    bank.push_back(cardGeneration.getNewCard());

//    Card *toAdding;
//    bool isEqual;
//    while (bank.size() != amount) {
//        //qDebug() << "DROCH: " << bank.size();
//        toAdding = cardGeneration.getNewCard();
//        isEqual = false;

//        for(int i(0); i < bank.size(); i++) {
//            if (*toAdding == bank[i]) {
//                isEqual = true;
//                delete toAdding;
//                break;
//            }
//        }
//        if (!isEqual) {

//            bank.push_back(toAdding);
//            //qDebug() << bank.size();
//        }
//        //else qDebug() << "EQUAL";
//        //delete toAdding;
//        isEqual = false;
//    }
//}
void CardDistribution::appointCardsToPlayer(Player *player)
{
    QVector<Card *> vec;

    for(int i(0); i < amountsOfCards; i++) {
        vec.push_back(cardGeneration.getNewCard());
    }

    player->setCards(vec);
}
