#ifndef ROOM_H
#define ROOM_H

#include <QObject>

class Room : public QObject
{
    Q_OBJECT

public:
    explicit Room(QObject *parent = nullptr);
    Room(int numberOfRoom = 0, QString description = "", QString nickname = "", int bet = 0) :
        numberOfRoom_(numberOfRoom), description_(description), nickname_(nickname), bet_(bet) {}

signals:

public slots:
    int numberOfRoom();
    int bet();
    QString description();
    QString nickname();

private:
    int numberOfRoom_;
    QString description_;
    QString nickname_;
    int bet_;

};

#endif // ROOM_H
