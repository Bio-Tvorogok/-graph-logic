
#ifndef BREADTH_FOR_SEARCH_
#define BREADTH_FOR_SEARCH_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QtMath>
#include <QDebug>
#include <QMetaType>
#include "KLogic/kedge.h"

class breadth_for_search : public QThread
{
    Q_OBJECT
public:
    explicit breadth_for_search(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix, int first_, int second_);
private:
    void run();
    int n;
    int first;
    int second;
    QVector<QVector<QVector<int>>> main_matrix;
    int Checker(QVector<int> edges);
signals:
    void progress(int);
    void result(QVector<KEdge>);

};

#endif // BREADTH_FOR_SEARCH_H
