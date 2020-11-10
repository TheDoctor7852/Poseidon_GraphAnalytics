#include "dfs.hpp"

/*
  Bekommt eine Referenz auf einen Graphen sowie die ID des Startknotens Übergeben.
  Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Knoten findet.
  Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält.
*/
std::vector<relationship*> analytics::dfs(graph_db_ptr& graph, node::id_t start){
  utils::FoundNodes found = utils::FoundNodes(graph->get_nodes()->as_vec().capacity());
  std::vector<relationship*> partial_vec = {};
  found.found_nodes[start] = true;
  dfs_recursive(graph, start, found, partial_vec);
  return partial_vec;
} 

/*
        Diese Funktion wird von dfs verwendet. Sollte man also eine normale Tiefensuche durchführen wollen, ist es ratsam dfs zu verwenden.

        Diese Funktion bekommt eine Referenz auf einen Graphen, die ID des Startknotens, ein FoundNodes Objekt sowie einen Vector vom Typ relationship* übergeben.
        Hierbei sind Standardmäßig alle Werte im Vector des FoundNodes-Objektes auf false gesetzt und der Vector vom Typ relationship* ist leer.
        Führt eine Tiefensuche durch, die alle vom Startknoten aus erreichbaren Knoten findet.
        Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält, welche sich hierbei im partial_vec befinden.
    */
void analytics::dfs_recursive (graph_db_ptr& graph, node::id_t start, utils::FoundNodes& found, std::vector<relationship*>& partial_vec){
  graph->foreach_from_relationship_of_node(graph->node_by_id(start),[&] (relationship& r) {
        node::id_t to_node = r.to_node_id();
        if((!found.found_nodes[to_node])){
            found.found_nodes[to_node] = true;
            partial_vec.push_back(&r);
            dfs_recursive(graph, to_node, found, partial_vec);
        }
    });
}