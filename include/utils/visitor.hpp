#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <iostream>

#include "boost/variant.hpp"
#include "boost/any.hpp"
#include "graph_pool.hpp"
#include "return_double.hpp"
#include "return_string.hpp"

    namespace utils{

        /*
            Visitor, der eine übergebene Variable von query_result(entspricht: boost::variant) auflöst
        */
        class Visitor : boost::static_visitor<>{
            
            // speichert den extrahierten Wert
            boost::any mem = 0;

            public:

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: node* sein
                */
                void operator() (node* n) const;

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: relationship* sein
                */
                void operator() (relationship* r) const;

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: integer sein
                */
                void operator() (int & i);

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: double sein
                */
                void operator() (double & d);

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: string sein
                */
                void operator() (std::string & s);

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: uint64_t sein
                */
                void operator() (uint64_t & u64) const;

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: ptime sein
                */
                void operator() (boost::posix_time::ptime & time) const;

                /*
                    wird ausgeführt, sollte die boost::variant vom Typ: null_t sein
                */
                void operator() (null_t & nullt) const;

                /*
                    Versucht das gespeicherte boost::any Objekt in einen String umzuwandeln.
                    Bei Erfolg wird ein returnString Objekt mit success = true und content = (der String) zurückgegeben
                    Bei Misserfolg wird ein returnString Objekt mit success=false und content=(die Exception) zurückgegeben
                */
                returnString getMemString();
                
                /*
                    Versucht das gespeicherte boost::any Objekt in einen Double umzuwandeln.
                    Bei Erfolg wird ein returnDouble Objekt mit success = true und content = (der Wert) zurückgegeben
                    Bei Misserfolg wird ein returnDouble Objekt mit success = false und content = 0 zurückgegeben
                */
                returnDouble getMemDouble();
        };
    }

#endif //VISITOR_HPP