CPP := C:/mingw64/bin/g++.exe
CPPFLAGS := -Iinclude -I.
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic

BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/gestiondeportiva.exe
MAIN_SRC := main.cpp
SOURCES := $(wildcard src/*/*.cpp) $(MAIN_SRC)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	@if not exist "$@" mkdir "$@"

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
	@if exist "$(TARGET)" del /Q "$(TARGET)"

rebuild: clean all
