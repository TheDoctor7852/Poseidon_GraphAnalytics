#include "bfs_.hpp"

/*
  bekommt eine Referenz auf einen Graphen sowie die ID des Startknotens Übergeben.
  Führt eine Breitensuche durch, die alle vom STartknoten aus erreichbaren Knoten findet.
  Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
*/
std::vector<relationship*> analytics::bfs(graph_db_ptr& graph, node::id_t start){
    utils::FoundNodes<bool> found = utils::FoundNodes<bool>(graph->get_nodes()->as_vec().capacity(), false);
    std::queue<node::id_t> active_nodes;
    std::vector<relationship*> result;

    active_nodes.push(start);
    found.found_nodes[start] = true;

    while((!active_nodes.empty())){
    node::id_t active_n = active_nodes.front();
    active_nodes.pop();
    graph->foreach_from_relationship_of_node(graph->node_by_id(active_n),[&] (relationship& r) {
        node::id_t to_node = r.to_node_id();
        if((!found.found_nodes[to_node])){
            found.found_nodes[to_node] = true;
            result.push_back(&r);
            active_nodes.push(to_node);
        }
      });
    }
    return result;
};

/*
  bekommt eine Referenz auf einen Graphen, die ID des Startknotens sowie einen Vector, der die ID's der gesuchten Knoten enthält, Übergeben.
  Führt eine Breitensuche durch, die stoppt sobald einer der Endknoten gefunden wurde oder es keine weiteren erreichbaren Knoten mehr gibt.
  Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
*/
std::vector<relationship*> analytics::bfs(graph_db_ptr& graph, node::id_t start, std::vector<node::id_t>& end){
    bool found_end_node = false;
    utils::FoundNodes<bool> found = utils::FoundNodes<bool>(graph->get_nodes()->as_vec().capacity(), false);
    std::queue<node::id_t> active_nodes;
    std::vector<relationship*> result;

    active_nodes.push(start);
    found.found_nodes[start] = true;

    if(std::find(end.begin(), end.end(), start) == end.end()){
        while((!active_nodes.empty()) && (!found_end_node)){
        node::id_t active_n = active_nodes.front();
        active_nodes.pop();
        graph->foreach_from_relationship_of_node(graph->node_by_id(active_n),[&] (relationship& r) {
            node::id_t to_node = r.to_node_id();
            if(std::find(end.begin(), end.end(), to_node) != end.end()){
                result.push_back(&r);
                found_end_node = true;
            } else if((!found.found_nodes[to_node]) && (!found_end_node)){
                found.found_nodes[to_node] = true;
                result.push_back(&r);
                active_nodes.push(to_node);
            }
        });
      }
    }
  if(found_end_node){
    return result;
  } else{
    return {};
  }
};

/*
  bekommt eine Referenz auf einen Graphen, die ID des Startknotens sowie einen Query Übergeben. 
  Hierbei ist wichtig, dass die Query noch durch eine .project() sowie .collect() Klausel erweitert werden kann. 
  Aus dieser Query werden die Endknoten ermittelt.
  Führt eine Breitensuche durch, die stoppt sobald einer der Endknoten gefunden wurde oder es keine weiteren erreichbaren Knoten mehr gibt.
  Rückgabe ist hierbei ein Vector der alle durchlaufenen Kanten enthält. 
*/
std::vector<relationship*> analytics::bfs(graph_db_ptr& graph, node::id_t start, query end){
    result_set result;
    std::vector <node::id_t> IDvec;
    end
      .project({PExpr_(0, builtin::string_rep(res))})
      .collect(result);
    end.start();

    result.wait();

    utils::Visitor det;
    int size = result.data.size();
    for(int i=0; i<size; i++){
        boost::apply_visitor(det, result.data.front()[0]);
        if (det.getMem().success){
            IDvec.push_back(utils::readNumberFromString(det.getMem().content));
        }
        result.data.pop_front();
    }
  return bfs(graph, start, IDvec);
}