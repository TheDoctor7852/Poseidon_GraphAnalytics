#include "relationship_weight.hpp"

/*
    erzeugt ein RelationshipWeight Objekt mit der übergebenen Relationship und dem dazugehörendem Gewicht.
*/
utils::RelationshipWeight::RelationshipWeight(relationship* r, double w){
    rel = r;
    weight = w;
}