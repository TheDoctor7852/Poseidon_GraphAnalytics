#ifndef LABEL_RETURN_HPP
#define LABEL_RETURN_HPP

#include <vector>

#include "graph_pool.hpp"
#include "property_tracker.hpp"

    namespace utils{
        
        /*
            Ist eine Struct die die ID's der verwendeten Knoten sowie deren Label verwaltet.
        */
        struct LabelReturn{

            //enthält die Knoten, die beim Label Prozess verwendet wurden. Das sind alle im Graph enthaltenen Knoten.
            std::vector<node::id_t> used_nodes;

            /*
                enthält die Label der jeweiligen Knoten. 
                Zugriff auf das label des Knoten k: label[node_id_von_k]
            */
            PropertyTracker<node::id_t> label;

            /*
                Standardkonstruktor
            */
            LabelReturn();

            /*
                initialisiert das Objekt mit dem gegebenen Vector der ID's der benutzten Knoten, sowie den dazu gehörenden Labels in einem PropertyTracker Objekt.
            */
            LabelReturn(std::vector<node::id_t> nodes, PropertyTracker<node::id_t> l);
        };
    }
#endif // LABEL_RETURN_HPP