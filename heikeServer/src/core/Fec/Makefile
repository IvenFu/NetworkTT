#
# generate source file list
#
LIB = libHikFec.a


prjPath = $(firstword $(subst NPQos_V1.0,NPQos_V1.0 ,$(PWD)))/project/linux
include $(prjPath)/Rules.make

VPATH :=./
VPATH += $(shell find ./$(src) -maxdepth 3 -type d)

SOURCES = $(foreach dir,$(VPATH),$(wildcard $(dir)/*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))
C_OBJS   = $(C_SRCS:%.c=%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)

INCLUDE_DIR :=
INCLUDE_DIR += -I./inc

TOOL_PREFIX = $(GLOBAL_PREFIX)
GCPP = $(TOOL_PREFIX)g++
CC	= $(TOOL_PREFIX)gcc
AR = $(TOOL_PREFIX)ar
RANLIB = $(TOOL_PREFIX)ranlib

GLOBAL_FLAG += -DCPU_CISC

CFLAGS = $(GLOBAL_FLAG)
CFLAGS += $(INCLUDE_DIR)


all : $(LIB) 

$(LIB) : $(C_OBJS) $(CPP_OBJS)
	$(AR) r $@ $(C_OBJS) $(CPP_OBJS)
	$(RANLIB) $@
	cp $@ $(GLOBAL_LIB)
	rm $@
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
%.o:%.cpp
	$(GCPP) $(CFLAGS) -c $^ -o $@

clean :
	rm -f $(LIB) $(C_OBJS) $(CPP_OBJS)

