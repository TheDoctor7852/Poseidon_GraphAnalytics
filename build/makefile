CXX_FLAGS := -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused -pthread  -Wall -Wno-deprecated -O3 -Wsign-compare   -std=c++17
#CXX_FLAGS := -pthread  -Wall -O3 -std=c++17
CXX_DEFINES := -DBOOST_ALL_NO_LIB -DBOOST_DATE_TIME_DYN_LINK -DHAS_BOOST -DUSE_LOGGING -DUSE_PMDK -DUSE_TX
OBJDIR := ../obj_dir
SRCDIR := ../src
PATH_TO_POSEIDON_CORE := /home/jan/Dokumente/Bachelorarbeit/poseidon_core
POS_COR_SRC := $(PATH_TO_POSEIDON_CORE)/src
POS_COR_BUILD := $(PATH_TO_POSEIDON_CORE)/build
SRCDIR_ANALYTICS := ../src/analytics
SRCDIR_UTILS := ../src/utils
INCLUDE := ../include
INC := -I$(INCLUDE)/analytics -I$(INCLUDE)/utils -I$(POS_COR_SRC) -I$(POS_COR_SRC)/btree -I$(POS_COR_SRC)/dict -I$(POS_COR_SRC)/ldbc -I$(POS_COR_SRC)/log -I$(POS_COR_SRC)/pool -I$(POS_COR_SRC)/py -I$(POS_COR_SRC)/qop -I$(POS_COR_SRC)/storage -I$(POS_COR_SRC)/tx -I$(POS_COR_BUILD)/_deps/spdlog-src/include -I$(POS_COR_BUILD)/_deps/csv-src -I$(POS_COR_BUILD)/_deps/catch-src/single_include/catch2 -I$(POS_COR_BUILD)/generated
LINKER_COMMANDS := -Wl,-rpath,$(POS_COR_BUILD) -L$(POS_COR_BUILD) -lposeidon_core -lpmemobj /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 -lpthread
SRCS_ANALYTICS := $(wildcard $(SRCDIR)/analytics/*.cpp)
SRCS_UTILS := $(wildcard $(SRCDIR)/utils/*.cpp)
OBJECTS_ANALYTICS := $(patsubst $(SRCDIR)/analytics/%.cpp,$(OBJDIR)/%.o,$(SRCS_ANALYTICS))
OBJECTS_UTILS := $(patsubst $(SRCDIR)/utils/%.cpp,$(OBJDIR)/%.o,$(SRCS_UTILS))
	
all: $(OBJECTS_ANALYTICS) $(OBJECTS_UTILS)
	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -shared -o libgraph_analytics.so $^

$(OBJDIR)/%.o: $(SRCDIR_UTILS)/%.cpp
	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -c -fPIC -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR_ANALYTICS)/%.cpp 
	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -c -fPIC -o $@ $< 
#all: $(OBJECTS_ANALYTICS) $(OBJECTS_UTILS)
#	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -shared -o libgraph_analytics.so $^ $(LINKER_COMMANDS)

#$(OBJDIR)/%.o: $(SRCDIR_UTILS)/%.cpp
#	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -c -fPIC -o $@ $<

#$(OBJDIR)/%.o: $(SRCDIR_ANALYTICS)/%.cpp 
#	g++ $(CXX_FLAGS) $(CXX_DEFINES) $(INC) -c -fPIC -o $@ $<