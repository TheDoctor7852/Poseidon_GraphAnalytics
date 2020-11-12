#ifndef PROPERTY_TRACKER_HPP
#define PROPERTY_TRACKER_HPP

#include <vector>

namespace utils{

    /*
        Dient zur Verwaltung von Eigenschaften auf den Knoten.
    */
   template<typename T>
    struct PropertyTracker{

        /*
            ist der Vector vom Typ T. Hier werden die Eigenschaften gespeichert,
            und es kann mittels der node_id auf die gespeicherten Eigenschaften zugegriffen werden.
        */
        std::vector<T> propertys;

        /*
            Standard Konstruktor. Initialisiert einen leeren Vector vom Typ T.
        */
        PropertyTracker();

        /*
            initialisiert den Vector mit der gegebenen Länge. Alle Einträge sind zu Begin der übergebene default_value.
        */
        PropertyTracker(int number_of_nodes, T default_value);
    };

    /*
        Standard Konstruktor. Initialisiert einen leeren Vector vom Typ T.
    */
    template<typename T>
    PropertyTracker<T>::PropertyTracker(){
        propertys = std::vector<T>();
    }

    /*
    initialisiert den Vector mit der gegebenen Länge. Alle Einträge sind zu Begin der übergebene default_value.
    */
    template<typename T>
    PropertyTracker<T>::PropertyTracker(int number_of_nodes, T default_value){
        propertys = std::vector<T>(number_of_nodes, default_value);
    } 
}

#endif //PROPERTY_TRACKER_HPP