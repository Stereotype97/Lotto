#include "sack.h"
#include <QDebug>

Sack::Sack(QObject *parent) : QObject(parent)
{
    kegs.resize(90);
    std::iota(kegs.begin(), kegs.end(), 1);
    viewed = 0;
    shuffle();
}

short Sack::nextKeg()
{
    ++viewed;
    //if sack is not empty
    if (kegs.size() > 0) {
        if (viewed == 0) {
            kegs.pop_front();
        }
        short keg = kegs.front();
        return keg;
    }

    return 0; //sack is empty
}

void Sack::shuffle()
{
    std::random_shuffle(kegs.begin(), kegs.end());
}
