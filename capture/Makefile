CC = x86_64-w64-mingw32-g++

CFLAGS = -Wall -Wextra -I./include -O2 -std=c++17 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00
LDFLAGS = -lgdi32 -lws2_32

SRC_DIR = src
OBJ_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

EXECUTABLE = telemetry.exe

all: $(EXECUTABLE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(TEST_OBJECTS) $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) -o $@ $(LDFLAGS)

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)
