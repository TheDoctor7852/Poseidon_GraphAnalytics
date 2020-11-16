#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "number_from_nodestring.hpp"

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

int main(){
  test();
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
  }*/