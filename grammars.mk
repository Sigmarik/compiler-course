include paths.mk

PARSERS = $(shell python3 grammarer.py .y $(GRAM_DIR))
PARSER_CPPS = $(foreach word,$(PARSERS),$(BISON_DIR)/$(word).cpp)

LEXERS = $(shell python3 grammarer.py .l $(GRAM_DIR))
LEXER_CPPS = $(foreach word,$(LEXERS),$(FLEX_DIR)/$(word).cpp)

flex-bison: bison flex

bison: $(PARSER_CPPS)

flex: $(LEXER_CPPS)

$(BISON_DIR)/%.y.cpp: %.y
	@mkdir -p $(dir $@)
	bison -d -o $@ $^

$(FLEX_DIR)/%.l.cpp: %.l
	@mkdir -p $(dir $@)
	flex -o $@ $^


clear-grams:
	rm -rf $(BISON_DIR)
	rm -rf $(FLEX_DIR)
