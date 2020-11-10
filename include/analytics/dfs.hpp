#ifndef DFS_HPP
#define DFS_HPP

#include <algorithm>
#include <vector>

#include "found_nodes.hpp"
#include "graph_pool.hpp"

namespace analytics{

    /*
        Bekommt eine Referenz auf einen Graphen sowie die ID des Startknotens Übergeben.
        Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Knoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält.
    */
    std::vector<relationship*> dfs(graph_db_ptr& graph, node::id_t start);

    /*
        Bekommt eine Referenz auf einen Graphen, die ID des Startknotens sowie ein Array mit ID's für Endknoten Übergeben.
        Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Endknoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält, oder wenn es keinen Weg gibt ein leerer Vector.
    */
    std::vector<relationship*> dfs(graph_db_ptr& graph, node::id_t start, std::vector<node::id_t>& end);

    /*
        Diese Funktion wird von dfs verwendet. Sollte man also eine normale Tiefensuche durchführen wollen, ist es ratsam dfs zu verwenden.

        Diese Funktion bekommt eine Referenz auf einen Graphen, die ID des Startknotens, ein FoundNodes Objekt sowie einen Vector vom Typ relationship* übergeben.
        Hierbei sind Standardmäßig alle Werte im Vector des FoundNodes-Objektes auf false gesetzt und der Vector vom Typ relationship* ist leer.
        Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Knoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält, welche sich hierbei im partial_vec befinden.
    */
    void dfs_recursive (graph_db_ptr& graph, node::id_t start, utils::FoundNodes<bool>& found, std::vector<relationship*>& partial_vec);

    /*
        Diese Funktion wird von dfs verwendet. Sollte man also eine normale Tiefensuche durchführen wollen, ist es ratsam dfs zu verwenden.

        Diese Funktion bekommt eine Referenz auf einen Graphen, die ID des Startknotens, ein FoundNodes Objekt, einen Vector vom Typ relationship*, einen Vektor mit ID's von Endknoten sowie die Referenz auf eine Variable die angibt, ob ein Endknoten gefunden wurde übergeben.
        Hierbei sind Standardmäßig alle Werte im Vector des FoundNodes-Objektes auf false gesetzt und der Vector vom Typ relationship* ist leer.
        Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Endknoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält, welche sich hierbei im partial_vec befinden.
    */
    void dfs_recursive (graph_db_ptr& graph, node::id_t start, utils::FoundNodes<bool>& found, std::vector<relationship*>& partial_vec,std::vector<node::id_t>& end, bool& found_endnode);

}

#endif