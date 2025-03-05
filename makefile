include paths.mk

CXXFLAGS = -I . -I /usr/local/include/antlr4-runtime/ -L /usr/local/lib/libantlr4-runtime.so.4.9.3

BOLD = \\033[1m
STYLE_RESET = \\033[0m

RED 	= \\033[31m
GREEN 	= \\033[32m
YELLOW 	= \\033[33m
BLUE 	= \\033[34m
PINK 	= \\033[35m
CYAN 	= \\033[36m
GREY 	= \\033[37m

$(shell make -f grammars.mk > /dev/null)

all: $(BLD_DIR)/language

# Yes, it could have been done with makefile alone,
# but I wanted to see how it would have looked like
# with python scripts.
SOURCES = $(shell python3 sourcerer.py $(SRC_DIR) $(LIB_DIR) $(ANTLR_DIR))
OBJECTS = $(shell python3 objectifier.py $(OBJ_DIR) .o $(SOURCES))
DEPENDS = $(shell python3 objectifier.py $(DEP_DIR) .d $(SOURCES))

$(BLD_DIR)/language: depend $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo $(GREEN)$(BOLD) Linking everything together...$(STYLE_RESET)
	$(CXX) $(OBJECTS) -o $@

depend: $(DEPENDS)

depends/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $^ -MM -MT $(shell python3 objectifier.py $(OBJ_DIR) .o $^) -MF $@

-include depends/*.d

objects/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo $(GREEN) Building "$^" $(STYLE_RESET)
	@$(CXX) -c $^ $(CXXFLAGS) -o $@
