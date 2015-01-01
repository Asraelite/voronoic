OBJS = main.c

OBJ_NAME = voronoi

all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME)

