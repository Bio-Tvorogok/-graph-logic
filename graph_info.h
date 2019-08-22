#ifndef GRAPH_INFO_H
#define GRAPH_INFO_H

#include <QVector>

struct graph_info {
    int source_id;
    int dest_id;
    QVector<int> edges;
};


#endif // GRAPH_INFO_H
