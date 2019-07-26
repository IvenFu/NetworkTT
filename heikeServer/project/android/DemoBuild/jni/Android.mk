LOCAL_PATH := $(call my-dir)

SRC_PATH  := $(LOCAL_PATH)/../../../../src/demo/DemoAudio
INC_PATH  := $(LOCAL_PATH)/../../../../inc
LIB_PATH  := $(LOCAL_PATH)/../../../../lib/

include $(CLEAR_VARS)
LOCAL_MODULE :=hpr
LOCAL_SRC_FILES := $(LIB_PATH)/android/libhpr.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := Probing

VPATH :=
VPATH +=$(SRC_PATH)

SOURCES = $(foreach dir,$(VPATH), $(wildcard $(dir)/*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))

LOCAL_SRC_FILES := $(C_SRCS)
LOCAL_SRC_FILES += $(CPP_SRCS)

LOCAL_C_INCLUDES := $(SRC_PATH)
LOCAL_C_INCLUDES += $(INC_PATH)
LOCAL_C_INCLUDES += $(INC_PATH)/third/hpr

LOCAL_LDLIBS :=  -lNPQos -llog -lc
LOCAL_LDLIBS += -L$(LIB_PATH)/android 

LOCAL_STATIC_LIBRARIES :=
LOCAL_STATIC_LIBRARIES += hpr
			
LOCAL_CFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  
LOCAL_CFLAGS += -DLOCALE_NOT_USED

APP_ABI := armeabi-v7a

include $(BUILD_SHARED_LIBRARY)

