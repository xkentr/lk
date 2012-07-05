LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += lib/debug

OBJS += \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/dpc.o \
	$(LOCAL_DIR)/event.o \
	$(LOCAL_DIR)/main.o \
	$(LOCAL_DIR)/mutex.o \
	$(LOCAL_DIR)/thread.o \
	$(LOCAL_DIR)/timer.o \
	$(LOCAL_DIR)/newlib_stubs.o
