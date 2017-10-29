OBJS = main.cpp

OBJ_NAME = voronoi

all : $(OBJS)
	g++ $(OBJS) -w -g -lSDL2 -o $(OBJ_NAME)
