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

void create_pool();
void create_data();
void create_different_label_testgraph();
void create_labelprop_testgraph();
void expand_test_graph();
void pageRank_small_test_graph();
void pageRank_medium_test_graph();
void PageRank_example();
void create_bigGraph();

/*
  nur innerhalb von transaktionen verwenden sonst speicherzugrifsfehler
  Problem: gibt jemand ID ein, die nicht gibt, dann gibt Speicherfehler.
*/
std::vector<node::id_t> relToNode_recursive(std::vector<relationship*>& input, node::id_t endnodes, int pos){
  
  if(endnodes == input[0]->from_node_id()){
    return {endnodes};
  }else if(pos<0){
    return {};
  }else if(input[pos]->to_node_id() == endnodes){
    std::vector<node::id_t> v = relToNode_recursive(input,input[pos]->from_node_id(),pos-1);
    v.push_back(endnodes);
    return v;
  }else{
    return relToNode_recursive(input,endnodes,pos-1);
  }
}

std::vector<node::id_t> relToNode(std::vector<relationship*>& input, node::id_t endnodes){
  if(input.empty()){
    return {};
  } else{
    return relToNode_recursive(input, endnodes, input.size()-1);
  }
}

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
  //warscheinlich langsamer weil vector geschlossen wird, wenn 1 thread darin liest bzw. wenn 1 thread darin etwas ändert müssen die anderen neu laden (cache wird invalide)
