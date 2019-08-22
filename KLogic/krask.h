#ifndef KRASK_H
#define KRASK_H
#include <QThread>
#include <QVector>

class krask: public QThread
{
    Q_OBJECT
private:
    void run();
    QVector<QVector<QVector<int>>> main_matrix;
    QVector<int> return_color;
    bool all_or_not();
    bool ready_for_color(int node, int color);
public:
    krask(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix);
signals:
    void result(QVector<int>);
};

#endif // KRASK_H
