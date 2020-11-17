#include "pagerank.hpp"

/*
    Dient dazu sich die verwendeten Knoten sowie den, in jeder iteration berechneten, alten und neuen PageRank zu speichern.
*/
struct PageRankUtil{
    std::vector<node::id_t> used_nodes;
    utils::PropertyTracker<double> old_pagerank;
    utils::PropertyTracker<double> new_pagerank;
};

/*
    Initialisiert die während PageRank verwendete Datenstrucktur PageRankUtil
*/
PageRankUtil initialisePageRank(graph_db_ptr& graph){

    node::id_t active_node;
    PageRankUtil result;

    //Finde die ID's aller Knoten
    graph->nodes([&] (node& n){
      result.used_nodes.push_back(n.id());
    });

    result.old_pagerank = utils::PropertyTracker<double>(graph->get_nodes()->as_vec().capacity(),0);
    result.new_pagerank = utils::PropertyTracker<double>(graph->get_nodes()->as_vec().capacity(),0);

    //weise jedem Knoten die selbe Warsch zu besucht zu werden
    for(size_t i=0; i<result.used_nodes.size(); i++){
      result.old_pagerank.propertys[result.used_nodes[i]] = 1.0/result.used_nodes.size();
    }
  
  return result;
}

void countOutgoingRel(graph_db_ptr& graph,PageRankUtil& current_pagerank, utils::PropertyTracker<int>& num_outgoing_relationships, std::vector<node::id_t>& nodes_without_outgoing_rel){

    int counter = 0;
    node::id_t active_node = 0;

    for(size_t i = 0; i < current_pagerank.used_nodes.size(); i++){
            active_node = current_pagerank.used_nodes[i];
            graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {counter++;});
            num_outgoing_relationships.propertys[active_node] = counter;

            if(counter == 0){
                nodes_without_outgoing_rel.push_back(active_node);
            }

            counter = 0;
        }
}

/*
    Führt einen PageRank über den gegeben Graphen aus. Optional können ein damping_factor, eine Schranke, die Anzahl an Maximalen Itterationen sowie ein Parameter der bestimmt ob teleportation erlaubt ist.
    Der damping_factor bestimmt die Warscheinlichkeit, dass auf dem Knoten keiner Kante gefolgt wird und statdessen eine zufälliger Knoten gewählt wird. Im paper wurde für diesen ein Wert 0.85 empfohlen.
    Die Schranke gibt an, welcher maximale Abstand des alten PageRank zum neuen PageRank (eine Iteration) erlaubt ist. Bei einer Schranke von 0.1 ist somit die Änderung aller Werte im Vergleich zum vorherigen Wert <= 0.1
    max_turns gibt die maximalen iterationen an.
    teleportation gibt an, ob bei knoten ohne Ausgangskante eine Kante zu jedem Knoten angenommen werden soll. -> Je nachdem ob dies an oder ausgeschalten ist, können Ergebnisse variieren;
*/
utils::PageRankReturn analytics::PageRank(graph_db_ptr& graph, double damping_factor, double converge_limit, int max_turns, bool teleportation){

    int counter = 0;
    double sum = 0;
    node::id_t active_node = 0;
    std::vector<node::id_t> nodes_without_outgoing_rel = {};
    utils::PropertyTracker<int> num_outgoing_relationships = utils::PropertyTracker<int>(graph->get_nodes()->as_vec().capacity(),0);
    double max_difference = std::numeric_limits<double>::max();

    // prüfe ob der damping_factor ok ist
    if((damping_factor > 1) || (damping_factor < 0)){
        std::cout << "damping factor needs to be: 0 <= damping_factor <= 1" << std::endl;
        return utils::PageRankReturn();
    }else{

        PageRankUtil current_pagerank = initialisePageRank(graph);

        countOutgoingRel(graph, current_pagerank, num_outgoing_relationships, nodes_without_outgoing_rel);

        while((max_difference >= converge_limit) && (counter<max_turns)){
            max_difference = 0;

            for(size_t i = 0; i < current_pagerank.used_nodes.size(); i++){
                active_node = current_pagerank.used_nodes[i];
                graph->foreach_to_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                    //kann nicht div 0 , weil wenn Knoten ankommende rel. hat, muss anderer Knoten >0 ausgehende rel. haben
                    sum = sum + current_pagerank.old_pagerank.propertys[r.from_node_id()]/num_outgoing_relationships.propertys[r.from_node_id()];
                });

                //wenn teleportation erlaubt, dann addiere alle knoten ohne ausgehende Kanten ensprechend hinzu.
                if(teleportation){

                    for(size_t s = 0; s < nodes_without_outgoing_rel.size(); s++){
                        if(nodes_without_outgoing_rel[s] != active_node){
                        sum = sum + current_pagerank.old_pagerank.propertys[nodes_without_outgoing_rel[s]]/(current_pagerank.used_nodes.size()-1); // ausgehende Kanten zu allen Knoten (auser sich selber)
                        }
                    }
                }
                current_pagerank.new_pagerank.propertys[active_node] = (1-damping_factor) + (damping_factor*sum);
                //std:: cout << (1-damping_factor) + (damping_factor*sum) << "    ";
                sum = 0;

                //bestimme die maximale Differenz zwischen dem alten und neuem PageRank in dieser Iteration
                if(std::abs(current_pagerank.new_pagerank.propertys[active_node]-current_pagerank.old_pagerank.propertys[active_node])>max_difference){
                    max_difference = std::abs(current_pagerank.new_pagerank.propertys[active_node]-current_pagerank.old_pagerank.propertys[active_node]);
                }
            }

            //std::cout << max_difference << std::endl;
            counter++;
            current_pagerank.old_pagerank.propertys = current_pagerank.new_pagerank.propertys;
        }
        std::cout << "Schleife durchlaufen: " << counter << std::endl;
        return utils::PageRankReturn(current_pagerank.used_nodes, current_pagerank.new_pagerank);
    }
}