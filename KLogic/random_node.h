#ifndef RANDOM_NODE_H
#define RANDOM_NODE_H
#include <QThread>
#include <QVector>
#include <QRandomGenerator>
#include <QTime>
#include <QtMath>
#include <QDebug>

class random_node: public QThread
{
    Q_OBJECT
private:
    void run();
public:
    random_node(QWidget *parent = nullptr);
signals:
    void result(QVector<QVector<QVector<int>>>, QVector<QPair<qreal,qreal>>);
    void progress(int);
};

#endif // RANDOM_NODE_H
