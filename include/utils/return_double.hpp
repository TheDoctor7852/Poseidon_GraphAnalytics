#ifndef RETURN_DOUBLE_HPP
#define RETURN_DOUBLE_HPP


    namespace utils{

        /*
            Zur Verwendung mit dem Visitor gedacht. 
            Gibt Auskunft darüber, ob die Umwandlung des boost::variant Objektes in einen Double funktioniert hat.
        */
        struct returnDouble {
            // gibt Auskunft, ob das auslesen erfolgreich war.
            bool success;

            /* 
                sollte das Auslesen erfolgreich gewesen sein, dann ist hier der ensprechende Double gespeichert.
                sollte das Auslesen nicht erfolgreich gesesen sein, wird hier 0 gespeichert.
            */
            double content;

            returnDouble(bool suc, double con);
        };

    }

#endif //RETURN_DOUBLE_HPP