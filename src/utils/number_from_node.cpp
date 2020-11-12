#include "number_from_nodestring.hpp"

/*
    bekommt die Repräsentation eines Knotens als String (diese Repräsentation kann mittels einer query erzeugt werden)
    und gibt diese dann als integer zurück.
*/
int utils::readNumberFromString(std::string node_desc){
    int i= 5;
    std::string number="";
    do{
        number += node_desc[i];
        i++;
    } while(node_desc[i] != ']');
  return std::stoi(number);
}