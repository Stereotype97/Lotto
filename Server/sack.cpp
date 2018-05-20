#include "sack.h"
#include <QDebug>

Sack::Sack(QObject *parent) : QObject(parent)
{
    viewed = 0;
    shuffle();
//    qDebug() << "Sack" << kegs;

}

short Sack::nextKeg()
{
    //if sack is not empty
    if (kegs.size() > 0) {
        short keg = kegs.back();

        viewed++;//просмотрено
        if (viewed == 0) {
            kegs.pop_back();
        }
        qDebug() << "take " << keg;
        return keg;
    }

    return 0; //sack is empty
}

void Sack::shuffle()
{
    kegs.resize(90);
    std::iota(kegs.begin(), kegs.end(), 1);
    std::random_shuffle(kegs.begin(), kegs.end());
}
