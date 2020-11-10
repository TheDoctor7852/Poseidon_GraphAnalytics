#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"


void test() {
  auto pool = graph_pool::open("./graph/pool"); 
  auto graph = pool->open_graph("my_graph"); 

  /*auto tx = graph->begin_transaction();
  
  namespace pj = builtin;

    result_set rs;
    query q = query(graph)
              //.nodes_where("Node", "id", [&] (auto &p) {return p.equal(4);} )
              .nodes_where("Node", "name", [&] (auto &p) {return p.equal(graph->get_code("aaa1"));} )
              //.all_nodes("Node")
              //.property("name", [&] (auto &p) {return p.equal(graph->get_code("aaa5"));})
              .property("other", [&](auto &p) { return p.equal(graph->get_code("BBB1")); }) //ist wie die WHERE clausel bei SQL bzw. Cypher
              .project({PExpr_(0, pj::string_property(res, "name")), 
                        PExpr_(0, pj::string_rep(res))}) // gibt den Konten als eine STring representation zurück. Diese begint mit Node[node_id] -> aus diesem String kann man die node id extrahieren
              .limit(3)
              .collect(rs);
    q.start();

    rs.wait(); 

    //graph->nodes_where("name", [&](auto &p) { return p.name == "aaa7"; });
    //auto &n=graph->node_by_id(rs.data.front()[0]);
    //std::cout << n.property_list << std::endl;

    if(rs.data.size() == 0){
      std::cout << "empty" << std::endl;
    }
    if(rs.data.size() == 1){
        std::cout << rs.data.front()[0] << std::endl;//boost::apply_visitor(Printer(), rs.data.front()[0]);
        std::cout << rs.data.front()[1] << std::endl;
    }
    if(rs.data.size() == 3){
        std::cout << "geschafft" << std::endl;
        std::cout << rs.data.front()[0] << std::endl;//boost::apply_visitor(Printer(), rs.data.front()[0]);
    }
    //int node_id = std::stoi(boost::any_cast<std::string>(boost::any(rs.data.front()[1])).substr(5,1));
    Determiner det;
    boost::apply_visitor(det, rs.data.front()[1]);
    if (det.getMem().success){
      std::cout << "node id: " << readNumberFromString(det.getMem().content) << std::endl;
    }
    std::vector<node::id_t> end;
    end.push_back(5);
    std::vector<relationship*> path = bfs(graph, readNumberFromString(det.getMem().content), end);
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }
    std::cout << "bfs without end:" << std::endl;
    path = bfs(graph, readNumberFromString(det.getMem().content));
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }
    std::cout << "dfs without end:" << std::endl;
    path = dfs(graph, readNumberFromString(det.getMem().content));
    for(std::size_t i=0; i<path.size(); i++){
      std::cout << path[i]->from_node_id() << "  --->  " << path[i]->to_node_id() << std::endl;
    }
    if(path.empty()){
      std::cout << "no path" << std::endl;
    }
    graph->abort_transaction();*/

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
    
    graph->abort_transaction();
}

int main(){
  test();
  return 0;
}