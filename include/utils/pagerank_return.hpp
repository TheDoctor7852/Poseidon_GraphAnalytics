#ifndef PAGERANK_RETURN_HPP
#define PAGERANK_RETURN_HPP

#include <vector>

#include "graph_pool.hpp"
#include "property_tracker.hpp"

    namespace utils{

        /*
            Return Typ einer Page Rank Operation. Enthält die betrachteten Knoten sowie deren PageRank.
        */
        struct PageRankReturn{ 

            //enthält die in PageRank betrachteten Knoten
            std::vector<node::id_t> used_nodes;

            //enthält den PageRank der Knoten. Zugriff: pagerank.property[node_id]
            utils::PropertyTracker<double> pagerank;

            /*
                Standardkonstruktor
            */
            PageRankReturn();

            /*
                erzeugt ein PageRankReturn Object mit den übergebenen Parametern
            */
            PageRankReturn(std::vector<node::id_t> input_nodes, utils::PropertyTracker<double> input_pagerank);
        };        
    }
#endif //PAGERANK_RETURN_HPP