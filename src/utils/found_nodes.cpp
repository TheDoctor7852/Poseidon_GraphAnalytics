#include "found_nodes.hpp"

utils::FoundNodes::FoundNodes(int number_of_nodes){
    found_nodes = std::vector<bool>(number_of_nodes, false);
};