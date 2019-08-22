QT += widgets

HEADERS += \
        edge.h \
        node.h \
        graphwidget.h \
    automata.h \
    nodeedgeparent.h \
    dlginput.h \
    formgraph.h \
    mainwindow.h \
    jsonfuncs.h \
    matrix.h \
    KLogic/breadth_for_search.h \
    KLogic/kedge.h \
    KLogic/best_first_seatch.h \
    KLogic/dijkstra_algoritm.h \
    formaction.h \
    KLogic/a_star.h \
    KLogic/poln.h \
    KLogic/random_node.h \
    KLogic/krask.h \
    KLogic/vostonav.h

SOURCES += \
        edge.cpp \
        main.cpp \
        node.cpp \
        graphwidget.cpp \
    automata.cpp \
    dlginput.cpp \
    nodeedgeparent.cpp \
    formgraph.cpp \
    mainwindow.cpp \
    matrix.cpp \
    KLogic/breadth_for_search.cpp \
    KLogic/best_first_seatch.cpp \
    KLogic/dijkstra_algoritm.cpp \
    formaction.cpp \
    KLogic/a_star.cpp \
    KLogic/poln.cpp \
    KLogic/random_node.cpp \
    KLogic/krask.cpp \
    KLogic/vostonav.cpp

FORMS += \
    formgraph.ui \
    mainwindow.ui \
    matrix.ui
