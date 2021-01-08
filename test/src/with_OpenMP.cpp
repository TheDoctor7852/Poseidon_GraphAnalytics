#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "pagerank.hpp"
#include"pagerank_return.hpp"

#include <thread>
#include <future>
#include <thread_pool.hpp>

double convertBoostAnyToDouble(boost::any input){
    try{
        return boost::any_cast<double>(input);
    } catch (boost::bad_any_cast &e){ //sollte boost::any vom Typ int sein, schlägt die obere Umwandlung mit boost::bad_any_cast fehl.
        return static_cast<double>(boost::any_cast<int>(input)); // boost::any wird erst in einen integer umgewandelt um dann in eine double-Typ überführt zu werden. 
    }
}

/*
    Bekommt einen Vector vom Typ RelationshipWeight übergeben. Ermittelt anhand des weight-Parameters (von RelationshipWeight) die maximalen Elemente.
    Diese werden dann in einem Vector vom Typ RelationshipWeight zurückgegeben.
*/
std::vector<utils::RelationshipWeight> determin_max_values(std::vector<utils::RelationshipWeight>& from_node){
    //initialisiere die benötigten Variablen
    std::vector<utils::RelationshipWeight> result = {};
    bool found_max_last_round = true;
    utils::RelationshipWeight max = utils::RelationshipWeight(nullptr,std::numeric_limits<double>::min());
    int position_of_max_ele = 0;

    //solange wie in der letzten Runde ein Element >= dem maximalen Element gefunden wurde, wird nach einem weiteren maximalen Element gesucht. -> es werden alle maximale Elemente gefunden. 
    while ((found_max_last_round) && (!from_node.empty())){
        found_max_last_round = false;
        auto max_entry = std::max_element(from_node.begin(),from_node.end(),[] (const utils::RelationshipWeight &a, const utils::RelationshipWeight &b) {
            return a.weight < b.weight;
        });
        position_of_max_ele = std::distance(from_node.begin(), max_entry);
        if(max.weight <= from_node[position_of_max_ele].weight){
            max = from_node[position_of_max_ele];
            result.push_back(max);
            found_max_last_round = true;
            from_node.erase(from_node.begin() + position_of_max_ele);
        }
    }
    return result;
};

/*
    Bekommt einen Vector vom Typ RelationshipWeight übergeben. Ermittelt anhand des weight-Parameters (von RelationshipWeight) die minimalen Elemente.
    Diese werden dann in einem Vector vom Typ RelationshipWeight zurückgegeben.
*/
std::vector<utils::RelationshipWeight> determin_min_values(std::vector<utils::RelationshipWeight>& from_node){
    //initialisiere die benötigten Variablen
    std::vector<utils::RelationshipWeight> result = {};
    bool found_min_last_round = true;
    utils::RelationshipWeight min = utils::RelationshipWeight(nullptr,std::numeric_limits<double>::max());
    int position_of_min_ele = 0;

    //solange wie in der letzten Runde ein Element <= dem minimalen Element gefunden wurde, wird nach einem weiteren minimalen Element gesucht. -> es werden alle minimalen Elemente gefunden.
    while ((found_min_last_round) && (!from_node.empty())){
        found_min_last_round = false;
        auto min_entry = std::min_element(from_node.begin(),from_node.end(),[] (const utils::RelationshipWeight &a, const utils::RelationshipWeight &b) {
        return a.weight < b.weight;
        });
        position_of_min_ele = std::distance(from_node.begin(), min_entry);
        if(min.weight >= from_node[position_of_min_ele].weight){
         min = from_node[position_of_min_ele];
        result.push_back(min);
        found_min_last_round = true;
        from_node.erase(from_node.begin() + position_of_min_ele);
        }
    }
    return result;
};

/*
    Erstellt eine Matrix, die von jedem Knoten aus die Kante(n) mit dem maximalen Gewicht enthält. Als Gewicht wird hierbei der Wert der übergebenen Property jeder Relation betrachtet.
*/
std::vector<std::vector<utils::RelationshipWeight>> create_max_weight_matrix(graph_db_ptr& graph, std::vector<node::id_t>& nodes, std::string property, double default_value){
    //initialisiere die benötigten Variablen
    node::id_t active_node = 0;
    std::vector<std::vector<utils::RelationshipWeight>> result = {};
    std::vector<utils::RelationshipWeight> from_node = {};
    std::vector<utils::RelationshipWeight> ret = {};

    for(size_t i=0; i<nodes.size(); i++){
        node::id_t active_node = nodes[i];
        // sammle alle ausgehenden Relationen des aktuellen Knotens
        graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
            if(graph->get_rship_description(r.id()).has_property(property)){
                from_node.push_back(utils::RelationshipWeight(&r,convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
            } else{
                // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
                from_node.push_back(utils::RelationshipWeight(&r,default_value)); 
            }
        });
        //finde die Relationships(Kanten) mit dem maximalen Gewicht
        result.push_back(determin_max_values(from_node));
        from_node.clear();
    }
    return result;
};

