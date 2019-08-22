#include "matrix.h"
#include "ui_matrix.h"

Matrix::Matrix(QWidget *parent, bool is_def_of_dkt) :
    QDialog(parent),
    ui(new Ui::Matrix)
{
    ui->setupUi(this);
    if (is_def_of_dkt) {
        setWindowTitle("Adjacency Matrix");
        def_of_dkt = is_def_of_dkt;
    } else {
        setWindowTitle("Dkt Matrix");
        def_of_dkt = is_def_of_dkt;
    }
}

Matrix::~Matrix()
{
    delete ui;
}

void Matrix::setMatrix(QVector<QPair<QPair<int, int>, QVector<double>>> mat)
{
    ui->matrixWidget->setColumnCount(mat.length());
    ui->matrixWidget->setRowCount(mat.length());
    QStringList list;
    foreach(auto item, mat){
        list.push_back(QString::number(item.first.first));
    }
    ui->matrixWidget->setHorizontalHeaderLabels(list);
    ui->matrixWidget->setVerticalHeaderLabels(list);

    foreach(auto item, mat){
        int res_connect = mat.count();
        QTableWidgetItem* cell = new QTableWidgetItem(QString::number(res_connect));
        //ui->matrixWidget->item

    }
}

void Matrix::update_matrix(QVector<QVector<QVector<KEdge>>> ways_in, QVector<QVector<int>> way_size_in)
{
    if (!def_of_dkt) {
        ways = ways_in;
        way_size = way_size_in;

        is_custom_changed = true;
        qDebug() << "emiting";
        ui->matrixWidget->setColumnCount(ways.length());
        ui->matrixWidget->setRowCount(ways.length());

        QStringList list;
        for (auto i = 0; i < ways.count(); i++){
            list.push_back(QString::number(i));
        }

        ui->matrixWidget->setHorizontalHeaderLabels(list);
        ui->matrixWidget->setVerticalHeaderLabels(list);


        for (int i = 0; i < way_size.count(); i++) {
            for (int j = 0; j < way_size[i].count(); j++){
                if ((way_size[i][j] != 0) && (way_size[i][j] != INT_MAX)) {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(way_size[i][j]));
                    ui->matrixWidget->setItem(i,j,item);
                } else {
                    QTableWidgetItem* item = new QTableWidgetItem(QString::number(0));
                    ui->matrixWidget->setItem(i,j,item);
                }
            }
        }
        is_custom_changed = false;
    }
}

void Matrix::update_adjacency_matrix(QVector<QVector<int>> mat)
{
    if (def_of_dkt) {
        is_custom_changed = true;
        ui->matrixWidget->setColumnCount(mat.length());
        ui->matrixWidget->setRowCount(mat.length());

        QStringList list;
        for (auto i = 0; i < mat.count(); i++){
            list.push_back(QString::number(i));
        }

        ui->matrixWidget->setHorizontalHeaderLabels(list);
        ui->matrixWidget->setVerticalHeaderLabels(list);


        for (int i = 0; i < mat.count(); i++) {
            for (int j = 0; j < mat[i].count(); j++){
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(mat[i][j]));
                ui->matrixWidget->setItem(i,j,item);
            }
        }
        is_custom_changed = false;
    }
}


void Matrix::on_matrixWidget_cellChanged(int row, int column)
{
    //if (ui->matrixWidget->item(row, column)->text() == ui->matrixWidget->item(column,row)->text()){
    //
    //}

    if (def_of_dkt) {
        if (!is_custom_changed) {
            //qDebug() << "emmit";
            emit cell_chanded(row, column, ui->matrixWidget->item(row, column)->text());
        }
    }
}

void Matrix::on_matrixWidget_cellClicked(int row, int column)
{
    if (!def_of_dkt) {
        qDebug() << "emiting kek";
        emit way_return(ways[row][column]);
    }
}
