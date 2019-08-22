#include "formgraph.h"
#include "ui_formgraph.h"

#include <QJsonDocument>

FormGraph::FormGraph(QWidget *parent) :
    QWidget(parent), automat(nullptr),
    ui(new Ui::FormGraph),
    _source(nullptr), connFlag(false)
{
    ui->setupUi(this);
    qRegisterMetaType<QVector<KEdge>>("QVector<KEdge>");
    qRegisterMetaType<QVector<QVector<QVector<KEdge>>>>("QVector<QVector<QVector<KEdge>>>");
    qRegisterMetaType<QVector<QVector<QVector<int>>>>("QVector<QVector<QVector<int>>>");
    qRegisterMetaType<QVector<QPair<qreal, qreal>>>("QVector<QPair<qreal, qreal>>");
    qRegisterMetaType<QVector<QVector<int>>>("QVector<QVector<int>>");
    qRegisterMetaType<QVector<QPair<int, int>>>("QVector<QPair<int, int>>");

    //dlgInput = new DlgInput();
    actions.second = 0;

    connect(ui->btnCreateNode, &QPushButton::clicked, this, &FormGraph::onBtnCreateNodeClicked);
    connect(ui->btnConnectNode, &QPushButton::clicked, this, &FormGraph::onBtnConnectNodeClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &FormGraph::onBtnDeleteClicked);
    connect(ui->grafViewScene->scene(), &QGraphicsScene::selectionChanged, this, &FormGraph::sceneSelectionChanged);
    //connect(dlgInput->btnApply, &QPushButton::clicked, this, &FormGraph::onBtnApplyClicked);
    connect(ui->btnCheck, &QPushButton::clicked, this, &FormGraph::onBtnCheckClicked);
    connect(ui->grafViewScene, &GraphWidget::editItem, this, &FormGraph::showInput);
    connect(this, &FormGraph::emit_update_matrix, dkt_matrix, &Matrix::update_matrix);
    connect(this, &FormGraph::emit_update_adjacency_matrix, def_matrix, &Matrix::update_adjacency_matrix);

    connect(ui->grafViewScene, &GraphWidget::mousePressed, this, &FormGraph::MouseGraphPressed);
    connect(ui->grafViewScene, &GraphWidget::mouseReleased, this, &FormGraph::MouseGraphReleased);

    connect(def_matrix, &Matrix::cell_chanded, this, &FormGraph::OnCellChaged);
    connect(dkt_matrix, &Matrix::way_return, this, &FormGraph::OnCellClickedDkt);


    //connect(ui->grafViewScene->scene(), &QGraphicsScene::, this, &FormGraph::sceneSelectionChanged);
}


FormGraph::~FormGraph()
{
    //QMessageBox::warning(this, "adw", "awd");
    ui->grafViewScene->scene()->clearSelection();
    nodes.clear();
    edges.clear();
    delete ui;
    //delete dlgInput;
}

bool FormGraph::CreateAutomat(QStringList source)
{
    automat = new Automata::Universal(source);
    if (!automat->f || !automat->t){
        qDebug() << "Win::CreateAutomat : !automat";
        return false;
    }
    ui->lNameGraf->setText((automat->t->type() == Automata::Mili::Type ? "Автомат Мили": "Автомат Мура"));
    ui->btnCheck->setEnabled(true);
    ui->lTip->setText(QString("Вых файл: OutFile.png"));
    return true;
}

void FormGraph::CreateAutomat(Automata::Universal *_automat)
{
    automat = _automat;
    ui->lNameGraf->setText((automat->t->type() == Automata::Mili::Type ? "Автомат Мили": "Автомат Мура"));
    ui->lTip->setText(QString("Вых файл: OutFile.png"));
}

void FormGraph::closeEvent(QCloseEvent */*event*/)
{
  //  dlgInput->close();
    // Важно! disconnect нужен для корректного выхода из приложения!
    disconnect(ui->grafViewScene->scene(), nullptr, nullptr, nullptr);
}

void FormGraph::showInput()
{
    auto dlgInput = new DlgInput(this);
    if (ui->grafViewScene->scene()->selectedItems().size() == 0) {  // nullptr
        // Ничего не выделено
        dlgInput->lTipInput->clear();
        dlgInput->eInput->clear();
        //dlgInput->eInput->setEnabled(false);
        dlgInput->hide();
    } else {
        QGraphicsItem *it;
        it = ui->grafViewScene->scene()->selectedItems().at(0);
        //dlgInput->eInput->setEnabled(true);
        if (Node *n = dynamic_cast<Node *>(it)) {
            if (automat) {
                //dlgInput->eInput->setValidator(new QRegExpValidator(QRegExp("(([1-9]*)|(([1-9]*)\\.([0-9]*)))")));
                dlgInput->lTipInput->setText(automat->t->tipNode());
            } else {
                dlgInput->lTipInput->setText("Введите текст");
            }
            dlgInput->eInput->setText(n->textContent());
        } else if (EdgeParent *e = dynamic_cast<EdgeParent *>(it)) {
            if (automat) {
                //dlgInput->eInput->setValidator(new QRegExpValidator(QRegExp("(([1-9]*)|(([1-9]*)\\.([0-9]*)))")));
                dlgInput->lTipInput->setText(automat->t->tipEdge());
            } else {
                dlgInput->lTipInput->setText("Введи текст");
            }
            dlgInput->eInput->setText(e->textContent());
        }

        //qDebug() << "ТЫ ГЕЙ";
        switch (dlgInput->exec()) {
        case QDialog::Accepted: {
             if (ui->grafViewScene->scene()->selectedItems().size() == 1) {
                 qDebug() << "grafViewScene->scene()->selectedItems().size() == "
                     << ui->grafViewScene->scene()->selectedItems().size();


                 auto it = ui->grafViewScene->scene()->selectedItems().at(0);
                 NodeEdgeParent *nodeEdge = dynamic_cast<NodeEdgeParent*>(it);
                 if (nodeEdge) {
                    if (auto edge = dynamic_cast<EdgeParent*>(nodeEdge)){
                        auto act = fillEdgeMass(edge, dlgInput->eInput->text().toInt());
                        PushAction(FormAction(act));
                    }
                    nodeEdge->setTextContent(dlgInput->eInput->text());
                    //act = fillEdgeMass()
                 } else { // if (it->type() == Edge::Type) {
                    qDebug() << "It does not NodeEdgeParent";
                 }
             }
             break;
        }
        case QDialog::Rejected: {
            break;
        }
        }
        dlgInput->close();
    }
}

void FormGraph::onBtnCreateNodeClicked()
{
    ui->btnConnectNode->setChecked(false);

    int x, y;           // расположение вершины на сцене
    int numNode = 0;
    bool flFinding = false;     // флаг нахождения, при решение с каким состоянием создавать вершину
    Node *node;
    if (automat) {
        for (auto i = 0; i < automat->f->countA; i++) {
            flFinding = false;
            for (int j = 0; j < nodes.size(); j++) {
                QString s = nodes[j]->textContent().section(QRegExp("[^0-9]+"), 0, 0, QString::SectionSkipEmpty);
                if (s.toInt() == i) {
                    flFinding = true;
                    break;
                }
            }
            if (!flFinding || i == automat->f->countA - 1) {
                numNode = i;
                break;
            }
        }
        QString nodeText = (automat->t->type() == Automata::Mura::Type ? "a%1/y1" : "a%1");
        node = new Node(ui->grafViewScene, QString(nodeText).arg(numNode), is_show_id);
    } else {
        node = new Node(ui->grafViewScene, nullptr, is_show_id);
        numNode = node->id();
    }
    // Определяет сколько вершин будут появлятся на одной оси У
    int nodeInRow = 6;
    //if (event == nullptr) {
        x = - 2 * (2 * Node::Radius + 10) +
            ((!flFinding ? numNode : nodes.size()) % nodeInRow)
            * (2 * Node::Radius + 10);
        y = -100 + 2 * Node::Radius + 10 +
            ((!flFinding ? numNode : nodes.size()) / nodeInRow)
            * (2 * Node::Radius + 10);
    //} else {
       // x = event->pos().x();
      //  y = event->pos().y();
    //}
    nodes.append(node);
    node->setPos(x, y);

    auto action_node = fillNodeAction(node);
    PushAction(FormAction(action_node, false));
    //actions.push_back(FormAction(action_node, false));

    _source = nullptr;
    connFlag = 0;
    ui->lTip->setText("Добавлена вершина.");
    //if (nodes.size()==9){
    //    ui->btnCreateNode->setEnabled(false);
    //}
}

