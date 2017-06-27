

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)




LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcwmp/include/
LOCAL_C_INCLUDES += external/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libXmlParse
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libxml2_ch/include
LOCAL_C_INCLUDES += external/icu4c/common

LOCAL_SRC_FILES:= src/conf.c  \
    src/cwmpd.c  \
    src/process.c  \
    src/thread.c \
    src/agent.c \
    src/getopt.c \
    src/getopt_init.c \
    src/modules/data_model.c  \
    src/modules/ping.c  \
    src/modules/paravalue.c  \
    src/modules/paracom.c \
    src/modules/paraapi.c



	
LOCAL_STATIC_LIBRARIES += libcwmp
LOCAL_STATIC_LIBRARIES += libssl libcrypto libxml2_ch
LOCAL_SHARED_LIBRARIES += libXmlParse 
LOCAL_SHARED_LIBRARIES += libicuuc
LOCAL_SHARED_LIBRARIES += liblog libcutils 

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= cwmpd

LOCAL_CFLAGS+=-O3 -DUSE_CWMP_MEMORY_POOL -Wall -g -DANDROID -DUSE_CWMP_OPENSSL -DSYS_DATA_MODE

include $(BUILD_EXECUTABLE)