#ifndef BFS__HPP
#define BFS__HPP

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

#include "property_tracker.hpp"
#include "graph_pool.hpp"
#include "number_from_nodestring.hpp"
#include "qop.hpp"
#include "query.hpp"
#include "visitor.hpp"

namespace analytics{

    /*
        bekommt eine Referenz auf einen Graphen sowie die ID des Startknotens Übergeben.
        Führt eine Breitensuche durch, die alle vom Startknoten aus erreichbaren Knoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
    */
    std::vector<relationship*> bfs(graph_db_ptr& graph, node::id_t start);
        
    /*
        bekommt eine Referenz auf einen Graphen, die ID des Startknotens sowie einen Vector, der die ID's der gesuchten Knoten enthält, Übergeben.
        Führt eine Breitensuche durch, die stoppt sobald einer der Endknoten gefunden wurde oder es keine weiteren erreichbaren Knoten mehr gibt.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
    */
    std::vector<relationship*> bfs(graph_db_ptr& graph, node::id_t start, std::vector<node::id_t>& end);

    /*
        bekommt eine Referenz auf einen Graphen, die ID des Startknotens sowie einen Query Übergeben. 
        Hierbei ist wichtig, dass die Query noch durch eine .project() sowie .collect() Klausel erweitert werden kann. 
        Aus dieser Query werden die Endknoten ermittelt.
        Führt eine Breitensuche durch, die stoppt sobald einer der Endknoten gefunden wurde oder es keine weiteren erreichbaren Knoten mehr gibt.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
    */
    std::vector<relationship*> bfs(graph_db_ptr& graph, node::id_t start, query end);

}

#endif //BFS__HPP