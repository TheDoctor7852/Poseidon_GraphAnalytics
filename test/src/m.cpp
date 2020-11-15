#include <vector>

#include "qop.hpp"
#include "graph_pool.hpp"
#include "bfs_.hpp"
#include "dfs.hpp"

#include "property_tracker.hpp"
#include "number_from_nodestring.hpp"
#include "visitor.hpp"
#include "query.hpp"
#include <algorithm>
#include <random>
#include <limits>

double convertBoostAnyToDouble(boost::any input){
  try{
    return boost::any_cast<double>(input);
  } catch (boost::bad_any_cast &e){
    return static_cast<double>(boost::any_cast<int>(input));
  }
}

struct LabelReturn{
  std::vector<node::id_t> used_nodes;
  utils::PropertyTracker<node::id_t> label; // hierführ wird Standarkonstruktor gebraucht oder: LabelReturn(std::vector<node::id_t> nodes, utils::PropertyTracker<node::id_t> l) : label(0,0) {}

  LabelReturn(){
    used_nodes = std::vector<node::id_t>();
    label = utils::PropertyTracker<node::id_t>();
  }

  LabelReturn(std::vector<node::id_t> nodes, utils::PropertyTracker<node::id_t> l){
    used_nodes = nodes;
    label = l;
  }
};

struct RelationshipWeight{
  relationship* rel;
  double weight;

  RelationshipWeight(relationship* r, double w){
    rel = r;
    weight = w;
  }
};

std::vector<RelationshipWeight> determin_max_values(std::vector<RelationshipWeight>& from_node){
  std::vector<RelationshipWeight> result = {};
  bool found_max_last_round = true;
  RelationshipWeight max = RelationshipWeight(nullptr,std::numeric_limits<double>::min());
  int position_of_max_ele = 0;

  while ((found_max_last_round) && (!from_node.empty())){
    found_max_last_round = false;
    auto max_entry = std::max_element(from_node.begin(),from_node.end(),[] (const RelationshipWeight &a, const RelationshipWeight &b) {
      return a.weight < b.weight;
    });
    position_of_max_ele = std::distance(from_node.begin(), max_entry);
    if(max.weight<=from_node[position_of_max_ele].weight){
      max = from_node[position_of_max_ele];
      result.push_back(max);
      found_max_last_round = true;
      from_node.erase(from_node.begin() + position_of_max_ele);
    }
  }
  return result;
};

std::vector<RelationshipWeight> determin_min_values(std::vector<RelationshipWeight>& from_node){
  std::vector<RelationshipWeight> result = {};
  bool found_min_last_round = true;
  RelationshipWeight min = RelationshipWeight(nullptr,std::numeric_limits<double>::max());
  int position_of_min_ele = 0;

  while ((found_min_last_round) && (!from_node.empty())){
    found_min_last_round = false;
    auto min_entry = std::min_element(from_node.begin(),from_node.end(),[] (const RelationshipWeight &a, const RelationshipWeight &b) {
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

std::vector<std::vector<RelationshipWeight>> create_max_weight_matrix(graph_db_ptr& graph, std::vector<node::id_t>& nodes, std::string property, double default_value){
  node::id_t active_node = 0;
  std::vector<std::vector<RelationshipWeight>> result = {};
  std::vector<RelationshipWeight> from_node = {};
  std::vector<RelationshipWeight> ret = {};

  for(size_t i=0; i<nodes.size(); i++){
    node::id_t active_node = nodes[i];
    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
        if(graph->get_rship_description(r.id()).has_property(property)){
            from_node.push_back(RelationshipWeight(&r,convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
        } else{
          from_node.push_back(RelationshipWeight(&r,default_value)); // sollte die Kante nicht die Property haben, wird der default_value verwendet 
        }
      });
    result.push_back(determin_max_values(from_node));
    from_node.clear();
  }
  return result;
};

std::vector<std::vector<RelationshipWeight>> create_min_weight_matrix(graph_db_ptr& graph, std::vector<node::id_t>& nodes, std::string property, double default_value){
  node::id_t active_node = 0;
  std::vector<std::vector<RelationshipWeight>> result = {};
  std::vector<RelationshipWeight> from_node = {};
  std::vector<RelationshipWeight> ret = {};

  for(size_t i=0; i<nodes.size(); i++){
    node::id_t active_node = nodes[i];
    graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
        if(graph->get_rship_description(r.id()).has_property(property)){
            from_node.push_back(RelationshipWeight(&r,convertBoostAnyToDouble(graph->get_rship_description(r.id()).properties.at(property))));
        } else{
          from_node.push_back(RelationshipWeight(&r,default_value)); // sollte die Kante nicht die Property haben, wird der default_value verwendet 
        }
      });
    result.push_back(determin_min_values(from_node));
    from_node.clear();
  }
  return result;
};

LabelReturn labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max = true, int max_runs = 50){
  //benutze trozdem mal das, da, wenn Knoten löscht eine leere Stelle da ist. -> alle nachfolgenden Knoten sind um 1 nach vorne gerückt
  utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
  std::vector<node::id_t> nodes = {};
  std::vector<RelationshipWeight> active_vector = {};
  std::random_device rd;
  std::mt19937 rng(rd());
  utils::Visitor vis;
  node::id_t active_node;
  RelationshipWeight active_relationship = RelationshipWeight(nullptr,0);
  bool did_change_last_run = true;
  int number_of_turns = 0;

  std::vector<std::vector<RelationshipWeight>> matrix;

  result_set rs;
  query q = query(graph)
              .all_nodes("Node")
              .project({PExpr_(0, builtin::string_rep(res))})
              .collect(rs);
    q.start();
    rs.wait();
  while(!rs.data.empty()){
    boost::apply_visitor(vis, rs.data.front()[0]);
    if (vis.getMemString().success){
        active_node = utils::readNumberFromString(vis.getMemString().content);
        nodes.push_back(active_node);
      }
    rs.data.pop_front();
  }
  for(size_t i=0; i<nodes.size(); i++){
    label.propertys[i] = nodes[i];
  }

  if(max){
    matrix = create_max_weight_matrix(graph,nodes,property,default_value);
  } else{
    matrix = create_min_weight_matrix(graph,nodes,property,default_value);
  }
  
  for(size_t i=0; i<matrix.size(); i++){
    for (size_t s=0; s<matrix[i].size(); s++){
      std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
    }
    std::cout << std::endl;
  }

  while(did_change_last_run && (number_of_turns < max_runs)){
    std::random_shuffle(matrix.begin(), matrix.end());
    did_change_last_run = false;
    for(size_t i=0; i< matrix.size(); i++){
      active_vector = matrix[i];
      if (active_vector.size()>1){
        std::uniform_int_distribution<int> dist(0,active_vector.size()-1);
        active_relationship = active_vector[dist(rng)];
        if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
          did_change_last_run = true;
          label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
        }
      }else if(active_vector.size() == 1){
        active_relationship = active_vector[0];
        if(label.propertys[active_relationship.rel->from_node_id()] != label.propertys[active_relationship.rel->to_node_id()]){
          did_change_last_run = true;
          label.propertys[active_relationship.rel->from_node_id()] = label.propertys[active_relationship.rel->to_node_id()];
        }
      }
    }
    number_of_turns++;
  }

  std::cout << "Schleife durchlaufen: " << number_of_turns << std::endl;
  return LabelReturn(nodes, label);
}

