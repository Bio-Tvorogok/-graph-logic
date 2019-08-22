#ifndef BEST_FIRST_SEATCH_H
#define BEST_FIRST_SEATCH_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QtMath>
#include <QMetaType>
#include <QDebug>
#include "KLogic/kedge.h"

class Best_First_seatch: public QThread
{
    Q_OBJECT
public:
    explicit Best_First_seatch(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix, int first_, int second_);
private:
    void run();
    int n;
    int first;
    int second;
    QVector<QVector<QVector<int>>> main_matrix;
    int Checker(QVector<int> edges);

signals:
    void progress(int );
    void result(QVector<KEdge>);

};

#endif // BEST_FIRST_SEATCH_H
