#ifndef NUMBER_FROM_NODESTRING_HPP
#define NUMBER_FROM_NODESTRING_HPP

#include <string>

    namespace utils{

        /*
            bekommt die Repräsentation eines Knotens als String (diese Repräsentation kann mittels einer query erzeugt werden)
            und gibt diese dann als integer zurück.
        */
        int readNumberFromString(std::string node_desc);
    }
#endif