LabelReturn labelPropagation(graph_db_ptr& graph, int max_runs = 50){
  //benutze trozdem mal das, da, wenn Knoten löscht eine leere Stelle da ist. -> alle nachfolgenden Knoten sind um 1 nach vorne gerückt
  utils::PropertyTracker<node::id_t> label = utils::PropertyTracker<node::id_t>(graph->get_nodes()->as_vec().capacity(),0);
  std::vector<node::id_t> nodes = {};
  std::vector<relationship*> from_node = {};
  std::random_device rd;
  std::mt19937 rng(rd());
  utils::Visitor vis;
  node::id_t active_node;
  relationship* active_relationship = nullptr;
  bool did_change_last_run = true;
  int number_of_turns = 0;

  result_set rs;
  query q = query(graph)
              .all_nodes("Node")
              .project({PExpr_(0, builtin::string_rep(res))})
              .collect(rs);
    q.start();
    rs.wait();
  while(!rs.data.empty()){
    boost::apply_visitor(vis, rs.data.front()[0]);
    if (vis.getMemString().success){
        active_node = utils::readNumberFromString(vis.getMemString().content);
        nodes.push_back(active_node);
      }
    rs.data.pop_front();
  }
  for(size_t i=0; i<nodes.size(); i++){
    label.propertys[i] = nodes[i];
  }

  while(did_change_last_run && (number_of_turns < max_runs)){
    std::random_shuffle(nodes.begin(), nodes.end());
    did_change_last_run = false;
    for(size_t i=0; i< nodes.size(); i++){
      active_node = nodes[i];
      graph->foreach_from_relationship_of_node(graph->node_by_id(active_node), [&] (relationship& r) {
        from_node.push_back(&r);
      });
      if (!from_node.empty()){
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
  return LabelReturn(nodes, label);
}

void Label_Test(){
  auto pool = graph_pool::open("./graph/pool"); 
  auto graph = pool->open_graph("Label_Prop_Test"); 

  auto tx = graph->begin_transaction();

  LabelReturn result = labelPropagation(graph);
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
    
    LabelReturn result = labelPropagation(graph);
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

  graph->add_relationship(Q,R,"KNOW",{{"values", boost::any(4)}});
  graph->add_relationship(Q,S,"KNOW",{{"values", boost::any(2)}});
  graph->add_relationship(R,S,"KNOW",{{"values", boost::any(2)}});

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

int main(){
  //test();
  //create_labelprop_testgraph();
  //expand_test_graph();
  auto pool = graph_pool::open("./graph/pool");
  auto graph = pool->open_graph("Label_Prop_Test");

  auto tx = graph->begin_transaction();

  LabelReturn result = labelPropagation(graph,"values",1.0);
  for(size_t i=0; i<result.used_nodes.size(); i++){
    std::cout << "Knoten:  " << graph->get_node_description(result.used_nodes[i]).properties.at("name") << "    hat Label:    " << result.label.propertys[result.used_nodes[i]] << std::endl;
  }

  graph->abort_transaction();

  return 0;
}


  /*
  for(size_t i=0; i<matrix.size(); i++){
    for (size_t s=0; s<matrix[i].size(); s++){
      std::cout << graph->get_node_description(matrix[i][s].rel->from_node_id()).properties.at("name") << " ---> " << graph->get_node_description(matrix[i][s].rel->to_node_id()).properties.at("name")<< "  mit Gewicht: " << matrix[i][s].weight << "  ";
    }
    std::cout << std::endl;
  }*/