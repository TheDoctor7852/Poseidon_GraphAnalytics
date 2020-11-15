#include "label_return.hpp"

/*
    Standardkonstruktor
*/
utils::LabelReturn::LabelReturn(){
    used_nodes = std::vector<node::id_t>();
    label = utils::PropertyTracker<node::id_t>();
  }

/*
    initialisiert das Objekt mit dem gegebenen Vector der ID's der benutzten Knoten, sowie den dazu gehörenden Labels in einem PropertyTracker Objekt.
*/
utils::LabelReturn::LabelReturn(std::vector<node::id_t> nodes, utils::PropertyTracker<node::id_t> l){
    used_nodes = nodes;
    label = l;
  }