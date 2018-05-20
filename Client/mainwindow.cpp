#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

#define KEG_INTERVAL 300

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    currentKeg = 0;
    isGameFinished = false;
    isSkipTurn = false;
    colorPuttedKeg = new QColor("red");
    timer = new QTimer();
    timer->setInterval(KEG_INTERVAL);
    timerStatusLostTime = new QTimer();
    timerStatusLostTime->setInterval(KEG_INTERVAL / (100 + 1));

    ui->lastPicture->setVisible(false);

    QGridLayout *main = new QGridLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;

    QVBoxLayout *myDataVLayout = new QVBoxLayout;
    myDataVLayout->addWidget(ui->myNick);
    myDataVLayout->addWidget(ui->firstPlayerIcon);

    QVBoxLayout *firstTextLayout = new QVBoxLayout;
    firstTextLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    QHBoxLayout *informationLayout = new QHBoxLayout;
    QVBoxLayout *lostTimeLayout = new QVBoxLayout;
    lostTimeLayout->addWidget(ui->lostTimeLabel);
    lostTimeLayout->addWidget(ui->progressLostTime);

    QVBoxLayout *bankLayout = new QVBoxLayout;
    bankLayout->addWidget(ui->bankLabel);
    bankLayout->addWidget(ui->bank);

    informationLayout->addWidget(ui->currentKeg);
    informationLayout->addLayout(lostTimeLayout);
    informationLayout->addLayout(bankLayout);

    QVBoxLayout *secondTextLayout = new QVBoxLayout;
    secondTextLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding));

    QVBoxLayout *enemyDataVLayout = new QVBoxLayout;
    enemyDataVLayout->addWidget(ui->opponentNick);
    enemyDataVLayout->addWidget(ui->secondPlayerIcon);

    topLayout->addLayout(myDataVLayout);
    topLayout->addLayout(firstTextLayout);
    topLayout->addLayout(informationLayout);
    topLayout->addLayout(secondTextLayout);
    topLayout->addLayout(enemyDataVLayout);
    //end top layout

    QHBoxLayout *centerLayout = new QHBoxLayout;
    QVBoxLayout *myCardsAndReady = new QVBoxLayout;

    QVBoxLayout *myCards = new QVBoxLayout;
    myCards->addWidget(ui->myCard1);
    myCards->addWidget(ui->myCard2);
    myCards->addWidget(ui->myCard3);

    myCardsAndReady->addWidget(ui->iReadyLabel);
    myCardsAndReady->addLayout(myCards);

    QVBoxLayout *enemyCardsAndReady = new QVBoxLayout;

    QVBoxLayout *enemyCards = new QVBoxLayout;
    enemyCards->addWidget(ui->enemyField1);
    enemyCards->addWidget(ui->enemyField2);
    enemyCards->addWidget(ui->enemyField3);

    enemyCardsAndReady->addWidget(ui->enemyReadyLabel);
    enemyCardsAndReady->addLayout(enemyCards);

    centerLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum));
    centerLayout->addLayout(myCardsAndReady);
    //    centerLayout->addSpacerItem(new QSpacerItem(10, 10));
    centerLayout->addWidget(ui->line);
    //    centerLayout->addSpacerItem(new QSpacerItem(10, 10));
    centerLayout->addLayout(enemyCardsAndReady);
    centerLayout->addSpacerItem(new QSpacerItem(20, 20));
    //end center layout

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum));
    bottomLayout->addWidget(ui->reelection);
    bottomLayout->addWidget(ui->readyButton);
    //end bottom layout

    main->addLayout(topLayout, 0, 0, 1, 6, Qt::AlignCenter);
    main->addLayout(centerLayout, 1, 0, 1, 6, Qt::AlignCenter);
    main->addLayout(bottomLayout, 2, 0, 1, 3, Qt::AlignCenter);

    centralWidget()->setLayout(main);

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    auto winSize = size();
    ui->frame->setFixedSize(winSize);
    int widthOfSection = winSize.width() * 0.045;
    //    int heightOfSection = winSize.height() * 0.065;
    int newWidth = widthOfSection * Card::COLUMNS + 2;
    int newHeight = (newWidth / 3) + 2;
    ui->myCard1->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard1->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard1->setFixedSize(newWidth, newHeight);

    ui->myCard2->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard2->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard2->setFixedSize(newWidth, newHeight);

    ui->myCard3->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard3->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->myCard3->setFixedSize(newWidth, newHeight);

    ui->enemyField1->setFixedSize(newWidth, newHeight);
    ui->enemyField2->setFixedSize(newWidth, newHeight);
    ui->enemyField3->setFixedSize(newWidth, newHeight);

    if (ui->shirt1) ui->shirt1->setFixedSize(newWidth, newHeight);
    if (ui->shirt2) ui->shirt2->setFixedSize(newWidth, newHeight);
    if (ui->shirt3) ui->shirt3->setFixedSize(newWidth, newHeight);


    ui->enemyCard1->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard1->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard1->setFixedSize(newWidth, newHeight);

    ui->enemyCard2->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard2->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard2->setFixedSize(newWidth, newHeight);

    ui->enemyCard3->verticalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard3->horizontalHeader()->setDefaultSectionSize(widthOfSection);
    ui->enemyCard3->setFixedSize(newWidth, newHeight);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete timer;
    delete timerStatusLostTime;
}

