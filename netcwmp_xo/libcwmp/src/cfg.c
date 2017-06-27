/************************************************************************
 * Id: cfg.c                                                            *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/

#include <cwmp/cwmp.h>
#include <pool/pool.h>
#include <cwmp/log.h>
#include <cwmp/cfg.h>
#ifndef SYS_DATA_MODE
#include <ini.h>
#else
#include "sysdata.h"
#endif

#ifndef SYS_DATA_MODE
typedef struct conf_t conf_t;

struct conf_t {
        char * filename;
        FILE * fd;
};


static conf_t	* cwmp_conf_handle = NULL;
#else

#endif
int cwmp_conf_open(const char * filename)
{
#ifndef	SYS_DATA_MODE
    FUNCTION_TRACE();
    cwmp_conf_handle = malloc(sizeof(cwmp_conf_handle));
    if (!cwmp_conf_handle)
    {
        cwmp_log_error("conf malloc faild.\n");
        return CWMP_ERROR;
    }
    cwmp_conf_handle->filename = TRstrdup(filename);
#else
	
#endif
    return CWMP_OK;
}

void cwmp_conf_split(char * name, char **s , char **k)
{
    *s = strchr(name, ':');
    if(*s == NULL)
    {
        k = &name;
        *s = "cwmp";
    }
    else
    {
        *s[0] = 0;
        *k = *s+1;
        *s = name;
    }
}

int cwmp_conf_get(const char * key, char *value)
{
#ifndef SYS_DATA_MODE
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    //char value[INI_BUFFERSIZE] = {0};
    FUNCTION_TRACE();
    if(key == NULL)
    {
        return -1;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    ini_gets(s,k,NULL,value,INI_BUFFERSIZE, cwmp_conf_handle->filename);
#else
	
	CH_GetSysData(key,value,INI_BUFFERSIZE);
#endif
    return 0;
}

int cwmp_conf_set(const char * key, const char * value)
{
#ifndef SYS_DATA_MODE
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    FUNCTION_TRACE();
    if(key == NULL)
    {
        return CWMP_ERROR;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    return ini_puts(s, k, value, cwmp_conf_handle->filename);
#else
    return CH_SetSysData(key,value);
#endif
}

char * cwmp_conf_pool_get(pool_t * pool, const char * key)
{
#ifndef SYS_DATA_MODE
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    char value[INI_BUFFERSIZE] = {0};
    //FUNCTION_TRACE();
    if(key == NULL)
    {
        return NULL;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);

    cwmp_conf_split(name, &s, &k);

    ini_gets(s,k,NULL,value,INI_BUFFERSIZE, cwmp_conf_handle->filename);

    return pool_pstrdup(pool, value);
#else
   	char value[INI_BUFFERSIZE] = {0};
	int ret = 0;
	ret = CH_GetSysData(key,value,INI_BUFFERSIZE);

	if(ret !=0 || strlen(value) == 0) return NULL;
	
	return  pool_pstrdup(pool, value);
#endif
}

int cwmp_conf_get_int(const char * key)
{
#ifndef SYS_DATA_MODE
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};

    FUNCTION_TRACE();
    if(key == NULL)
    {
        return 0;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    return (int)ini_getl(s,k,0,cwmp_conf_handle->filename);
#else
	return 0;
#endif
}