void FormGraph::onBtnConnectNodeClicked()
{
    if (ui->grafViewScene->scene()->selectedItems().size() > 0) {
        _source = dynamic_cast<Node *> (ui->grafViewScene->scene()->selectedItems().at(0));
        if (_source) {
            ui->grafViewScene->scene()->clearSelection();
            ui->lTip->setText("Выберите, куда будет проведена дуга.");
            connFlag = 2;
        } else {
            ui->lTip->clear();
            connFlag = 0;
        }
    }
    /* if (!_source) {
        if (connFlag == 0) {    // это условие не обязательное
            lTip->setText("Выберите вершину источник, потом получатель дуги");
            connFlag = 1;
            grafViewScene->scene()->clearSelection();
        }

    }*/
}

void FormGraph::onBtnDeleteClicked()
{
    if (ui->grafViewScene->scene()->selectedItems().size()) {
        short is_edge_or_node = 0;
        ItemInfoEdge action_edge;
        ItemInfoNode action_node;
        _source = nullptr;
        connFlag = 0;
        auto i = ui->grafViewScene->scene()->selectedItems().at(0);
        if (Node* n = dynamic_cast<Node*>(i)) {
            //actions
            //actions.push_back(new FormAction(true, true, n));
            // Filling action

            action_node = fillNodeAction(n);
            is_edge_or_node = -1;
            //actions.push_back(FormAction(action_node, true));

            foreach (auto e, n->edges()) {
                edges.removeAll(e);
            }
            if (!removeItem(n)) return;
            //PushAction(FormAction(action_node, true));
            ui->btnCreateNode->setEnabled(true);
            ui->lTip->setText("Вершина удалена.");
        } else if (EdgeParent *e = dynamic_cast<EdgeParent*>(i)) {
            action_edge = fillEdgeAction(e);
            is_edge_or_node = 1;
            //actions.push_back(FormAction(action_edge, true));
            if (!removeItem(e)) return;
            //PushAction(FormAction(action_edge, true));
            ui->lTip->setText("Дуга удалена.");

        } else {
            qDebug() << tr("I don't know what it is. type == %1").arg(i->type());
        }
        ui->grafViewScene->scene()->removeItem(i);
        //ui->grafViewScene->scene()->update();
        delete i;
        if (is_edge_or_node == -1)
            PushAction(FormAction(action_node, true));
        else if (is_edge_or_node == 1)
            PushAction(FormAction(action_edge, true));
    }
}

void FormGraph::onBtnCheckClicked()
{
    if (automat) {
        if (automat->f->format() == Automata::Table::Format) {
            checkedTable();
        } else if (automat->f->format() == Automata::Matrix::Format) {
            checkedMatrixStr();
        }
    }
}

void FormGraph::checkedTable()
{
    QMessageBox msgBox;
    if (nodes.size() != automat->f->countA) {
        msgBox.setText("Error.");
        msgBox.exec();
        return;
    }
    int flFail = 0;
    QVector<QMultiMap<QString, int> > ch;
    if (automat->t->type() == Automata::Mura::Type) {
        ch.resize(nodes.size());
        // Проверка на очередность вершин
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (listNums(nodes.at(j)->textContent())
                        .at(0).toInt() == i) {
                    break;
                } else if (j == nodes.size() - 1) {
                    flFail = 1;
                }
            }
        }
        for (int i = 0; i < nodes.size(); i++) {
            // "a2/y1,y3" or "a1/-"
            QStringList nums = listNums(nodes.at(i)->textContent());
            if (nums.at(0).toInt() >= automat->f->countA) {
                flFail = 2;
                break;
            }
            if (nums.size() == 1) { // "a1/-"
                ch[(nums.at(0).toInt())].insert("y", 0);
            } else {
                for (int j = 1; j < nums.size(); j++) {
                    ch[(nums.at(0).toInt())].insert("y", nums.at(j).toInt());
                }
            }
        }

        for (int i = 0; i < edges.size(); i++) {
            if (flFail) {
                break;
            }
            int sourceNum = listNums(edges.at(i)->sourceNode()->textContent())
                    .at(0).toInt();
            int destNum = listNums(edges.at(i)->destNode()->textContent())
                    .at(0).toInt();
            // "x1" or "x2,x4"
            QStringList nums = listNums(edges.at(i)->textContent());
            for (int j = 0; j < nums.size(); j++) {
                ch[(sourceNum)].insert(QString("x%1").arg(nums.at(j)), destNum);
            }
        }
        // Заполнение пропусков
        for(int i = 0; i < ch.size(); i++) {
            for (int j = 1; j < automat->f->countX + 1; j++) {
                bool flMissKey = true;
                for (auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1").arg(j), 0);
                }
            }
        }
    } else if (automat->t->type() == Automata::Mili::Type) {
        ch.resize(nodes.size());
        // Проверка на очередность вершин
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes.at(i)->textContent()
                        .remove(0, 1).toInt() == i) {
                    break;
                } else if (j == nodes.size() - 1) {
                    flFail = 1;
                }
            }
        }

        for (int i = 0; i < edges.size(); i++) {
            if (flFail) {
                break;
            }
            int sourceNum = listNums(edges.at(i)->sourceNode()->textContent()).at(0).toInt();
            int destNum = listNums(edges.at(i)->destNode()->textContent()).at(0).toInt();
            // "x1/y1,y2" or "x2/-"
            QStringList nums = listNums(edges.at(i)->textContent());
            if (nums.at(0).toInt() > automat->f->countX) {
                flFail = 2;
                break;
            }
            ch[sourceNum].insert(tr("x%1a").arg(nums.at(0)), destNum);
            if (nums.size() == 1) {
                ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), 0);
            } else {
                for (int j = 1; j < nums.size(); j++) {
                    if (nums.at(j).toInt() > automat->f->countY) {
                        flFail = 3;
                        break;
                    }
                    ch[sourceNum].insert(tr("x%1y").arg(nums.at(0)), nums.at(j).toInt());
                }
            }
        }
        // Заполнение пустот
        for (int i = 0; i < automat->f->countA; i++) {
            for (int j = 1; j < automat->f->countX; j++) {
                bool flMissKey = true;
                for(auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1a").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1a").arg(j), 0);
                }
                flMissKey = true;
                for(auto key : ch[i].uniqueKeys()) {
                    if (tr("x%1y").arg(j) == key) {
                        flMissKey = false;
                        break;
                    }
                }
                if (flMissKey) {
                    ch[i].insert(tr("x%1y").arg(j), 0);
                }
            }
        }
    }
    if (!flFail && automat->check(ch)) {
        QPixmap pixMap = QPixmap::grabWidget(ui->grafViewScene);
        pixMap.save("OutFile.png");
        msgBox.setText("Всё верно! Результат сохранён!");
        msgBox.exec();
    } else {
        msgBox.setText(tr("Неверно!\nКод ошибки: #%1").arg(flFail));
        msgBox.exec();
    }
}

void FormGraph::checkedMatrixStr()
{
    QStringList result;
    result.append(QString("%1").arg(automat->t->type()));
    result.append(QString("%1").arg(automat->f->format()));
    result.append(QString("%1").arg(nodes.size()));
    if (automat->t->type() == Automata::Mili::Type) {
        result.append(QString("%1").arg(nodes.size()));
    } else if (automat->t->type() == Automata::Mura::Type) {
        result.append(QString("%1").arg(nodes.size() + 1));
        // Заполнение строчки с y2,y3 или y1
        for (int aN = 0; aN < nodes.size(); aN++) {
            bool flError = true;
            foreach (auto node, nodes) {
                if (listNums(node->textContent())
                        .at(0).toInt() == aN) {
                    result.append(node->textContent().split("/").at(1));
                    flError = false;
                    break;
                }
            }
            if (flError) {
                qDebug() << "Вершины идут не по порядку: а0, а1, а4, а5";
                QMessageBox msgBox;
                msgBox.setText("Error.");
                msgBox.exec();
                return;
            }
        }
    }

    for (auto sourceN = 0; sourceN < nodes.size(); sourceN++) {
        for (auto destN = 0; destN < nodes.size(); destN++) {
            QStringList duplicateEdge;  // условно, объеденяются через запятую
            foreach (auto edge, edges) {
                if ((listNums(edge->sourceNode()->textContent())
                        .at(0).toInt() == sourceN)
                        && (listNums(edge->destNode()->textContent())
                            .at(0).toInt() == destN)) {
                    duplicateEdge.append(edge->textContent());
                }
            }
            if (duplicateEdge.size() == 0) {
                if (automat->t->type() == Automata::Mili::Type) {
                    result.append("-/-");
                } else if (automat->t->type() == Automata::Mura::Type) {
                    result.append("-");
                }
            } else {
                result.append(duplicateEdge.join(","));
            }
        }
    }
    if (automat->check(result)) {
        savePng("OutFile.png");
        QMessageBox msgBox;
        msgBox.setText("Всё верно! Результат сохранён!");
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Неверно!");
        msgBox.exec();
    }
}

