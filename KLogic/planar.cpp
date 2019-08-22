#include "planar.h"

planar::planar(QWidget *parent) : QThread()
{


}

void planar::setting(QVector<QVector<QVector<int>>> matrix){
    for (int var = 0; var < matrix.size(); var++) {
        QVector<int> a;
        n.push_back(0);
        main_matrix.push_back(a);
        for (int var_ = 0; var_ < matrix.size(); var_++) {
            int b = 1;
            main_matrix[var].push_back(b);
            if (matrix[var][var_].isEmpty()){
                 main_matrix[var][var_] = 0;
            }
            else {
                if(var != var_){
                    n[var] += 1;
                }
            }
        }
    }

}

void planar::run(){
    int count = n.size(), n1, n2;
    QVector<bool> close(count, false);
    while (count > 6) {
        n1 = count;
        n2 = n1;
        for (int var = 0; var < n.size() - 1; var++) {
            for (int var_ = var + 1; var_ < n.size(); var_++) {
                if (n1 != n2 && main_matrix[n1][n2] == 1){
                    n1 = var;
                    n2 = var_;
                    break;
                }
            }

            if (n1 != count){
                break;
            }
        }
        if (n1 != count){
            count--;
            for (int var = n1; var < main_matrix.size() - 1; var++) {
                if (n[var] > 0){
                    for (int var_ = var + 1; var_ < main_matrix.size(); var_++) {
                        if (main_matrix[var][var_] == 1 && ((n[n1] >= n[var] && n[n2] >= n[var]) || (n[n1] >= n[var_] && n[n2] >= n[var_]))){
                            n1 = var;
                            n2 = var_;
                        }
                    }
                }
            }
            n[n1] = 0;
            close[n1] = true;
            for (int var = 0; var < n.size(); var++) {
                if (main_matrix[var][n1] == 1 && var != n1){
                    if(main_matrix[var][n2] == 0){
                        main_matrix[var][n2] = main_matrix[n2][var] = 1;
                        n[n2]++;
                    }
                    main_matrix[var][n1]=main_matrix[n1][var]=0;
                }
            }
        }
        else {
            emit result(true);
            break;
        }
    }
    if (count < 6){
        if (count == 5){
            QVector<int> n_5(5);
            for (int var = 0; var < count; var++) {
                n_5[var] = var;
            }
            emit result(Checker_5(n_5));
        }
        else {
            emit result(true);
        }
    }
    else {
        QVector<int> n_3_3(6);
        int c = 0;
        for (int var = 0; var < n.size(); var++) {
            if (!close[var]){
                n_3_3[c] = var;
                c++;
            }
        }
        if (Checker_3_3(n_3_3)){
            n1 = count;
            n2 = n1;
            for (int var = 0; var < n.size() - 1; var++) {
                for (int var_ = var + 1; var_ < n.size(); var_++) {
                    if (n1 != n2 && main_matrix[n1][n2] == 1){
                        n1 = var;
                        n2 = var_;
                        break;
                    }
                }

                if (n1 != count){
                    break;
                }
            }
            if (n1 != count){
                count--;
                for (int var = n1; var < main_matrix.size() - 1; var++) {
                    if (n[var] > 0){
                        for (int var_ = var + 1; var_ < main_matrix.size(); var_++) {
                            if (main_matrix[var][var_] == 1 && ((n[n1] >= n[var] && n[n2] >= n[var]) || (n[n1] >= n[var_] && n[n2] >= n[var_]))){
                                n1 = var;
                                n2 = var_;
                            }
                        }
                    }
                }
                n[n1] = 0;
                close[n1] = true;
                for (int var = 0; var < n.size(); var++) {
                    if (main_matrix[var][n1] == 1 && var != n1){
                        if(main_matrix[var][n2] == 0){
                            main_matrix[var][n2] = main_matrix[n2][var] = 1;
                            n[n2]++;
                        }
                        main_matrix[var][n1]=main_matrix[n1][var]=0;
                    }
                }
                QVector<int> n_5(5);
                c = 0;
                for (int var = 0; var < n.size(); var++) {
                    if (!close[var]){
                        n_5[c] = var;
                        c++;
                    }
                }
                emit result(Checker_5(n_5));
            }
            else {
                emit result(true);
            }
        }
        else {
            emit result(false);
        }
    }
}

bool planar::Checker_5(QVector<int> n_5){
    for (int var = 0; var < 4; var++) {
        for (int var_ = var + 1; var_ < 5; var_++) {
            if (main_matrix[n_5[var]][n_5[var_]] == 0){
                return true;
            }
        }
    }
    return false;
}

bool planar::Checker_3_3(QVector<int> n_3_3){
    for (int var = 0; var < 6; var++) {
        if (n[n_3_3[var]] < 3) return true;
    }
    for (int var_1 = 0; var_1 < 6; var_1++) {
        for (int var_2 = 0; var_2 < 6; var_2++) {
            if (var_1 != var_2){
                for (int var_3 = 0; var_3 < 6; var_3++) {
                    if(var_1 != var_3 && var_2 != var_3){
                        for (int var_4 = 0; var_4 < 6; var_4++) {
                            if(var_1 != var_4 && var_2 != var_4 && var_3 != var_4){
                                for (int var_5 = 0; var_5 < 6; var_5++) {
                                    if(var_1 != var_5 && var_2 != var_5 && var_3 != var_5 && var_4 != var_5){
                                        for (int var_6 = 0; var_6 < 6; var_6++) {
                                            if(var_1 != var_6 && var_2 != var_6 && var_3 != var_6 && var_4 != var_6 && var_5 != var_6 && main_matrix[n_3_3[var_1]][n_3_3[var_4]] == 1 && main_matrix[n_3_3[var_1]][n_3_3[var_5]] == 1 && main_matrix[n_3_3[var_1]][n_3_3[var_6]] == 1  && main_matrix[n_3_3[var_2]][n_3_3[var_4]] == 1 && main_matrix[n_3_3[var_2]][n_3_3[var_5]] == 1 && main_matrix[n_3_3[var_2]][n_3_3[var_6]] == 1  && main_matrix[n_3_3[var_3]][n_3_3[var_4]] == 1 && main_matrix[n_3_3[var_3]][n_3_3[var_5]] == 1 && main_matrix[n_3_3[var_3]][n_3_3[var_6]] == 1){
                                                return false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
