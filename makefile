# Diretórios importantes.
SRC_DIR := src/
BUILD_DIR := build/

CXXFLAGS := -pedantic-errors -Wall -std=c++17
LDLIBS :=

# Lista de arquivos .cpp
CPP_FILES := $(shell find $(SRC_DIR) -name "*.cpp")

# Lista de arquivos .o
OBJ_FILES := $(patsubst $(SRC_DIR)%.cpp, $(BUILD_DIR)%.o, $(CPP_FILES))

# Lista de headers sem implementação.
HEADERSONLY := $(filter-out $(patsubst $(SRC_DIR)%.cpp, $(SRC_DIR)%.hpp, $(CPP_FILES)), $(shell find $(SRC_DIR) -name "*.hpp"))

# Indicando para o make quais targets não estão associados com arquivos.
.PHONY: main clean debug

# Target padrão.
ALL: $(BUILD_DIR) main

# Target para gerar executável para debugging.
debug: CXXFLAGS += -g
debug: ALL

# Target para ligação.
main: $(BUILD_DIR)main.o $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@ $(LDLIBS)

# Target para compilação do arquivo main.
$(BUILD_DIR)main.o: $(SRC_DIR)main.cpp $(HEADERSONLY)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target para compilação dos outros arquivos.
$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp $(SRC_DIR)%.hpp $(HEADERSONLY)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target para criar o diretório build.
$(BUILD_DIR):
ifeq ($(wildcard $(BUILD_DIR)),)
    $(shell mkdir $(BUILD_DIR))
endif

# Target para apagar objetos e executável.
clean:
	rm -rf $(BUILD_DIR) *.o main
	clear