bool ThreadFunc(std::vector<std::vector<utils::RelationshipWeight>>& matrix, utils::PropertyTracker<node::id_t>& label, size_t start, size_t end){//std::promise<bool>* promObj){
  if(start < end){

    std::vector<utils::RelationshipWeight> active_vector = {};
    utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
    bool did_change_last_run = false;
    std::random_device rd;  // mal schauen ob auch als referenz übergeben kann
    std::mt19937 rng(rd()); // mal schauen ob auch als referenz übergeben kann
    
    for(size_t i=start; i < end; i++){
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
    //std::cout << "Did_change:   " << did_change_last_run << std::endl;
    return did_change_last_run;//promObj->set_value(did_change_last_run);
  } else{
    //std::cout << "default:   " << false << std::endl;
    return false;//promObj->set_value(false);
  }
}

bool ThreadFunc(std::vector<std::vector<utils::RelationshipWeight>>* m, utils::PropertyTracker<node::id_t>* label, size_t start, size_t end){//std::promise<bool>* promObj){
  if(start < end){
    std::vector<std::vector<utils::RelationshipWeight>> &matrix = *m;
    std::vector<utils::RelationshipWeight> active_vector = {};
    utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
    bool did_change_last_run = false;
    std::random_device rd;  // mal schauen ob auch als referenz übergeben kann
    std::mt19937 rng(rd()); // mal schauen ob auch als referenz übergeben kann
    
    for(size_t i=start; i < end; i++){
      active_vector = matrix[i];
      // wenn es mehrere Kanten mit maximalen bzw. minimalen Gewicht gibt, wird uniform zufällig entschieden. 
      if (active_vector.size()>1){
      std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
      active_relationship = active_vector[dist(rng)];
      if(label->propertys[active_relationship.rel->from_node_id()] != label->propertys[active_relationship.rel->to_node_id()]){
        did_change_last_run = true;
        label->propertys[active_relationship.rel->from_node_id()] = label->propertys[active_relationship.rel->to_node_id()];
      }
      //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
      }else if(active_vector.size() == 1){
        active_relationship = active_vector[0];
        if(label->propertys[active_relationship.rel->from_node_id()] != label->propertys[active_relationship.rel->to_node_id()]){
          did_change_last_run = true;
          label->propertys[active_relationship.rel->from_node_id()] = label->propertys[active_relationship.rel->to_node_id()];
        }
      }
      //sollte es keine ausgehenden Kante geben, so wird dieser Knoten vernachlässigt.
    }
    //std::cout << "Did_change:   " << did_change_last_run << std::endl;
    return did_change_last_run;//promObj->set_value(did_change_last_run);
  } else{
    //std::cout << "default:   " << false << std::endl;
    return false;//promObj->set_value(false);
  }
}

struct Threaded_Thing{

  std::vector<std::vector<utils::RelationshipWeight>>* matrix;
  utils::PropertyTracker<node::id_t>* label;
  size_t start;
  size_t end;
  Threaded_Thing(std::vector<std::vector<utils::RelationshipWeight>>* m, utils::PropertyTracker<node::id_t>* l, size_t s, size_t e){
    matrix = m;
    label = l;
    start = s;
    end = e;
  };

  bool operator()(){
    return ThreadFunc(matrix, label, start, end);
  }
};


std::random_device rd; 
std::mt19937 rng(rd());
struct Threaded_Thing2{

  utils::RelationshipWeight active_relationship = utils::RelationshipWeight(nullptr,0);
  bool did_change_last_run = false;

  std::vector<utils::RelationshipWeight>* matrix;
  utils::PropertyTracker<node::id_t>* label;
  Threaded_Thing2(std::vector<utils::RelationshipWeight>* m, utils::PropertyTracker<node::id_t>* l){
    matrix = m;
    label = l;
  };

  node::id_t operator()(){
    std::vector<utils::RelationshipWeight>& m = *matrix;
    if (m.size()>1){
      std::uniform_int_distribution<int> dist(0,m.size()-1);
      active_relationship = m[dist(rng)];
      return label->propertys[active_relationship.rel->to_node_id()];
      //sollte es nur eine maximale ausgehende Relationship(Kante) geben, wird diese gewählt.
      }else if(m.size() == 1){
        active_relationship = m[0];
        return label->propertys[active_relationship.rel->to_node_id()];
      }
  }
};

utils::LabelReturn labelPropagation_parallel(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs,int thread_count=std::thread::hardware_concurrency()){
    //initialisiere die benötigten Variablen
    thread_pool pool = thread_pool(thread_count);
    utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
    std::vector<node::id_t> nodes = {};
    bool did_change_last_run = true;
    int number_of_turns = 0;
    std::vector<std::vector<utils::RelationshipWeight>> matrix;
    std::vector<size_t> thread_startAndEnd = {0};
    //std::vector<std::promise<bool>> promises = {};
    //std::vector<std::future<node::id_t>> futures = {};
    std::vector<std::future<bool>> futures = {};
    initialiseActiveNodesAndLabel(graph, nodes, label);

    //bestimme ob Matrix mit maximalen oder minimalen Gewichten benötigt wird.
    if(max){
        matrix = create_max_weight_matrix(graph,nodes,property,default_value);
    } else{
        matrix = create_min_weight_matrix(graph,nodes,property,default_value);
    }
    for(size_t i=0; i<thread_count; i++){
      if(i<(nodes.size() % thread_count)){
        thread_startAndEnd.push_back(thread_startAndEnd[i]+1+(nodes.size()/thread_count));
      } else{
        thread_startAndEnd.push_back(thread_startAndEnd[i]+(nodes.size()/thread_count));
      }
    }

    //std::vector<utils::PropertyTracker<node::id_t>> labels = std::vector<utils::PropertyTracker<node::id_t>>(num_of_processors,label);

    auto start = std::chrono::high_resolution_clock::now();
    while(did_change_last_run && (number_of_turns < max_runs)){
        //randomisiere die Reihenfolge, in der die Knoten durchlaufen werden.
        std::random_shuffle(matrix.begin(), matrix.end());
        did_change_last_run = false;
        for(int i=0;i<thread_startAndEnd.size()-1;i++){
          //std::promise<bool> p;
          //std::future<bool> f = p.get_future();
          //futures.push_back(std::move(f));
          std::future<bool> f = (pool.submit(Threaded_Thing(&matrix, &label, thread_startAndEnd[i], thread_startAndEnd[i+1])));//&p);
          //std::future<node::id_t> f = (pool.submit(Threaded_Thing2(&matrix[i], &label)));
          futures.push_back(std::move(f));
          //promises.push_back(std::move(p));
          /*node::id_t help = f.get();
          if (help!=label.propertys[i]){
            label.propertys[i]= help;
            did_change_last_run = true;
          }*/
        }/*
        for(size_t i=0;i<futures.size();i++){
          node::id_t help = futures[i].get();
          if (help!=label.propertys[i]){
            label.propertys[i]= help;
            did_change_last_run = true;
          }*/
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

void PageRank_Test(){
  auto pool1 = graph_pool::open("./graph/pool");
  auto graph1 = pool1->open_graph("PageRank_small_Test");

  auto tx1 = graph1->begin_transaction();
  
  std::cout << "PageRank small example:" << std::endl;
  
  utils::PageRankReturn r = analytics::PageRank(graph1);
  for (size_t s=0; s<r.used_nodes.size(); s++){
      std::cout << graph1->get_node_description(r.used_nodes[s]).properties.at("name") << "  mit Gewicht: " << r.pagerank.propertys[s] << std::endl;
    }
  graph1->abort_transaction();
  pool1->close();

  std::cout << std::endl;

  auto pool2 = graph_pool::open("./graph/pool");
  auto graph2 = pool2->open_graph("PageRank_medium_Test");

  auto tx2 = graph2->begin_transaction();
  
  std::cout << "PageRank medium example:" << std::endl;
  
  r = analytics::PageRank(graph2);
  for (size_t s=0; s<r.used_nodes.size(); s++){
      std::cout << graph2->get_node_description(r.used_nodes[s]).properties.at("name") << "  mit Gewicht: " << r.pagerank.propertys[s] << std::endl;
    }
  graph2->abort_transaction();
  pool2->close();

  std::cout << std::endl;

  auto pool3 = graph_pool::open("./graph/pool");
  auto graph3 = pool3->open_graph("PageRank_example_Test");

  auto tx3 = graph3->begin_transaction();
  
  std::cout << "PageRank Book example:" << std::endl;
  
  r = analytics::PageRank(graph3);
  for (size_t s=0; s<r.used_nodes.size(); s++){
      std::cout << graph3->get_node_description(r.used_nodes[s]).properties.at("name") << "  mit Gewicht: " << r.pagerank.propertys[s] << std::endl;
    }
  graph3->abort_transaction();
  pool3->close();

}

void Label_Test(){
  auto pool = graph_pool::open("./graph/pool"); 
  auto graph = pool->open_graph("Label_Prop_Test"); 

  auto tx = graph->begin_transaction();

  utils::LabelReturn result = analytics::labelPropagation(graph);
  for(size_t i=0; i<result.used_nodes.size(); i++){
    std::cout << "Knoten:  " << graph->get_node_description(result.used_nodes[i]).properties.at("name") << "    hat Label:    " << result.label.propertys[result.used_nodes[i]] << std::endl;
  }

  result = analytics::labelPropagation(graph,"values",1.0);
  for(size_t i=0; i<result.used_nodes.size(); i++){
    std::cout << "Knoten:  " << graph->get_node_description(result.used_nodes[i]).properties.at("name") << "    hat Label:    " << result.label.propertys[result.used_nodes[i]] << std::endl;
  }

  graph->abort_transaction();
  pool->close();

}

void test() {
  auto pool = graph_pool::open("./graph/pool"); 
  auto graph = pool->open_graph("my_graph"); 

    auto tx = graph->begin_transaction();

    std::cout << "bfs:" << std::endl << std::endl;
    std::vector<node::id_t> end = {5};
    std::vector<relationship*> path = analytics::bfs(graph, 0);
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }

    std::cout << "bfs with endnode 5:" << std::endl << std::endl;
    path = analytics::bfs(graph, 0, end);
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }

    std::cout << "dfs:" << std::endl << std::endl;
    path = analytics::dfs(graph, 0);
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }

    std::cout << "dfs with endnode 5:" << std::endl << std::endl;
    path = analytics::dfs(graph, 6, end);
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }
    
    utils::LabelReturn result = analytics::labelPropagation(graph);
    for(size_t i=0; i<result.used_nodes.size(); i++){
      std::cout << "Knoten:  " << i << "    hat Label:    " << result.label.propertys[result.used_nodes[i]] << std::endl;
    }

    graph->abort_transaction();
    pool->close();
}