void FormGraph::sceneSelectionChanged()
{
    //dlgInput->hide();
    QList<QGraphicsItem *> l = ui->grafViewScene->scene()->selectedItems();
    if (l.size() == 1) {
        ui->lTip->setText("Выделена вершина.");
        if (Node *node = dynamic_cast<Node *>(l.at(0))) {
            // Выделена вершина!
            ui->btnConnectNode->setEnabled(true);
            if (connFlag == 1) {
                // Назначен "Источник"
                _source = node;
                connFlag = 2;
                ui->lTip->setText("Выберите вершину куда будет проведена дуга.");
            } else if (connFlag == 2) {
                // Нужно соединить с новой вершиной
                EdgeParent *e;
                if (automat) {
                    if (_source == node) {
                        e = new EdgeCircle(_source, (automat->t->type() == Automata::Mura::Type ? "x1" : "x1/y1"), is_show_id);
                    } else {
                        e = new Edge(_source, node, (automat->t->type() == Automata::Mura::Type ? "x1" : "x1/y1"), is_show_id);
                    }
                } else {
                    if (_source == node) {
                        e = new EdgeCircle(_source, nullptr, is_show_id, is_edge_directed);
                    } else {
                        e = new Edge(_source, node, nullptr, is_show_id, is_edge_directed);
                    }
                }
                auto action_edge = fillEdgeAction(e);
                PushAction(FormAction(action_edge, false));
                //actions.push_back(FormAction(action_edge, false));

                edges.append(e);
                ui->btnConnectNode->setChecked(false);
                connFlag = 0;
                _source = nullptr;
            } else if (connFlag==3){

            }
        } else {
            // Выделена стрелка
            ui->lTip->setText("Выделена дуга.");
            ui->btnConnectNode->setEnabled(false);
            connFlag = false;
            _source = nullptr;
        }
        ui->btnDelete->setEnabled(true);

    } else if (l.size() > 1) {
        // Всегда должено быть выделено не более 1ого элемента
        qDebug() << "grafViewScene->scene()->selectedItems().size() == " << l.size();
    } else {
        // Пропало выделение (после удаления или нажатия на "Соединить")
        ui->lTip->setText("");
        ui->btnDelete->setEnabled(false);
        ui->btnConnectNode->setEnabled(false);
    }
}

void FormGraph::savePng(QString fileName) const
{
    QPixmap pixMap = QPixmap::grabWidget(ui->grafViewScene);
    pixMap.save(fileName);
}


bool FormGraph::saveGraph(QString fileName, bool jsonFormat) const
{
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }
    QJsonObject graphJson;
    automat->writeToJson(graphJson);                            // automat
    ui->grafViewScene->writeToJson(graphJson);                  // scene
    QJsonArray jsonNodes;
    std::for_each(nodes.begin(), nodes.end(), [&] (Node *n) {
        QJsonObject json;
        n->writeToJson(json);
        jsonNodes.append(json); });
    graphJson["nodes"] = jsonNodes;                  // nodes
    QJsonArray jsonEdges;
    std::for_each(edges.begin(), edges.end(), [&] (EdgeParent *e) {
        QJsonObject json;
        e->writeToJson(json);
        jsonEdges.append(json); });
    graphJson["edges"] = jsonEdges;                  // edges
    QJsonDocument saveDoc(graphJson);
    saveFile.write(jsonFormat ?
                       saveDoc.toJson()
                     : saveDoc.toBinaryData());
    saveFile.close();
    return true;
}

void FormGraph::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Delete:
        onBtnDeleteClicked();
        break;
    case Qt::Key_N:
    case 1058:
        onBtnCreateNodeClicked();
        break;
    case Qt::Key_C:
    case 1057:
        onBtnConnectNodeClicked();
        break;
    case Qt::Key_Enter:
    case 16777220:
        onBtnCheckClicked();
        break;
    case Qt::Key_E:
        onBtnConnectNodeClicked();
        break;
    case Qt::Key_Control:
       // qDebug() << "pressed";
        ctrl_pressed = true;
        break;
    case Qt::Key_Z:
        if (ctrl_pressed) {
            BackAction(true);
                //actions.pop_back();
        }
        break;
    case Qt::Key_X:
        if (ctrl_pressed) {
            BackAction(false);
        }
        break;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}

void FormGraph::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {

    case Qt::Key_Control:
        ctrl_pressed = false;
        break;
    default:
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void FormGraph::ReadAdjacencyMatrix()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open Adjacency"), QDir::homePath(), "AM File (*.am)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QFile::ReadOnly | QFile::Text))
            QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
        else {
            ClearAll();
            ui->DirectedEdgeBox->setChecked(true);
            is_edge_directed = true;
            int nodes_source_id = 0;
            QTextStream in(&file);
            QVector<QVector<int>> connection_arr;
            bool first_read = true;

            while (!in.atEnd()){
                auto line = in.readLine();
                if (line[0] == "%") {
                    continue;
                }
                else if (line.contains("Vertex")) {
                    // Creating Nodes
                    auto pos_one = line.indexOf("{", 0);
                    auto pos_two = line.indexOf("(", pos_one);
                    QString node_name = "";
                    if ((pos_one != -1) && (pos_two != -1))
                        node_name = line.mid(pos_one + 1, pos_two - (pos_one + 1));

                    pos_one = pos_two;
                    pos_two = line.indexOf(",", pos_one);
                    qreal x = 0;
                    if ((pos_one != -1) && (pos_two != -1))
                        x = line.mid(pos_one + 1, pos_two - (pos_one + 1)).toFloat();

                    pos_one = pos_two;
                    pos_two = line.indexOf(")", pos_one);
                    qreal y = 0;
                    if ((pos_one != -1) && (pos_two != -1))
                        y = line.mid(pos_one + 1, pos_two - (pos_one + 1)).toFloat();

                    createSimpleNode(QPointF(x,y), node_name);
                } else {
                    //Creating Edges


                    auto list = line.split(" ");
                    if (first_read) {
                        connection_arr.resize(list.count());
                        for (int i = 0; i < connection_arr.count(); i++)
                            connection_arr[i].resize(list.count());
                        first_read = false;
                    }


                    if (list.count() == nodes.count()){
                        for (int dest = 0; dest < list.count(); dest++){
                            connection_arr[nodes_source_id][dest] = list[dest].toInt();
                        }
                        nodes_source_id++;
                    }
                }
                //qDebug() << line;
            }
            for (int source = 0; source < connection_arr.count(); source++){
                for (int dest = 0; dest < connection_arr.count(); dest++){
                    if (connection_arr[source][dest] > 0)
                        if ((connection_arr[dest][source] == connection_arr[source][dest]) && (dest != source)){
                            connection_arr[dest][source] = 0;
                            createSimpleEdge(source, dest, connection_arr[source][dest], false);
                        } else {
                            createSimpleEdge(source, dest, connection_arr[source][dest], true);
                        }
                }
            }
            emit_update_adjacency_matrix(getSimpleGraph());
            file.close();
        }
    }

}

void FormGraph::ReadIncidenceMatrix()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open Incidence"), QDir::homePath(), "IM File (*.im)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QFile::ReadOnly | QFile::Text))
            QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
        else {
            ClearAll();
            ui->DirectedEdgeBox->setChecked(true);
            is_edge_directed = true;
            int nodes_source_id = 0;
            QTextStream in(&file);
            QVector<QVector<int>> im_arr;
            while (!in.atEnd()){
                auto line = in.readLine();
                if (line[0] == "%") {
                    continue;
                }
                else if (line.contains("Vertex")) {
                    // Creating Nodes
                    auto pos_one = line.indexOf("{", 0);
                    auto pos_two = line.indexOf("(", pos_one);
                    QString node_name = "";
                    if ((pos_one != -1) && (pos_two != -1))
                        node_name = line.mid(pos_one + 1, pos_two - (pos_one + 1));

                    pos_one = pos_two;
                    pos_two = line.indexOf(",", pos_one);
                    qreal x = 0;
                    if ((pos_one != -1) && (pos_two != -1))
                        x = line.mid(pos_one + 1, pos_two - (pos_one + 1)).toFloat();

                    pos_one = pos_two;
                    pos_two = line.indexOf(")", pos_one);
                    qreal y = 0;
                    if ((pos_one != -1) && (pos_two != -1))
                        y = line.mid(pos_one + 1, pos_two - (pos_one + 1)).toFloat();
                    createSimpleNode(QPointF(x, y), node_name);
                } else {
                    //Creating Edges
                    auto list = line.split(" ");
                    im_arr.push_back({});
                    foreach (auto item, list)
                        im_arr.last().push_back(item.toInt());
                }

                //qDebug() << line;
            }
            for (int i_l = 0; i_l < im_arr[0].count(); i_l++){
                int node_source = -1;
                int node_dest = -1;
                bool directed = false;
                for (int i_c = 0; i_c < im_arr.count(); i_c++){
                    auto item = im_arr[i_c][i_l];
                    if (item != 0) {
                        if ((node_source == -1) && (item == 1))
                            node_source = i_c;
                        else if (item != 0)
                            node_dest = i_c;
                        if (item == -1)
                            directed = true;
                    }
                }

                createSimpleEdge(node_source, node_dest, 1, directed);
            }
            emit_update_adjacency_matrix(getSimpleGraph());
            file.close();
        }
    }

}

