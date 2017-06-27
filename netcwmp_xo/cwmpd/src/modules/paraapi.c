#include "paraapi.h"
#include "paracom.h"

int dm_getMACAddress(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	return getMACAddress(pdata,datasize);
}
int dm_getIPAddress(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	return getIPAddress(pdata,datasize);
}
int dm_setIPAddress(cwmp_t * cwmp,unsigned char *pdata)
{
	return setIPAddress(pdata);
}
int dm_getNetMask(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	return getNetMask(pdata,datasize);
}
int dm_setNetMask(cwmp_t * cwmp,unsigned char *pdata)
{
	return setNetMask(pdata);
}
int dm_getDefaultGateway(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	return getDefaultGateway(pdata,datasize);
}
int dm_getDNSServers(cwmp_t * cwmp,char *dns1,int size1, char *dns2,int size2)
{
	return getDNSServers(dns1,size1, dns2,size2);
}
int dm_getDNSServers1(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	return getDNSServers1(pdata,datasize);
}


int dm_getConnectionRequestURL(cwmp_t * cwmp,unsigned char *pdata,int datasize)
{
	int ret = 0;
	unsigned char iptemp[32]={0};
	
	ret = getIPAddress(iptemp,sizeof(iptemp));
	if(ret != 0)
	{
		sprintf(iptemp,"127.0.0.1");
	}
	snprintf(pdata,datasize,"http://%s:%d",iptemp,cwmp->httpd_port);
	return ret;
}