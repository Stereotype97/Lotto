#ifndef GAMECARD_H
#define GAMECARD_H
#include <QVector>
#include <card.h>

class CardGeneration
{
public:
    CardGeneration();
    Card* getNewCard();

    static void showCard(Card *card);

private:
    QVector<int> getPositionsForLine();
    bool comparePositionsOnLines(QVector<int> v1, QVector<int> v2);

    Card *getCardWithRecognizedPlaces();
    void addNewNumbersToAuxiliarySet(int range);

    static int countGeneration;

    QVector<Card *> bankOfCards;
    QVector<QVector<int> > auxiliarySetOfNumbers;
};

#endif // GAMECARD_H
