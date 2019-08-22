#include "vostonav.h"

vostonav::vostonav(QWidget *parent) : QThread()
{

}

void vostonav::setting(QVector<int> vec){
    main_vec = vec;
}

void vostonav::run(){

    QVector<QPair<int, int>> vec_copy;
    for (int var = 0; var < main_vec.size(); var++) {
        QPair<int, int> add;
        add.first = main_vec[var];
        add.second = var;
        vec_copy.push_back(add);
    }
    QVector<KEdge> steps;

    bool is_perfect = false;
    bool is_extremal = true;
    if (sort(vec_copy) != vec_copy){
        is_perfect = true;
    }
    vec_copy = sort(vec_copy);
    QVector<QVector<QVector<int>>> main_matrix(vec_copy.size());
    for (int var = 0; var < vec_copy.size(); var++) {
        main_matrix[var].resize(vec_copy.size());
    }
    for (int var = 0; var < vec_copy.size(); var++) {
        if (realy(vec_copy) &&  vec_copy[0].first > 0){
            QVector<int> r;
            r.push_back(0);
            int last = vec_copy[1].first;
            for (int var_ = 1; var_ < vec_copy.size(); var_++) {
                if (vec_copy[var_].first == last){
                    r[r.size() - 1] = r.last() + 1;
                }
                else {
                    r.push_back(1);
                }
                last = vec_copy[var].first;
            }
            last = vec_copy[0].first;
            for (int var_ = 0; var_ < r.size(); var_++) {
                last -= r[var_];
                if (last < 0){
                    is_extremal = false;
                }
                if(last <= 0){
                    break;
                }
            }
            for (int var_ = 1; var_ < vec_copy.size(); var_++) {
                vec_copy[var_].first -= 1;
                vec_copy[0].first -= 1;
                KEdge new_steps;
                new_steps.Edge_id = 0;
                new_steps.First_node = vec_copy[0].second;
                new_steps.Second_node = vec_copy[var_].second;
                steps.push_back(new_steps);
                main_matrix[vec_copy[var_].second][vec_copy[0].second].push_back(1);
                if (vec_copy[0].first == 0){
                    break;
                }
            }

            std::sort(vec_copy.begin(), vec_copy.end(), [](const QPair<int, int>& first, const QPair<int, int>& second){
                return first.first > second.first;
            });
        }
        else{
            break;
        }
    }
    if (is_extremal){
        emit result(main_matrix, realy(vec_copy), is_extremal, is_perfect, steps, base(main_matrix));
    }
    else {
        QVector<QPair<int, int>> bs;
        emit result(main_matrix, realy(vec_copy), false, false, steps, bs);
    }
}

bool vostonav::realy(QVector<QPair<int, int>> vec){
    int k = vec[0].first;
    int var = 1;
    while(var <= vec.size()){
        if (vec[var].first > 0){
            k -= 1;

        }
        if( k == 0 ){
            return true;
        }
    }
    return false;
}

QVector<QPair<int, int>> vostonav::sort(QVector<QPair<int, int>> vec){
    std::sort(vec.begin(), vec.end(), [](const QPair<int, int>& first, const QPair<int, int>& second){
        return first.first > second.first;
    });
    return vec;
}

QVector<QPair<int, int>> vostonav::base(QVector<QVector<QVector<int>>> matrix){
    QVector<QPair<int, int>> bs;
    std::sort(matrix.begin(), matrix.end(), [](const QVector<QVector<int>>& first, const QVector<QVector<int>>& second){
        int k1 = 0;
        int k2 = 0;
        for (int var = 0; var < first.size(); var++) {
            if(!first[var].isEmpty()){
                k1 += 1;
            }
        }
        for (int var = 0; var < second.size(); var++) {
            if(!second[var].isEmpty()){
                k2 += 1;
            }
        }
        return k1 > k2;
    });
    for (int var = 0; var < matrix.size(); var++){
        std::sort(matrix[var].begin(), matrix[var].end(), [](const QVector<int>& first, const QVector<int>& second){
            return !first.isEmpty();
        });
    }
    for (int var_ = 0; var_ < matrix.size(); ++var_) {
        if (matrix[var_][var_].isEmpty()){
            matrix[var_][var_].push_back(1);
        }
    }
    for (int var = 0; var < matrix.size(); var++) {
        QPair<int, int> first;
        first.first = var;
        first.second = 0;

        for (int var_ = 0; var_ < matrix.size(); var_++) {

             if (matrix[var][var_].isEmpty()){
                 break;
             }
             else {
                 first.second = var_;
             }
        }

        if (first.second!= 0){
            bs.push_back(first);
        }
    }
    for (int var_ = 0; var_ < bs.size()/2; var_++) {
        bs.removeLast();
    }
    for (int var = bs.size() - 1; var > 0; var--) {
        if(bs[var].second == bs[var-1].second){
            bs.removeAt(var--);
        }
    }

    return bs;
}
