#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QMainWindow>
#include <client.h>
#include <mainwindow.h>
#include <QPushButton>

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = 0);
    ~HomeWindow();

    void setClient(Client *client);

private slots:
    void on_toLobby_clicked();

    void on_join_clicked();

    void openRoom();
    void fillMyGameData(const QString &nickname, const unsigned &level,
                        const unsigned &experience, const unsigned &money,
                        const unsigned &winnings, const unsigned &losses,
                        const unsigned &draws);

    void changeLocation();
    void addAvailableRoom(int number, QString nickname, int bet);

    void on_create_clicked();

    void on_back_clicked();

    void on_pushButton_clicked();

    void on_update_clicked();

private:
    Ui::HomeWindow *ui;
    Client *client;
    MainWindow *game;

    QObjectList roomButtons;
    bool isHome;
    bool isWaitingOfOpponent;
};

#endif // HOMEWINDOW_H