int main(){
  
  //auto pool = graph_pool::open("./graph/pool"); 
  //auto graph = pool->open_graph("Big_Graph_Test"); 
  //auto graph = pool->open_graph("Label_Prop_Test");

  auto pool = graph_pool::open("./graph/50000nodeGraph");
  auto graph = pool->open_graph("50000nodeGraph");

    auto tx = graph->begin_transaction();

    auto start2 = std::chrono::high_resolution_clock::now();
    utils::LabelReturn label = labelPropagation_parallel(graph,"values",1.0,true,10000);
    auto stop2 = std::chrono::high_resolution_clock::now();

    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2); 
  
    std::cout << "Time taken by function: "
         << duration2.count() << " microseconds" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    utils::LabelReturn result = labelPropagation(graph,"values",1.0,true, 10000);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl; 
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

void create_data() {
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("my_graph");

  auto tx = graph->begin_transaction();

      graph->add_node("Node", {{"id", boost::any(7)},
                               {"name", boost::any(std::string("aaa7"))},
                               {"other", boost::any(std::string("BBB7"))}});

      graph->add_node("Node", {{"id", boost::any(6)},
                               {"name", boost::any(std::string("aaa6"))},
                               {"other", boost::any(std::string("BBB6"))}});

      graph->add_node("Node", {{"id", boost::any(5)},
                               {"name", boost::any(std::string("aaa5"))},
                               {"other", boost::any(std::string("BBB5"))}});

      graph->add_node("Node", {{"id", boost::any(4)},
                               {"name", boost::any(std::string("aaa4"))},
                               {"other", boost::any(std::string("BBB4"))}});

      graph->add_node("Node", {{"id", boost::any(3)},
                               {"name", boost::any(std::string("aaa3"))},
                               {"other", boost::any(std::string("BBB3"))}});

      graph->add_node("Node", {{"id", boost::any(2)},
                               {"name", boost::any(std::string("aaa2"))},
                               {"other", boost::any(std::string("BBB2"))}});

      graph->add_node("Node", {{"id", boost::any(1)},
                               {"name", boost::any(std::string("aaa1"))},
                               {"other", boost::any(std::string("BBB1"))}});

    graph->add_relationship(0,3,"KNOW",{});
    graph->add_relationship(0,4,"KNOW",{});
    graph->add_relationship(0,1,"KNOW",{});
    graph->add_relationship(1,3,"KNOW",{});
    graph->add_relationship(1,5,"KNOW",{});
    graph->add_relationship(2,6,"KNOW",{});
    graph->add_relationship(2,0,"KNOWS",{});
    graph->add_relationship(2,3,"KNOW",{});
    graph->add_relationship(3,4,"KNOW",{});
    graph->add_relationship(3,5,"KNOW",{});
    graph->add_relationship(4,6,"KNOW",{});
    graph->add_relationship(4,1,"KNOW",{});
    graph->add_relationship(5,6,"KNOW",{}); 
    graph->add_relationship(6,0,"KNOW",{});

  graph->commit_transaction();
  pool->close();
}

