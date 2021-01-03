#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "pagerank.hpp"
#include"pagerank_return.hpp"

double convertBoostAnyToDouble(boost::any input){
    try{
        return boost::any_cast<double>(input);
    } catch (boost::bad_any_cast &e){ //sollte boost::any vom Typ int sein, schlägt die obere Umwandlung mit boost::bad_any_cast fehl.
        return static_cast<double>(boost::any_cast<int>(input)); // boost::any wird erst in einen integer umgewandelt um dann in eine double-Typ überführt zu werden. 
    }
}

utils::LabelReturn labelPropagation_parallel(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
  graph->parallel_nodes( [&] (node& n){
    graph->update_node(graph->node_by_id(n.id()),{{"id",boost::any((int) n.id())}});
  });
  return utils::LabelReturn();
}

utils::LabelReturn labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
  std::random_device rd;
  std::mt19937 rng(rd());

  graph->nodes( [&] (node& n){
    graph->update_node(graph->node_by_id(n.id()),{{"id",boost::any((int) n.id())}});
  });

  graph->nodes( [&] (node& n) {
    double max_value = std::numeric_limits<double>::min();
    std::vector<relationship*> vec = {};
//SCHLEIFE MIT WHILE
    graph->foreach_from_relationship_of_node(n, [&] (relationship& r) {
      double weight = 0;
      if(graph->get_rship_description(r.id()).has_property(property)){
        weight = convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property));
      } else{
        // sollte die Kante nicht die Property haben, wird der übergebene default_value verwendet 
        weight = default_value; 
      }
      if(weight > max_value){
        max_value = weight;
        vec = {&r};
      } else if(weight == max_value){
        vec.push_back(&r);
      }
    });

    if(vec.size() > 1){
      std::uniform_int_distribution<int> dist(0,vec.size()-1);
      graph->update_node(graph->node_by_id(n.id()),{{"id",boost::any(graph->get_node_description(vec[dist(rng)]->to_node_id()).properties.at("id"))}});
    }else if(vec.size() == 1){
      graph->update_node(graph->node_by_id(n.id()),{{"id",boost::any(graph->get_node_description(vec[0]->to_node_id()).properties.at("id"))}});
    }
  });
  return utils::LabelReturn();
}

int main(){

  auto pool = graph_pool::open("./graph/pool"); 
  //auto graph = pool->open_graph("Big_Graph_Test"); 
  auto graph = pool->open_graph("Label_Prop_Test");

    auto tx = graph->begin_transaction();

    auto start2 = std::chrono::high_resolution_clock::now();
    utils::LabelReturn label = labelPropagation_parallel(graph,"values",1.0,true,500);
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

    result_set rs;

    auto q = query(graph)
                .all_nodes("Node")
                .project({PExpr_(0, builtin::int_property(res, "id"))})
                .collect(rs);
    q.start();
    rs.wait();

    std::cout << rs.data.size() << std::endl;

    size_t s=rs.data.size();

    for(size_t i=0; i < s; i++){
      std::cout << rs.data.front()[0] << std::endl;
      rs.data.pop_front();
    }

    graph->abort_transaction();
   
  return 0;
}