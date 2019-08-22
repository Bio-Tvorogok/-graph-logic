#ifndef NODEEDGEPARENT_H
#define NODEEDGEPARENT_H
#include <QString>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "jsonfuncs.h"

//#define DEBUG_NODE_EDGE

class GraphWidget;

// абстрактный класс
class NodeEdgeParent : public QGraphicsItem
{
public:
    NodeEdgeParent(GraphWidget *graphWidget, int id, QString text = nullptr, bool is_show_id = false);
    enum { Type = UserType + 1 };
    int type() const override { return Type; }
    int id() const;
    virtual void setTextContent(QString text);
    virtual QString textContent() const { return _textContent; }
    virtual void writeToJson(QJsonObject &json) const;
    virtual void readFromJson(const QJsonObject &json);
    void set_color(QColor color, bool* is_changed = nullptr);
    void SetId(int id);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void show_id(bool);
    GraphWidget *graph;
    QString name_save;
protected:  
    bool id_show = false;
    QColor _color = Qt::black;
    QString _textContent;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    int _id;
    // область перерисовки с запасом
    QRectF enoughBoundingRect(QRectF rect) const;
};


#endif // NODEEDGEPARENT_H

