#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <client.h>
#include <QSound>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setClient(Client *value);
    void resizeEvent(QResizeEvent *event);

public slots:
    void putTheKeg(int first, int second);
signals:
    void updateRooms();
private slots:
    void myCardFilling();
    void enemyCardFilling();
    void startGame();
    void setCurrentKeg(int keg);
    void enemyTurn(int indexOfCard, int keg);
    void on_readyButton_clicked();
    void changeLostTime();

    void getPrematureVictory();
    void gameEndedByMe();
    void gameEndedByEnemy();

    void on_reelection_clicked();
    void waitBeforeGame();
    void closeEvent(QCloseEvent *event);

    void changeBank(int bank);
    void skipNextTurn();
    void resetAfterSkip();

    void on_lastPicture_clicked();
    void resetAllConnections();

private:
    Ui::MainWindow *ui;
    Client *client;

    int currentKeg;
    QColor *colorPuttedKeg;
    QTimer *timer;
    QTimer *timerStatusLostTime;
    bool isGameFinished;
    bool isSkipTurn;
};

#endif // MAINWINDOW_H
