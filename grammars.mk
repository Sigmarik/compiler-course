include paths.mk

GRAMMARS = $(shell python3 grammarer.py $(GRAM_DIR))

full-antlr: $(ANTLR_DIR)
	python3 fix-deprecated.py $(ANTLR_DIR)

$(ANTLR_DIR): $(GRAMMARS)
	@echo $(YELLOW)$(BOLD) Compiling grammars
	@echo $^ $(STYLE_RESET)
	@antlr4 -Dlanguage=Cpp $^ -o $(ANTLR_DIR)
