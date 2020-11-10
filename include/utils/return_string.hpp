#ifndef RETURN_STRING_HPP
#define RETURN_STRING_HPP

#include <string>

    namespace utils{

        /*
            Zur Verwendung mit dem Visitor gedacht. 
            Gibt Auskunft darüber, ob die Umwandlung des boost::variant objectes in einen String funktioniert hat.
        */
        struct returnString {
            // gibt Auskunft, ob das auslesen erfolgreich war.
            bool success;

            /* 
                sollte das Auslesen erfolgreich gewesen sein, dann ist hier der ensprechende String gespeichert.
                sollte das Auslesen nicht erfolgreich gesesen sein, wird hier die enstprechende exception gespeichert werden.
            */
            std::string content;

            returnString(bool suc, std::string con);
        };

    }

#endif