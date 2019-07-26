#!/bin/bash

PRJ = heikeServer

TARGET_NAME = ProbingServer

TARGET = $(TARGET_NAME)

prjPath = $(firstword $(subst $(PRJ),$(PRJ) ,$(PWD)))/project/linux
include $(prjPath)/Rules.make


VPATH := ./

SOURCES = $(foreach dir,$(VPATH),$(wildcard $(dir)/*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))
C_OBJS   = $(C_SRCS:%.c=%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)


INCLUDE_DIR := 
INCLUDE_DIR += -I../../../inc/third/tpr
INCLUDE_DIR += -I../../../inc/config
INCLUDE_DIR += -I./inc


TOOL_PREFIX = $(GLOBAL_PREFIX)
GCPP = $(TOOL_PREFIX)g++
CC	= $(TOOL_PREFIX)gcc
AR = $(TOOL_PREFIX)ar
RANLIB = $(TOOL_PREFIX)ranlib
STRIP = $(TOOL_PREFIX)strip

CFLAGS += $(GLOBAL_FLAG)
CFLAGS += $(INCLUDE_DIR)


LDFLAGS += -L$(GLOBAL_LIB)
LDFLAGS += -L$(GLOBAL_LIB_THIRD)

LDFLAGS2 := $(libs-y)
LDFLAGS2 += $(libs_third-y)

$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(GCPP) $(LDFLAGS) $^ $(LDFLAGS2) -o $@

	@echo -e "\nCongratulation!! you make $(TARGET) successfully ^_^!!!!!!!!!!!!!!\n"

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
%.o:%.cpp
	$(GCPP) $(CFLAGS) -c $^ -o $@


.PHONY: all clean
		
all: $(TARGET)

clean:
	-rm -f $(TARGET) $(OBJS) $(CPP_OBJS)
