AUTOMAKE_OPTIONS=foreign subdir-objects
lib_LIBRARIES = libcwmp.a
libcwmp_a_SOURCES =list.c buffer.c  cfg.c  cwmp.c  http.c ssl.c  ini.c  log.c  md5.c  memory.c   session.c  util.c event.c queue.c model.c  callback.c hashtab.c paratab.c netlink.c  cJSON.c httpd.c httpjson.c url.c urlcmd.c ring_buffer.c  unix_com.c cwmp_com.c
libcwmp_a_SOURCES += pool/pool.c pool/mpool.c
libcwmp_a_SOURCES += xmlet/attr.c  xmlet/document.c  xmlet/namemap.c  xmlet/list.c   xmlet/xmlet.c xmlet/element.c   xmlet/node.c  xmlet/buffer.c  xmlet/parser.c
libcwmp_a_SOURCES += api/cwmpapi.c 

AM_CPPFLAGS = -Wall -DUSE_CWMP_MEMORY_POOL -DSYS_DATA_MODE -fPIC -lm -I./ -I$(top_srcdir)/libcwmp/include -I$(top_srcdir)/libcwmp/src/inc -I$(top_srcdir)/../libXmlParse









