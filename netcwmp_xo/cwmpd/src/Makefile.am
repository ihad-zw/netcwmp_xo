bin_PROGRAMS = cwmpd

cwmpd_SOURCES = conf.c  cwmpd.c  process.c  thread.c agent.c getopt.c getopt_init.c modules/data_model.c  modules/ping.c  modules/paravalue.c  modules/paracom.c modules/paraapi.c


AM_CPPFLAGS = -DUSE_CWMP_MEMORY_POOL -I$(top_srcdir)/include  -I$(top_srcdir)/cwmpd/include -I$(top_srcdir)/libpool/include  -I$(top_srcdir)/libxmlet/include -I$(top_srcdir)/libcwmp/include  

AUTOMAKE_OPTIONS = foreign subdir-objects  

cwmpd_LDADD = -L$(top_srcdir)/libcwmp/src  -lcwmp	-lXmlParse -lpthread -lm


cwmpd_CPPFLAGS = $(AM_CPPFLAGS) 

cwmpd_LDFLAGS =  -D_GNU_SOURCE -DUSE_CWMP_MEMORY_POOL  

