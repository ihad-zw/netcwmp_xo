#ifndef __PARAAPI_H__
#define __PARAAPI_H__
#include "cwmp/cwmp.h"

extern int dm_getMACAddress(cwmp_t * cwmp,unsigned char *pdata,int datasize);
extern int dm_getIPAddress(cwmp_t * cwmp,unsigned char *pdata,int datasize);
extern int dm_setIPAddress(cwmp_t * cwmp,unsigned char *pdata);
extern int dm_getNetMask(cwmp_t * cwmp,unsigned char *pdata,int datasize);
extern int dm_setNetMask(cwmp_t * cwmp,unsigned char *pdata);
extern int dm_getDefaultGateway(cwmp_t * cwmp,unsigned char *pdata,int datasize);
extern int dm_getDNSServers(cwmp_t * cwmp,char *dns1,int size1, char *dns2,int size2);
extern int dm_getDNSServers1(cwmp_t * cwmp,unsigned char *pdata,int datasize);
extern int dm_getConnectionRequestURL(cwmp_t * cwmp,unsigned char *pdata,int datasize);

#endif
