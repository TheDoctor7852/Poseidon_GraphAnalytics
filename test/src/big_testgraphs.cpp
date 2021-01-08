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

void create_5000node_graph();
void create_10000node_graph();
void create_20000node_graph();
void create_30000node_graph();
void create_40000node_graph();
void create_50000node_graph();

int main(){
    //create_5000node_graph();
    create_10000node_graph();
    //create_20000node_graph();
    //create_30000node_graph();
    //create_40000node_graph();
    //create_50000node_graph();
}

void create_10000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/10000nodeGraph",2000000000);
    auto graph = pool->create_graph("10000nodeGraph");



    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<10000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->commit_transaction();

    std::cout << "Nodes done" << std::endl;

  for (int s=0; s<10000; s++){

    auto n_id = nodes[s]; 

    auto tx2 = graph->begin_transaction();
     
    for(int i=0;i<200;i++){
      std::uniform_int_distribution<int> dist_node(0,9999);
      std::uniform_int_distribution<int> dist_weight(0,99);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */  
      
      if(to_node != n_id){
        graph->add_relationship(n_id,to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
    std::cout << "turn " << n_id << " executed" << std::endl;   
    graph->commit_transaction();
  } 
}

void create_5000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/5000nodeGraph",838860800);
    auto graph = pool->create_graph("5000nodeGraph");

    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<5000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->nodes([&] (node& n){
    for(int i=0;i<50;i++){
      std::uniform_int_distribution<int> dist_node(0,4999);
      std::uniform_int_distribution<int> dist_weight(0,49);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */
      if(to_node != n.id()){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
  });

    graph->commit_transaction();
}

void create_20000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/20000nodeGraph",2500000000);
    auto graph = pool->create_graph("20000nodeGraph");



    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<20000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->commit_transaction();

    std::cout << "Nodes done" << std::endl;

  for (int s=0; s<20000; s++){

    auto n_id = nodes[s]; 

    auto tx2 = graph->begin_transaction();
     
    for(int i=0;i<200;i++){
      std::uniform_int_distribution<int> dist_node(0,19999);
      std::uniform_int_distribution<int> dist_weight(0,199);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */  
      
      if(to_node != n_id){
        graph->add_relationship(n_id,to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
    std::cout << "turn " << n_id << " executed" << std::endl;   
    graph->commit_transaction();
  } 
}

void create_30000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/30000nodeGraph",3000000000);
    auto graph = pool->create_graph("30000nodeGraph");



    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<30000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->commit_transaction();

    std::cout << "Nodes done" << std::endl;

  for (int s=0; s<30000; s++){

    auto n_id = nodes[s]; 

    auto tx2 = graph->begin_transaction();
     
    for(int i=0;i<300;i++){
      std::uniform_int_distribution<int> dist_node(0,29999);
      std::uniform_int_distribution<int> dist_weight(0,299);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */  
      
      if(to_node != n_id){
        graph->add_relationship(n_id,to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
    std::cout << "turn " << n_id << " executed" << std::endl;   
    graph->commit_transaction();
  } 
}

void create_40000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/40000nodeGraph",3500000000);
    auto graph = pool->create_graph("40000nodeGraph");



    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<40000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->commit_transaction();

    std::cout << "Nodes done" << std::endl;

  for (int s=0; s<40000; s++){

    auto n_id = nodes[s]; 

    auto tx2 = graph->begin_transaction();
     
    for(int i=0;i<400;i++){
      std::uniform_int_distribution<int> dist_node(0,39999);
      std::uniform_int_distribution<int> dist_weight(0,399);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */  
      
      if(to_node != n_id){
        graph->add_relationship(n_id,to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
    std::cout << "turn " << n_id << " executed" << std::endl;   
    graph->commit_transaction();
  } 
}

void create_50000node_graph(){
    std::vector<node::id_t> nodes = {};
    node::id_t to_node = 0;
    bool already_in_use = false;

    std::random_device rd;  
    std::mt19937 rng(rd());

    auto pool = graph_pool::create("./graph/50000nodeGraph",8388608000);
    auto graph = pool->create_graph("50000nodeGraph");



    auto tx = graph->begin_transaction();
    
    for(int i = 0; i<50000; i++){
        nodes.push_back(graph->add_node("Node", {{"id", boost::any(i)}}));
    };

    graph->commit_transaction();

    std::cout << "Nodes done" << std::endl;

  for (int s=0; s<50000; s++){

    auto n_id = nodes[s]; 

    auto tx2 = graph->begin_transaction();
     
    for(int i=0;i<500;i++){
      std::uniform_int_distribution<int> dist_node(0,49999);
      std::uniform_int_distribution<int> dist_weight(0,499);
      to_node = dist_node(rng);
      /*
      graph->foreach_from_relationship_of_node(graph->node_by_id(to_node), [&] (relationship& r){
        if(r.to_node_id() == n.id()){already_in_use = true;}
      });
      if((to_node != n.id()) && !already_in_use){
        graph->add_relationship(n.id(),to_node,"KNOW",{{"values", boost::any(3)}});
      }
      already_in_use = false; */  
      
      if(to_node != n_id){
        graph->add_relationship(n_id,to_node,"KNOW",{{"values", boost::any(dist_weight(rng))}});
      }
    }
    std::cout << "turn " << n_id << " executed" << std::endl;   
    graph->commit_transaction();
  } 
}