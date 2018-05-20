#ifndef PLAYER_H
#define PLAYER_H
#include <card.h>
#include <QDebug>
#include <QVector>
#include <QObject>

class Player : public QObject
{
    Q_OBJECT

    bool isReElectionable;
protected:
    int IdentProfile; //money
    QVector<Card *> cards;
    void checkFillingOfCard(Card *card); //README
    //поле для отслежения ошибочных нажатий игрока (попытка putKeg *******)
public:
    explicit Player(QObject *parent = nullptr);
    Player(int id);
    Q_INVOKABLE virtual void putKeg(int value, int index);
    Q_INVOKABLE QVector<Card *> getCards();
    Q_INVOKABLE Card *getCard(int index);
    Q_INVOKABLE int amountOfCards();

    void setCards(QVector<Card *> nCards);
signals:
    void playerWon();
private:
};

#endif // PLAYER_H
