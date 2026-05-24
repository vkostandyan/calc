
CC = g++
CFLAGS = -std=c++17 -I../.. -I../includes -O2
SRCS = src/lexer.cpp src/parser.cpp src/main.cpp

all: compiler

compiler: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o compiler

clean:
	rm -f compiler *.o
NAME1 = Compiler
NAME2 = Executor

CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++17

SRC_DIR = src

COMPILER_SRC = $(SRC_DIR)/mainCompiler.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/tokenizer.cpp $(SRC_DIR)/parser.cpp \
		$(SRC_DIR)/utils.cpp $(SRC_DIR)/VM.cpp $(SRC_DIR)/SymbolTable.cpp
COMPILER_OBJ = $(COMPILER_SRC:.cpp=.o)

EXEC_SRC = $(SRC_DIR)/mainExec.cpp $(SRC_DIR)/CPU.cpp $(SRC_DIR)/Loader.cpp $(SRC_DIR)/Memory.cpp $(SRC_DIR)/Executor.cpp
EXEC_OBJ = $(EXEC_SRC:.cpp=.o)

all: $(NAME1) $(NAME2)

$(NAME1): $(COMPILER_OBJ)
	$(CXX) $(CXXFLAGS) $(COMPILER_OBJ) -o $(NAME1)

$(NAME2): $(EXEC_OBJ)
	$(CXX) $(CXXFLAGS) $(EXEC_OBJ) -o $(NAME2)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(COMPILER_OBJ) $(EXEC_OBJ)

fclean: clean
	rm -f $(NAME1) $(NAME2)

re: fclean all

.PHONY: all clean fclean re