void MainWindow::putTheKeg(int first, int second)
{
    QTableWidget *current = (QTableWidget *)sender();
    qDebug() << current->objectName();

    //    qDebug() << "cellClicked()" << first << second << current->currentItem()->text().toInt();
    if (current->currentItem()->textColor() != *colorPuttedKeg &&
            current->currentItem()->text().toInt() == currentKeg) {
        auto lastSymbol = current->objectName().back();
        int indexOfCard = ((lastSymbol.isDigit()) ? lastSymbol.digitValue() : -1) - 1;
        //        qDebug() << indexOfCard;

        current->item(first, second)->setTextColor(*colorPuttedKeg);
        client->iPlayer()->putKeg(indexOfCard, current->currentItem()->text().toInt());
        client->putTheKeg(indexOfCard, current->currentItem()->text().toInt());//to another player
    }
    else {
        qDebug() << "Be accuracy";
    }
}

void MainWindow::myCardFilling()
{
    QVector<int> card1 = client->iPlayer()->getAllNumbersFromPlayerByIndex(0);
    QVector<int> card2 = client->iPlayer()->getAllNumbersFromPlayerByIndex(1);
    QVector<int> card3 = client->iPlayer()->getAllNumbersFromPlayerByIndex(2);

    int count = 0;
    QString value;
    for (int i(0); i < Card::ROWS; i++) {
        for(int j(0); j < Card::COLUMNS; j++){
            value = (card1[count] != 0) ? QString::number(card1[count]) : "";
            QTableWidgetItem *item1 = new QTableWidgetItem(value);
            item1->setTextAlignment(Qt::AlignCenter);
            ui->myCard1->setItem(i, j, item1);

            value = (card2[count] != 0) ? QString::number(card2[count]) : "";
            QTableWidgetItem *item2 = new QTableWidgetItem(value);
            item2->setTextAlignment(Qt::AlignCenter);
            ui->myCard2->setItem(i, j, item2);

            value = (card3[count] != 0) ? QString::number(card3[count]) : "";
            QTableWidgetItem *item3 = new QTableWidgetItem(value);
            item3->setTextAlignment(Qt::AlignCenter);
            ui->myCard3->setItem(i, j, item3);

            ++count;
        }
    }
}

void MainWindow::enemyCardFilling()
{
    QVector<int> card1 = client->enemyPlayer()->getAllNumbersFromPlayerByIndex(0);
    QVector<int> card2 = client->enemyPlayer()->getAllNumbersFromPlayerByIndex(1);
    QVector<int> card3 = client->enemyPlayer()->getAllNumbersFromPlayerByIndex(2);

    ui->enemyReadyLabel->setText("ГОТОВ");
    int count = 0;
    QString value;
    for (int i(0); i < Card::ROWS; i++) {
        for(int j(0); j < Card::COLUMNS; j++){
            value = (card1[count] != 0) ? QString::number(card1[count]) : "";
            QTableWidgetItem *item1 = new QTableWidgetItem(value);
            item1->setTextAlignment(Qt::AlignCenter);
            ui->enemyCard1->setItem(i, j, item1);

            value = (card2[count] != 0) ? QString::number(card2[count]) : "";
            QTableWidgetItem *item2 = new QTableWidgetItem(value);
            item2->setTextAlignment(Qt::AlignCenter);
            ui->enemyCard2->setItem(i, j, item2);

            value = (card3[count] != 0) ? QString::number(card3[count]) : "";
            QTableWidgetItem *item3 = new QTableWidgetItem(value);
            item3->setTextAlignment(Qt::AlignCenter);
            ui->enemyCard3->setItem(i, j, item3);

            ++count;
        }
    }

}

