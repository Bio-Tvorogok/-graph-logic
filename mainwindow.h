#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <formgraph.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event) override;
    FormGraph *formGraph;
protected slots:
    void graphSave() const;
    void graphSaveAs();
    void graphOpen();


protected slots:
    void LoadAdjacencyMatrix();
    void SaveAdjacencyMatrix();
    void ReadIncidenceMatrix();
    void SaveIncidenceMatrix();
    void SaveLikeImage();
    void SaveLikeWay();
    void bfs();
    void bfis();
    void dkt();
    void as();
    void dkt_prop();
    void poln_create();
    void rand_graph();
    void color();
    void recovery();
    void about();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
