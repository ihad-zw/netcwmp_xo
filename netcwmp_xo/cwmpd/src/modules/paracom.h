#ifndef __PARACOM_H__
#define __PARACOM_H__
extern int  getCurrentNetname(unsigned char *pname,int len);
extern int getMACAddress(unsigned char *pdata,int datasize);
extern int getIPAddress(unsigned char *pdata,int datasize);
extern int setIPAddress(unsigned char *pdata);
extern int getNetMask(unsigned char *pdata,int datasize);
extern int setNetMask(unsigned char *pdata);
extern int getDefaultGateway(unsigned char *pdata,int datasize);
extern int getDNSServers(char *dns1,int size1, char *dns2,int size2);
extern int getDNSServers1(unsigned char *pdata,int datasize);

#endif
