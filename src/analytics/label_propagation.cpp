#include "label_propagation.hpp"

/*
    Bekommt eine Variable vom Typ boost::any übergeben, und Versucht diese in einen Wert vom Typ double umzuwandeln.    
*/
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

/*
    bestimmt mittels einer Anfrage alle im Graphen vorhandenen Knoten. Weiterhin werden den aus der Anfrage bestimmten Knoten initiale Label zugeordnet.
*/
void initialiseActiveNodesAndLabel(graph_db_ptr& graph, std::vector<node::id_t>& nodes, utils::PropertyTracker<node::id_t>& label){

    node::id_t active_node;

    graph->nodes([&] (node& n){
      nodes.push_back(n.id());
    });

    for(size_t i=0; i<nodes.size(); i++){
        label.propertys[i] = nodes[i];
    }
}


/*
    Bekommt den Graph sowie optional die maximale Anzahl an Runden übergeben.
    Wählt jede Runde zufällig einen Nachfolgeknoten aus, dessen Label übernaommen wird.
*/
utils::LabelReturn analytics::labelPropagation(graph_db_ptr& graph, int max_runs){
    //initialisiere die benötigten Variablen
    utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
    std::vector<node::id_t> nodes = {};
    std::vector<relationship*> from_node = {};
    std::random_device rd;
    std::mt19937 rng(rd());
    node::id_t active_node;
    relationship* active_relationship = nullptr;
    bool did_change_last_run = true;
    int number_of_turns = 0;

    initialiseActiveNodesAndLabel(graph, nodes, label);

    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        std::random_shuffle(nodes.begin(), nodes.end());
        did_change_last_run = false;

        for(size_t i=0; i< nodes.size(); i++){
            active_node = nodes[i];
            //bestimme alle ausgehenden Relationships(Kanten) des aktuellen Knotens
            graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
                from_node.push_back(&r);
            });
            if (!from_node.empty()){
                //bestimme zufällig den Knoten, von dem das Label übernommen wird
                std::uniform_int_distribution<int> dist(0,from_node.size()-1);
                active_relationship = from_node[dist(rng)];
                if(label.propertys[active_node] != label.propertys[active_relationship->to_node_id()]){
                did_change_last_run = true;
                }
                label.propertys[active_node] = label.propertys[active_relationship->to_node_id()];
            }
            from_node.clear();
        }
        number_of_turns++;
    }

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return utils::LabelReturn(nodes, label);
}

utils::LabelReturn analytics::labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
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

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return utils::LabelReturn(nodes, label);
}

/*
    Kann eingebaut werden, um die erzeugt Matrix auszugeben.
        for(size_t i=0; i<matrix.size(); i++){
            for (size_t s=0; s<matrix[i].size(); s++){
                std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
            }
            std::cout << std::endl;
        }
*/