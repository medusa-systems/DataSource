
S            = ${HOME}/dshello
CXX_OPTS     = -std=c++20 -O2
DEFINES      = -DVERBOSE_DATA_SOURCE
INCLUDE_DIRS = -I$(S) 

OBJECTS = SoundSource.o DataSourceException.o Channel.o Container.o processing.o main.o


LINK_LIBS = -lsndfile 


all: dshello 


dshello: $(OBJECTS) 
	g++ $(OBJECTS) $(LINK_LIBS) -o dshello 

main.o: $(S)/main.cpp $(S)/processing.hpp $(S)/dshello.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(S)/main.cpp -c -o main.o 

processing.o: $(S)/processing.cpp $(S)/processing.hpp $(S)/Channel.hpp $(S)/Container.hpp $(S)/dshello.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(S)/processing.cpp -c -o processing.o 

Channel.o: $(S)/Channel.cpp $(S)/Channel.hpp $(S)/SoundSource.hpp $(S)/dshello.hpp
	g++ $(CXX_OPTS) -Wno-unused-result $(INCLUDE_DIRS) $(DEFINES) $(S)/Channel.cpp -c -o Channel.o 

Container.o: $(S)/Container.cpp $(S)/Container.hpp $(S)/SoundSource.hpp $(S)/dshello.hpp
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(S)/Container.cpp -c -o Container.o 

SoundSource.o: $(S)/SoundSource.cpp $(S)/SoundSource.hpp $(S)/DataSource.hpp $(S)/dshello.hpp
	g++ $(CXX_OPTS) -Wno-unused-result $(INCLUDE_DIRS) $(DEFINES) $(S)/SoundSource.cpp -c -o SoundSource.o 

DataSourceException.o: $(S)/DataSourceException.cpp $(S)/DataSourceException.hpp 
	g++ $(CXX_OPTS) $(INCLUDE_DIRS) $(DEFINES) $(S)/DataSourceException.cpp -c -o DataSourceException.o 


# CLEANING UP

clean:
	rm -f *.o dshello 


