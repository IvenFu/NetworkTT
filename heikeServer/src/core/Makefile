#!/bin/bash

PRJ = heikeServer

TARGET_NAME = libNPQos

TARGET = $(TARGET_NAME).so
TARGET_F = $(subst $(TARGET_NAME),$(TARGET_NAME)_$(VERSION),$(TARGET))
TARGET_NOSTRIP = $(TARGET).nostrip

prjPath = $(firstword $(subst $(PRJ),$(PRJ) ,$(PWD)))/project/linux
include $(prjPath)/Rules.make


VPATH :=
VPATH +=NPQos
VPATH +=Util
VPATH +=audio
VPATH +=Rtcp
VPATH +=sync
VPATH +=video
VPATH +=common

VPATH_2 :=./
VPATH_2 += $(foreach dir,$(VPATH),$(shell find ./$(dir) -maxdepth 3 -type d))

SOURCES = $(foreach dir,$(VPATH_2),$(wildcard $(dir)/*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))
C_OBJS   = $(C_SRCS:%.c=%.o)
CPP_OBJS = $(CPP_SRCS:%.cpp=%.o)


INCLUDE_DIR := 
INCLUDE_DIR += -I../../inc/third/hpr
INCLUDE_DIR += -I../../inc/third/hlog
INCLUDE_DIR += -I../../inc/third/imux
INCLUDE_DIR += -I../../inc/third/decoder
INCLUDE_DIR += -I../../inc/config
INCLUDE_DIR += -I./Fec/inc
INCLUDE_DIR += -I./inc
INCLUDE_DIR += -I./inc/audio
INCLUDE_DIR += -I./sync



TOOL_PREFIX = $(GLOBAL_PREFIX)
GCPP = $(TOOL_PREFIX)g++
CC	= $(TOOL_PREFIX)gcc
AR = $(TOOL_PREFIX)ar
RANLIB = $(TOOL_PREFIX)ranlib
STRIP = $(TOOL_PREFIX)strip

CFLAGS += $(GLOBAL_FLAG)
CFLAGS += $(INCLUDE_DIR)


LDFLAGS := -Wl,-Bsymbolic -shared
LDFLAGS += -L$(GLOBAL_LIB)
LDFLAGS += -L$(GLOBAL_LIB_THIRD)

LDFLAGS2 := $(libs-y)
LDFLAGS2 += $(libs_third-y)
LDFLAGS2 += $(libs_trans-y)

$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(GCPP) $(LDFLAGS) $^ $(LDFLAGS2) -o $@
ifeq ($(VER),release)
	cp $@ $(SDK_PATH)/$(TARGET_NOSTRIP)
	$(STRIP) $@
	cp $@ $(SDK_PATH)	
	cp $@ $(SDK_PATH)/$(TARGET_F)
else
	cp $@ $(SDK_PATH)	
	cp $@ $(SDK_PATH)/$(TARGET_F)
endif
	cp inc/NPQos.h $(SDK_PATH)
	@rm *.so -f
	@echo -e "\nCongratulation!! you make $(TARGET) successfully ^_^!!!!!!!!!!!!!!\n"

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
%.o:%.cpp
	$(GCPP) $(CFLAGS) -c $^ -o $@


.PHONY: all clean
		
all: $(TARGET)

clean:
	-rm -f $(TARGET) $(OBJS) $(CPP_OBJS) $(SDK_PATH)/$(TARGET)* $(SDK_PATH)/$(TARGET_F)
