CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17

ifeq ($(OS),Windows_NT)
EXEEXT := .exe
else
EXEEXT :=
endif

TARGET := experiments/test_coral_cache$(EXEEXT)
SOURCE := experiments/test_coral_cache.cpp
HEADERS := $(wildcard coral/*.h)

.PHONY: all coral clean

all: coral

coral: $(TARGET)

$(TARGET): $(SOURCE) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCE) -Icoral -o $(TARGET)

ifeq ($(OS),Windows_NT)
clean:
	-del /Q $(subst /,\,$(TARGET))
else
clean:
	$(RM) $(TARGET)
endif