void PageRank_example(){
  // Bsp aus Buch: Graph Algorithms by Amy E. Hodler and Mark Needham  (O'Reilly). ISBN 978-1-492-05781-9
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("PageRank_example_Test");

  auto tx = graph->begin_transaction();

  auto James = graph->add_node("Person",{{"name", boost::any(std::string("James"))}});
  auto David = graph->add_node("Person",{{"name", boost::any(std::string("David"))}});
  auto Amy = graph->add_node("Person",{{"name", boost::any(std::string("Amy"))}});
  auto Charles = graph->add_node("Person",{{"name", boost::any(std::string("Charles"))}});
  auto Doug = graph->add_node("Person",{{"name", boost::any(std::string("Doug"))}});
  auto Mark = graph->add_node("Person",{{"name", boost::any(std::string("Mark"))}});
  auto Alice = graph->add_node("Person",{{"name", boost::any(std::string("Alice"))}});
  auto Bridget = graph->add_node("Person",{{"name", boost::any(std::string("Bridget"))}});
  auto Michael = graph->add_node("Person",{{"name", boost::any(std::string("Michael"))}});

  graph->add_relationship(James,David,"FOLLOWS",{});
  graph->add_relationship(David, Amy, "FOLLOWS",{});
  graph->add_relationship(Charles,Doug,"FOLLOWS",{});
  graph->add_relationship(Doug,Mark,"FOLLOWS",{});
  graph->add_relationship(Mark,Doug,"FOLLOWS",{});
  graph->add_relationship(Mark,Alice,"FOLLOWS",{});
  graph->add_relationship(Alice,Doug,"FOLLOWS",{});
  graph->add_relationship(Alice,Charles,"FOLLOWS",{});
  graph->add_relationship(Alice,Michael,"FOLLOWS",{});
  graph->add_relationship(Alice,Bridget,"FOLLOWS",{});
  graph->add_relationship(Bridget,Alice,"FOLLOWS",{});
  graph->add_relationship(Bridget,Doug,"FOLLOWS",{});
  graph->add_relationship(Bridget,Michael,"FOLLOWS",{});
  graph->add_relationship(Michael,Bridget,"FOLLOWS",{});
  graph->add_relationship(Michael,Alice,"FOLLOWS",{});
  graph->add_relationship(Michael,Doug,"FOLLOWS",{});

  graph->commit_transaction();
  pool->close();
}

