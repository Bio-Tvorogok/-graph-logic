#ifndef FORMACTION_H
#define FORMACTION_H

#include "nodeedgeparent.h"
#include "edge.h"
#include "node.h"

struct ItemInfoEdge{
    int soure;
    int dest;
    int id;
    int mass;
    bool is_directed;
};

struct ItemInfoNode{
public:
    QPointF pos;
    QString name;
    int id;
    QVector<ItemInfoEdge> edge_data;
};

struct ItemEdgeMass{
    int soure;
    int dest;
    int id;
    int new_mass;
    int old_mass;
};

struct ItemNodeMove{
    QPointF old_pos;
    QPointF new_pos;
    int id;
};

class FormAction
{
public:
    FormAction(ItemInfoEdge item_info, bool eoa);
    FormAction(ItemInfoNode item_info, bool eoa);
    FormAction(ItemEdgeMass item_mass);
    FormAction(ItemNodeMove item_move);

public:
    // true: erase, false: add
    bool EraseAdd_or_MoveMass;
    bool erase_or_add;
    bool mass_or_move;
    // true : node, false: edge
    bool node_or_edge;
    ItemInfoEdge item_edge;
    ItemInfoNode item_node;
    ItemEdgeMass item_mass;
    ItemNodeMove item_move;
};

#endif // FORMACTION_H
