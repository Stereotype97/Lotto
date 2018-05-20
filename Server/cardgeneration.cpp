#include "cardgeneration.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <iostream>
#include <qalgorithms.h>

using namespace std;

int CardGeneration::countGeneration;

CardGeneration::CardGeneration()
{
    countGeneration = QTime::currentTime().msec();
    //qDebug() << "CardGeneration";
    auxiliarySetOfNumbers.resize(9);
    for (int i(0); i < 9; i++) {
        addNewNumbersToAuxiliarySet(i);
    }

}

Card *CardGeneration::getNewCard()
{
    Card *card = getCardWithRecognizedPlaces();

    //проходим по текущей карточке и заполняем ее пустые места
    for(int j(0); j < Card::ROWS; j++) {
        for (int k(0); k < Card::COLUMNS; k++) {
            //Предназначена ли текущая ячейка для заполнения
            if (card->operator [](j)[k] == 1) {
                //Есть ли еще значения в вспомогательном векторе
                if (auxiliarySetOfNumbers[k].size() <= 1){
                    //Необходимо его пополнить
                    addNewNumbersToAuxiliarySet(k);
                }
                //Если значение в карточке уже присутсвует. Вызвано слияним нескольких векторов
                if (card->contains(auxiliarySetOfNumbers[k].front()) != qMakePair(-1, -1)) {
                    auxiliarySetOfNumbers[k].pop_front();
                }
                //Заполняем ячейку значением из соответсвующего десятка
                card->operator [](j)[k] = auxiliarySetOfNumbers[k].takeFirst();
            }
        }
    }
    showCard(card);
    qDebug() << "-----------------------------------";
    return card;
}


void CardGeneration::showCard(Card *card)
{
    QString str;
    for(int i(0); i < Card::ROWS; i++) {
        for(int j(0); j < Card::COLUMNS; j++) {
            if ((*card)[i][j] != 0){
                str += QString::asprintf(" %2d", (*card)[i][j]);
            }
            else{
                str += "   ";
            }
        }
        str += "\n";
    }
    QStringList lines = str.split("\n");
    qDebug().noquote() << lines[0];
    qDebug().noquote() << lines[1];
    qDebug().noquote() << lines[2];
}

QVector<int> CardGeneration::getPositionsForLine()
{
    qsrand(countGeneration++);
    QVector<int> locatingInRow = { 0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::random_shuffle(locatingInRow.begin(), locatingInRow.end());
    locatingInRow.resize(5);

    return locatingInRow;
}

bool CardGeneration::comparePositionsOnLines(QVector<int> v1, QVector<int> v2)
{
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

    return v1 == v2;
}

Card *CardGeneration::getCardWithRecognizedPlaces()
{
    Card *card = new Card();
    qsrand(countGeneration++);

    QVector<int> positionsInFirstLine(5);
    QVector<int> positionsInSecondLine(5);

    positionsInFirstLine = getPositionsForLine();
    positionsInSecondLine = getPositionsForLine();
    while (comparePositionsOnLines(positionsInFirstLine, positionsInSecondLine)) {
        positionsInSecondLine = getPositionsForLine();
    }

    int amountItemInLine = 5;

    int index = 0;
    for (int j(0); j < amountItemInLine; j++) {
        index = positionsInFirstLine[j]; //find needed place for number
        card->operator[](0)[index] = 1; //the cell is marked
    }

    index = 0;
    for (int j(0); j < amountItemInLine; j++) {
        index = positionsInSecondLine[j]; //find needed place for number
        card->operator[](1)[index] = 1; //the cell is marked
    }

    QVector<int> freePositions = { 0, 1, 2, 3, 4, 5, 6, 7, 8};

    QVector<int> placesOnLastLine;

    for (int i(0); i < Card::COLUMNS; i++) {
        if ((*card)[Card::ROWS - 2][i] == 0 && (*card)[Card::ROWS - 3][i] == 0) {
            placesOnLastLine.push_back(i);
            freePositions.removeOne(i);
        }
        else if ((*card)[Card::ROWS - 2][i] != 0 && (*card)[Card::ROWS - 3][i] != 0) {
            freePositions.removeOne(i);
        }
    }
    std::random_shuffle(freePositions.begin(), freePositions.end());
    //showCard(*card);
    while (placesOnLastLine.size() < amountItemInLine) {
        placesOnLastLine.push_back(freePositions.front());
        freePositions.pop_front();
    }
    //    qDebug() << "places" << placesOnLastLine;

    for(int i(0); i < amountItemInLine; i++){
        int index = placesOnLastLine[i];
        card->operator [](Card::ROWS - 1)[index] = 1; //the cell is marked
    }
    return card;
}

void CardGeneration::addNewNumbersToAuxiliarySet(int range)
{
    qsrand(countGeneration++);
    QVector<int> numbers;
    //if first line - we have 9 values (1-9)
    if(range == 0) {
        numbers.resize(9);
        std::iota(numbers.begin(), numbers.end(), 1);
    }
    else { //x0-x9
        numbers.resize(10);
        std::iota(numbers.begin(), numbers.end(), 10 * range);
    }

    if (range == Card::COLUMNS - 1){ //80-90
        numbers.push_back(90);
    }
    std::random_shuffle(numbers.begin(), numbers.end());

    auxiliarySetOfNumbers[range].reserve(auxiliarySetOfNumbers[range].size() + numbers.size());
    std::copy(numbers.begin(), numbers.end(), std::back_inserter(auxiliarySetOfNumbers[range]));

}


