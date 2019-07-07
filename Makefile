CC		:= g++
TARGET		:= "dist/Boid\ Simulation"
BUILDDIR	:= build
SRCDIR		:= src
CFLAGS		:= -std=c++11 -g
SRCEXT		:= cpp
SOURCES 	:= $(wildcard $(SRCDIR)/*.$(SRCEXT))
OBJECTS		:= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))
LIB 			:= -lSDL2

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@printf "\e[33m\e[1mBuilding...\e[0m\n";
	@mkdir -p $(BUILDDIR)
	@echo "  $(notdir $@) from $(notdir $<)"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	@printf "\e[95m\e[1mLinking...\e[0m\n";
	@mkdir -p dist
	@echo "  $(notdir $(OBJECTS))"
	@$(CC) $(CFLAGS) $(LIB) -o $@ $^

PHONY: r
r:
	@printf "\e[36m\e[1mRunning...\e[0m\n"
	@printf "  \e[94m\e[1m"$(notdir "$(TARGET)\e[0m\n")
	@ ./$(TARGET)

PHONY: run
run:
	@mkdir -p $(BUILDDIR)
	@for source in $(basename $(notdir $(SOURCES))); do\
		printf "\e[33m\e[1mBuilding...\e[0m\n";\
		echo "  $$source.o from $$source.$(SRCEXT)";\
		$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$$source.o $(SRCDIR)/$$source.$(SRCEXT);\
	done
	@printf "\e[95m\e[1mLinking...\e[0m\n";
	@mkdir -p dist
	@echo "  $(notdir $(OBJECTS))"
	@$(CC) $(CFLAGS) $(LIB) -o $(TARGET) $(OBJECTS)
	@printf "\e[36m\e[1mRunning...\e[0m\n"
	@printf "  \e[94m\e[1m"$(notdir "$(TARGET)\e[0m\n")
	@ ./$(TARGET)

PHONY: clean
clean:
	@printf "\e[91m\e[1mCleaning...\e[0m\n"
	@echo "  /$(BUILDDIR)"
	@echo "  /$(TARGET)"
	@$(RM) -r $(BUILDDIR) $(OBJECTS)
	@$(RM) "./$(TARGET)"

PHONY: crun
crun:
	@printf "\e[91m\e[1mCleaning...\e[0m\n"
	@echo "  /$(BUILDDIR)"
	@echo "  /$(TARGET)"
	@$(RM) -r $(BUILDDIR) $(OBJECTS)
	@$(RM) "./$(TARGET)"
	@mkdir -p $(BUILDDIR)
	@for source in $(basename $(notdir $(SOURCES))); do\
		printf "\e[33m\e[1mBuilding...\e[0m\n";\
		echo "  $$source.o from $$source.$(SRCEXT)";\
		$(CC) $(CFLAGS) -c -o $(BUILDDIR)/$$source.o $(SRCDIR)/$$source.$(SRCEXT);\
	done
	@printf "\e[95m\e[1mLinking...\e[0m\n";
	@mkdir -p dist
	@echo "  $(notdir $(OBJECTS))"
	@$(CC) $(CFLAGS) $(LIB) -o $(TARGET) $(OBJECTS)
	@printf "\e[36m\e[1mRunning...\e[0m\n"
	@printf "  \e[94m\e[1m"$(notdir "$(TARGET)\e[0m\n")
	@ ./$(TARGET)
