#ifndef CARD_H
#define CARD_H
#include <QVector>
#include <QDebug>
#include <QPair>
#include <QMap>
#include <QObject>

class Card : public QObject
{
    Q_OBJECT

    QVector<QVector<int> > numbers;
public:
    explicit Card(QObject *parent = nullptr);
    Card(QVector<QVector<int> > numbers);
    QVector<QVector<int> > getNumbers();
    Q_INVOKABLE QVector<int> getAllNumbers();
    void setNumbers(QVector<QVector<int> > numbers_);
    QPair<int, int> contains(int value);
    bool setKeg(int value);

    QVector<int>& operator[](int n) {
        //static_assert(n > numbers.size() - 1, "Error. ");
        return numbers[n];
    }

    bool operator==(Card *second);
    friend QDebug& operator<<(QDebug& os, Card& card);
    enum {ROWS = 3, COLUMNS = 9};

    QPair<bool, bool> isNeedTakePartOfCush(); //first and second lines
    bool isWinner(); //third line

//signals:
//    void thirdLineFilled();
private:

    QMap<int, int> fillingLines;
    void controlFillingOfLines(int indexOfCard);
};


#endif // CARD_H
