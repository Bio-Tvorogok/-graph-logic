#include "nodeedgeparent.h"
#include <QDebug>
#include <QPainter>
class GraphWidget;

NodeEdgeParent::NodeEdgeParent(GraphWidget *graphWidget, int id, QString text, bool is_show_id)
    : _textContent(text), _id(id), id_show(is_show_id)
{
    graph = graphWidget;
    if (text.isEmpty()) {
        _textContent = QString("%1").arg(_id);
        name_save = _textContent;
    } else {
        _textContent = text;
        name_save = text;
    }
    show_id(id_show);
}

int NodeEdgeParent::id() const
{
    return _id;
}

void NodeEdgeParent::setTextContent(QString text) {
    if (id_show)
        _textContent = QString::number(_id);
    else
        _textContent = text;
    name_save = text;
    update();
}

void NodeEdgeParent::writeToJson(QJsonObject &json) const
{
    QJsonObject jsonIt;
    jsonIt["Type"] = type();
    jsonIt["id"] = _id;
    jsonIt["textContent"] = _textContent;
    json["Item"] = jsonIt;
}

void NodeEdgeParent::readFromJson(const QJsonObject &json)
{
    if (missKey(json, "Item")) {
        return;
    }
    QJsonObject jsonIt = json["Item"].toObject();
    if (missKeys(jsonIt, QStringList { "Type", "id", "textContent" })) {
        return;
    }
    _id = jsonIt["id"].toInt();  // check idStatic todo in derived
    _textContent = jsonIt["textContent"].toString();
}

void NodeEdgeParent::set_color(QColor color, bool* is_changed)
{
    if (is_changed != nullptr)
        *is_changed = true;
    _color = color;
}

void NodeEdgeParent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
//        scene()->clearSelection();
//        setSelected(true);
//        emit graph->editItem();
    }
    QGraphicsItem::mousePressEvent(event);
}

void NodeEdgeParent::show_id(bool is_show)
{
    if (is_show) {
        id_show = true;
        _textContent = QString::number(_id);
        update();
    } else{
        //qDebug() << name_save;
        id_show = false;
        _textContent = name_save;
        update();
    }
}

void NodeEdgeParent::SetId(int id)
{
    //_textContent = QString("%1").arg(id);
    _id = id;
    show_id(id_show);
}

void NodeEdgeParent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef DEBUG_NODE_EDGE
    QPen pen2(Qt::yellow, 0, Qt::DotLine);
    painter->setOpacity(0.4);
    painter->setPen(pen2);
    painter->drawRect(boundingRect());
    painter->setOpacity(1);
    QPen pen(Qt::red, 0, Qt::SolidLine);
    painter->strokePath(shape(), pen);
#else
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
#endif
}

QRectF NodeEdgeParent::enoughBoundingRect(QRectF rect) const
{
    const qreal enough = 50;
    rect.moveCenter(rect.center() - QPointF(enough, enough));
    rect.setSize(rect.size() + QSizeF(2 * enough, 2 * enough));
    return rect;
}
