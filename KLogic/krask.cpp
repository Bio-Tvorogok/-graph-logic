#include "krask.h"

krask::krask(QWidget *parent) : QThread()
{

}

void krask::setting(QVector<QVector<QVector<int> > > matrix){
    main_matrix = matrix;
}

void krask::run(){
    return_color = *new QVector<int>(main_matrix.size(), 0);
    QVector<int> checker(return_color);
    int color = 1;
    return_color[0] = color;
    while (!all_or_not()){
        for (int var = 1; var < return_color.size(); var++) {
            if(return_color[var] == 0 && ready_for_color(var,color)){
                return_color[var] = color;
            }
        }
        if (return_color == checker){
            color++;
        }
        checker =*new QVector<int>(return_color);
    }
    emit result(return_color);

}

bool krask::all_or_not(){
    for (int var = 0; var < return_color.size(); ++var) {
        if (return_color[var] == 0){
            return false;
        }
    }
    return true;
}

bool krask::ready_for_color(int node, int color){
    for (int var = 0; var < main_matrix.size(); var++){
        if(var != node && !main_matrix[node][var].isEmpty() && return_color[var] == color){
            return false;
        }
    }
    return true;
}
