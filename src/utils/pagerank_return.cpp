#include "pagerank_return.hpp"

/*
    Standardkonstruktor
*/
utils::PageRankReturn::PageRankReturn(){
    used_nodes = std::vector<node::id_t>();
    pagerank = utils::PropertyTracker<double>();
}

/*
    erzeugt ein PageRankReturn Object mit den übergebenen Parametern
*/
utils::PageRankReturn::PageRankReturn(std::vector<node::id_t> input_nodes, utils::PropertyTracker<double> input_pagerank){
    used_nodes = input_nodes;
    pagerank = input_pagerank;
}