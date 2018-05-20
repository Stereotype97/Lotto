#include "homewindow.h"
#include "ui_homewindow.h"
#include<QAction>

HomeWindow::HomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    ui->create->setVisible(false);
    ui->back->setVisible(false);
    ui->scrollArea->setVisible(false);
    ui->bets->setVisible(false);
    ui->update->setVisible(false);

    isHome = true;
    isWaitingOfOpponent = false;

    ui->scrollArea->setWidgetResizable(false);
    ui->scrollAreaWidgetContents->resize(250 + 10, ui->scrollArea->size().height());
}

HomeWindow::~HomeWindow()
{
    delete ui;
}

void HomeWindow::setClient(Client *client)
{
    this->client = client;
    connect(this->client, &Client::startDistribution, this, &HomeWindow::openRoom);
    connect(client, &Client::isMyGameData, this, &HomeWindow::fillMyGameData);
    connect(client, &Client::isAvailableRooms, this, &HomeWindow::addAvailableRoom);

}

void HomeWindow::on_toLobby_clicked()
{
    client->getAvailableRooms();
    changeLocation();
    qDebug() << roomButtons;
}

void HomeWindow::on_join_clicked()
{
    QPushButton *room = (QPushButton *)sender();
    QString name = room->text();
    QRegExp rx("(\\d+)");
    rx.indexIn(name);

    client->joinToRoom(rx.cap(1).toInt());
}

void HomeWindow::openRoom()
{
    game = new MainWindow();
    game->setClient(client);
    connect(game, &MainWindow::updateRooms, this, &HomeWindow::on_update_clicked);
    game->show();
}

void HomeWindow::fillMyGameData(const QString &nickname, const unsigned &level,
                                const unsigned &experience, const unsigned &money,
                                const unsigned &winnings, const unsigned &losses,
                                const unsigned &draws)
{
    ui->nickname->setText(nickname);
    ui->level->setText(QString::number(level));
    ui->experienceProgress->setValue(experience);
    //    ui->experienceProgress->setMaximum(limit);
    ui->money->setText(QString::number(money));
    ui->victories->setText(QString::number(winnings));
    ui->losses->setText(QString::number(losses));
    ui->draws->setText(QString::number(draws));
}

void HomeWindow::changeLocation()
{
    isHome = !isHome;

    ui->frame->setVisible(isHome);
    ui->nickname->setVisible(isHome);
    ui->levelLabel->setVisible(isHome);
    ui->level->setVisible(isHome);
    ui->experienceProgress->setVisible(isHome);
    ui->moneyLabel->setVisible(isHome);
    ui->money->setVisible(isHome);
    ui->victoriesLabel->setVisible(isHome);
    ui->victories->setVisible(isHome);
    ui->lossesLabel->setVisible(isHome);
    ui->losses->setVisible(isHome);
    ui->drawLabel->setVisible(isHome);
    ui->draws->setVisible(isHome);
    ui->line->setVisible(isHome);
    ui->line_2->setVisible(isHome);
    ui->line_3->setVisible(isHome);
    ui->toLobby->setVisible(isHome);

    ui->create->setVisible(!isHome);
    ui->back->setVisible(!isHome);
    ui->scrollArea->setVisible(!isHome);
    ui->bets->setVisible(!isHome);
    ui->update->setVisible(!isHome);
}

void HomeWindow::addAvailableRoom(int number, QString nickname, int bet)
{
    qDebug() << "addRoom";
    QString str_t = "Комната: #%1\nСоздана: %2\nСтавка: %3";
    QString str = str_t.arg(number)
            .arg(nickname)
            .arg(bet);
    QPushButton *availableRoom = new QPushButton(str);
    connect(availableRoom, &QPushButton::clicked, this, &HomeWindow::on_join_clicked);
    ui->verticalLayout->addWidget(availableRoom);
    roomButtons.push_back(availableRoom);
    //    qDebug() << roomButtons;
}

void HomeWindow::on_create_clicked()
{
    if (!isWaitingOfOpponent) {
        client->createRoom(ui->bets->currentText().toInt());
        isWaitingOfOpponent = true;
        ui->create->setText("Отменить");
        ui->update->setEnabled(false);
        ui->back->setEnabled(false);
    }
    else {
        client->removeRoom();
        isWaitingOfOpponent = false;
        ui->create->setText("Создать игру");
        ui->update->setEnabled(true);
        ui->back->setEnabled(true);
    }
}

void HomeWindow::on_back_clicked()
{
    client->getMyGameData();
    changeLocation();

    for(int i(roomButtons.size() - 1); i >= 0; i--) {
        ui->verticalLayout->removeWidget((QPushButton *)roomButtons[i]);
        delete roomButtons[i];
    }
    roomButtons.clear();
}

void HomeWindow::on_pushButton_clicked()
{
    QPushButton *b = new QPushButton(QString("Комната: #%1\nСоздана: 2\nСтавка: 3")
                                     .arg(ui->verticalLayout->count()));
    b->setMinimumSize(250, 50);
    b->setMaximumSize(250,50);
    ui->verticalLayout->addWidget(b);

    qDebug() << b->size();
    if (ui->verticalLayout->count() * (b->size().height() + 5) > ui->scrollArea->size().height())
        ui->scrollAreaWidgetContents->resize(b->size().width() + 10, ui->verticalLayout->count() * (b->size().height() + 5));
}

void HomeWindow::on_update_clicked()
{
    for(int i(roomButtons.size() - 1); i >= 0; i--) {
        ui->verticalLayout->removeWidget((QPushButton *)roomButtons[i]);
        delete roomButtons[i];
    }
    roomButtons.clear();
    client->getAvailableRooms();

    if (isWaitingOfOpponent) {
        isWaitingOfOpponent = false;
        ui->create->setText("Создать игру");
        ui->update->setEnabled(true);
        ui->back->setEnabled(true);
    }
}
