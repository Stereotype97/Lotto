#ifndef SACK_H
#define SACK_H

#include <QObject>
#include <QVector>

class Sack : public QObject
{
    Q_OBJECT

    QVector<short> kegs;
    short viewed : 1;

public:
    explicit Sack(QObject *parent = nullptr);

    Q_INVOKABLE short nextKeg();
    Q_INVOKABLE void shuffle();

signals:

public slots:
};

#endif // SACK_H
