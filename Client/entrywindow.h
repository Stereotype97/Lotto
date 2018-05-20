#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

#include <QMainWindow>
#include <mainwindow.h>
#include <homewindow.h>

namespace Ui {
class EntryWindow;
}

class EntryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EntryWindow(QWidget *parent = 0);
    ~EntryWindow();
    void resizeEvent(QResizeEvent *event);

private slots:

    void on_entryButton_clicked();
    void openHome();

private:
    Ui::EntryWindow *ui;
    Client *client;
    HomeWindow home;
};

#endif // ENTRYWINDOW_H
