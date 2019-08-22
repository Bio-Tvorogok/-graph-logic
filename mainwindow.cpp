#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    formGraph = ui->formGraph;

    //connect(ui->actSave, &QAction::triggered, this, &MainWindow::graphSave);
    //connect(ui->actSaveAs, &QAction::triggered, this, &MainWindow::graphSaveAs);
    //connect(ui->actOpen, &QAction::triggered, this, &MainWindow::graphOpen);

    connect(ui->LoadAM, &QAction::triggered, this, &MainWindow::LoadAdjacencyMatrix);
    connect(ui->SaveAM, &QAction::triggered, this, &MainWindow::SaveAdjacencyMatrix);

    connect(ui->LoadIM, &QAction::triggered, this, &MainWindow::ReadIncidenceMatrix);
    connect(ui->SaveIM, &QAction::triggered, this, &MainWindow::SaveIncidenceMatrix);

    connect(ui->SaveImage, &QAction::triggered, this, &MainWindow::SaveLikeImage);
    connect(ui->SaveWay, &QAction::triggered, this, &MainWindow::SaveLikeWay);

    connect(ui->Bfs, &QAction::triggered, this, &MainWindow::bfs);
    connect(ui->Bfis, &QAction::triggered, this, &MainWindow::bfis);
    connect(ui->Dkt, &QAction::triggered, this, &MainWindow::dkt);
    connect(ui->As, &QAction::triggered, this, &MainWindow::as);
    connect(ui->Dkt_prop, &QAction::triggered, this, &MainWindow::dkt_prop);
    connect(ui->Poln, &QAction::triggered, this, &MainWindow::poln_create);
    connect(ui->Rand, &QAction::triggered, this, &MainWindow::rand_graph);
    connect(ui->Color, &QAction::triggered, this, &MainWindow::color);
    connect(ui->Recovery, &QAction::triggered, this, &MainWindow::recovery);

    connect(ui->About, &QAction::triggered, this, &MainWindow::about);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if ((formGraph->nodes.count() > 0) && (formGraph->is_chaged))
        SaveAdjacencyMatrix();
    event->accept();
}

void MainWindow::graphSave() const {
    ui->formGraph->saveGraph("graph.graph");
}

void MainWindow::graphSaveAs() {
    QString fileName = QFileDialog::getSaveFileName(this,
                        "Сохранить файл",
                        "graph.graph",
                        "Graph Files (*.graph)");
    ui->formGraph->saveGraph(fileName);
}

void MainWindow::graphOpen() {
    QString fileName = QFileDialog::getOpenFileName(this,
                        "Открыть файл", "", "Graph Files (*.graph)");
    FormGraph *f = FormGraph::openGraph(fileName);
    if (!f) {
        qWarning("!f");
        return;
    }
    delete ui->formGraph;
    ui->formGraph = f;
    ui->gridLayout->addWidget(f, 0, 0, 1, 1);
    repaint();
}

void MainWindow::LoadAdjacencyMatrix()
{
    ui->formGraph->ReadAdjacencyMatrix();
}

void MainWindow::SaveAdjacencyMatrix()
{
    ui->formGraph->SaveAdjacencyMatrix();
}

void MainWindow::ReadIncidenceMatrix()
{
    ui->formGraph->ReadIncidenceMatrix();
}

void MainWindow::SaveIncidenceMatrix()
{
    ui->formGraph->SaveIncidenceMatrix();
}

void MainWindow::SaveLikeImage()
{
    ui->formGraph->SaveImage();
}

void MainWindow::SaveLikeWay()
{
    ui->formGraph->SaveWay();
}

void MainWindow::bfs()
{
    ui->formGraph->Algo_Checked(0);
}

void MainWindow::bfis()
{
    ui->formGraph->Algo_Checked(1);
}

void MainWindow::dkt()
{
    ui->formGraph->Algo_Checked(2);
}

void MainWindow::as()
{
    ui->formGraph->Algo_Checked(3);
}

void MainWindow::dkt_prop()
{
    ui->formGraph->Algo_Checked(4);
}

void MainWindow::poln_create()
{
    ui->formGraph->Algo_Checked(5);
}

void MainWindow::rand_graph()
{
    ui->formGraph->Algo_Checked(7);
}

void MainWindow::color()
{
    ui->formGraph->Algo_Checked(6);
}

void MainWindow::recovery()
{
    ui->formGraph->Algo_Checked(8);
}

void MainWindow::about()
{
    QMessageBox::about(this, "About", "Сделали:\n Каменев Артем, Кирдяшкин Игорь");
}
