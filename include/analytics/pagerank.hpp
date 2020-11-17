#ifndef PAGERANK_HPP
#define PAGERANK_HPP

#include <cmath>
#include <limits>

#include "pagerank_return.hpp"

    namespace analytics{

        /*
            Führt einen PageRank über den gegeben Graphen aus. Optional können ein damping_factor, eine Schranke, die Anzahl an Maximalen Itterationen sowie ein Parameter der bestimmt ob teleportation erlaubt ist.
            Für den damping_factor d muss gelten: 0 <= d <= 1. Er bestimmt die Warscheinlichkeit, dass auf dem Knoten keiner Kante gefolgt wird und statdessen eine zufälliger Knoten gewählt wird. Im paper wurde für diesen ein Wert 0.85 empfohlen.
            Die Schranke gibt an, welcher maximale Abstand des alten PageRank zum neuen PageRank (eine Iteration) erlaubt ist. Bei einer Schranke von 0.1 ist somit die Änderung aller Werte im Vergleich zum vorherigen Wert <= 0.1
            max_turns gibt die maximalen iterationen an.
            teleportation gibt an, ob bei knoten ohne Ausgangskante eine Kante zu jedem Knoten angenommen werden soll. -> Je nachdem ob dies an oder ausgeschalten ist, können Ergebnisse variieren;
        */
        utils::PageRankReturn PageRank(graph_db_ptr& graph, double damping_factor = 0.85, double converge_limit = 0.1, int max_turns = 50, bool teleportation = true);
    }
#endif //PAGERANK_HPP