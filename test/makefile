CXX_FLAGS := -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused -pthread  -Wall -Wno-deprecated -O3 -Wsign-compare   -std=c++17
CXX_DEFINES := -DBOOST_ALL_NO_LIB -DBOOST_DATE_TIME_DYN_LINK -DHAS_BOOST -DUSE_LOGGING -DUSE_PMDK -DUSE_TX
OBJDIR := obj_files
SRCDIR := src
POS_COR_INC := ../poseidon_core/include
INC := -I$(POS_COR_INC) -I$(POS_COR_INC)/btree -I$(POS_COR_INC)/dict -I$(POS_COR_INC)/ldbc -I$(POS_COR_INC)/log -I$(POS_COR_INC)/pool -I$(POS_COR_INC)/py -I$(POS_COR_INC)/qop -I$(POS_COR_INC)/storage -I$(POS_COR_INC)/tx -I$(POS_COR_INC)/_deps/spdlog-src/include -I$(POS_COR_INC)/_deps/csv-src -I$(POS_COR_INC)/_deps/catch-src/single_include/catch2
LINKER_COMMANDS := -Wl,-rpath,"/home/jan/Dokumente/Bachelorarbeit/poseidon_core/build" -L/home/jan/Dokumente/Bachelorarbeit/poseidon_core/build -lposeidon_core -lpmemobj /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 -lpthread

test_m: $(OBJDIR)/m.o
	g++ -g $(CXX_FLAGS) $(OBJDIR)/m.o -o test_m $(LINKER_COMMANDS) -L../build -lgraph_analytics -Wl,-rpath,"../build"

$(OBJDIR)/m.o: $(SRCDIR)/m.cpp
	g++ -g $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -I../include/analytics -I../include/utils -o $@ -c $(SRCDIR)/m.cpp