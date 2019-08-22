#include "dijkstra_algoritm.h"
#include "kedge.h"

Dijkstra_algoritm::Dijkstra_algoritm(QWidget *parent) : QThread()
{

}

void Dijkstra_algoritm::setting(QVector<QVector<QVector<int>>> matrix, int mode_, int first_, int second_)
{
    n = matrix.length();
    pr___ = fact(n) * n;
    main_matrix = matrix;
    mode = mode_;
    first = first_;
    second = second_;
}

int Dijkstra_algoritm::Checker(QVector<int> edges){
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

void Dijkstra_algoritm::run(){

    pr = 0;
    if(mode == 0){
        QVector<int> d_;
        QVector<int> open(n);
        for (int var = 0; var < n; var++) {
            open[var] = var;
        }

        open.removeAt(first);
        QVector<KEdge> way;
        for(int i = 0; i < n; i++){
            QVector<int> tempVector(n, INT_MAX);

            d.push_back(tempVector);
        }
        for (int var_ = 0; var_ < n; var_++) {
            QVector<QVector<KEdge>> p_;
            for(int i = 0; i < n; i++)
            {
                QVector<KEdge> tempVector;
                p_.push_back(tempVector);
            }
            p.push_back(p_);
        }
        Dijkstra(first, way, open, first, 0);
        for (int var_ = 0; var_ < d.size(); var_++) {
            if (d[first][var_] == INT_MAX){
                d[first][var_] = 0;
            }
        }
        emit result_only_one(p[first][second], d[first][second]);

    }
    else{
        for(int i = 0; i < n; i++)
        {
            QVector<int> tempVector(n, INT_MAX);

            d.push_back(tempVector);
        }
        for (int var_ = 0; var_ < n; var_++) {
            QVector<QVector<KEdge>> p_;
            for(int i = 0; i < n; i++)
            {
                QVector<KEdge> tempVector;
                p_.push_back(tempVector);
            }
            p.push_back(p_);
        }
        for (int var = 0; var < n; var++) {
            QVector<int> open(n);
            for (int var = 0; var < n; var++) {
                open[var] = var;
            }

            open.removeAt(var);
            d[var][var] = 0;
            QVector<KEdge> way;
            Dijkstra(var, way, open, var, 0);


        }
        for (int var = 0; var < d.size(); var++) {
            for (int var_ = 0; var_ < d.size(); var_++) {
                if (d[var][var_] == INT_MAX){
                    d[var][var_] = 0;
                }
            }
        }
        emit result(p, d);
        ///////////////////////////////////////////////////////////////
        if (mode == 2){
            QVector<int> weight(n, 0);
            QVector<int> Stepen(n, 0);
            for (int var = 0; var < d.size(); var++) {
                for (int var_ = 0; var_ < d.size(); var_++) {
                    if (d[var][var_] > weight[var]){
                        weight[var] = d[var][var_];
                    }
                }
            }
            for (int var = 0; var < main_matrix.size(); var++) {
                for (int var_ = 0; var_ < main_matrix.size(); var_++) {
                    if (!main_matrix[var][var_].isEmpty()){
                        Stepen[var]+=main_matrix[var][var_].size();
                    }
                }
            }
            //////////////////////
            int var_rav = 0;
            int komp = 0;
            QVector<int> komp_in(n);
            while(true){
                komp++;
                QVector<int> open;

                QVector<bool> close(n, false);
                open.push_back(var_rav);
                close[var_rav] = true;
                while(!open.isEmpty()){
                    komp_in[open[0]] = komp;
                    for (int var = 0; var < n; var++) {
                        if (!close[var] && var != open.first() && Sviaz_slab(main_matrix[open.first()][var], true)){
                            close[var] = true;
                            open.push_back(var);
                            komp_in[var] = komp;
                        }
                    }
                    open.removeFirst();
                }
                while(var_rav < close.size() && close[var_rav]){
                    var_rav++;
                }
                if(var_rav == close.size()){
                    break;
                }
            }
            int bridge = 0;
            int sharnir = 0;
            int dop_komp = 0;
            for (int var = 0; var < n; var++) {
                for (int var_ = var + 1; var_ < n; ++var_) {
                    if (main_matrix[var][var_].size() == 1){
                        dop_komp = 0;
                        var_rav = 0;
                        while(true){
                            dop_komp++;
                            QVector<int> open;
                            QVector<bool> close(n, false);
                            open.push_back(var_rav);
                            close[var_rav] = true;
                            while(!open.isEmpty()){
                                for (int var__ = 0; var__ < n; var__++) {
                                    if (!close[var__] && (var__ != var || open.first() != var_) && (var__ != var_ || open.first() != var) && Sviaz_slab(main_matrix[open.first()][var__],false)){
                                        close[var__] = true;
                                        open.push_back(var__);
                                    }
                                }
                                open.removeFirst();
                            }
                            while( var_rav < close.size() && close[var_rav]){
                                var_rav++;
                            }
                            if(var_rav == close.size()){
                                break;
                            }
                        }
                        if (dop_komp!=komp){
                            bridge++;
                        }
                    }
                }
                dop_komp = 0;
                var_rav = 0;
                while(true){
                    dop_komp++;
                    QVector<int> open;
                    QVector<bool> close(n, false);
                    close[var] = true;
                    if (var_rav == var) var_rav++;
                    open.push_back(var_rav);
                    close[var_rav] = true;
                    while(!open.isEmpty()){
                        for (int var__ = 0; var__ < n; var__++) {
                            if (!close[var__] && var__ != var && Sviaz_slab(main_matrix[open.first()][var__],true) ){
                                close[var__] = true;
                                open.push_back(var__);
                            }
                        }
                        open.removeFirst();
                    }
                    while(var_rav < close.size() && close[var_rav]){
                        var_rav++;
                        if (var_rav == var) var_rav++;
                    }
                    if(var_rav >= close.size()){
                        break;
                    }
                }
                if(dop_komp !=komp){
                    sharnir++;
                }
            }

            if(komp == 1){
                bool siln_slab = true;
                for (int var_var = 0; var_var < n; var_var++){
                    komp = 0;
                    var_rav = 0;
                    bool first_vhod = true;
                    while(true){
                        QVector<bool> close(n, false);
                        komp++;
                        QVector<int> open;
                        if (first_vhod){
                            first_vhod = false;

                            open.push_back(var_var);
                            close[var_var] = true;
                        }
                        else {

                            open.push_back(var_rav);
                            close[var_rav] = true;
                        }
                        while(!open.isEmpty()){
                            for (int var = 0; var < n; var++) {
                                if (!close[var] && Sviaz_siln(main_matrix[open.first()][var])){
                                    close[var] = true;
                                    open.push_back(var);
                                }
                            }
                            open.removeFirst();
                        }
                        var_rav = 0;
                        while(var_rav < close.size() && close[var_rav] ){
                            var_rav++;
                        }
                        if(var_rav == close.size()){
                            var_rav = 0;
                            break;
                        }
                    }
                    if (komp != 1){
                        siln_slab = false;
                    }
                }
                if(siln_slab){
                    emit Weihts(weight, Stepen, komp_in, bridge, sharnir, 1);
                }
                else {
                    emit Weihts(weight, Stepen, komp_in, bridge, sharnir, 2);
                }
            }
            else {

                emit Weihts(weight, Stepen, komp_in, bridge, sharnir, 0);
            }
            /////////
        }
    }

}

void Dijkstra_algoritm::Dijkstra(int f_node, QVector<KEdge> w, QVector<int> o, int now_node, int len){
    if(!o.empty()){
        for (int var = 0; var < o.size(); var++) {
            pr++;
            if( Checker(main_matrix[now_node][o[var]]) >= 0){
                QVector<KEdge> w_(w);
                QVector<int> o_(o);
                KEdge e;
                e.First_node = now_node;
                e.Second_node = o[var];
                e.Edge_id = Checker(main_matrix[now_node][o[var]]);
                w_.push_back(e);
                o_.removeAt(var);
                if(d[f_node][o[var]] > len + main_matrix[now_node][o[var]][Checker(main_matrix[now_node][o[var]])]){
                    d[f_node][o[var]] = len + main_matrix[now_node][o[var]][Checker(main_matrix[now_node][o[var]])];
                    p[f_node][o[var]] = w_;
                }
                Dijkstra(f_node, w_,o_,o[var],len + main_matrix[now_node][o[var]][Checker(main_matrix[now_node][o[var]])]);
            }
        }
    }
}

int Dijkstra_algoritm::fact(int N)
{
    if(N < 0)
        return 0;
    if (N == 0)
        return 1;
    else
        return N * fact(N - 1);
}

bool Dijkstra_algoritm::Sviaz_slab(QVector<int> w, bool first){
    if(first)return !w.isEmpty();
    else {
        return w.size() > 0;
    }
}

bool Dijkstra_algoritm::Sviaz_siln(QVector<int> w){
    if(w.isEmpty()){
        return false;
    }
    else {
        for (int var = 0; var < w.size(); var++) {
            if (w[var] > 0) return true;
        }
        return false;
    }
}
