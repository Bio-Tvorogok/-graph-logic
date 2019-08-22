#ifndef A_STAR_H
#define A_STAR_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QtMath>
#include "kedge.h"

class A_star: public QThread
{
    Q_OBJECT
public:
    explicit A_star(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix, int first_, int second_, QVector<QPair<qreal, qreal> > x_and_y);
private:
    void run();
    int n;
    int first;
    int second;
    int pr;
    QVector<QPair<qreal, qreal>> x_y;
    int pr___;
    QVector<QVector<QVector<int>>> main_matrix;
    int Checker(QVector<int> edges);

signals:
    void progress(int);
    void result(QVector<KEdge>, int);

};

#endif // A_STAR_H