void MainWindow::startGame()
{
    //    QThread::sleep(3);
    ui->enemyField1->removeWidget(ui->enemyField1->currentWidget());
    ui->enemyField2->removeWidget(ui->enemyField2->currentWidget());
    ui->enemyField3->removeWidget(ui->enemyField3->currentWidget());

    client->getCurrentKeg();
    timer->start();
    timerStatusLostTime->start();

    ui->readyButton->setVisible(false);
    ui->reelection->setVisible(false);
    ui->iReadyLabel->setVisible(false);
    ui->enemyReadyLabel->setVisible(false);

    client->getCurrentBank();

    connect(ui->myCard1, SIGNAL(cellClicked(int,int)), this, SLOT(putTheKeg(int,int)));
    connect(ui->myCard2, SIGNAL(cellClicked(int,int)), this, SLOT(putTheKeg(int,int)));
    connect(ui->myCard3, SIGNAL(cellClicked(int,int)), this, SLOT(putTheKeg(int,int)));

    connect(client, &Client::iWon, this, &MainWindow::gameEndedByMe);
    connect(client, &Client::enemyWon, this, &MainWindow::gameEndedByEnemy);
    connect(client, &Client::bankChanged, this, &MainWindow::changeBank);
    connect(client, &Client::skipTurn, this, &MainWindow::skipNextTurn);
}

void MainWindow::setCurrentKeg(int keg)
{
    currentKeg = keg;
    ui->currentKeg->setText(QString::number(currentKeg));
    ui->progressLostTime->setValue(ui->progressLostTime->maximum());
    //    QSound::play(":/changeKeg.wav");

    if (!isSkipTurn) {
        if (currentKeg == 0) {
            timer->stop();
            timerStatusLostTime->stop();
            client->getDraw();
            isGameFinished = true;
            qDebug() << "DRAW";
            ui->lastPicture->setStyleSheet("border-image: url(:/draw.jpeg);");
            ui->lastPicture->setFixedSize(size());
            ui->lastPicture->setVisible(true);
        }
    }
    else {
        QTimer::singleShot(KEG_INTERVAL, this, &MainWindow::resetAfterSkip);
        ui->myCard1->blockSignals(true);
        ui->myCard2->blockSignals(true);
        ui->myCard3->blockSignals(true);
        ui->iReadyLabel->setVisible(true);
        isSkipTurn = false;
    }
}

void MainWindow::enemyTurn(int indexOfCard, int keg)
{
    QTableWidget *enemyCard = nullptr;
    if (indexOfCard == 0) enemyCard = ui->enemyCard1;
    else if (indexOfCard == 1) enemyCard = ui->enemyCard2;
    else if (indexOfCard == 2) enemyCard = ui->enemyCard3;

    Q_ASSERT_X(enemyCard != nullptr, "MainWindow::enemyTurn", "enemyCard is NULL");

    QPair<int, int> coordinates = client->enemyPlayer()->getCard(indexOfCard)->contains(keg);
    enemyCard->item(coordinates.first, coordinates.second)->setTextColor(*colorPuttedKeg);

    client->enemyPlayer()->putKeg(indexOfCard, keg);
}

void MainWindow::setClient(Client *value)
{
    client = value;

    connect(client, &Client::myCardsSet, this, &MainWindow::myCardFilling);
    connect(client, &Client::enemyCardsSet, this, &MainWindow::enemyCardFilling);
    client->getCards();

    connect(timer, &QTimer::timeout, client, &Client::getCurrentKeg);
    connect(client, &Client::newCurrentKeg, this, &MainWindow::setCurrentKeg);
    connect(timerStatusLostTime, &QTimer::timeout, this, &MainWindow::changeLostTime);
    connect(client, &Client::isNeedToPutKeg, this, &MainWindow::enemyTurn);

    connect(client, &Client::opponentHasGone, this, &MainWindow::getPrematureVictory);
}