void FormGraph::SaveAdjacencyMatrix(bool save_all)
{
   auto path = QFileDialog::getSaveFileName(this, tr("Save Adjacency"), QDir::homePath(), "AM file (*.am)");
   if (!path.isEmpty()) {
       QFile file(path);
       if (!file.open(QFile::WriteOnly | QFile::Text)) {
           QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
       } else {
           QTextStream out(&file);
           auto save_nodes = getGraph();
           foreach (auto node, nodes){
               out << "Vertex{" << node->textContent() << "(" << node->pos().x() << "," << node->pos().y() << ")}" << endl;
           }
           //QVector<int> connection_arr;
           QVector<QVector<int>> connection_arr;
           connection_arr.resize(nodes.count());
           for (int i = 0; i < connection_arr.count(); i++)
               connection_arr[i].resize(nodes.count());

           foreach (auto edge, edges){
               if (!edge->is_directed){
                   connection_arr[edge->sourceNode()->id()][edge->destNode()->id()] = edge->mass;
                   connection_arr[edge->destNode()->id()][edge->sourceNode()->id()] = edge->mass;
               } else {
                   connection_arr[edge->sourceNode()->id()][edge->destNode()->id()] = edge->mass;
               }
           }

           for (int i = 0; i < connection_arr.count(); i++) {
               for (int j = 0; j < connection_arr.count(); j++){
                   if (j == connection_arr.count() - 1)
                       out << connection_arr[i][j] << endl;
                   else
                       out << connection_arr[i][j] << " ";
               }
           }

           is_chaged = false;
           file.close();
       }
   }

}

void FormGraph::SaveIncidenceMatrix()
{

    auto path = QFileDialog::getSaveFileName(this, tr("Save Incidence"), QDir::homePath(), "IM file (*.im)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
        } else {
            QTextStream out(&file);
            auto save_nodes = getGraph();
            foreach (auto node, nodes){
                out << "Vertex{" << node->textContent() << "(" << node->pos().x() << "," << node->pos().y() << ")}" << endl;
            }

            // Resize
            QVector<QVector<int>> connection_arr;
            connection_arr.resize(nodes.count());
            for (int i = 0; i < connection_arr.count(); i++)
                connection_arr[i].resize(edges.count());

            for (int i_node = 0; i_node < nodes.count(); i_node++){
                for (int i_edge = 0; i_edge < edges.count(); i_edge++) {
                    if ((edges[i_edge]->sourceNode()->id() == i_node) || (edges[i_edge]->destNode()->id() == i_node)) {
                        if (edges[i_edge]->is_directed){
                            if (edges[i_edge]->sourceNode()->id() == i_node)
                                connection_arr[i_node][i_edge] = 1;
                            else
                                connection_arr[i_node][i_edge] = -1;
                        } else {
                            connection_arr[i_node][i_edge] = 1;
                        }
                    }
                }
            }

            foreach (auto i_node, connection_arr)
                for (int i_edge = 0; i_edge < i_node.count(); i_edge++){
                    if (i_edge == i_node.count() - 1)
                        out << i_node[i_edge] << "\n";
                    else
                        out << i_node[i_edge] << " ";
                }

            is_chaged = false;
            file.close();
        }
    }
}

void FormGraph::SaveImage()
{
    auto path = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::homePath(), "png file (*.png)");
    if (!path.isEmpty()) {
        QPixmap map = ui->grafViewScene->grab(ui->grafViewScene->rect());
        map.save(path);
    }
}

