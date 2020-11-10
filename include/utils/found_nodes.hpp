#ifndef FOUND_NODES_HPP
#define FOUND_NODES_HPP

#include <vector>

namespace utils{

    /*
        Verwaltet die bereits von den Suchalgorithmen gefundene Knoten.
        Ist ein Knoten unentdeckt, so ist der Wert im Vector an der Stelle seiner id false.
        Ist ein Knoten entdeckt, so ist der Wert im Vector an der Stelle seiner id true.
    */
    struct FoundNodes{

        /*
            wurde ein Knoten gefunden, so ist der Wert an der Stelle true sonst false
        */
        std::vector<bool> found_nodes;

        /*
            initialisiert den Vector mit der gegebenen Länge. Alle Einträge sind zu Begin false.
        */
        FoundNodes(int number_of_nodes);
    };
    
}

#endif //FOUND_NODES_HPP