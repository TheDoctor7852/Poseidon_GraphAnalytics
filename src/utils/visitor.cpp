#include "visitor.hpp"

/*
    wird ausgeführt, sollte die boost::variant vom Typ: node* sein
*/
void utils::Visitor::operator() (node*) const{
    std::cout << "node" << std::endl;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: relationship* sein
*/
void utils::Visitor::operator() (relationship* r) const {
        std::cout << "relationship" << std::endl;
    }

/*
    wird ausgeführt, sollte die boost::variant vom Typ: integer sein
*/
void utils::Visitor::operator() (int & i) {
   mem = i;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: double sein
*/
void utils::Visitor::operator() (double & d) {
    mem = d;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: string sein
*/
void utils::Visitor::operator() (std::string & s) {
    mem = s;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: uint64_t sein
*/
void utils::Visitor::operator() (uint64_t & u64) const {
    std::cout << "uint64_t" << std::endl;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: ptime sein
*/
void utils::Visitor::operator() (boost::posix_time::ptime & time) const {
    std::cout << "time" << std::endl;
}

/*
    wird ausgeführt, sollte die boost::variant vom Typ: null_t sein
*/
void utils::Visitor::operator() (null_t & nullt) const {
    std::cout << "null_t" << std::endl;
}

/*
    Versucht das gespeicherte boost::any Objekt in einen String umzuwandeln.
    Bei Erfolg wird ein returnString Object mit success=true und content=(der String) zurückgegeben
    Bei Misserfolg wird ein returnString Object mit success=false und content=(die Exception) zurückgegeben
*/
utils::returnString utils::Visitor::getMemString(){ 
    try{
        return returnString(true, boost::any_cast<std::string>(mem));
      } catch(std::exception& e){
          return returnString(false, e.what());
      } 
    }
/*
    Versucht das gespeicherte boost::any Objekt in einen Double umzuwandeln.
    Bei Erfolg wird ein returnDouble Objekt mit success = true und content = (der Wert) zurückgegeben
    Bei Misserfolg wird ein returnDouble Objekt mit success = false und content = 0 zurückgegeben
*/
utils::returnDouble utils::Visitor::getMemDouble(){ 
    try{
        return returnDouble(true, boost::any_cast<double>(mem));
      } catch(std::exception& e){
          return returnDouble(false, 0);
      } 
    }