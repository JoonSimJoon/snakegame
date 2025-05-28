CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I../include -I./include
LDFLAGS = -lncurses

SRC_DIR = src
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/map.cpp \
       $(SRC_DIR)/snake.cpp \
       $(SRC_DIR)/game.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = snakegame

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)