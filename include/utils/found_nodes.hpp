#ifndef FOUND_NODES_HPP
#define FOUND_NODES_HPP

#include <vector>

namespace utils{

    /*
        Verwaltet die bereits von den Suchalgorithmen gefundene Knoten.
        Ist ein Knoten unentdeckt, so ist der Wert im Vector an der Stelle seiner id false.
        Ist ein Knoten entdeckt, so ist der Wert im Vector an der Stelle seiner id true.
    */
   template<typename T>
    struct FoundNodes{

        /*
            wurde ein Knoten gefunden, so ist der Wert an der Stelle true sonst false
        */
        std::vector<T> found_nodes;

        /*
            initialisiert den Vector mit der gegebenen Länge. Alle Einträge sind zu Begin der übergebene default_value.
        */
        FoundNodes(int number_of_nodes, T default_value);
    };


    /*
    initialisiert den Vector mit der gegebenen Länge. Alle Einträge sind zu Begin der übergebene default_value.
    */
    template<typename T>
    FoundNodes<T>::FoundNodes(int number_of_nodes, T default_value){
        found_nodes = std::vector<T>(number_of_nodes, default_value);
    } 
}

#endif //FOUND_NODES_HPP