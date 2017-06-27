#ifndef __CWMP_COM_H__
#define __CWMP_COM_H__

#include <cwmp/httpd.h>
#include <unix_com.h>

extern int cwmp_com_GetParameterValues(unix_com_info * pucinfo,char *pname,char *ptype,char *pvalue,int valuelen);
extern int cwmp_com_SetParameterValues(unix_com_info * pucinfo,char *pname,char *ptype,char *pvalue);
extern unix_com_info *cwmp_com_create(cwmp_t * cwmp,const char *paramname,pool_t * pool);
extern int cwmp_com_init(cwmp_t * cwmp);


#endif
