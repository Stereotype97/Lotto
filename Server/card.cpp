#include "card.h"

Card::Card(QObject *parent) : QObject(parent)
{
    numbers.resize(ROWS);
    for(int i(0); i < ROWS; i++) {
        numbers[i].resize(COLUMNS); //reserve
    }

    fillingLines[0] = 0;
    fillingLines[1] = 0;
    fillingLines[2] = 0;
}

Card::Card(QVector<QVector<int> > numbers)
{
    this->numbers = numbers;
}

QVector<QVector<int> > Card::getNumbers()
{
    return numbers;
}

QVector<int> Card::getAllNumbers()
{
    return (numbers[0] + numbers[1] + numbers[2]);
}

void Card::setNumbers(QVector<QVector<int> > numbers_)
{
    numbers = numbers_;
}

 QPair<int, int> Card::contains(int value)
{
    int column = value / 10;
    if (column == 9) column--;
    for(int i(0); i < ROWS; i++) {
        if (numbers[i][column] == value)
            return qMakePair(i, column);
    }
    return qMakePair(-1, -1);
}

bool Card::setKeg(int value)
{
    auto coordinates = this->contains(value);
    if (coordinates == qMakePair(-1, -1))
        return false;

    numbers[coordinates.first][coordinates.second] = -1;
    fillingLines[coordinates.first]++;


    return true;
}


bool Card::operator==(Card *second)
{
    int repeating = 0;
    for(int i(0); i < ROWS; i++) {
        for(int j(0); j < COLUMNS; j++) {
            if (numbers[i][j] != 0 && (second->contains(numbers[i][j]) != qMakePair(-1, -1))) {
                repeating++;
            }
            if (repeating > 2) return true;
        }
    }
    return false;
}

QPair<bool, bool> Card::isNeedTakePartOfCush()
{
    bool first = (fillingLines[0] == 5) ? true : false;
    bool second = (fillingLines[1] == 5) ? true : false;

    controlFillingOfLines(0);
    controlFillingOfLines(1);
    return qMakePair(first, second);
}

bool Card::isWinner()
{
    bool result = fillingLines[2] == 5;
    controlFillingOfLines(2);
    return result;
}

void Card::controlFillingOfLines(int indexOfCard)
{
        if (fillingLines[indexOfCard] == 5)
            fillingLines[indexOfCard]++;
}


QDebug& operator<< (QDebug& os, Card& card)
{
    os << "\n";
    for(int i(0); i < Card::ROWS; i++) {
        for(int j(0); j < Card::COLUMNS; j++) {
            if (card[i][j] != 0){
                os << card[i][j] << " ";
            }
            else{
                os << "  ";
            }
        }
        os << "\n";
    }
    return os << "\n-------------------------";
}



