# ==========================================================================
# Build system
# Description: configure for all Makefiles in whole project.
# Author :wangxinghe
# Date:2016.11.20
# <version> <time>    <author>     <desc>
#  v1.0.2   2016.11.20  wangxinghe  create kbuild system.
# ==========================================================================


#############Setting start###########################################################
PRJ = NPQos_V1.0

#version define
VERSION = x64_v1.0.0

#debug or release
VER = release

#arm   linux64
platform = linux64

#GLOBAL_PREFIX := arm-hik_v7a-linux-uclibcgnueabi-
#GLOBAL_PREFIX := arm-hisiv300-linux-
GLOBAL_PREFIX := 
#############Setting end############################################################

MAIN_PATH := $(firstword $(subst $(PRJ),$(PRJ) ,$(PWD)))
GLOBAL_LIB := $(MAIN_PATH)/lib/$(platform)/
GLOBAL_LIB_THIRD := $(MAIN_PATH)/lib/$(platform)/third/
INC_PATH := $(MAIN_PATH)/inc/
SDK_PATH :=$(MAIN_PATH)/sdk/$(platform)/

SRC_PATH := $(firstword $(subst $(PRJ),$(PRJ) ,$(PWD)))/src/core/

include $(prjPath)/Lib.build
include $(prjPath)/Target.build

GLOBAL_FLAG :=
GLOBAL_FLAG += -Os -g
GLOBAL_FLAG += -Wall -fsigned-char -Wno-unused -D_GNU_SOURCE 
GLOBAL_FLAG += -fPIC -D_REENTRANT
ifeq ($(platform),linux64)
GLOBAL_FLAG += -m64
endif
