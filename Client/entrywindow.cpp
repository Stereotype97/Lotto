#include "entrywindow.h"
#include "ui_entrywindow.h"
#include <QDebug>
#include <QGridLayout>
#include <QSpacerItem>

EntryWindow::EntryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EntryWindow)
{
    ui->setupUi(this);
    client = new Client();
    connect(client, &Client::isAccountCorrect, this, &EntryWindow::openHome);

    QGridLayout *main = new QGridLayout;
    QSpacerItem *spacer = new QSpacerItem(50,50);
    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *smallLayout = new QVBoxLayout();
    layout->addSpacerItem(spacer);
    layout->addWidget(ui->frame_2);
    smallLayout->addWidget(ui->login);
    layout->addSpacerItem(new QSpacerItem(10, 10));
    smallLayout->addWidget(ui->password);
    layout->addLayout(smallLayout);
    layout->addWidget(ui->entryButton);
    layout->addSpacerItem(spacer);

    main->addLayout(layout, 0, 0, Qt::AlignCenter);
    centralWidget()->setLayout(main);
}

EntryWindow::~EntryWindow()
{
    delete ui;
}

void EntryWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    auto winSize = size();
//    ui->frame->setGeometry(0, 0, winSize.width(), winSize.height());
    ui->frame->setFixedSize(winSize);
}


void EntryWindow::on_entryButton_clicked()
{
    client->connectToServer(ui->login->text(), ui->password->text());
}

void EntryWindow::openHome()
{
    qDebug() << "GO TO HOME";
    close();
    home.setClient(client);
    home.show();
    client->getMyGameData();
}
