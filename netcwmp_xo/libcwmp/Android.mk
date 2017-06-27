

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/inc
LOCAL_C_INCLUDES += external/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libXmlParse
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libxml2_ch/include
LOCAL_C_INCLUDES += external/icu4c/common

LOCAL_SRC_FILES:= src/list.c \
    src/buffer.c  \
    src/cfg.c  \
    src/cwmp.c  \
    src/http.c \
    src/ssl.c  \
    src/ini.c  \
    src/log.c  \
    src/md5.c  \
    src/memory.c   \
    src/session.c  \
    src/util.c \
    src/event.c \
    src/queue.c \
    src/model.c  \
    src/callback.c \
    src/hashtab.c \
    src/paratab.c \
    src/netlink.c  \
    src/cJSON.c \
    src/httpd.c \
    src/httpjson.c \
    src/url.c \
    src/urlcmd.c \
    src/ring_buffer.c  \
    src/cwmp_com.c \
    src/pool/pool.c \
    src/pool/mpool.c \
    src/xmlet/attr.c  \
    src/xmlet/document.c  \
    src/xmlet/namemap.c  \
    src/xmlet/list.c   \
    src/xmlet/xmlet.c \
    src/xmlet/element.c   \
    src/xmlet/node.c  \
    src/xmlet/buffer.c  \
    src/xmlet/parser.c  \
    src/api/cwmpapi.c  \
    src/dm_xml.c \
    src/cwmp_data.c

LOCAL_STATIC_LIBRARIES += libssl libcrypto libxml2_ch
LOCAL_SHARED_LIBRARIES += libXmlParse 

LOCAL_SHARED_LIBRARIES += liblog libcutils 

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libcwmp

LOCAL_CFLAGS+=-O3 -DUSE_CWMP_MEMORY_POOL -Wall  -DUSE_CWMP_OPENSSL -DSYS_DATA_MODE

include $(BUILD_STATIC_LIBRARY)