prjPath :=.

SLIENT  := -s
KBUILD_VERBOSE := -1

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif

ifeq ($(KBUILD_VERBOSE),1)
  SLIENT  := 
else
  SLIENT  := -s
endif

ifeq ($(KBUILD_VERBOSE),-1)
	jobs := 36
else
	jobs := 1	
endif

include Rules.make


LIBS_PATH	:=$(libs_src-y)
LIBS_CLEAN_PATH := $(addprefix _clean_,$(LIBS_PATH) )

TARGETS_PATH		:= $(targets_src-y)
TARGETS_CLEAN_PATH	:= $(addprefix _clean_,$(TARGETS_PATH) )
###########################################


all:   
ifeq ($(platform),arm)
	cp config_arm.h $(INC_PATH)config/config.h
else
	cp config.h $(INC_PATH)config/config.h
endif
	@make libs  -j$(jobs) $(SLIENT)
	@make targets  $(SLIENT)

clean:
	@make libs_clean -j $(SLIENT)
	@make targets_clean -j  $(SLIENT)

libs:$(LIBS_PATH) FORCE

targets:$(TARGETS_PATH) FORCE

targets_clean: $(TARGETS_CLEAN_PATH) FORCE

libs_clean: $(LIBS_CLEAN_PATH) FORCE
	-rm -f $(GLOBAL_LIB)/*.a

$(LIBS_PATH):FORCE 
	make -C $@

$(LIBS_CLEAN_PATH):  FORCE
	echo 'making clean in $(patsubst _clean_%,%,$@)'
	make -C $(patsubst _clean_%,%,$@) clean

$(TARGETS_PATH):FORCE 
	make -C $@

$(TARGETS_CLEAN_PATH):  FORCE
	echo 'making clean in $(patsubst _clean_%,%,$@)'
	make -C $(patsubst _clean_%,%,$@) clean



FORCE:

.PHONY: all clean libs_clean libs  targets_clean targets  FORCE
	


