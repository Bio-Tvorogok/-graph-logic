#include "formaction.h"

FormAction::FormAction(ItemInfoNode item, bool eoa) : erase_or_add(eoa), item_node(item)
{
    node_or_edge = true;
    EraseAdd_or_MoveMass = true;
}

FormAction::FormAction(ItemInfoEdge item, bool eoa) : erase_or_add(eoa), item_edge(item)
{   
    node_or_edge = false;
    EraseAdd_or_MoveMass = true;
}

FormAction::FormAction(ItemEdgeMass item_mass) : item_mass(item_mass)
{
    mass_or_move = true;
    EraseAdd_or_MoveMass = false;
}

FormAction::FormAction(ItemNodeMove item_move) : item_move(item_move)
{
    mass_or_move = false;
    EraseAdd_or_MoveMass = false;
}




//if (erase_or_add) {
//    if (node_or_edge){
//
//
//        auto item_curr = static_cast<Node*>(item);
//        auto item_copy = new Node(nullptr, item_curr->textContent(), true);
//        item_copy->setPos({item_curr->x(), item_curr->y()});
//        //item_curr
//        foreach (auto i, item_curr->edges()) {
//            item_copy->edges().push_back(i);
//        }
//        item = item_copy;
//    } else {
//
//    }
//}
