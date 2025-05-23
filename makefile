include paths.mk

LLVM_CONFIG ?= llvm-config-18

CXXFLAGS = -I . -I $(FLEX_DIR)/$(GRAM_DIR) -I $(BISON_DIR)/$(GRAM_DIR) \
	-I $(GRAM_DIR) -I $(SRC_DIR) -I $(LIB_DIR) -isystem $(EXTERN_DIR)/flex/src \
	$(shell $(LLVM_CONFIG) --cxxflags) -ggdb
# -DFLEX_DEBUG_TOKENS

LIBRARIES = $(shell $(LLVM_CONFIG) --link-shared --libs)

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
# with python scripts instead.
SOURCES = $(shell python3 sourcerer.py $(SRC_DIR) \
	$(LIB_DIR) $(ANTLR_DIR) $(BISON_DIR) $(FLEX_DIR))
OBJECTS = $(shell python3 objectifier.py $(OBJ_DIR) .o $(SOURCES))
DEPENDS = $(shell python3 objectifier.py $(DEP_DIR) .d $(SOURCES))

$(BLD_DIR)/language: depend $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo $(GREEN)$(BOLD) Linking everything together...$(STYLE_RESET)
	$(CXX) $(OBJECTS) $(LIBRARIES) -o $@

run: $(BLD_DIR)/language
	./$(BLD_DIR)/language

depend: $(DEPENDS)

depends/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $^ -MM -MT $(shell python3 objectifier.py $(OBJ_DIR) .o $^) -MF $@

-include depends/*.d

objects/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo $(GREEN) Building "$^" $(STYLE_RESET)
	@$(CXX) -c $^ $(CXXFLAGS) -o $@

include grammars.mk

clean: clear
clear: clear-grams
	@rm -rf build
	@rm -rf depends
	@rm -rf objects
