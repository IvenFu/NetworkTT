LOCAL_PATH := $(call my-dir)

SRC_PATH  := $(LOCAL_PATH)/../../../src/
INC_PATH  := $(LOCAL_PATH)/../../../inc/
LIB_PATH  := $(LOCAL_PATH)/../../../lib/

include $(CLEAR_VARS)
LOCAL_MODULE := libtpr

VPATH := $(SRC_PATH)
VPATH += $(SRC_PATH)/posix/

SOURCES    = $(foreach dir, $(VPATH), $(wildcard $(dir)*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))

LOCAL_SRC_FILES := $(C_SRCS)
LOCAL_SRC_FILES += $(CPP_SRCS)


LOCAL_C_INCLUDES += $(INC_PATH)/


#LOCAL_LDLIBS :=  -llog -lc


LOCAL_CFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  
LOCAL_CFLAGS += -DLOCALE_NOT_USED

APP_ABI := armeabi-v7a

include $(BUILD_STATIC_LIBRARY)

