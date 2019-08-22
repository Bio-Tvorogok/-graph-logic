#include "breadth_for_search.h"

breadth_for_search::breadth_for_search(QWidget *parent) : QThread()
{
}

void breadth_for_search::setting(QVector<QVector<QVector<int>>> matrix, int first_, int second_)
{
    n = matrix.length();
    first = first_;
    second = second_;
    main_matrix = matrix;
}

void breadth_for_search::run(){
    QVector<QVector<int>> edge;
    for(int i = 0; i < n; i++)
    {
        QVector<int> tempVector;

        for(int j = 0; j < n; j++)
        {
            tempVector.push_back(-1);
        }
        edge.push_back(tempVector);
    }
    QQueue<int> q;
    QVector<KEdge> way;
    int pr = 0;
    q.enqueue(first);
    QVector<bool> used (n);
    QVector<int> d (n), p (n);
    used[first] = true;
    p[first] = -1;
    while (!q.empty()) {
        pr++;
        emit progress(qFloor(pr/n*100));
        int v = q.front();
        q.dequeue();
        for (int i=0; i<main_matrix.size(); ++i) {
            if (v == i)
                continue;
            edge[v][i] = Checker(main_matrix[v][i]);
            if (!used[i] & (edge[v][i]>=0) & v!=i) {
                used[i] = true;
                q.enqueue(i);
                d[i] = d[v] + 1;
                p[i] = v;
                if (i == second){
                    break;
                }
            }
        }
    }
    KEdge e;
    if (used[second]){
        int v = second;
        do{
            e.First_node = v;
            e.Second_node = p[v];
            e.Edge_id = edge[p[v]][v];
            way.push_back(e);
            v = p[v];
        }while(p[v]!=-1);

    }
    emit progress(100);
    qDebug() << "emiting";
    emit result(way);
}

int breadth_for_search::Checker(QVector<int> edges){
    if (edges.isEmpty()) return -1;
    for (int var = 0; var < edges.size(); var++) {
        if (edges[var] > 0) return var;
    }
    return -1;
}
