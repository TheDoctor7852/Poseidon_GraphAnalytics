#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "pagerank.hpp"
#include"pagerank_return.hpp"


utils::LabelReturn labelPropagation_parallel(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
  graph->nodes( [&] (node& n){
    graph->update_node(graph->node_by_id(n.id()),{{"id",boost::any((int) n.id())}});
  });
  /*graph->update_node(graph->node_by_id(0),{{"id",boost::any(0)}});
  graph->update_node(graph->node_by_id(1),{{"id",boost::any(1)}});
  graph->update_node(graph->node_by_id(2),{{"id",boost::any(2)}});
  graph->update_node(graph->node_by_id(3),{{"id",boost::any(3)}});
  graph->update_node(graph->node_by_id(4),{{"id",boost::any(4)}});
  graph->update_node(graph->node_by_id(5),{{"id",boost::any(5)}});
  graph->update_node(graph->node_by_id(6),{{"id",boost::any(6)}});
  graph->update_node(graph->node_by_id(7),{{"id",boost::any(7)}});
  graph->update_node(graph->node_by_id(8),{{"id",boost::any(8)}});
  graph->update_node(graph->node_by_id(9),{{"id",boost::any(9)}});
  graph->update_node(graph->node_by_id(10),{{"id",boost::any(10)}});
  graph->update_node(graph->node_by_id(11),{{"id",boost::any(11)}});
  graph->update_node(graph->node_by_id(12),{{"id",boost::any(12)}});
  graph->update_node(graph->node_by_id(13),{{"id",boost::any(13)}});
  graph->update_node(graph->node_by_id(14),{{"id",boost::any(14)}});
  graph->update_node(graph->node_by_id(15),{{"id",boost::any(15)}});
  graph->update_node(graph->node_by_id(16),{{"id",boost::any(16)}});
  graph->update_node(graph->node_by_id(17),{{"id",boost::any(17)}});
  graph->update_node(graph->node_by_id(18),{{"id",boost::any(18)}});
  graph->update_node(graph->node_by_id(19),{{"id",boost::any(19)}});*/
  return utils::LabelReturn();
}

utils::LabelReturn labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max, int max_runs){
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
/*
    auto start = std::chrono::high_resolution_clock::now();
    utils::LabelReturn result = labelPropagation(graph,"values",1.0,true, 500);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start); 
  
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl; 
*/
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