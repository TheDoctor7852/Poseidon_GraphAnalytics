#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"
#include "label_propagation.hpp"
#include "label_return.hpp"
#include "pagerank.hpp"
#include"pagerank_return.hpp"


void create_pool();
void create_data();
void create_different_label_testgraph();
void create_labelprop_testgraph();
void expand_test_graph();
void pageRank_small_test_graph();
void pageRank_medium_test_graph();
void PageRank_example();


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


  /*
  for(size_t i=0; i<matrix.size(); i++){
    for (size_t s=0; s<matrix[i].size(); s++){
      std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
    }
    std::cout << std::endl;
  }
  */