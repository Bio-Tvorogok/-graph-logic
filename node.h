#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>
#include <QString>
#include "nodeedgeparent.h"
class GraphWidget;
class EdgeParent;
class Edge;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Node : public NodeEdgeParent
{
public:
    QPen _pen = QPen(Qt::black, 2);
    Node(GraphWidget *graphWidget, QString text = nullptr, bool is_show_id = false);
    virtual ~Node() override;

    void writeToJson(QJsonObject &json) const override;
    void readFromJson(const QJsonObject &json) override;
    void addEdge(EdgeParent *edge);
    QList<EdgeParent*> edges() const;
    enum { Type = NodeEdgeParent::Type + 1 };
    int type() const override { return Type; }
    enum { Radius = 40 };

    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    static int idStatic();
    void removeEdge(EdgeParent *edge);
protected:
    QList<EdgeParent *> edgeList;
    QPointF newPos;
    static int _idStatic;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif // NODE_H
