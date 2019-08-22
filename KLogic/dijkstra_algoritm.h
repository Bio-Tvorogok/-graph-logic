#ifndef DIJKSTRA_ALGORITM_H
#define DIJKSTRA_ALGORITM_H

#include <QThread>
#include <QVector>
#include <QQueue>
#include <QtMath>
#include "kedge.h"

class Dijkstra_algoritm : public QThread
{
    Q_OBJECT
public:
    explicit Dijkstra_algoritm(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix, int mode, int first_, int second_);
private:
    void run();
    int n;
    int mode;
    int first;
    int second;
    int pr;

    QVector<QVector<QVector<KEdge>>> p;
    QVector<QVector<int>> d;
    int pr___;
    int fact(int N);
    QVector<QVector<QVector<int>>> main_matrix;
    int Checker(QVector<int> edges);
    void Dijkstra(int f_node, QVector<KEdge> w, QVector<int> o, int now_node, int len);
    bool Sviaz_slab(QVector<int> w, bool first);
    bool Sviaz_siln(QVector<int> w);

signals:
    void progress(int);
    void result_only_one(QVector<KEdge>, int);
    void result(QVector<QVector<QVector<KEdge>>>, QVector<QVector<int>>);
    void Weihts(QVector<int>, QVector<int>,QVector<int>, int, int, int);

};

#endif // DIJKSTRA_ALGORITM_H
