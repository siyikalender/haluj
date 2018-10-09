#

EXAMPLE_DIR := ../examples

PROJECTS := $(subst /,, $(subst $(EXAMPLE_DIR)/,, $(sort $(dir $(wildcard $(EXAMPLE_DIR)/*/)))))
OUTPUTS := $(foreach output, $(PROJECTS), out-$(output))

all: $(PROJECTS)
	@echo 'List of projects:' $(PROJECTS)

$(PROJECTS):
	@echo 'Building Project:' $@
	@$(MAKE) -f Makefile.module DEFS=$(DEFS) NAME=$@-example MAIN_DIR=$(EXAMPLE_DIR)/$@

$(OUTPUTS):
	@echo 'Removing Project:' $@
	@rm -rf $@-example*

clean: $(OUTPUTS)

.PHONY: all clean
