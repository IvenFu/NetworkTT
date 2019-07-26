LOCAL_PATH := $(call my-dir)

SRC_PATH  := $(LOCAL_PATH)/../../../src/core
INC_PATH  := $(LOCAL_PATH)/../../../inc
LIB_PATH  := $(LOCAL_PATH)/../../../lib/

include $(CLEAR_VARS)
LOCAL_MODULE :=hpr
LOCAL_SRC_FILES := $(LIB_PATH)/android/libhpr.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE :=g711codec
LOCAL_SRC_FILES := $(LIB_PATH)/android/libg711codec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=aaccodec
LOCAL_SRC_FILES := $(LIB_PATH)/android/libaaccodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=aip
LOCAL_SRC_FILES := $(LIB_PATH)/android/libaip.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=aacldcodec
LOCAL_SRC_FILES := $(LIB_PATH)/android/libaacldcodec.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=opuscodec
LOCAL_SRC_FILES := $(LIB_PATH)/android/libopuscodec.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := NPQos

rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*), $(call rwildcard,$d/,$2))


VPATH :=
VPATH +=$(SRC_PATH)

SOURCES = $(foreach CHILD_PATH,$(VPATH), $(call rwildcard,$(CHILD_PATH), *.*))
C_SRCS   = $(filter %.c, $(SOURCES))
CPP_SRCS = $(filter %.cpp,$(SOURCES))

LOCAL_SRC_FILES := $(C_SRCS)
LOCAL_SRC_FILES += $(CPP_SRCS)

LOCAL_C_INCLUDES := $(SRC_PATH)/Fec/inc
LOCAL_C_INCLUDES += $(SRC_PATH)/inc
LOCAL_C_INCLUDES += $(SRC_PATH)/inc/audio
LOCAL_C_INCLUDES += $(SRC_PATH)/sync
LOCAL_C_INCLUDES += $(INC_PATH)/third/hpr
LOCAL_C_INCLUDES += $(INC_PATH)/third/imux
LOCAL_C_INCLUDES += $(INC_PATH)/third/decoder
LOCAL_C_INCLUDES += $(INC_PATH)/config


LOCAL_LDLIBS :=  -llog -lc

LOCAL_STATIC_LIBRARIES :=
LOCAL_STATIC_LIBRARIES += hpr
#LOCAL_STATIC_LIBRARIES += _imux
#LOCAL_STATIC_LIBRARIES += avi_mux
#LOCAL_STATIC_LIBRARIES += flv_mux_32
#LOCAL_STATIC_LIBRARIES += mp4_mux_32
#LOCAL_STATIC_LIBRARIES += rtp_pack_lib_32
#LOCAL_STATIC_LIBRARIES += ts_mux_lib_32
#LOCAL_STATIC_LIBRARIES += psmux
LOCAL_STATIC_LIBRARIES += g711codec
LOCAL_STATIC_LIBRARIES += aaccodec
LOCAL_STATIC_LIBRARIES += aip
LOCAL_STATIC_LIBRARIES += aacldcodec
LOCAL_STATIC_LIBRARIES += opuscodec

				
LOCAL_CFLAGS += -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  
LOCAL_CFLAGS += -DLOCALE_NOT_USED

APP_ABI := armeabi-v7a

include $(BUILD_SHARED_LIBRARY)