/*
    Erstellt eine Matrix, die von jedem Knoten aus die Kante(n) mit dem minimalen Gewicht enthält. Als Gewicht wird hierbei der Wert der übergebenen Property jeder Relation betrachtet.
*/
std::vector<std::vector<utils::RelationshipWeight>> create_min_weight_matrix(graph_db_ptr& graph, std::vector<node::id_t>& nodes, std::string property, double default_value){
    //initialisiere die benötigten Variablen
    node::id_t active_node = 0;
    std::vector<std::vector<utils::RelationshipWeight>> result = {};
    std::vector<utils::RelationshipWeight> from_node = {};
    std::vector<utils::RelationshipWeight> ret = {};

    for(size_t i=0; i<nodes.size(); i++){
        node::id_t active_node = nodes[i];
        // sammle alle ausgehenden Relationen des aktuellen Knotens
        graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
            if(graph->get_rship_description(r.id()).has_property(property)){
                from_node.push_back(utils::RelationshipWeight(&r,convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
            } else{
                // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
                from_node.push_back(utils::RelationshipWeight(&r,default_value)); // sollte die Kante nicht die Property haben, wird der default_value verwendet 
            }
        });
        //finde die Relationships(Kanten) mit dem minimalem Gewicht
        result.push_back(determin_min_values(from_node));
        from_node.clear();
    }
    return result;
};

void initialiseActiveNodesAndLabel(graph_db_ptr& graph, std::vector<node::id_t>& nodes, utils::PropertyTracker<node::id_t>& label){

    node::id_t active_node;

    graph->nodes([&] (node& n){
      nodes.push_back(n.id());
    });

    for(size_t i=0; i<nodes.size(); i++){
        label.propertys[i] = nodes[i];
    }
}

utils::LabelReturn labelPropagation_SIMD(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
    //initialisiere die benötigten Variablen
    utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
    std::vector<node::id_t> nodes = {};
    std::random_device rd;
    std::mt19937 rng(rd());
    utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
    bool did_change_last_run = true;
    int number_of_turns = 0;
    std::vector<std::vector<utils::RelationshipWeight>> matrix;

    initialiseActiveNodesAndLabel(graph, nodes, label);

    //bestimme ob Matrix mit maximalen oder minimalen Gewichten benötigt wird.
    if(max){
        matrix = create_max_weight_matrix(graph,nodes,property,default_value);
    } else{
        matrix = create_min_weight_matrix(graph,nodes,property,default_value);
    }

    auto start = std::chrono::high_resolution_clock::now();
    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        std::random_shuffle(matrix.begin(), matrix.end());
        did_change_last_run = false;
        #pragma omp simd 
        for(size_t i=0; i< matrix.size(); i++){
            std::vector<utils::RelationshipWeight> active_vector = matrix[i];
            // wenn es mehrere Kanten mit maximalen bzw. minimalen Gewicht gibt, wird uniform zufällig entschieden. 
            if (active_vector.size()>1){
                std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
                active_relationship = active_vector[dist(rng)];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
            }else if(active_vector.size() == 1){
                active_relationship = active_vector[0];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            }
            //sollte es keine ausgehenden Kante geben, so wird dieser Knoten vernachlässigt.
        }
        number_of_turns++;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return utils::LabelReturn(nodes, label);
}
//speedup idee: teile for schleife in 2 schleifen -> die äußere schleife parralelisieren (die zählt die kanzen Prozesse modulo, die innere Schleife wird nicht parallelisiert und läuft so durch-> prozesse müssen für kurze instruktionen nicht immer wechseln)
utils::LabelReturn labelPropagation_parallel(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
    //initialisiere die benötigten Variablen
    utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
    std::vector<node::id_t> nodes = {};
    std::random_device rd;
    std::mt19937 rng(rd());
    utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
    bool did_change_last_run = true;
    int number_of_turns = 0;
    std::vector<std::vector<utils::RelationshipWeight>> matrix;

    initialiseActiveNodesAndLabel(graph, nodes, label);

    //bestimme ob Matrix mit maximalen oder minimalen Gewichten benötigt wird.
    if(max){
        matrix = create_max_weight_matrix(graph,nodes,property,default_value);
    } else{
        matrix = create_min_weight_matrix(graph,nodes,property,default_value);
    }

    auto start = std::chrono::high_resolution_clock::now();
    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        std::random_shuffle(matrix.begin(), matrix.end());
        did_change_last_run = false;
    #pragma omp parallel for 
        for(size_t i=0; i< matrix.size(); i++){
            std::vector<utils::RelationshipWeight> active_vector = matrix[i];
            // wenn es mehrere Kanten mit maximalen bzw. minimalen Gewicht gibt, wird uniform zufällig entschieden. 
            if (active_vector.size()>1){
                std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
                active_relationship = active_vector[dist(rng)];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
            }else if(active_vector.size() == 1){
                active_relationship = active_vector[0];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            }
            //sollte es keine ausgehenden Kante geben, so wird dieser Knoten vernachlässigt.
        }
        number_of_turns++;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return utils::LabelReturn(nodes, label);
}

