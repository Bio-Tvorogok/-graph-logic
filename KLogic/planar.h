#ifndef PLANAR_H
#define PLANAR_H
#include <QThread>
#include <kedge.h>
#include <QVector>

class planar: public QThread
{
    Q_OBJECT
public:
    planar(QWidget *parent = nullptr);
    void setting(QVector<QVector<QVector<int>>> matrix);
private:
    void run();
    QVector<QVector<int>> main_matrix;
    QVector<int>n;
    bool Checker_5(QVector<int> n_5);
    bool Checker_3_3(QVector<int> n_3_3);
signals:
    void progress(int);
    void result(bool);
};

#endif // PLANAR_H
