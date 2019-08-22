#include "best_first_seatch.h"

Best_First_seatch::Best_First_seatch(QWidget *parent) : QThread()
{

}



void Best_First_seatch::setting(QVector<QVector<QVector<int>>> matrix, int first_, int second_)
{
    n = matrix.length();
    first = first_;
    second = second_;
    main_matrix = matrix;
}

int Best_First_seatch::Checker(QVector<int> edges){
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

void Best_First_seatch::run(){
    QList<int> open;
    QVector<KEdge> way;
    int pr = 0;
    open.append(first);
    QVector<bool> close(n);
    QVector<int> p (n);
    p[first] = -1;
    KEdge next_best;
    int next;
    while (!open.empty()){

        pr++;
        emit progress(qFloor(pr/n*100));
        close[open.first()] = true;
        for (int var__ = 0; var__ < n; ++var__) {
            if (!close[var__] && (Checker(main_matrix[open.first()][var__]) >=0) && (!open.contains(var__))){
                open.append(var__);
            }
        }
        next = INT_MAX;
        if(p[open.first()]!=-1){
            for (int var = 0; var < n; var++) {
                if(close[var] && (var != open.first()) && (Checker(main_matrix[var][open.first()])>=0) && (main_matrix[var][open.first()][Checker(main_matrix[var][open.first()])]<next)){
                    p[open.first()] = var;
                    next = main_matrix[var][open.first()][Checker(main_matrix[var][open.first()])];
                }
            }
        }
        if(open.first() == second) break;
        open.removeFirst();
        next = INT_MAX;
        for (int var = 0; var < n; var++) {
            if(close[var]){
                for (int var_ = 0; var_ < open.size(); var_++) {
                    if(((Checker(main_matrix[var][open.at(var_)]))>=0) && main_matrix[var][open.at(var_)][Checker(main_matrix[var][open.at(var_)])] < next){
                        next_best.First_node = var;
                        next_best.Second_node = open.at(var_);
                        next_best.Edge_id = Checker(main_matrix[var][open.at(var_)]);
                        next = main_matrix[var][open.at(var_)][Checker(main_matrix[var][open.at(var_)])];
                    }
                }
            }
        }
        if (next != INT_MAX){
            open.swap(0, open.indexOf(next_best.Second_node));
        }
    }
    KEdge e;
    if (close[second]){
        int v = second;
        do{
            e.First_node = v;
            e.Second_node = p[v];
            e.Edge_id = Checker(main_matrix[p[v]][v]);
            way.push_back(e);
            v = p[v];
        }while(p[v]!=-1);

    }


    emit progress(100);
    emit result(way);
}
