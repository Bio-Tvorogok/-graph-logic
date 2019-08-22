#include "random_node.h"

random_node::random_node(QWidget *parent) : QThread()
{

}

void random_node::run(){
    qreal max_x = 15000;
    QTime midnight(0,0,0);
    QRandomGenerator generator_x(midnight.secsTo(QTime::currentTime()));
    wait(1000);
    QRandomGenerator generator_y(midnight.secsTo(QTime::currentTime()));
    int n = generator_x.bounded(9000, 10110);
    n += 6;
    int pr = 0;
    QVector<QVector<QVector<int>>> Result_matrix;
    for (int var_ = 0; var_ < n; var_++) {
        QVector<QVector<int>> p_;
        for(int i = 0; i < n; i++)
        {
            QVector<int> tempVector;
            p_.push_back(tempVector);
        }
        Result_matrix.push_back(p_);
    }

    QVector<QPair<qreal, qreal>> x_y(n);
    x_y[0].first = -max_x;
    x_y[0].second = 0;
    x_y[1].first = max_x;
    x_y[1].second = 0;
    x_y[2].first = -max_x;
    x_y[2].second = -max_x;
    x_y[3].first = max_x;
    x_y[3].second = max_x;
    x_y[4].first = 0;
    x_y[4].second = 0;
    x_y[5].first = 0;
    x_y[5].second = max_x;

    for (int var = 6; var < x_y.size(); var++) {
        qDebug() << var;
        x_y[var].first = (qreal)generator_x.bounded((int)-max_x,(int)max_x);
        x_y[var].second = (qreal)generator_y.bounded((int)-max_x,(int)max_x);
        for (int var_ = 0; var_ < var; var_++) {
            if (x_y[var].first == x_y[var_].first && x_y[var].second == x_y[var_].second){
                generator_x = *new QRandomGenerator(midnight.secsTo(QTime::currentTime()));
                wait(1000);
                generator_y = *new QRandomGenerator(midnight.secsTo(QTime::currentTime()));
                var--;
            }
        }
    }
    for (int var = 0; var < Result_matrix.size(); var++){
        if (pr != qFloor(100 * var / n)){
            pr =  qFloor(100 * var / n);
            emit progress(pr);
          }
        for (int var_ = var + 1; var_ < Result_matrix.size(); var_++){


            int64_t r = qFloor(qSqrt((x_y[var].first - x_y[var_].first) * (x_y[var].first - x_y[var_].first) + (x_y[var].second - x_y[var_].second) * (x_y[var].second - x_y[var_].second)));
            if (generator_x.bounded(0,10000) > qCeil(r / qFloor(qSqrt(qPow(max_x/40, 2) * 2))) * 10000){
                Result_matrix[var_][var].push_back(r);
                Result_matrix[var][var_].push_back(r);
            }

        }
    }
    emit progress(100);
    emit result(Result_matrix, x_y);
}
