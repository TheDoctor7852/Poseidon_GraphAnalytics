#ifndef RELATIONSHIP_WEIGHT_HPP
#define RELATIONSHIP_WEIGHT_HPP

#include "graph_pool.hpp"

    namespace utils{

        /*
            Ist eine Struct, die Relationships(Kanten) ein Gewicht zuordnet.
        */
        struct RelationshipWeight{

            //enthält eine Relationship
            relationship* rel;

            //enthält das Gewicht einer Property der enthaltenen Relationship
            double weight;

            /*
                erzeugt ein RelationshipWeight Objekt mit der übergebenen Relationship und dem dazugehörendem Gewicht.
            */
            RelationshipWeight(relationship* r, double w);
        };
    }
#endif // RELATIONSHIP_WEIGHT_HPP