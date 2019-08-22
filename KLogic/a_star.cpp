#include "a_star.h"

A_star::A_star(QWidget *parent) : QThread()
{

}


void A_star::run(){

    QVector<KEdge> way;

    QVector<int> p(n, -1);
    QVector<int> w(n, 0);
    QVector<int> open;
    open.push_back(first);
    QVector<bool> close(n, false);
    close[first] = true;
    while (true) {
        QVector<QPair<KEdge, intmax_t>> variants;
        for (int var = 0; var < open.size(); var++) {
            for (int var_ = 0; var_ < n; var_++) {
                if (!close[var_] && Checker(main_matrix[open[var]][var_]) >= 0){
                    QPair<KEdge, intmax_t> may;
                    KEdge edge;
                    edge.First_node = open[var];
                    edge.Second_node = var_;

                    edge.Edge_id = Checker(main_matrix[open[var]][var_]);
                    may.first = edge;
                    may.second = main_matrix[open[var]][var_][Checker(main_matrix[open[var]][var_])] + (intmax_t)sqrt((x_y[open[var]].first - x_y[second].first) * (x_y[open[var]].first - x_y[second].first) + (x_y[open[var]].second - x_y[second].second) * (x_y[open[var]].second - x_y[second].second));
                    variants.push_back(may);
                }
            }
        }
        if (variants.size() == 0){
            break;
        }
        else {
            intmax_t max  = variants[0].second + w[variants[0].first.First_node];
            KEdge next = variants[0].first;
            for (int var = 0; var < variants.size(); var++) {
                if (max > variants[var].second + w[variants[var].first.First_node]){
                    max = variants[var].second + w[variants[var].first.First_node];
                    next = variants[var].first;
                }
            }
            close[next.Second_node];
            open.push_back(next.Second_node);
            close[next.Second_node] = true;
            p[next.Second_node] = next.First_node;
            w[next.Second_node] = w[next.First_node] + main_matrix[next.First_node][next.Second_node][Checker(main_matrix[next.First_node][next.Second_node])];
            if (next.Second_node == second){
                break;
            }
        }
    }
    if (close[second]){
        int v = second;
        KEdge e;
        do{
            e.First_node = v;
            e.Second_node = p[v];
            e.Edge_id = Checker(main_matrix[p[v]][v]);
            way.push_back(e);
            v = p[v];
        }while(p[v]!=-1);
    }
    emit progress(100);
    emit result(way, w[second]);
}

void A_star::setting(QVector<QVector<QVector<int> > > matrix, int first_, int second_, QVector<QPair<qreal, qreal>> x_and_y){
    main_matrix = matrix;
    first = first_;
    second = second_;
    pr = matrix.size();
    x_y = x_and_y;
    n = matrix.size();
}

int A_star::Checker(QVector<int> edges){
    if (edges.isEmpty()) return -1;
    int min = -1;
    for (int var_ = 0; var_ < edges.size(); var_++) {
        if (edges[var_] > 0 ){
            min = var_;
            break;
        }
    }
    if (min == -1) return -1;
    for (int var = 1; var < edges.size(); var++) {
        if (edges[var] > 0 && edges[var] < edges[min]) min = var;
    }
    return min;
}
