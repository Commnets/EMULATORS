EXEEXT = .exe
CXXFLAGS += -Wall

SDL_PATH = c:/libs/SDL-1.2.13/mingw32/
SDL_INC = $(SDL_PATH)include/SDL
SDL_LIB = $(SDL_PATH)lib

DEFINES = -DWIN32 -DSIDPLAYER_STANDALONE
INCLUDES = -I. -Icommon -I$(SDL_INC)
LIBS = -L$(SDL_LIB) -lmingw32 -lSDLmain -lSDL

all: player$(EXEEXT)

sid.o: sid.cpp
	$(CXX) $(CXXFLAGS) -c sid.cpp $(DEFINES) $(INCLUDES)

main.o: main.cpp player_sid.h player_zak.h config.h
	$(CXX) $(CXXFLAGS) -c main.cpp $(DEFINES) $(INCLUDES)

player_sid.o: player_sid.cpp player_sid.h config.h
	$(CXX) $(CXXFLAGS) -c player_sid.cpp $(DEFINES) $(INCLUDES)

player_zak.o: player_zak.cpp player_zak.h config.h
	$(CXX) $(CXXFLAGS) -c player_zak.cpp $(DEFINES) $(INCLUDES)
	
player$(EXEEXT): main.o sid.o player_sid.o player_zak.o
	$(CXX) $(CXXFLAGS) -o $@ main.o sid.o player_sid.o player_zak.o $(DEFINES) $(INCLUDES) $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o *.exe