void MainWindow::on_readyButton_clicked()
{
    client->readyToPlay();
    connect(client, &Client::startGame, this, &MainWindow::waitBeforeGame);
    ui->iReadyLabel->setText("ГОТОВ");
}

void MainWindow::changeLostTime()
{
    int newValue = (ui->progressLostTime->value() != 0) ? ui->progressLostTime->value() - 1 : 0;
    ui->progressLostTime->setValue(newValue);
}

void MainWindow::gameEndedByMe()
{
    getPrematureVictory();
    client->getVictory();
}

void MainWindow::gameEndedByEnemy()
{
    isGameFinished = true;
    qDebug() << "You lose";
    ui->lastPicture->setStyleSheet("border-image: url(:/loss.jpg);");
    ui->lastPicture->setFixedSize(size());
    ui->lastPicture->setVisible(true);
    timer->stop();
    timerStatusLostTime->stop();
}

void MainWindow::getPrematureVictory()
{
    isGameFinished = true;
    ui->lastPicture->setFixedSize(size());
    ui->lastPicture->setVisible(true);
    timer->stop();
    timerStatusLostTime->stop();
}

void MainWindow::on_reelection_clicked()
{
    client->getCards();
    client->iPlayer()->setIsReElectionable(false);
    ui->reelection->setEnabled(false);
}

void MainWindow::waitBeforeGame()
{
    QTimer::singleShot(3000, this, &MainWindow::startGame);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "EXIT";

    if (!isGameFinished) {
        QMessageBox msgBox(QMessageBox::Warning,
                           QString::fromUtf8("Предупреждение"),
                           QString::fromUtf8("Вы автоматически проиграете"),
                           0, this);
        msgBox.addButton(QString::fromUtf8("&Все равно выйти"),
                         QMessageBox::AcceptRole);
        msgBox.addButton(QString::fromUtf8("&Остаться"),
                         QMessageBox::RejectRole);

        if (msgBox.exec() == QMessageBox::AcceptRole){
            client->exit();
            timer->stop();
            timerStatusLostTime->stop();
            emit updateRooms();
            event->accept();
        }
        else
            event->ignore();
    }
}

void MainWindow::changeBank(int bank)
{
    ui->bank->setText(QString::number(bank));
}

void MainWindow::skipNextTurn()
{
    qDebug() << "SKIP";
    ui->iReadyLabel->setText("ПРОПУСК ХОДА");

    isSkipTurn = true;
}

void MainWindow::resetAfterSkip()
{
    ui->iReadyLabel->setVisible(false);
    ui->myCard1->blockSignals(false);
    ui->myCard2->blockSignals(false);
    ui->myCard3->blockSignals(false);
}

void MainWindow::on_lastPicture_clicked()
{
    emit updateRooms();
    resetAllConnections();
    close();
}

void MainWindow::resetAllConnections()
{
    disconnect(client, &Client::myCardsSet, this, &MainWindow::myCardFilling);
    disconnect(client, &Client::enemyCardsSet, this, &MainWindow::enemyCardFilling);
    disconnect(timer, &QTimer::timeout, client, &Client::getCurrentKeg);
    disconnect(client, &Client::newCurrentKeg, this, &MainWindow::setCurrentKeg);
    disconnect(timerStatusLostTime, &QTimer::timeout, this, &MainWindow::changeLostTime);
    disconnect(client, &Client::isNeedToPutKeg, this, &MainWindow::enemyTurn);

    disconnect(client, &Client::opponentHasGone, this, &MainWindow::getPrematureVictory);
    disconnect(client, &Client::iWon, this, &MainWindow::gameEndedByMe);
    disconnect(client, &Client::enemyWon, this, &MainWindow::gameEndedByEnemy);
    disconnect(client, &Client::startGame, this, &MainWindow::waitBeforeGame);
    disconnect(client, &Client::bankChanged, this, &MainWindow::changeBank);
}
