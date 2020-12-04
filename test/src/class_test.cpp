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

class Node{
    node::id_t id;
    node::id_t label;
    std::vector<std::shared_ptr<Node>> neighbours;
    public:
    Node(node::id_t ID){
        id = ID;
        neighbours = {};
    }
    Node(node::id_t ID, node::id_t l){
        id = ID;
        label = l;
        neighbours = {};
    }

    void add_neighbour(std::shared_ptr<Node> node){
        neighbours.push_back(node);
    }

    node::id_t getID(){
        return id;
    }

    node::id_t getLabel(){
        return label;
    }

    std::vector<std::shared_ptr<Node>> get_neighbours(){
        return neighbours; 
    }

    bool update_label(std::shared_ptr<Node> n){
        if(label != n->getLabel()){
            label = n->getLabel();
            return true;
        }else{
            return false;
        }
    }
};

class Node_Pool{
    std::vector<std::shared_ptr<Node>> v;
    size_t position_of_last;
    public:
    Node_Pool(graph_db_ptr& graph){
        v = std::vector<std::shared_ptr<Node>>(graph->get_nodes()->as_vec().capacity());
        position_of_last = 0;
    }
    
    std::shared_ptr<Node> return_node_at(size_t i){
        return v[i];
    }

    void shuffle_active_pool(){
        std::random_shuffle(v.begin(), v.begin()+position_of_last+1);
    }

    size_t size(){
        return position_of_last+1;
    }

    void add_Node(std::shared_ptr<Node> n){
        v[n->getID()] = n;
        if(n->getID() > position_of_last){
            position_of_last = n->getID();
        }
    }
};

double convertBoostAnyToDouble(boost::any input){
    try{
        return boost::any_cast<double>(input);
    } catch (boost::bad_any_cast &e){ //sollte boost::any vom Typ int sein, schlägt die obere Umwandlung mit boost::bad_any_cast fehl.
        return static_cast<double>(boost::any_cast<int>(input)); // boost::any wird erst in einen integer umgewandelt um dann in eine double-Typ überführt zu werden. 
    }
}

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

