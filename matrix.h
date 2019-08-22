#ifndef MATRIX_H
#define MATRIX_H

#include <QDialog>
#include <QVector>
#include <QDebug>
#include "KLogic/kedge.h"

namespace Ui {
class Matrix;
}

class Matrix : public QDialog
{
    Q_OBJECT

public:
    explicit Matrix(QWidget *parent = nullptr, bool is_def_of_dkt = true);
    ~Matrix();

    void setMatrix(QVector<QPair<QPair<int, int>, QVector<double>>> mat = {});

public slots:
    void update_matrix(QVector<QVector<QVector<KEdge>>>, QVector<QVector<int>>);
    void update_adjacency_matrix(QVector<QVector<int>>);

private slots:
    void on_matrixWidget_cellChanged(int row, int column);

    void on_matrixWidget_cellClicked(int row, int column);

signals:
    void cell_chanded(int, int, QString);
    void way_return(QVector<KEdge>);

private:
    Ui::Matrix *ui;
    bool is_custom_changed = false;
    bool def_of_dkt = true;

    QVector<QVector<QVector<KEdge>>> ways;
    QVector<QVector<int>> way_size;
};

#endif // MATRIX_H
