#ifndef POLN_H
#define POLN_H
#include <QThread>
#include <QVector>
#include <QtMath>
#include "kedge.h"

class poln : public QThread
{
    Q_OBJECT
private:
    void run();
    QVector<QVector<QVector<int>>> main_matrix;
    bool Checker(QVector<int> edge1, QVector<int> edge2);
public:
    poln(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>>);
signals:
    void progress(int);
    void result(QVector<KEdge>);
};

#endif // POLN_H
