
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)

# See Makefile for the original list of objects.
# Many were removed.
OBJS += \
	$(LOCAL_DIR)/lapi.o \
	$(LOCAL_DIR)/lcode.o \
	$(LOCAL_DIR)/lctype.o \
	$(LOCAL_DIR)/ldebug.o \
	$(LOCAL_DIR)/ldo.o \
	$(LOCAL_DIR)/ldump.o \
	$(LOCAL_DIR)/lfunc.o \
	$(LOCAL_DIR)/lgc.o \
	$(LOCAL_DIR)/llex.o \
	$(LOCAL_DIR)/lmem.o \
	$(LOCAL_DIR)/lobject.o \
	$(LOCAL_DIR)/lopcodes.o \
	$(LOCAL_DIR)/lparser.o \
	$(LOCAL_DIR)/lstate.o \
	$(LOCAL_DIR)/lstring.o \
	$(LOCAL_DIR)/ltable.o \
	$(LOCAL_DIR)/ltm.o \
	$(LOCAL_DIR)/lundump.o \
	$(LOCAL_DIR)/lvm.o \
	$(LOCAL_DIR)/lzio.o \
	$(LOCAL_DIR)/lauxlib.o \
	$(LOCAL_DIR)/lbaselib.o \
	$(LOCAL_DIR)/linit.o \

# lbitlib, lcorolib, ltablib were removed, seem important.
