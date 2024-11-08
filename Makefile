CC=gcc
SRC=main.c
OBJ=$(SRC:.cpp=.o)
EXE=monitor_switch

$(EXE): $(OBJ)
	$(CC) `pkg-config --cflags gtk+-3.0` $(OBJ) -o $(EXE) `pkg-config --libs gtk+-3.0`

.cpp.o:
	$(CC) -c $< -o $@