void create_max_weight_matrix_OO(graph_db_ptr& graph, Node_Pool& nodes, std::string property, double default_value){
    //initialisiere die benötigten Variablen
    node::id_t active_node = 0;
    std::vector<utils::RelationshipWeight> from_node = {};
    std::vector<utils::RelationshipWeight> ret = {};

    for(size_t i=0; i<nodes.size(); i++){
        std::shared_ptr<Node> active_node = nodes.return_node_at(i);
        // sammle alle ausgehenden Relationen des aktuellen Knotens
        graph->foreach_from_relationship_of_node(graph->node_by_id(active_node->getID()), [&] (relationship& r) {
            if(graph->get_rship_description(r.id()).has_property(property)){
                from_node.push_back(utils::RelationshipWeight(&r,convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
            } else{
                // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
                from_node.push_back(utils::RelationshipWeight(&r,default_value)); 
            }
        });
        //finde die Relationships(Kanten) mit dem maximalen Gewicht
        ret = determin_max_values(from_node);
        for(size_t i=0; i<ret.size(); i++){
            active_node->add_neighbour(nodes.return_node_at(ret[i].rel->to_node_id()));
        }
        ret.clear();
        from_node.clear();
    }
};

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

void initialiseActiveNodesAndLabel_OO(graph_db_ptr& graph, Node_Pool& nodes){

    graph->nodes([&] (node& n){
      nodes.add_Node(std::make_shared<Node>(n.id(),n.id()));
    });
}

void initialiseActiveNodesAndLabel(graph_db_ptr& graph, std::vector<node::id_t>& nodes, utils::PropertyTracker<node::id_t>& label){

    node::id_t active_node;

    graph->nodes([&] (node& n){
      nodes.push_back(n.id());
    });

    for(size_t i=0; i<nodes.size(); i++){
        label.propertys[i] = nodes[i];
    }
}
  //warscheinlich langsamer weil vector geschlossen wird, wenn 1 thread darin liest bzw. wenn 1 thread darin etwas ändert müssen die anderen neu laden (cache wird invalide)
bool ThreadFunc(Node_Pool* node_pool, size_t start, size_t end){
  if(start < end){

    std::vector<std::shared_ptr<Node>> active_vector = {};
    std::shared_ptr<Node> active_relationship = nullptr;
    bool did_change_last_run = false;
    bool did_change = false;
    std::random_device rd;  // mal schauen ob auch als referenz übergeben kann
    std::mt19937 rng(rd()); // mal schauen ob auch als referenz übergeben kann
    
    for(size_t i=start; i < end; i++){
      active_vector = node_pool->return_node_at(i)->get_neighbours();
      // wenn es mehrere Kanten mit maximalen bzw. minimalen Gewicht gibt, wird uniform zufällig entschieden. 
      if (active_vector.size()>1){
      std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
      active_relationship = active_vector[dist(rng)];
        did_change = node_pool->return_node_at(i)->update_label(active_relationship);
      //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
      }else if(active_vector.size() == 1){
        active_relationship = active_vector[0];
        did_change = node_pool->return_node_at(i)->update_label(active_relationship);
      }
      //sollte es keine ausgehenden Kante geben, so wird dieser Knoten vernachlässigt.
      if(did_change){
          did_change_last_run = true;
      }
    }
    //std::cout << "Did_change:   " << did_change_last_run << std::endl;
    return did_change_last_run;//promObj->set_value(did_change_last_run);
  } else{
    //std::cout << "default:   " << false << std::endl;
    return false;//promObj->set_value(false);
  }
}

struct Threaded_Thing{

  Node_Pool* node_pool;
  size_t start;
  size_t end;
  Threaded_Thing(Node_Pool* m, size_t s, size_t e){
    node_pool = m;
    start = s;
    end = e;
  };

  bool operator()(){
    return ThreadFunc(node_pool, start, end);
  }
};

Node_Pool labelPropagation_parallel(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
    Node_Pool node_pool = Node_Pool(graph);
    bool did_change_last_run = true;
    int number_of_turns = 0;
    std::vector<size_t> thread_startAndEnd = {0};
    thread_pool pool = thread_pool();
    std::vector<std::future<bool>> futures = {};

    initialiseActiveNodesAndLabel_OO(graph,node_pool);

    create_max_weight_matrix_OO(graph,node_pool,property,default_value);

    for(size_t i=0; i<std::thread::hardware_concurrency(); i++){
      if(i<(node_pool.size() % std::thread::hardware_concurrency())){
        thread_startAndEnd.push_back(thread_startAndEnd[i]+1+(node_pool.size()/std::thread::hardware_concurrency()));
      } else{
        thread_startAndEnd.push_back(thread_startAndEnd[i]+(node_pool.size()/std::thread::hardware_concurrency()));
      }
    }

    auto start = std::chrono::high_resolution_clock::now();
    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        node_pool.shuffle_active_pool();
        did_change_last_run = false;
        for(int i=0;i<thread_startAndEnd.size()-1;i++){
          std::future<bool> f = (pool.submit(Threaded_Thing(&node_pool, thread_startAndEnd[i], thread_startAndEnd[i+1])));
          futures.push_back(std::move(f));
        }
          for(size_t i=0;i<futures.size();i++){
          if (futures[i].get()){
            did_change_last_run = true;
          }
        }
        futures.clear();
        number_of_turns++;
    }

     auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;

    std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
    return node_pool;
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

//ergebnis des Algo stimmt noch nicht ganz
int main(){

  auto pool = graph_pool::open("./graph/pool"); 
  auto graph = pool->open_graph("Big_Graph_Test"); 
  //auto graph = pool->open_graph("Label_Prop_Test");

    auto tx = graph->begin_transaction();

    auto start2 = std::chrono::high_resolution_clock::now();
    Node_Pool label = labelPropagation_parallel(graph,"values",1.0,true,500);
    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2); 
  
    std::cout << "Time taken by function: "
         << duration2.count() << " microseconds" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    utils::LabelReturn result = labelPropagation(graph,"values",1.0,true, 500);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl; 
    /*
    for(size_t i=0; i<result.used_nodes.size(); i++){
      std::cout << graph->get_node_description(result.used_nodes[i]).properties.at("name") << "   Label:"<< result.label.propertys[i] << std::endl;
    }
    std::cout << std::endl;

    for(size_t i=0; i<label.size(); i++){
      std::cout << graph->get_node_description(label.return_node_at(i)->getID()).properties.at("name") << "   Label:"<< label.return_node_at(i)->getLabel() << std::endl;
    }*/
}