void pageRank_medium_test_graph(){
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("PageRank_medium_Test");

  auto tx = graph->begin_transaction();

  auto A = graph->add_node("Node",{{"name", boost::any(std::string("A"))}});
  auto B = graph->add_node("Node",{{"name", boost::any(std::string("B"))}});
  auto C = graph->add_node("Node",{{"name", boost::any(std::string("C"))}});
  auto D = graph->add_node("Node",{{"name", boost::any(std::string("D"))}});
  auto E = graph->add_node("Node",{{"name", boost::any(std::string("E"))}});

  graph->add_relationship(A,B,"KNOW",{}); 
  graph->add_relationship(A,C,"KNOW",{}); 
  graph->add_relationship(B,A,"KNOW",{});
  graph->add_relationship(C,B,"KNOW",{});
  graph->add_relationship(C,D,"KNOW",{});

  graph->commit_transaction();
  pool->close();
}

void pageRank_small_test_graph(){
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("PageRank_small_Test");

  auto tx = graph->begin_transaction();

  auto A = graph->add_node("Node",{{"name", boost::any(std::string("A"))}});
  auto B = graph->add_node("Node",{{"name", boost::any(std::string("B"))}});
  auto C = graph->add_node("Node",{{"name", boost::any(std::string("C"))}});

  graph->add_relationship(A,B,"KNOW",{}); 
  graph->add_relationship(A,C,"KNOW",{}); 
  graph->add_relationship(B,A,"KNOW",{});
  graph->add_relationship(C,B,"KNOW",{});

  graph->commit_transaction();
  pool->close();
}

