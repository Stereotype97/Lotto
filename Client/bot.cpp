#include "bot.h"

Bot::Bot(QObject *parent) : Player(parent)
{
    IdentProfile = 0;
}

void Bot::putKeg(int index, int value)
{
    qDebug() << "bot put the keg in " << cards[index]->getAllNumbers();
    for(int i(0); i < 3; i++) {
        cards[i]->setKeg(value);
        checkFillingOfCard(cards[i]);
    }
}
