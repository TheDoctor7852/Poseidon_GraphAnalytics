CXX_FLAGS := -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused -pthread -fopenmp -Wall -Wno-deprecated -O3 -Wsign-compare   -std=c++17
CXX_DEFINES := -DBOOST_ALL_NO_LIB -DBOOST_DATE_TIME_DYN_LINK -DHAS_BOOST -DUSE_LOGGING -DUSE_PMDK -DUSE_TX
OBJDIR := obj_files
SRCDIR := src
PATH_TO_POSEIDON_CORE := /home/jan/Dokumente/poseidon_core#hier der totale oder abstratkte Pfad zu Poseidon
POS_COR_SRC := $(PATH_TO_POSEIDON_CORE)/src#falls das src-Verzischniss umbenannt wurde auch hier umbenennen
POS_COR_BUILD := $(PATH_TO_POSEIDON_CORE)/build#falls das build-Verzischniss umbenannt(bzw. anders benannt) wurde auch hier umbenennen
INC := -I$(POS_COR_SRC) -I$(POS_COR_SRC)/btree -I$(POS_COR_SRC)/dict -I$(POS_COR_SRC)/ldbc -I$(POS_COR_SRC)/log -I$(POS_COR_SRC)/pool -I$(POS_COR_SRC)/py -I$(POS_COR_SRC)/qop -I$(POS_COR_SRC)/storage -I$(POS_COR_SRC)/tx -I$(POS_COR_BUILD)/_deps/spdlog-src/include -I$(POS_COR_BUILD)/_deps/csv-src -I$(POS_COR_BUILD)/_deps/catch-src/single_include/catch2 -I$(POS_COR_BUILD)/generated
LINKER_COMMANDS := -Wl,-rpath,$(POS_COR_BUILD) -L$(POS_COR_BUILD) -lposeidon_core -lpmemobj /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 -lpthread

test_m: $(OBJDIR)/m.o
	g++  $(CXX_FLAGS) $(OBJDIR)/m.o -o test_m $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build"

$(OBJDIR)/m.o: $(SRCDIR)/m.cpp
	g++  $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/m.cpp

test_par: $(OBJDIR)/class_test.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/class_test.o -o test_par $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build"

$(OBJDIR)/class_test.o: $(SRCDIR)/class_test.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/class_test.cpp

test_galois: $(OBJDIR)/galois_test.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/galois_test.o -o test_par $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build" -L/home/jan/Dokumente/Bachelorarbeit/Galois/build/libgalois -lgalois_shmem

$(OBJDIR)/galois_test.o: $(SRCDIR)/galois_test.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -I/home/jan/Dokumente/Bachelorarbeit/Galois/libgalois/include/ -o $@ -c $(SRCDIR)/galois_test.cpp

test_imp: $(OBJDIR)/test_imp.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/test_imp.o -o test_imp $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build"

$(OBJDIR)/test_imp.o: $(SRCDIR)/impl_with_allnodes.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/impl_with_allnodes.cpp

big_graph: $(OBJDIR)/big_graph.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/big_graph.o -o big_graph $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build"

$(OBJDIR)/big_graph.o: $(SRCDIR)/big_testgraphs.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/big_testgraphs.cpp

test_omp: $(OBJDIR)/test_omp.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/test_omp.o -o test_omp $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build" -fopenmp

$(OBJDIR)/test_omp.o: $(SRCDIR)/with_OpenMP.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/with_OpenMP.cpp -fopenmp