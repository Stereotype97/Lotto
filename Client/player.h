#ifndef PLAYER_H
#define PLAYER_H
#include <card.h>
#include <QDebug>
#include <QVector>
#include <QObject>

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isReElectionable READ isReElectionable WRITE setIsReElectionable NOTIFY isReElectionableChanged)

protected:
    QVector<Card *> cards;
    void checkFillingOfCard(Card *card); //README
    bool isReElectionable_;

public:
    int IdentProfile; //money
    explicit Player(QObject *parent = nullptr);
    explicit Player(int id);
    Q_INVOKABLE virtual void putKeg(int index, int value);
    Q_INVOKABLE QVector<int> getAllNumbersFromPlayerByIndex(int index);
    Q_INVOKABLE QVector<Card *> getCards();
    Q_INVOKABLE Card *getCard(int index);
    Q_INVOKABLE int amountOfCards();

    void setCards(QVector<Card *> nCards);
public slots:
    void setIsReElectionable(bool const &b);
protected slots:
    bool isReElectionable();

signals:
    void firstLineFilled();
    void secondLineFilled();
    void playerWon();
    void isReElectionableChanged();

private:
};

#endif // PLAYER_H
