#ifndef LABEL_PROPAGATION_HPP
#define LABEL_PROPAGATION_HPP

#include <algorithm>
#include <limits>
#include <random>

#include "label_return.hpp"
#include "number_from_nodestring.hpp"
#include "property_tracker.hpp"
#include "query.hpp"
#include "relationship_weight.hpp"
#include "visitor.hpp"

namespace analytics{

    /*
        Bekommt den Graph sowie optional die maximale Anzahl an Runden übergeben.
        Wählt jede Runde zufällig einen Nachfolgeknoten aus, dessen Label übernommen wird.
    */
    utils::LabelReturn labelPropagation(graph_db_ptr& graph, int max_runs = 50);

    /*
       Bekommt den Graph, eine Property (Eigenschaft) der Relationships, einen default Wert, einen optionalen Parameter max sowie optional die maximale Anzahl an Runden übergeben.
       Die Property bestimmt welches Attribut der Relationships zur Auswertung verwendet wird.
       Sollte eine Relationship diese Property nicht besitzen, wird stadessen der default_value verwendet.
       max gibt an ob nach maximalen oder minimalen Gewichten gesucht werden soll. (max == true) => maximale Kantengewichte, (max == false) => inimale Kantengewichte
    */
    utils::LabelReturn labelPropagation(graph_db_ptr& graph, std::string property, double default_value, bool max = true, int max_runs = 50);

}

#endif // LABEL_PROPAGATION_HPP