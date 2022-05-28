CXX       := g++
CFLAGS    := -g -Wall -std=c++17 -Xpreprocessor -fopenmp -O2
TARGET    := boids
BUILDDIR  := build
TARGETDIR := dist
LIB       := -lSDL2 -lSDL2_gfx -lomp

SOURCES  := $(wildcard src/*.cpp)
OBJECTS  := $(patsubst src/%, $(BUILDDIR)/%, $(SOURCES:.cpp=.o))

ifeq ($(OS), Windows_NT)
	CFLAGS += -mconsole
	TARGET := $(TARGET).exe
endif


$(BUILDDIR)/%.o : src/%.cpp
	@mkdir -p $(BUILDDIR)
	@$(CXX) $(CFLAGS) -c -o $@ $^
	
$(TARGETDIR)/$(TARGET) : $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@$(CXX) $(CFLAGS) -o $@ $^ $(LIB) 

PHONY: r
r:
	@./$(TARGETDIR)/$(TARGET)

PHONY: clean
clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(TARGETDIR)