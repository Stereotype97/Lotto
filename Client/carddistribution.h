#ifndef CARDDISTRIBUTION_H
#define CARDDISTRIBUTION_H
#include <cardgeneration.h>
#include <QDebug>
#include <player.h>
#include <QObject>

class CardDistribution : public QObject
{
    Q_OBJECT

    CardGeneration cardGeneration;
    int amountsOfCards;

    QVector<Card *> bank;
public:
    explicit CardDistribution(QObject *parent = nullptr);
    Q_INVOKABLE void appointCardsToPlayer(Player *player);
};

#endif // CARDDISTRIBUTION_H
