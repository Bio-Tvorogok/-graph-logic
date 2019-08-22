#ifndef FORMGRAPH_H
#define FORMGRAPH_H

#include <QWidget>
#include <QApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMultiMap>
#include <QSpacerItem>
#include <QTableWidget>
#include <QHeaderView>
#include <QList>
#include <QMessageBox>
#include <QMetaType>
#include <QtAlgorithms>
#include <QFileDialog>
#include <algorithm>
#include <QProgressDialog>
#include <QTime>
#include "automata.h"
#include "graphwidget.h"
#include "dlginput.h"
#include "matrix.h"
#include "formaction.h"
#include "KLogic/best_first_seatch.h"
#include "KLogic/breadth_for_search.h"
#include "KLogic/dijkstra_algoritm.h"
#include "KLogic/a_star.h"
#include "KLogic/poln.h"
#include "KLogic/random_node.h"
#include "KLogic/krask.h"
#include "KLogic/vostonav.h"
#include "KLogic/kedge.h"

namespace Ui {
class FormGraph;
}

class FormGraph : public QWidget
{
    Q_OBJECT

public:
    explicit FormGraph(QWidget *parent = nullptr);
    ~FormGraph() override;
    void closeEvent(QCloseEvent *event) override;
    Automata::Universal *automat;
    bool is_chaged = false;

private:
    Ui::FormGraph *ui;
    //DlgInput *dlgInput;

    Node *_source;  // Сохраняет вершину при нажатии на кнопку "Соединить"
    // Флаг поднимается при нажатии на кнопку "Соединить"
    // connFlag == 1 Надо отметить "Источник" потом "Получатель"
    // connFlag == 2 Надо отметить только "Получатель"
    int connFlag;

// KLogic
private:
    QPair<QList<FormAction>, int> actions;

    // Algoritm
    breadth_for_search* bfs;
    Best_First_seatch* bfis;
    Dijkstra_algoritm* dkt;
    A_star* as;
    poln* pn;
    random_node* rn;
    krask* ks;
    vostonav* vst;


    QTime timer;

    bool is_color_changed = false;
    QColor def_color = Qt::black;
    QPair<QPointF, int> old_pos_save;

    Matrix* def_matrix = new Matrix(this, true);
    Matrix* dkt_matrix = new Matrix(this, false);

    QProgressDialog* pd;


    bool ctrl_pressed = false;
    bool is_show_id = false;
    bool is_edge_directed = false;

    QVector<KEdge> tmp_way = {};



public:
    QList <Node*> nodes;
    QList <EdgeParent*> edges;
    static FormGraph *openGraph(QString fileName, bool jsonFormat = false);

    QVector<QVector<QVector<int>>> getGraph();
    QVector<QVector<int>> getSimpleGraph();
    void ShowResultDialog(QString data);
    void Algo_Checked(int);

private:
    bool isHex(const QString&);
    void ClearAll();
    void setDefColor();
    bool isEgdesInSameNodes(EdgeParent*, EdgeParent*);
    bool isSameNodesInEdge(EdgeParent*, Node*, Node*);
    bool removeItem(Node* node);
    bool removeItem(EdgeParent* edge);
    Node* createSimpleNode(QPointF, QString = "");
    EdgeParent* createSimpleEdge(Node*, Node*);
    EdgeParent* createSimpleEdge(int, int, int, bool);
    void showWay(QVector<KEdge>);
    void RemoveNodeSimple(Node*);
    void RemoveEdgeSimple(EdgeParent*);

    //Actions Func
    void PushAction(FormAction);
    void BackAction(bool);
    ItemInfoEdge fillEdgeAction(EdgeParent*);
    ItemInfoNode fillNodeAction(Node*);
    ItemEdgeMass fillEdgeMass(EdgeParent*, int);
    ItemNodeMove fillNodeMove(Node*, QPointF);
    void BackErasedNode(FormAction);
    void BackErasedEdge(FormAction);
    void EraseAddedNode(FormAction);
    void EraseAddedEdge(FormAction);
    void MoveNode(FormAction, bool);
    void RenameMassEdge(FormAction, bool);




public slots:
    bool CreateAutomat(QStringList source);
    void CreateAutomat(Automata::Universal *_automat);
    void showInput();
    void onBtnCreateNodeClicked();
    void onBtnConnectNodeClicked();
    void onBtnDeleteClicked();
    void onBtnCheckClicked();
    void sceneSelectionChanged();
    void savePng(QString fileName) const;
    void checkedTable();
    void checkedMatrixStr();
    bool saveGraph(QString fileName, bool jsonFormat = false) const;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    //File Read
    void ReadAdjacencyMatrix();
    void ReadIncidenceMatrix();

    //File Write
    void SaveAdjacencyMatrix(bool = true);
    void SaveIncidenceMatrix();
    void SaveImage();
    void SaveWay();
    void SaveDijkstraProp(QVector<int> weight, QVector<int> degrees, int indicator, int rad = 0, int diam = 0);

    //Actions
    void MouseGraphPressed();
    void MouseGraphReleased();

    //Matrix
    void OnCellChaged(int, int, QString);
    void OnCellClickedDkt(QVector<KEdge>);

public slots:
    void on_testButton_clicked();
// KLogic
private slots:
    void get_way_bfs(QVector<KEdge>);
    void get_way_bfis(QVector<KEdge>);
    void get_way_dkt(QVector<KEdge>, int);
    void get_way_as(QVector<KEdge>, int);
    void on_waysButton_clicked();
    void get_weight_dijkstra(QVector<int>, QVector<int>,QVector<int>, int, int, int);
    void get_graph_poln(QVector<KEdge>);
    void get_random_graph(QVector<QVector<QVector<int>>>, QVector<QPair<qreal,qreal>>);
    void get_color(QVector<int>);
    void get_recovery(QVector<QVector<QVector<int>>>, bool, bool, bool, QVector<KEdge>, QVector<QPair<int,int>>);

// For Matrix
    void get_ways_dkt_matrix(QVector<QVector<QVector<KEdge>>>, QVector<QVector<int>>);
    void on_ShowIdBox_clicked(bool checked);
    void on_DirectedEdgeBox_clicked(bool checked);
    void on_MatrixButton_clicked();

    void on_TestSave_clicked();
    void on_btnClear_clicked();

signals:
    void emit_update_matrix(QVector<QVector<QVector<KEdge>>>, QVector<QVector<int>>);
    //TODO emit change the mass
    void emit_update_adjacency_matrix(QVector<QVector<int>>);

};

#endif // FORMGRAPH_H
