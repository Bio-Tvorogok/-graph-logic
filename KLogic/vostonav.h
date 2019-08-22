#ifndef VOSTONAV_H
#define VOSTONAV_H
#include <QThread>
#include <QVector>
#include "kedge.h"

class vostonav: public QThread
{
    Q_OBJECT
private:
    void run();
    QVector<int> main_vec;
public:
    vostonav(QWidget *parent = nullptr);
    void setting(QVector<int> vec);
    bool realy(QVector<QPair<int, int>>);
    QVector<QPair<int, int>> sort(QVector<QPair<int, int>> vec);
    QVector<QPair<int, int>> base(QVector<QVector<QVector<int>>> matrix);
signals:
    // 1 - Граф для постройки
    // 2 - если тру то все норм, иначе все печально
    // 3 - если тру то экстремальный
    // 4 - если тру то совершенный
    // 5 - шаги постройки графа
    // 6 - база, (только если экстремальный) просто на вывод
    void result(QVector<QVector<QVector<int>>>, bool, bool, bool, QVector<KEdge>, QVector<QPair<int,int>>);
};

#endif // VOSTONAV_H
