/************************************************************************
 * Id: log.c                                                            *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/
 
 
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "cwmp/log.h"

#ifdef ANDROID
#include <cutils/log.h>
#include <stdio.h>
#define LOG_TAG "cwmpd"
#define LOGI(fmt, args...) {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);}
#else
#include <stdio.h>
#define LOGI 
#endif



static cwmp_log_t 	    g_cwmp_log_file=
{
	NULL,//stdout,
	CWMP_LOG_ERROR,
	NULL,
};
static cwmp_log_t 		*	g_ot_log_file_ptr = &g_cwmp_log_file;

static char g_temp_buf[1024]={0};



//��ȡLog�����ļ�

int cwmp_log_init(const char * filename, int level)
{

    g_cwmp_log_file.file = NULL;
    g_cwmp_log_file.name = NULL;
    if (filename)
    {
        g_cwmp_log_file.file = fopen(filename,"a+");
        g_cwmp_log_file.name = strdup(filename);
    }

    if (g_cwmp_log_file.file == NULL)
    {
        g_cwmp_log_file.file = stdout;
    }

    g_cwmp_log_file.level = level;

    g_ot_log_file_ptr = &g_cwmp_log_file;

    return 0;
}

void cwmp_log_fini()
{
    if (g_cwmp_log_file.name)
    {
        free(g_cwmp_log_file.name);
    }

    if ((g_cwmp_log_file.file != stdout) && (g_cwmp_log_file.file != NULL))
    {
        fclose(g_cwmp_log_file.file);
    }

}

void cwmp_log_write(int level, cwmp_log_t * log, const char * fmt, va_list ap)
{
    cwmp_log_t * logger;

    if (log)
    {
        if (log->level < level )
        {
            return;
        }
        logger = log;
    }
    else
    {
        if (g_cwmp_log_file.level < level)
        {
            return;
        }
        logger = &g_cwmp_log_file;
    }

    logger = g_ot_log_file_ptr;
	if(logger->file == NULL)
		logger->file = stdout;


    vfprintf(logger->file, fmt, ap);
  //  fprintf(logger->file, "\n");
#ifdef ANDROID
	vsnprintf(g_temp_buf, sizeof(g_temp_buf)-1, fmt, ap);
	LOGI("%s",g_temp_buf);
#endif
	
    fflush(logger->file);
}

void cwmp_log_tracer(int level, cwmp_log_t * log,const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(level, log, fmt, ap);
    va_end(ap);
}

void cwmp_log_debug(const char * fmt, ...)
{
    va_list ap;
    if (g_ot_log_file_ptr->level < CWMP_LOG_DEBUG)
        return;


    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_DEBUG, g_ot_log_file_ptr, fmt, ap);
    va_end(ap);
}

void cwmp_log_info(const char * fmt, ...)
{
    va_list ap;
    if (g_ot_log_file_ptr->level < CWMP_LOG_INFO)
        return;


    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_INFO, g_ot_log_file_ptr, fmt, ap);
    va_end(ap);
}

void cwmp_log_error(const char * fmt, ...)
{
    va_list ap;
    if (g_ot_log_file_ptr->level < CWMP_LOG_ERROR)
        return;


    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_ERROR, g_ot_log_file_ptr, fmt, ap);
    va_end(ap);
}

void cwmp_log_alert(const char * fmt, ...)
{
    va_list ap;
    if (g_ot_log_file_ptr->level < CWMP_LOG_ALERT)
        return;


    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_ALERT, g_ot_log_file_ptr, fmt, ap);
    va_end(ap);
}

void cwmp_log_critical(const char * fmt, ...)
{
    va_list ap;
    if (g_ot_log_file_ptr->level < CWMP_LOG_CRIT)
        return;


    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_CRIT, g_ot_log_file_ptr, fmt, ap);
    va_end(ap);
}
void cwmp_log_dump(unsigned char *data,int len) 
{
	int i=0;
	char temp[100]={0};
	char datatemp[10]={0};
	
	for(i=0;i<len;i++)
	{
		//LOG_PRINTF("",data[i]); 
		sprintf(datatemp,"0x%02x ",data[i]);
		strcat(temp,datatemp);
		if((i+1)%16==0) 
		{
			cwmp_log_info("%s\n",temp); 
			memset(temp,0,sizeof(temp));
		}
	} 
	cwmp_log_info(" .END.\n");
}
