CXX      := g++
CFLAGS   := -g -Wall -std=c++17 -Xpreprocessor -fopenmp -O2
TARGET   := boids
BUILDDIR := build
LIB      := -lSDL2 -lSDL2_gfx -lomp

SOURCES  := $(wildcard ./*.cpp)
OBJECTS  := $(patsubst ./%, $(BUILDDIR)/%, $(SOURCES:.cpp=.o))

ifeq ($(OS), Windows_NT)
	CFLAGS += -mconsole
	TARGET := $(TARGET).exe
endif


$(BUILDDIR)/%.o : %.cpp
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CFLAGS) -c -o $@ $^
	
$(TARGET) : $(OBJECTS)
	@$(CXX) $(CFLAGS) -o $@ $^ $(LIB) 

PHONY: r
r:
	@./$(TARGET)

PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(TARGET)