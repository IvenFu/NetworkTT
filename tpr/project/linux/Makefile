#!bin/bash


SRC_PATH = ../../src
INC_PATH = ../../inc
LIB_PATH = ../../lib

#成果物名称
TARGET = libtpr.so

#编译器名称
GLOBAL_PREFIX :=

#编译选项
GLOBAL_FLAG :=
GLOBAL_FLAG += -Os -g
GLOBAL_FLAG += -Wall -fsigned-char -Wno-unused 
GLOBAL_FLAG += -fPIC
GLOBAL_FLAG += -m64

#源码路径
VPATH := $(SRC_PATH)
VPATH += $(SRC_PATH)/posix

SOURCES    = $(foreach dir, $(VPATH), $(wildcard $(dir)/*))
C_SRCS     = $(filter %.c, $(SOURCES))
CPP_SRCS   = $(filter %.cpp, $(SOURCES))
C_OBJS     = $(C_SRCS:%.c=%.o)
CPP_OBJS   = $(CPP_SRCS:%.cpp=%.o)



INCLUDE_DIR := -I$(INC_PATH)

TOOL_PREFIX = $(GLOBAL_PREFIX)
GCPP      = $(TOOL_PREFIX)g++
CC        = $(TOOL_PREFIX)gcc
AR        = $(TOOL_PREFIX)ar
RANDLIB   = $(TOOL_PREFIX)ranlib
STRIP     = $(TOOL_PREFIX)strip

CFLAGS += $(GLOBAL_FLAG)
CFLAGS += $(INCLUDE_DIR)

LDFALGS := -Wl,-Bsymbolic -shared
LDFALGS += -L$(LIB_PATH)

LDFALGS2 :=

$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(GCPP) $(LDFALGS) $^ $(LDFALGS2) -o $@
	@echo $(SOURCES)
	@echo -e "build $(TARGET) sucessful ^_^!!!!!!!!\n"

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@@
%.o:%.cpp
	$(GCPP) $(CFLAGS) -c $^ -o $@

all: $(TARGET)

clean: 
	-rm -f $(TARGET) $(C_OBJS) $(CPP_OBJS)

.PHONY:all clean
