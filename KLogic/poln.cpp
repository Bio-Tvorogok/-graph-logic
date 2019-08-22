#include "poln.h"

poln::poln(QWidget *parent) : QThread()
{


}

void poln::run(){
    int pr = 0;
    QVector<KEdge> out;
    for (int var = 0; var < main_matrix.size(); ++var) {
        for (int var_ = var; var_ < main_matrix.size(); ++var_) {
            if(pr != qFloor((var * main_matrix.size() + var_) * 100 / (main_matrix.size() * main_matrix.size()))){
                pr = qFloor((var * main_matrix.size() + var_) * 100 / (main_matrix.size() * main_matrix.size()));
                emit progress(pr);
            }


            if (!Checker(main_matrix[var][var_], main_matrix[var_][var]) && (var != var_)){
                KEdge a;
                a.First_node = var;
                a.Second_node = var_;
                a.Edge_id = main_matrix[var][var_].size();
                out.push_back(a);
            }
        }
    }
    emit result(out);
}

void poln::setting(QVector<QVector<QVector<int>>> matrix){
    main_matrix = matrix;
}

bool poln::Checker(QVector<int> edge1, QVector<int> edge2){
    bool e1 = false, e2 = false;
    if(edge1.size() > 0){
        for (int var = 0; var < edge1.size(); var++) {
            if(!e1 && edge1[var] > 0) e1 = true;
            if(!e2 && edge2[var] > 0) e2 = true;
            if (e1 && e2) break;
        }
        return e1 && e2;
    }
    else {
        return false;
    }
}
