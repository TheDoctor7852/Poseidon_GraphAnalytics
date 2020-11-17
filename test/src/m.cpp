#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "number_from_nodestring.hpp"

#include <limits>
#include <cmath>

struct PageRankUtil{
    std::vector<node::id_t> used_nodes;
    utils::PropertyTracker<double> old_pagerank;
    utils::PropertyTracker<double> new_pagerank;
};

struct PageRankReturn{
    std::vector<node::id_t> used_nodes;
    utils::PropertyTracker<double> pagerank;

    PageRankReturn(){
      used_nodes = std::vector<node::id_t>();
      pagerank = utils::PropertyTracker<double>();
    }

    PageRankReturn(PageRankUtil input){
      used_nodes = input.used_nodes;
      pagerank = input.new_pagerank;
    }
}; 

PageRankUtil initialisePageRank(graph_db_ptr& graph){
  node::id_t active_node;
  utils::Visitor vis;
  PageRankUtil result;

    result_set rs;
    query q = query(graph)
              .all_nodes()
              .project({PExpr_(0, builtin::string_rep(res))})
              .collect(rs);

    q.start();
    
    rs.wait();

    while(!rs.data.empty()){
        boost::apply_visitor(vis, rs.data.front()[0]);
        if (vis.getMemString().success){
            active_node = utils::readNumberFromString(vis.getMemString().content);
            result.used_nodes.push_back(active_node);
        }
        rs.data.pop_front();
    }

    result.old_pagerank = utils::PropertyTracker<double>(graph->get_nodes()->as_vec().capacity(),0);
    result.new_pagerank = utils::PropertyTracker<double>(graph->get_nodes()->as_vec().capacity(),0);

    for(size_t i=0; i<result.used_nodes.size(); i++){
      result.old_pagerank.propertys[result.used_nodes[i]] = 1.0/result.used_nodes.size();
    }
  
  return result;
}
// TODO: erweiterung Fall, dass Knoten keine ausgehenden Kanten hat
PageRankReturn PageRank(graph_db_ptr& graph, double damping_factor = 0.85, double converge_limit = 0.1, int max_turns = 50){

    int counter = 0;
    double sum = 0;
    node::id_t active_node = 0;
    utils::PropertyTracker<int> num_outgoing_relationships = utils::PropertyTracker<int>(graph->get_nodes()->as_vec().capacity(),0);
    double max_difference = std::numeric_limits<double>::max();

    if((damping_factor > 1) || (damping_factor < 0)){
      std::cout << "damping factor needs to be: 0 <= damping_factor <= 1" << std::endl;
      return PageRankReturn();
    }else{
      PageRankUtil current_pagerank = initialisePageRank(graph);
      for(size_t i = 0; i < current_pagerank.used_nodes.size(); i++){
        active_node = current_pagerank.used_nodes[i];
        graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {counter++;});
        num_outgoing_relationships.propertys[active_node] = counter;
        counter = 0;
      }
      counter = 0;
      while((max_difference >= converge_limit) && (counter<max_turns)){
        max_difference = 0;
        for(size_t i = 0; i < current_pagerank.used_nodes.size(); i++){
          active_node = current_pagerank.used_nodes[i];
          graph->foreach_to_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
            //hier mit if abfragen ob num_outgoing_rel =0, wenn ja, nehme anzhal der Knoten im Graphen und teile dadurch -> Sinks behoben
            sum = sum + current_pagerank.old_pagerank.propertys[r.from_node_id()]/num_outgoing_relationships.propertys[r.from_node_id()];
          });
          current_pagerank.new_pagerank.propertys[active_node] = (1-damping_factor) + (damping_factor*sum);
          std:: cout << (1-damping_factor) + (damping_factor*sum) << "    ";
          sum = 0;
          if(std::abs(current_pagerank.new_pagerank.propertys[active_node]-current_pagerank.old_pagerank.propertys[active_node])>max_difference){
            max_difference = std::abs(current_pagerank.new_pagerank.propertys[active_node]-current_pagerank.old_pagerank.propertys[active_node]);
          }
        }
      std::cout << max_difference << std::endl;
      counter++;
      current_pagerank.old_pagerank.propertys = current_pagerank.new_pagerank.propertys;
      }
      std::cout << "Schleife durchlaufen: " << counter << std::endl;
      return PageRankReturn(current_pagerank);
    }
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

    Label_Test();
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
  graph->add_relationship(O,P,"KNOW",{{"values", boost::any(1)}});
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
  graph->add_relationship(O,P,"KNOW",{{"values", boost::any(1)}});
  graph->add_relationship(P,L,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(P,M,"KNOW",{{"values", boost::any(3)}});

  graph->commit_transaction();
  pool->close();
}

void create_pool(){
  auto pool = graph_pool::create("./graph/pool",838860800);
  pool->close();
}

int main(){
  //test();
  //create_pool();
  //pageRank_small_test_graph();
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->open_graph("PageRank_small_Test");

  auto tx = graph->begin_transaction();

  PageRankReturn r = PageRank(graph);
  for (size_t s=0; s<r.used_nodes.size(); s++){
      std::cout << graph->get_node_description(r.used_nodes[s]).properties.at("name") << "  mit Gewicht: " << r.pagerank.propertys[s] << std::endl;
    }
  graph->abort_transaction();
  pool->close();
  //create_different_label_testgraph();
  //create_labelprop_testgraph();
  //expand_test_graph();
  return 0;
}


  /*
  for(size_t i=0; i<matrix.size(); i++){
    for (size_t s=0; s<matrix[i].size(); s++){
      std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
    }
    std::cout << std::endl;
  }
  */