utils::LabelReturn labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
    //initialisiere die benötigten Variablen
    utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
    std::vector<node::id_t> nodes = {};
    std::vector<utils::RelationshipWeight> active_vector = {};
    std::random_device rd;
    std::mt19937 rng(rd());
    utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
    bool did_change_last_run = true;
    int number_of_turns = 0;
    std::vector<std::vector<utils::RelationshipWeight>> matrix;

    initialiseActiveNodesAndLabel(graph, nodes, label);

    //bestimme ob Matrix mit maximalen oder minimalen Gewichten benötigt wird.
    if(max){
        matrix = create_max_weight_matrix(graph,nodes,property,default_value);
    } else{
        matrix = create_min_weight_matrix(graph,nodes,property,default_value);
    }

    auto start = std::chrono::high_resolution_clock::now();
    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        std::random_shuffle(matrix.begin(), matrix.end());
        did_change_last_run = false;

        for(size_t i=0; i< matrix.size(); i++){
            active_vector = matrix[i];
            // wenn es mehrere Kanten mit maximalen bzw. minimalen Gewicht gibt, wird uniform zufällig entschieden. 
            if (active_vector.size()>1){
                std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
                active_relationship = active_vector[dist(rng)];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
            }else if(active_vector.size() == 1){
                active_relationship = active_vector[0];
                if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
                    did_change_last_run = true;
                    label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
                }
            }
            //sollte es keine ausgehenden Kante geben, so wird dieser Knoten vernachlässigt.
        }
        number_of_turns++;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return utils::LabelReturn(nodes, label);
}


int main(){
  
  //auto pool = graph_pool::open("./graph/pool"); 
  //auto graph = pool->open_graph("Big_Graph_Test"); 
  //auto graph = pool->open_graph("Label_Prop_Test");

  auto pool = graph_pool::open("./graph/50000nodeGraph");
  auto graph = pool->open_graph("50000nodeGraph");

    auto tx = graph->begin_transaction();

    auto start2 = std::chrono::high_resolution_clock::now();
    utils::LabelReturn label = labelPropagation_parallel(graph,"values",1.0,true,1000);
    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2); 
  
    std::cout << "Time taken by function: "
         << duration2.count() << " microseconds" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    utils::LabelReturn result = labelPropagation(graph,"values",1.0,true, 1000);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl; 
    
    auto start3 = std::chrono::high_resolution_clock::now();
    utils::LabelReturn result3 = labelPropagation_SIMD(graph,"values",1.0,true, 1000);
    auto stop3 = std::chrono::high_resolution_clock::now();

    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration3.count() << " microseconds" << std::endl;
    /*
    for(size_t i=0; i<result.used_nodes.size(); i++){
      std::cout << graph->get_node_description(result.used_nodes[i]).properties.at("name") << "   Label:"<< result.label.propertys[i] << std::endl;
    }

    for(size_t i=0; i<label.used_nodes.size(); i++){
      std::cout << graph->get_node_description(label.used_nodes[i]).properties.at("name") << "   Label:"<< label.label.propertys[i] << std::endl;
    }*/

  //graph->abort_transaction();
  //pool->close();
  
  //test();
  //Label_Test();
  //PageRank_Test();
  //create_pool();
  //create_data();
  //pageRank_small_test_graph();
  //pageRank_medium_test_graph();
  //PageRank_example();
  //create_different_label_testgraph();
  //create_labelprop_testgraph();
  //expand_test_graph();
  //create_bigGraph();
  return 0;
}

/*
  ein pool muss erzeugt werden, bevor graphen darin gespeichert werden können
*/
void create_pool(){
  auto pool = graph_pool::create("./graph/pool",838860800);
  pool->close();
}