void expand_test_graph(){
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->open_graph("Label_Prop_Test");

  auto tx = graph->begin_transaction();

  auto Q = graph->add_node("Node",{{"name", boost::any(std::string("Q"))}});
  auto R = graph->add_node("Node",{{"name", boost::any(std::string("R"))}});
  auto S = graph->add_node("Node",{{"name", boost::any(std::string("S"))}});
  auto T = graph->add_node("Node",{{"name", boost::any(std::string("T"))}});
  auto K = graph->add_node("Node",{{"name", boost::any(std::string("K"))}});

  graph->add_relationship(Q,R,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(Q,S,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(R,S,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(K,S,"KNOW",{});

  graph->commit_transaction();
  pool->close();

}

void create_labelprop_testgraph(){
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("Label_Prop_Test");

  auto tx = graph->begin_transaction();

  auto A = graph->add_node("Node",{{"name", boost::any(std::string("A"))}});
  auto B = graph->add_node("Node",{{"name", boost::any(std::string("B"))}});
  auto C = graph->add_node("Node",{{"name", boost::any(std::string("C"))}});
  auto D = graph->add_node("Node",{{"name", boost::any(std::string("D"))}});
  auto E = graph->add_node("Node",{{"name", boost::any(std::string("E"))}});
  auto F = graph->add_node("Node",{{"name", boost::any(std::string("F"))}});
  auto G = graph->add_node("Node",{{"name", boost::any(std::string("G"))}});
  auto H = graph->add_node("Node",{{"name", boost::any(std::string("H"))}});
  auto I = graph->add_node("Node",{{"name", boost::any(std::string("I"))}});
  auto J = graph->add_node("Node",{{"name", boost::any(std::string("J"))}});
  auto L = graph->add_node("Node",{{"name", boost::any(std::string("L"))}});
  auto M = graph->add_node("Node",{{"name", boost::any(std::string("M"))}});
  auto N = graph->add_node("Node",{{"name", boost::any(std::string("N"))}});
  auto O = graph->add_node("Node",{{"name", boost::any(std::string("O"))}});
  auto P = graph->add_node("Node",{{"name", boost::any(std::string("P"))}});

  graph->add_relationship(A,B,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(A,D,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(B,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(C,A,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(C,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(D,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(D,E,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,A,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,C,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(F,G,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(G,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(G,I,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(H,G,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(H,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(I,H,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(J,I,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(L,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(L,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(M,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(M,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(N,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(N,P,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(O,P,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(O,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(P,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(P,M,"KNOW",{{"values", boost::any(3)}});

  graph->commit_transaction();
  pool->close();
}

void create_different_label_testgraph(){
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("Diff_Label_Test");

  auto tx = graph->begin_transaction();

  auto A = graph->add_node("Node1",{{"name", boost::any(std::string("A"))}});
  auto B = graph->add_node("Node2",{{"name", boost::any(std::string("B"))}});
  auto C = graph->add_node("Node3",{{"name", boost::any(std::string("C"))}});
  auto D = graph->add_node("Node1",{{"name", boost::any(std::string("D"))}});
  auto E = graph->add_node("Node2",{{"name", boost::any(std::string("E"))}});
  auto F = graph->add_node("Node3",{{"name", boost::any(std::string("F"))}});
  auto G = graph->add_node("Node1",{{"name", boost::any(std::string("G"))}});
  auto H = graph->add_node("Node2",{{"name", boost::any(std::string("H"))}});
  auto I = graph->add_node("Node3",{{"name", boost::any(std::string("I"))}});
  auto J = graph->add_node("Node1",{{"name", boost::any(std::string("J"))}});
  auto L = graph->add_node("Node2",{{"name", boost::any(std::string("L"))}});
  auto M = graph->add_node("Node3",{{"name", boost::any(std::string("M"))}});
  auto N = graph->add_node("Node1",{{"name", boost::any(std::string("N"))}});
  auto O = graph->add_node("Node2",{{"name", boost::any(std::string("O"))}});
  auto P = graph->add_node("Node3",{{"name", boost::any(std::string("P"))}});

  graph->add_relationship(A,B,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(A,D,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(B,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(C,A,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(C,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(D,C,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(D,E,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,A,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(E,C,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(F,G,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(G,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(G,I,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(H,G,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(H,J,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(I,H,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(J,I,"KNOW",{{"values", boost::any(5)}});
  graph->add_relationship(L,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(L,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(M,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(M,N,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(N,O,"KNOW",{{"values", boost::any(3)}});
  graph->add_relationship(N,P,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(O,P,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(O,F,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(P,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(P,M,"KNOW",{{"values", boost::any(3)}});

  graph->commit_transaction();
  pool->close();
}


  /*
  for(size_t i=0; i<matrix.size(); i++){
    for (size_t s=0; s<matrix[i].size(); s++){
      std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
    }
    std::cout << std::endl;
  }
  */

void create_bigGraph(){
  int node_count = 0;
  node::id_t to_node = 0;
  bool already_in_use = false;

  std::random_device rd;  
  std::mt19937 rng(rd());
  graph_db::mapping_t id_map;

  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->create_graph("Big_Graph_Test");

  auto tx = graph->begin_transaction();
  
  graph->import_typed_nodes_from_csv("Place","/home/jan/Dokumente/poseidon_core/test/places.csv", '|', id_map);

  graph->nodes([&] (node& n){node_count++;});
  graph->nodes([&] (node& n){
    for(int i=0;i<20;i++){
      std::uniform_int_distribution<int> dist(0,node_count-1);
      to_node = dist(rng);
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false;  
    }
  });

  graph->commit_transaction();
  pool->close();
}