void FormGraph::SaveWay()
{
    if (tmp_way.count()) {
        auto path = QFileDialog::getSaveFileName(this, tr("Save Way"), QDir::homePath(), "Way file (*.way)");
        if (!path.isEmpty()) {
            QFile file(path);
            if (!file.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
            } else {
                QTextStream out(&file);
                foreach (auto item, tmp_way){
                    out << "{" << item.First_node << ", " << item.Second_node << ", (" << item.Edge_id << ")}";
                }
                file.close();
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Сохраняемый путь пустой");
    }
}

void FormGraph::SaveDijkstraProp(QVector<int> weight, QVector<int> degrees, int indicator, int rad, int diam)
{
    auto path = QFileDialog::getSaveFileName(this, tr("Save Prop"), QDir::homePath(), "DktProp file (*.dktp)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, "File Error", "file not open", QMessageBox::Ok);
        } else {
            QTextStream out(&file);
            switch (indicator) {
            case 0:
                out << "Not connected" << endl;
                break;
            case 1:
                out << "Connected" << endl;
                break;
            case 2:
                out << "Weakly connected" << endl;
                break;
            case 3:
                out << "Strongly connected" << endl;
                break;
            }
            if ((indicator == 1) || (indicator == 3)){
                out << "Radius: " << rad << endl;
                out << "Diameter: " << diam << endl;
            }
            out << "Degrees: ";
            for (int i = 0; i < degrees.count(); i++)
                out << "(" << i << " - " << degrees[i] << ") ";
            out << endl;
            out << "Weight: ";
            for (int i = 0; i < weight.count(); i++)
                out << "(" << i << " - " << weight[i] << ") ";
            out << endl;

            file.close();
        }
    }
}

void FormGraph::MouseGraphPressed()
{
    qDebug() << "Pressed";
    //TODO more safe
    if (ui->grafViewScene->scene()->selectedItems().size() == 1){
        auto item = ui->grafViewScene->scene()->selectedItems().at(0);
        if (Node* n = dynamic_cast<Node*>(item)){
            old_pos_save.first = n->pos();
            old_pos_save.second = n->id();
        }
    }
}

void FormGraph::MouseGraphReleased()
{
    if (ui->grafViewScene->scene()->selectedItems().size() == 1){
        auto item = ui->grafViewScene->scene()->selectedItems().at(0);
        if (Node* n = dynamic_cast<Node*>(item)){
            if (n->pos() != old_pos_save.first){
                auto act = fillNodeMove(n, old_pos_save.first);
                PushAction(FormAction(act));
            }
        }
    }



}

void FormGraph::OnCellChaged(int source, int dest, QString mass)
{
    qDebug() << "cell changed";
    if (isHex(mass)){
        auto mass_int = mass.toInt();
        if (mass_int > 0) {
            bool isfind = false;
            int iter;
            for (iter = 0; iter < nodes[source]->edges().count(); iter++){
                if ((nodes[source]->edges()[iter]->destNode()->id() == dest) && (nodes[source]->edges()[iter]->sourceNode()->id() == source)){
                    isfind = true;
                    break;
                } else if ((nodes[source]->edges()[iter]->destNode()->id() == source) && (nodes[source]->edges()[iter]->sourceNode()->id() == dest) &&
                           (!nodes[source]->edges()[iter]->is_directed)) {
                    isfind = true;
                    break;
                }
            }
            if (isfind) {
                nodes[source]->edges()[iter]->setTextContent(mass);
            } else {
                createSimpleEdge(source, dest, mass_int, true);
            }
        } else {
            bool isfind = false;
            int iter;
            for (iter = 0; iter < nodes[source]->edges().count(); iter++){
                if ((nodes[source]->edges()[iter]->destNode()->id() == dest) && (nodes[source]->edges()[iter]->sourceNode()->id() == source)){
                    isfind = true;
                    break;
                }
            }
            if (isfind) {
                RemoveEdgeSimple(nodes[source]->edges()[iter]);
            }
        }
    }

}

void FormGraph::OnCellClickedDkt(QVector<KEdge> way)
{
    qDebug() << "kuk";
    showWay(way);
}

FormGraph *FormGraph::openGraph(QString fileName, bool jsonFormat) {
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open graph file.");
        return nullptr;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(jsonFormat ?
                              QJsonDocument::fromJson(saveData)
                            : QJsonDocument::fromBinaryData(saveData));
    const QJsonObject json = loadDoc.object();              // тут всё
    FormGraph *g = new FormGraph;
    g->CreateAutomat(Automata::Universal::readFromJson(json));      // Automat
    g->ui->grafViewScene->readFromJson(json);                       // scene
    if (missKeys(json, QStringList { "nodes", "edges" })) {
        delete g;
        return nullptr;
    }
    QJsonArray jsonNodes = json["nodes"].toArray();
    std::for_each(jsonNodes.begin(), jsonNodes.end(), [&] (QJsonValue j) {
        Node *n = new Node(g->ui->grafViewScene);
        n->readFromJson(j.toObject());
        g->nodes.append(n);
    });
    QJsonArray jsonEdges = json["edges"].toArray();
    std::for_each(jsonEdges.begin(), jsonEdges.end(), [&] (QJsonValue j) {
        EdgeParent *e = EdgeParent::create(j.toObject(), g->ui->grafViewScene);
        g->edges.append(e);
    });
    g->ui->btnConnectNode->setEnabled(false);
    g->ui->btnDelete->setEnabled(false);
    g->ui->btnCheck->setEnabled(true);
    return g;
}

QVector<QVector<QVector<int>>> FormGraph::getGraph()
{
    QVector<QVector<QVector<int>>> ret_value;
    foreach(auto node_one, nodes){
        ret_value.push_back(QVector<QVector<int>>());
        foreach(auto node_two, nodes){
            ret_value.last().push_back(QVector<int>());
            foreach(auto edge, node_one->edges()){
                if (isSameNodesInEdge(edge, node_one, node_two)) {
                    if (edge->is_directed){
                        if (node_one == edge->sourceNode())
                            ret_value.last().last().push_back(edge->mass);
                        else
                            ret_value.last().last().push_back(-edge->mass);
                    } else {
                        ret_value.last().last().push_back(edge->mass);
                    }
                }
            }
        }
    }
    return ret_value;
}

QVector<QVector<int> > FormGraph::getSimpleGraph()
{
    QVector<QVector<int>> connection_arr;
    connection_arr.resize(nodes.count());
    for (int i_node = 0; i_node < nodes.count(); i_node++)
        connection_arr[i_node].resize(nodes.count());

    for (int i_node = 0; i_node < nodes.count(); i_node++)
        for (int i_edge = 0; i_edge < nodes[i_node]->edges().count(); i_edge++){
            if (nodes[i_node]->edges()[i_edge]->is_directed) {
                if (nodes[i_node]->edges()[i_edge]->sourceNode()->id() == nodes[i_node]->id()){
                    connection_arr[i_node][nodes[i_node]->edges()[i_edge]->destNode()->id()] = nodes[i_node]->edges()[i_edge]->mass;
                }
            } else {
                if (nodes[i_node]->edges()[i_edge]->sourceNode()->id() == nodes[i_node]->id())
                    connection_arr[i_node][nodes[i_node]->edges()[i_edge]->destNode()->id()] = nodes[i_node]->edges()[i_edge]->mass;
                else
                    connection_arr[nodes[i_node]->edges()[i_edge]->destNode()->id()][nodes[i_node]->edges()[i_edge]->sourceNode()->id()] =
                            nodes[i_node]->edges()[i_edge]->mass;
            }
        }

    return connection_arr;
}

void FormGraph::ShowResultDialog(QString data)
{
    QMessageBox message;
    message.setText(data);
    message.setWindowTitle("Warning");
    message.setStandardButtons(QMessageBox::Ok);
    message.exec();
}

bool FormGraph::isHex(const QString &str)
{
    QRegExp reg("[0-9a-hA-H]+");
    return reg.indexIn(str) != -1;
}

void FormGraph::ClearAll()
{
    connFlag = 0;
    actions.first.clear();
    actions.second = -1;
    nodes.clear();
    edges.clear();
    ui->grafViewScene->scene()->clear();
    ui->grafViewScene->scene()->update();
    //removeItem(nodes)
    //nodes.removeFirst();
    //foreach (auto node, nodes)
        //nodes.removeFirst();
    //foreach (auto edge, edges)
       // edges.removeFirst();
    //nodes.erase(nodes.begin(), nodes.end());
    //edges.clear();
    //nodes.removeFirst();
    //update();

}

void FormGraph::setDefColor()
{
    // TODO check color in item
    foreach (auto item, nodes) {
        item->set_color(def_color);
        item->update();
    }
    foreach (auto item, edges) {
        item->set_color(def_color);
        item->update();
    }
    tmp_way = {};
}

bool FormGraph::isEgdesInSameNodes(EdgeParent *one, EdgeParent *two)
{
    if (((one->sourceNode() == two->sourceNode()) && (one->destNode() == two->destNode())) ||
            ((one->sourceNode() == two->destNode()) && (one->destNode() == two->sourceNode())))
        return true;
    return false;
}

bool FormGraph::isSameNodesInEdge(EdgeParent *edge, Node *node_one, Node *node_two)
{
    if (((edge->sourceNode() == node_one) && (edge->destNode() == node_two)) ||
            ((edge->sourceNode() == node_two) && (edge->destNode() == node_one)))
        return true;
    return false;
}

bool FormGraph::removeItem(Node *node)
{
    try {
        if (node) {
            if ((nodes.count() > 1) && (node != nodes.last())){
                // TODO fix error!!
                int index = nodes.indexOf(node);
                nodes.last()->SetId(node->id());
                nodes.swap(index, nodes.count() - 1);
                nodes.removeAll(nodes.last());
                nodes[index]->update();
            } else {
                nodes.removeAll(node);
            }
            return true;
        } else {
            return false;
        }
    } catch (...) {
        return false;
    }
}

bool FormGraph::removeItem(EdgeParent *edge)
{
    //May have some bugs ))
    try {
        if (edge) {
            int max_id_count = edge->id();
            int count_find = -1;
            for (int i = 0; i < edge->sourceNode()->edges().count(); i++) {
            //for (int i = 0; i < edges.count(); i++) {
                if (isEgdesInSameNodes(edge, edge->sourceNode()->edges()[i])) {
                    if (edge->id() != edge->sourceNode()->edges()[i]->id()) {
                        if (max_id_count < edge->sourceNode()->edges()[i]->id()) {
                            max_id_count = edge->sourceNode()->edges()[i]->id();
                            count_find = i;
                        }
                    }
                }
            }
            if (count_find >= 0) {
                edge->sourceNode()->edges()[count_find]->SetId(edge->id());
                edge->sourceNode()->edges()[count_find]->update();
                edges.removeAll(edge);
                return true;
            } else {
                edges.removeAll(edge);
                return true;
            }
        }
    } catch (...) {
        return false;
    }
}

Node* FormGraph::createSimpleNode(QPointF point, QString name)
{
    Node* node;
    if (name != "")
        node = new Node(ui->grafViewScene, name, is_show_id);
    else
        node = new Node(ui->grafViewScene, nullptr, is_show_id);
    node->setPos(point.x(), point.y());
    nodes.append(node);

    return node;
}

EdgeParent* FormGraph::createSimpleEdge(Node *source, Node *dest)
{
    if (connFlag == 0){
        EdgeParent *edge;
        if (source == dest)
            edge = new EdgeCircle(source, nullptr, is_show_id, is_edge_directed);
        else
            edge = new Edge(source, dest, nullptr, is_show_id, is_edge_directed);
        edges.append(edge);
        return edge;
    } else {
        qDebug() << "bad connection";
    }

    return nullptr;
}

EdgeParent *FormGraph::createSimpleEdge(int source, int dest, int mass, bool directed)
{
    if (connFlag == 0) {
            if ((source < nodes.count()) && (dest < nodes.count())) {
            EdgeParent *edge;
            if (source == dest){
                edge = new EdgeCircle(nodes[source], nullptr, is_show_id, directed);
                edge->setTextContent(QString::number(mass));
            } else {
                edge = new Edge(nodes[source], nodes[dest], nullptr, is_show_id, directed);
                edge->setTextContent(QString::number(mass));
            }
            edges.append(edge);
            return edge;
        }
    } else {
        qDebug() << "bad connection";
    }
    return nullptr;
}

void FormGraph::showWay(QVector<KEdge> way)
{
    if (is_color_changed)
        setDefColor();
    foreach(auto item, way){
        //auto it = qFind(nodes.begin(), nodes.end(), [item](const Node& node){return node.id() == item.First_node;});
        auto it_first = std::find_if(nodes.begin(), nodes.end(), [item](Node* node){return node->id() == item.First_node;});
        if (it_first != nodes.end()){
            auto it_second = std::find_if(nodes.begin(), nodes.end(), [item](Node* node){return node->id() == item.Second_node;});
            if (it_second != nodes.end()){
                foreach (auto edge, (*it_first)->edges())
                    if (isSameNodesInEdge(edge, *it_first, *it_second)) {
                        qDebug() << edge->mass << " == " << item.Edge_id;
                        if (edge->id() == item.Edge_id) {
                            is_color_changed = true;
                            (*it_first)->set_color(QColor(200, 20, 78), &is_color_changed);
                            (*it_first)->update();
                            (*it_second)->set_color(QColor(200, 20, 78), &is_color_changed);
                            (*it_second)->update();
                            edge->set_color(QColor(200, 20, 78), &is_color_changed);
                            edge->update();
                            break;
                        }
                    }

            }


            //(*it)->set_color(QColor(200, 20, 78));
            //(*it)->update();
        }
    }
}

ItemInfoEdge FormGraph::fillEdgeAction(EdgeParent *edge_source)
{
    ItemInfoEdge action_edge;
    action_edge.id = edge_source->id();
    action_edge.dest = edge_source->destNode()->id();
    action_edge.soure = edge_source->sourceNode()->id();
    action_edge.mass = edge_source->mass;
    action_edge.is_directed = edge_source->is_directed;
    return action_edge;
}

ItemInfoNode FormGraph::fillNodeAction(Node *node_source)
{
    ItemInfoNode action_node;
    action_node.id = node_source->id();
    auto pos = node_source->pos();
    //qDebug() << pos;
    action_node.pos = pos;
    action_node.name = node_source->name_save;
    foreach(auto edge, node_source->edges()){
        ItemInfoEdge action_edge;
        action_edge.id = edge->id();
        action_edge.dest = edge->destNode()->id();
        action_edge.soure = edge->sourceNode()->id();
        action_edge.mass = edge->mass;
        action_node.edge_data.push_back(action_edge);
    }
    return action_node;
}

ItemEdgeMass FormGraph::fillEdgeMass(EdgeParent *edge, int newm)
{
    ItemEdgeMass action_mass;
    action_mass.id = edge->id();
    action_mass.soure = edge->sourceNode()->id();
    action_mass.dest = edge->destNode()->id();
    action_mass.new_mass = newm;
    action_mass.old_mass = edge->mass;
    return action_mass;
}

ItemNodeMove FormGraph::fillNodeMove(Node *node, QPointF oldp)
{
    ItemNodeMove actions_move;
    actions_move.id = node->id();
    actions_move.new_pos = node->pos();
    actions_move.old_pos = oldp;
    return actions_move;
}

// Функция возврата делат ставку на то что все узлы и пути лежат в хранилище в нужном порядке
// Не допускайте их перетасовки!
void FormGraph::BackAction(bool Undo_Redo)
{
    if (!actions.first.empty()) {
        if (Undo_Redo && (actions.second >= 0)) { //Back
            auto act = actions.first[actions.second];
            if (act.EraseAdd_or_MoveMass) {
                if (act.erase_or_add){
                    // Back
                    if (act.node_or_edge) {
                        //Back erased node
                        BackErasedNode(act);
                    } else {
                        // Back erased edge
                        BackErasedEdge(act);
                    }

                } else {
                // Erase
                    if (act.node_or_edge){
                        //Erase added node
                        EraseAddedNode(act);
                    } else {
                        //Erase added edge
                        EraseAddedEdge(act);
                    }

                }
            } else { //Another Variant
                if (act.mass_or_move) {
                    // Move Back
                    RenameMassEdge(act, true);
                } else {
                    // Mass Back
                    MoveNode(act, true);
                }
            }
            //if (actions.second > 0)
            actions.second--;


            //#####################

        } else if (!Undo_Redo) { //Forward
            if (actions.second < actions.first.count() - 1) {
                auto act = actions.first[actions.second + 1];
                if (act.EraseAdd_or_MoveMass) {
                    if (act.erase_or_add){
                        // Back
                        if (act.node_or_edge){
                            //Erase added node
                            EraseAddedNode(act);
                        } else {
                            //Erase added edge
                            EraseAddedEdge(act);
                        }

                    } else {
                        if (act.node_or_edge) {
                            //Back erased node
                            BackErasedNode(act);
                        } else {
                            // Back erased edge
                            BackErasedEdge(act);
                        }
                        // Erase

                    }
                } else {
                    if (act.mass_or_move){
                        RenameMassEdge(act, false);
                    } else {
                        MoveNode(act, false);
                    }
                }
                if (actions.second < actions.first.count() - 1)
                    actions.second++;
            }

        }
        emit_update_adjacency_matrix(getSimpleGraph());
        is_chaged = true;
    }
    //actions.pop_back();
    qDebug() << "act: " << actions.second;
}

void FormGraph::RemoveNodeSimple(Node* node)
{
    foreach (auto edge, node->edges())
        edges.removeAll(edge);
    if (!removeItem(node)) return;
    ui->grafViewScene->scene()->removeItem(node);
    delete node;
}

void FormGraph::RemoveEdgeSimple(EdgeParent *edge)
{
    if (!removeItem(edge)) return;
    ui->grafViewScene->scene()->removeItem(edge);
    delete edge;
}

void FormGraph::PushAction(FormAction act)
{
    if (actions.second < actions.first.count() - 1) {
        int curr_count = actions.first.count();
        int erase_iter = actions.second + 1;
        for (int i = actions.second + 1; i < curr_count; i++)
            actions.first.removeAt(erase_iter);
    }
    actions.first.push_back(act);
    actions.second = actions.first.count() - 1;
    //if (!act.mass_or_move)
    auto simple = getSimpleGraph();
    emit_update_adjacency_matrix(simple);
    if (dkt_matrix->isVisible())
        dkt_matrix->close();
    is_chaged = true;
    if (is_color_changed)
        setDefColor();
    qDebug() << "act: " << actions.second;
    //actions.first.erase();
}

void FormGraph::BackErasedNode(FormAction act)
{
    if (act.item_node.id < nodes.count()){
        auto new_node = new Node(ui->grafViewScene, nullptr, is_show_id);
        nodes[act.item_node.id]->SetId(new_node->id());
        new_node->SetId(act.item_node.id);
        new_node->setTextContent(act.item_node.name);
        new_node->setPos(act.item_node.pos);
        nodes.push_back(new_node);
        nodes.swap(act.item_node.id, nodes.count() - 1);
        nodes.last()->update();
        nodes[act.item_node.id]->update();

        // Creating edges
        foreach (auto edge, act.item_node.edge_data){
            createSimpleEdge(edge.soure, edge.dest, edge.mass, edge.is_directed);
        }
    } else if (act.item_node.id == nodes.count()) {
        auto new_node = new Node(ui->grafViewScene, nullptr, is_show_id);
        new_node->SetId(act.item_node.id);
        new_node->setTextContent(act.item_node.name);
        new_node->setPos(act.item_node.pos);
        nodes.push_back(new_node);
        nodes.last()->update();

        foreach (auto edge, act.item_node.edge_data){
            createSimpleEdge(edge.soure, edge.dest, edge.mass, edge.is_directed);
        }
    }
}

void FormGraph::BackErasedEdge(FormAction act)
{
    bool find_flag = false;
    int iter = 0;
    int max_id = -1;
    for (int i = 0; i < nodes[act.item_edge.soure]->edges().count(); i++){
        if (((nodes[act.item_edge.soure]->edges()[i]->destNode()->id() == act.item_edge.dest) && (nodes[act.item_edge.soure]->edges()[i]->sourceNode()->id() == act.item_edge.soure)) ||
                ((nodes[act.item_edge.soure]->edges()[i]->sourceNode()->id() == act.item_edge.dest) && (nodes[act.item_edge.soure]->edges()[i]->destNode()->id() == act.item_edge.soure))) {
            if (nodes[act.item_edge.soure]->edges()[i]->id() > max_id) {
                max_id = nodes[act.item_edge.soure]->edges()[i]->id();
                iter = i;
                find_flag = true;
            }
        }
    }

    if ((find_flag) && (max_id >= act.item_edge.id)) {
        auto new_edge = createSimpleEdge(act.item_edge.soure, act.item_edge.dest, act.item_edge.mass, act.item_edge.is_directed);
        int id_save = new_edge->id();
        new_edge->SetId(act.item_edge.id);
        nodes[act.item_edge.soure]->edges()[iter]->SetId(id_save);
        new_edge->update();
        nodes[act.item_edge.soure]->edges()[iter]->update();
    } else {
        createSimpleEdge(act.item_edge.soure, act.item_edge.dest, act.item_edge.mass, act.item_edge.is_directed);
    }
}

void FormGraph::EraseAddedNode(FormAction act)
{
    RemoveNodeSimple(nodes[act.item_node.id]);
}

void FormGraph::EraseAddedEdge(FormAction act)
{
    bool find_flag = false;
    int i = 0;
    for (; i < nodes[act.item_edge.soure]->edges().count(); i++){
        if ((nodes[act.item_edge.soure]->edges()[i]->destNode()->id() == act.item_edge.dest) && (nodes[act.item_edge.soure]->edges()[i]->sourceNode()->id() == act.item_edge.soure) &&
                (nodes[act.item_edge.soure]->edges()[i]->id() == act.item_edge.id)){
            find_flag = true;
            break;
        }
    }
    if (find_flag)
        RemoveEdgeSimple(nodes[act.item_edge.soure]->edges()[i]);
    //else
}

void FormGraph::MoveNode(FormAction act, bool Back_Forward)
{
  //  qDebug() << "move";
    if (Back_Forward)
        nodes[act.item_move.id]->setPos(act.item_move.old_pos);
    else
        nodes[act.item_move.id]->setPos(act.item_move.new_pos);
}

void FormGraph::RenameMassEdge(FormAction act, bool Back_Forward)
{
    int iter;
    bool isfind;
    for (iter = 0; iter < nodes[act.item_mass.soure]->edges().count(); iter++){
        if ((nodes[act.item_mass.soure]->edges()[iter]->destNode()->id() == act.item_mass.dest) &&
                (nodes[act.item_mass.soure]->edges()[iter]->id() == act.item_mass.id)) {
            isfind = true;
            break;
        }
    }
    if (isfind) {
        if (Back_Forward){
            nodes[act.item_mass.soure]->edges()[iter]->setTextContent(QString::number(act.item_mass.old_mass));

        } else {
            nodes[act.item_mass.soure]->edges()[iter]->setTextContent(QString::number(act.item_mass.new_mass));
        }
    }
}

void FormGraph::Algo_Checked(int variant)
{
    int source = 0;
    int dest = 0;
    if (variant < 4) {
        if (isHex(ui->From->text()) && isHex(ui->To->text())) {
            source = ui->From->text().toInt();
            dest = ui->To->text().toInt();
            if ((source >= 0) && (dest >= 0) && (source < nodes.count()) && (dest < nodes.count())) {
                if (source != dest){
                    timer.start();
                    switch (variant) {
                    case 0:
                        bfs = new breadth_for_search(this);
                        bfs->setting(getGraph(), source, dest);
                        connect(bfs, &breadth_for_search::result, this, &FormGraph::get_way_bfs);
                        bfs->start();
                        qDebug() << 0;
                        break;
                    case 1:
                        bfis = new Best_First_seatch(this);
                        bfis->setting(getGraph(), source, dest);
                        connect(bfis, &Best_First_seatch::result, this, &FormGraph::get_way_bfis);
                        bfis->start();
                        qDebug() << 1;

                        break;
                    case 2:
                        dkt = new Dijkstra_algoritm(this);
                        dkt->setting(getGraph(), 0, source, dest);
                        connect(dkt, &Dijkstra_algoritm::result_only_one, this, &FormGraph::get_way_dkt);
                        dkt->start();
                        qDebug() << 2;
                        break;

                    case 3:
                        as = new A_star(this);
                        QVector<QPair<qreal, qreal>> coords;
                        foreach (auto node, nodes){
                            coords.push_back({node->pos().x(), node->pos().y()});
                        }
                        as->setting(getGraph(), source, dest, coords);
                        connect(as, &A_star::result, this, &FormGraph::get_way_as);
                        as->start();
                        break;
                    }
                } else {
                    QMessageBox::warning(this, "Алгоритм", "Выбрана одни и та же вершина");
                }
            } else {
                QMessageBox::warning(this, "Ошибка", "Некорректный ввод");
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Некорректный ввод");
        }
    } else if (variant < 7) {
        // Потавить ограничитель на вершины
        if (nodes.count() > 0) {
            switch (variant) {
            case 4:
                dkt = new Dijkstra_algoritm(this);
                dkt->setting(getGraph(), 2, 0, 0);
                connect(dkt, &Dijkstra_algoritm::Weihts, this, &FormGraph::get_weight_dijkstra);
                dkt->start();
                //qDebug() << 2;
                break;
            case 5:
                pn = new poln(this);
                pn->setting(getGraph());
                connect(pn, &poln::result, this, &FormGraph::get_graph_poln);
                pn->start();
                break;
            case 6:
                ks = new krask(this);
                ks->setting(getGraph());
                connect(ks, &krask::result, this, &FormGraph::get_color);
                ks->start();
                break;
            }
        } else
          QMessageBox::warning(this, "Ошибка", "Передан пустой граф");
    } else {
        switch (variant) {
        case 7:
            rn = new random_node(this);
            pd = new QProgressDialog("Random progress", "close", 0, 100, this);
            connect(rn, &random_node::result, this, &FormGraph::get_random_graph);
            connect(rn, &random_node::progress, pd, &QProgressDialog::setValue);
            //progress->show();
            pd->show();
            setEnabled(false);
            rn->start();
            break;
        case 8:
            auto dlgInput = new DlgInput(this);
            dlgInput->lTipInput->setText("Введите текст");
            dlgInput->is_recovery = true;
            switch (dlgInput->exec()) {
            case QDialog::Accepted: {
                auto text = dlgInput->eInput->text();
                auto list = text.split(' ');
                QVector<int> numbers;
                foreach (auto item, list)
                    if (item != "")
                        numbers.push_back(item.toInt());
                vst = new vostonav(this);
                vst->setting(numbers);
                connect(vst, &vostonav::result, this, &FormGraph::get_recovery);
                vst->start();
                break;
            }
            case QDialog::Rejected: {
                break;
            }
            }
            dlgInput->close();
            break;
        }
    }


}

void FormGraph::on_waysButton_clicked()
{
    // TODO change this
    auto graph = getGraph();
    dkt = new Dijkstra_algoritm(this);
    dkt->setting(getGraph(), 1, -1, -1);
    connect(dkt, &Dijkstra_algoritm::result, this, &FormGraph::get_ways_dkt_matrix);
    dkt->start();

}

void FormGraph::on_testButton_clicked()
{
    //for (int i = -50000; i < 50000; i += 100){
    //    for (int j = -50000; j < 500000; j += 100){
    //        createSimpleNode({i, j});
    //    }
    //}
   // TODO change call to thread;

    Algo_Checked(ui->variantBox->currentIndex());

}

void FormGraph::get_way_bfis(QVector<KEdge> way)
{
    if (is_color_changed)
        setDefColor();
    qDebug() << "Thread way done bfis";
    disconnect(bfis, &Best_First_seatch::result, this, &FormGraph::get_way_bfis);
    bfis->exit();
    delete bfis;
    if (way.count()) {
        showWay(way);
        tmp_way = way;

        int msecs_duration = timer.elapsed();
        QMessageBox::information(this, "Время алгоритма", QString::number(msecs_duration));
    }
    else
        QMessageBox::warning(this, "Алгоритм", "Маршрут не существует");
}


void FormGraph::get_way_bfs(QVector<KEdge> way)
{
    if (is_color_changed)
        setDefColor();
    qDebug() << "Thread way done bfs";
    disconnect(bfs, &breadth_for_search::result, this, &FormGraph::get_way_bfs);
    bfs->exit();
    delete bfs;
    if (way.count()) {
        showWay(way);
        tmp_way = way;

        int msecs_duration = timer.elapsed();
        QMessageBox::information(this, "Время алгоритма", QString::number(msecs_duration));
    }
    else
        QMessageBox::warning(this, "Алгоритм", "Маршрут не существует");

}

void FormGraph::get_way_dkt(QVector<KEdge> way, int length)
{
    if (is_color_changed)
        setDefColor();
    qDebug() << "Thread way done bfs";
    disconnect(dkt, &Dijkstra_algoritm::result_only_one, this, &FormGraph::get_way_dkt);
    dkt->exit();
    delete dkt;
    if (way.count()) {
        showWay(way);
        tmp_way = way;

        int msecs_duration = timer.elapsed();
        QMessageBox::information(this, "Время алгоритма", QString::number(msecs_duration));
    }
    else
        QMessageBox::warning(this, "Алгоритм", "Маршрут не существует");

}

void FormGraph::get_way_as(QVector<KEdge> way, int size)
{
    if (is_color_changed)
        setDefColor();
    disconnect(as, &A_star::result, this, &FormGraph::get_way_as);
    as->exit();
    delete as;
    if (way.count()) {
        showWay(way);
        tmp_way = way;

        int msecs_duration = timer.elapsed();
        QMessageBox::information(this, "Время алгоритма", QString::number(msecs_duration));
    }
    else
        QMessageBox::warning(this, "Алгоритм", "Маршрут не существует");
}

void FormGraph::get_ways_dkt_matrix(QVector<QVector<QVector<KEdge>>> ways, QVector<QVector<int>> ways_size)
{
    if (is_color_changed)
        setDefColor();
    qDebug() << "Thread way done dkt";
    disconnect(dkt, &Dijkstra_algoritm::result, this, &FormGraph::get_ways_dkt_matrix);
    dkt->exit();
    delete dkt;

    emit emit_update_matrix(ways, ways_size);
    dkt_matrix->show();
    emit_update_matrix(ways, ways_size);
}

void FormGraph::get_weight_dijkstra(QVector<int> weight, QVector<int> degrees, QVector<int> components, int bridges, int hinge, int indicator)
{
    // 0 - Не связный
    // 1 - Сильно связный
    // 2 - Слабо связный
    // Если нет ориентированнх ребер, то 1, 2 - Связный
    // Если Сильно связный или связный, то смотрю веса всех вершин, мин - радиус, макс, диаметр
    // мин число в weight - мин радиус, диаметр - макс число в первом векторе.

    if (is_color_changed)
        setDefColor();
    qDebug() << "Thread way done dkt";
    disconnect(dkt, &Dijkstra_algoritm::Weihts, this, &FormGraph::get_weight_dijkstra);
    dkt->exit();
    delete dkt;

    bool directed_flag = false;
    foreach (auto edge, edges){
        if (edge->is_directed) {
            directed_flag = true;
            break;
        }
    }

    qDebug() << indicator;
    QString out_str;
    int variant = -1;
    int rad = 0;
    int diam = 0;
    if (directed_flag) {
        // Ориентированне
        if (indicator == 1) {
            // Вывожу все
            // Связнный
            diam = *std::max_element(weight.begin(), weight.end());
            rad = *std::min_element(weight.begin(), weight.end());
            QString degrees_str;
            for (int item = 0; item < degrees.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(degrees[item]);
                degrees_str.push_back(push_str);
            }
            QString weight_str;
            for (int item = 0; item < weight.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(weight[item]);
                weight_str.push_back(push_str);
            }
            out_str = QString("Сильно связный\nРадиус - %1\nДиаметр - %2\nКоличество шарниров - %3\nКоличество мостов - %4\nСтепени - %5\nВеса - %6").arg(rad).arg(diam).arg(hinge).arg(bridges).arg(degrees_str).arg(weight_str);
            variant = 3;
        } else if (indicator == 2){

            QString degrees_str;
            for (int item = 0; item < degrees.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(degrees[item]);
                degrees_str.push_back(push_str);
            }
            out_str = QString("Слабо связный\nКоличество шарниров - %1\nКоличество мостов - %2\nСтепени - %3").arg(hinge).arg(bridges).arg(degrees_str);
            variant = 2;
        } else {
            QString degrees_str;
            for (int item = 0; item < degrees.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(degrees[item]);
                degrees_str.push_back(push_str);
            }
            auto max = std::max_element(components.begin(), components.end());
            out_str = QString("Не связный\nКоличество шарниров - %1\nКоличество мостов - %2\nСтепени - %3\nКомпонент - %4").arg(hinge).arg(bridges).arg(degrees_str).arg(*max);
            variant = 0;
        }
    } else {
        // Не ориентировааные
        if (indicator == 1) {
            // Вывожу все
            // Связнный
            diam = *std::max_element(weight.begin(), weight.end());
            rad = *std::min_element(weight.begin(), weight.end());
            QString degrees_str;
            for (int item = 0; item < degrees.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(degrees[item]);
                degrees_str.push_back(push_str);
            }
            QString weight_str;
            for (int item = 0; item < weight.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(weight[item]);
                weight_str.push_back(push_str);
            }

            out_str = QString("Связный\nРадиус - %1\nДиаметр - %2\nКоличество шарниров - %3\nКоличество мостов - %4\nСтепени - %5\nВеса - %6").arg(rad).arg(diam).arg(hinge).arg(bridges).arg(degrees_str).arg(weight_str);
            variant = 1;
        } else {
            QString degrees_str;
            for (int item = 0; item < degrees.count(); item++) {
                QString push_str = QString("(%1 - %2) ").arg(item).arg(degrees[item]);
                degrees_str.push_back(push_str);
            }
            auto max = std::max_element(components.begin(), components.end());
            out_str = QString("Не связный\nКоличество шарниров - %1\nКоличество мостов - %2\nСтепени - %3\nКомпонент - %4").arg(hinge).arg(bridges).arg(degrees_str).arg(*max);
            variant = 0;
        }
    }
    QMessageBox msb;
    msb.setText("Информация о графе");
    msb.setInformativeText(out_str);
    msb.setStandardButtons(QMessageBox::Ok | QMessageBox::Save);
    if (msb.exec() == QMessageBox::Save)
        SaveDijkstraProp(weight, degrees, variant, rad, diam);
}

void FormGraph::get_graph_poln(QVector<KEdge> poln_graph)
{
    if (is_color_changed)
        setDefColor();
    disconnect(pn, &poln::result, this, &FormGraph::get_graph_poln);
    pn->exit();
    delete pn;


    if (poln_graph.count() == 0)
        QMessageBox::warning(this, "Алгоритм", "Граф уже полный");
    else {
        edges.clear();
        QVector<ItemInfoNode> nodes_save;
        foreach (auto node, nodes) {
            auto info = fillNodeAction(node);
            nodes_save.push_back(info);
        }
        nodes.clear();
        ui->grafViewScene->scene()->clear();
        ui->grafViewScene->scene()->update();
        foreach (auto item, nodes_save)
            createSimpleNode(item.pos, item.name);


        foreach (auto item, poln_graph){
            createSimpleEdge(item.First_node, item.Second_node, 1, false);
        }
    }
}

void FormGraph::get_random_graph(QVector<QVector<QVector<int>>> graph, QVector<QPair<qreal, qreal>> coord)
{
    ClearAll();
    //ui->grafViewScene->scale(qreal(0.08), qreal(0.08));
    disconnect(rn, &random_node::result, this, &FormGraph::get_random_graph);
    disconnect(rn, &random_node::progress, pd, &QProgressDialog::setValue);
    rn->exit();
    delete rn;

    int value = 0;
    foreach (auto item, coord) {
        createSimpleNode({item.first, item.second});
        pd->setValue((value / coord.count()) * 100);
        value++;
        //qDebug() << item.first;
    }
    pd->setValue(100);
    for (auto iter_one = 0; iter_one < graph.count(); iter_one++){
        pd->setValue((iter_one / coord.count()) * 100);
        value++;
        for (auto iter_two = iter_one; iter_two < graph.count(); iter_two++){
            if (!graph[iter_one][iter_two].isEmpty())
                createSimpleEdge(iter_one, iter_two, graph[iter_one][iter_two][0], false);
        }
    }
    pd->setValue(100);
    pd->close();
    delete pd;
    setEnabled(true);

}

void FormGraph::get_color(QVector<int> color)
{
    if (is_color_changed)
        setDefColor();
    disconnect(ks, &krask::result, this, &FormGraph::get_color);
    ks->exit();
    delete ks;

    for (int iter = 0; iter < color.count(); iter++){
        nodes[iter]->setTextContent(QString::number(color[iter]));
    }
}

void FormGraph::get_recovery(QVector<QVector<QVector<int>>> graph, bool is_good, bool is_esk, bool is_perfet, QVector<KEdge> create_steps, QVector<QPair<int, int>> fundamental)
{
    ClearAll();
    disconnect(vst, &vostonav::result, this, &FormGraph::get_recovery);
    vst->exit();
    delete vst;

    qreal start = -graph.count() * 100;
    foreach (auto item, graph) {
        createSimpleNode({start, 0});
        start += 200;
        //qDebug() << item.first;
    }

    for (auto iter_one = 0; iter_one < graph.count(); iter_one++){
        for (auto iter_two = iter_one; iter_two < graph.count(); iter_two++){
            if (!graph[iter_one][iter_two].isEmpty())
                createSimpleEdge(iter_one, iter_two, graph[iter_one][iter_two][0], false);
        }
    }

    QString out_fund;
    if (is_esk) {
        for (int item = 0; item < fundamental.count(); item++) {
            QString push_str = QString("(%1 - {%2, %3}) ").arg(item).arg(fundamental[item].first).arg(fundamental[item].second);
            out_fund.push_back(push_str);
        }
    }
    QString out_steps;
    for (int item = 0; item < create_steps.count(); item++) {
        QString push_str = QString("(%1 - {%2, %3}) ").arg(create_steps[item].Edge_id).arg(create_steps[item].First_node).arg(create_steps[item].Second_node);
        out_steps.push_back(push_str);
    }

    QString out_str;
    if (is_esk)
        out_str = QString("Хороший: %1\nЭкстремальный: %2\nСовершенный: %3\nЭтапы создания: %4\nБаза: %5").arg(is_good).arg(is_esk).arg(is_perfet).arg(out_steps).arg(out_fund);
    else
        out_str = QString("Хороший: %1\nЭкстремальный: %2\nСовершенный: %3\nЭтапы создания: %4").arg(is_good).arg(is_esk).arg(is_perfet).arg(out_steps);

    QMessageBox msb;
    msb.setText("Информация о графе");
    msb.setInformativeText(out_str);
    msb.setStandardButtons(QMessageBox::Ok);
    msb.exec();
}

void FormGraph::on_ShowIdBox_clicked(bool checked)
{
    is_show_id = checked;
    foreach (auto node, nodes)
        node->show_id(is_show_id);
    foreach (auto edge, edges)
        edge->show_id(is_show_id);

}

void FormGraph::on_DirectedEdgeBox_clicked(bool checked)
{
    is_edge_directed = checked;
}

void FormGraph::on_MatrixButton_clicked()
{
    def_matrix->show();
}

void FormGraph::on_btnClear_clicked()
{
    ClearAll();
}

void FormGraph::on_TestSave_clicked()
{
    QMessageBox msb;
    //msb.information(this, "wad", "awd", QMessageBox::Ok, QMessageBox::Save);
    msb.setText("dwaaw");
    msb.setInformativeText("awdwadw");
    msb.setStandardButtons(QMessageBox::Ok | QMessageBox::Save);
  //  msb.setDefaultButton();
    int ret = msb.exec();
    qDebug() << ret;
}
