CXX_FLAGS := -Wno-unused-local-typedefs -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -Wno-unused -pthread  -Wall -Wno-deprecated -O3 -Wsign-compare   -std=c++17
#CXX_FLAGS := -pthread  -Wall -O3 -std=c++17
CXX_DEFINES := -DBOOST_ALL_NO_LIB -DBOOST_DATE_TIME_DYN_LINK -DHAS_BOOST -DUSE_LOGGING -DUSE_PMDK -DUSE_TX
OBJDIR := ../obj_dir
SRCDIR := ../src
SRCDIR_ANALYTICS := ../src/analytics
SRCDIR_UTILS := ../src/utils
POS_COR_INC := ../poseidon_core/include
INCLUDE := ../include
INC := -I$(INCLUDE)/analytics -I$(INCLUDE)/utils -I$(POS_COR_INC) -I$(POS_COR_INC)/btree -I$(POS_COR_INC)/dict -I$(POS_COR_INC)/ldbc -I$(POS_COR_INC)/log -I$(POS_COR_INC)/pool -I$(POS_COR_INC)/py -I$(POS_COR_INC)/qop -I$(POS_COR_INC)/storage -I$(POS_COR_INC)/tx -I$(POS_COR_INC)/_deps/spdlog-src/include -I$(POS_COR_INC)/_deps/csv-src -I$(POS_COR_INC)/_deps/catch-src/single_include/catch2
LINKER_COMMANDS := -Wl,-rpath,"/home/jan/Dokumente/Bachelorarbeit/poseidon_core/build" -L/home/jan/Dokumente/Bachelorarbeit/poseidon_core/build -lposeidon_core -lpmemobj /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 -lpthread
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