# Poseidon_GraphAnalytics
Dieses Projekt stellt eine Bibliothek mit Algorithmen, zur Analyse von Graphen, für die Graphdatenbank [Poseidon Core](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core) bereit. Aktuell stehen die folgenden Algorithmen zur Verfügung:  
- Breitensuche
- Tiefensuche
- Label Propagation
- PageRank

## Abhängigkeiten

Dieses Projekt wurde auf und für GNU/Linux entwickelt. Weitere Abhängigkeiten bestehen zu: 
- C++ Compiler der C++-17 unterstützt 
- eine gebaute Version der [Poseidon Core](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core) Datenbank
Sowie die zum Bau von [Poseidon Core](https://dbgit.prakinf.tu-ilmenau.de/code/poseidon_core) benötigten Abhängigkeiten
- [PMDK](https://github.com/pmem/pmdk/)
- [Boost](https://www.boost.org/) >= 1.60.0

## Bau des Projektes

Um das Projekt zu bauen, muss in der [makefile unter build](https://github.com/TheDoctor7852/Poseidon_GraphAnalytics/blob/main/build/makefile) das Attribut `PATH_TO_POSEIDON_CORE` geändert werden. Hierbei wird der absolute oder relative Pfad zu poseidon_core benötigt. Weiterhin müssen ggf. die Attribute `POS_COR_SRC` und `POS_COR_BUILD` angepasst werden.  
`POS_COR_SRC` benötigt den Pfad zu den Quelldateien von Poseidon (da sich dort die header befinden).  
`POS_COR_BUILD` benötigt den Pfad zu den Verzeichniss, in das Poseidon gebaut wurde.  
Wurden diese Pfade entsprechend angepasst, muss nur noch im Verzeichniss: _Poseidon_GraphAnalytics/build_ der Befehl `make` ausgeführt werden.

## Testbeispiele

In dem Verzeichniss _Poseidon_GraphAnalytics/test/src_ befindet sich die Datei [m.cpp](https://github.com/TheDoctor7852/Poseidon_GraphAnalytics/blob/main/test/src/m.cpp). Diese enthält Beispielgraphen sowie Beispiele dafür, wie die Algorithmen verwendet werden müssen. Auch hier müssen ggf. die Pfade in den Atributen `PATH_TO_POSEIDON_CORE`, `POS_COR_SRC` und `POS_COR_BUILD` angepasst werden. Danach muss im Verzeichniss _Poseidon_GraphAnalytics/test_ der Befehl `make` ausgeführt werden, um das Testbeispiel zu kompilieren.
