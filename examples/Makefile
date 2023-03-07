
S            = ${HOME}/datasource
E            = $(S)/examples
CXX_OPTS     = -std=c++20 -O2
DEFINES      = -DVERBOSE_DATA_SOURCE
INCLUDE_DIRS = -I$(S) -I$(E)

OBJECTS = SoundSource.o DataSourceException.o Channel.o Container.o processing.o main.o


LINK_LIBS = -lsndfile 


all: dshello 


dshello: $(OBJECTS) 
	g++ $(OBJECTS) $(LINK_LIBS) -o dshello 

main.o: $(E)/main.cpp $(E)/processing.hpp $(E)/dshello.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(E)/main.cpp -c -o main.o 

processing.o: $(E)/processing.cpp $(E)/processing.hpp $(E)/Channel.hpp $(E)/Container.hpp $(E)/dshello.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(E)/processing.cpp -c -o processing.o 

Channel.o: $(E)/Channel.cpp $(E)/Channel.hpp $(E)/SoundSource.hpp $(E)/dshello.hpp
	g++ $(CXX_OPTS) -Wno-unused-result $(INCLUDE_DIRS) $(DEFINES) $(E)/Channel.cpp -c -o Channel.o 

Container.o: $(E)/Container.cpp $(E)/Container.hpp $(E)/SoundSource.hpp $(E)/dshello.hpp
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(E)/Container.cpp -c -o Container.o 

SoundSource.o: $(E)/SoundSource.cpp $(E)/SoundSource.hpp $(S)/DataSource.hpp $(E)/dshello.hpp
	g++ $(CXX_OPTS) -Wno-unused-result $(INCLUDE_DIRS) $(DEFINES) $(E)/SoundSource.cpp -c -o SoundSource.o 

DataSourceException.o: $(S)/DataSourceException.cpp $(S)/DataSourceException.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(S)/DataSourceException.cpp -c -o DataSourceException.o 


# CLEANING UP

clean:
	rm -f *.o dshello 


