#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#ifndef ANDROID
#include <ifaddrs.h>
#endif
#include <sys/ioctl.h>

#include <sys/un.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>  



#include "paracom.h"

#define LOG_PRINTF	printf
#define DEBUG_PRINTF	do{}while(0);
//#define DEBUG_PRINTF	printf

int  getCurrentNetname(unsigned char *pname,int len)
{	
#if 0 
	FILE *procpt;	
	char line[512]={0};	
	char tmp[10][256]={0};	
	char cmdtemp[256]={0};	
	unsigned char netname[32]="eth0";	
	int devindex=0;	
	int netlen=0,cplen=0;		
	sprintf(cmdtemp,"ip route");	
	procpt = popen(cmdtemp,"r");		
	DEBUG_PRINTF("cmdtemp = %s\n",cmdtemp);		
	while(fgets(line,sizeof(line),procpt))	{		
		DEBUG_PRINTF("# %s\n",line);				
		sscanf(line,"%[^ ] %[^\n]",tmp[0],tmp[1]);		
		devindex=strspn(line,"dev");		
		DEBUG_PRINTF("devindex = %d  tmp[0] = %s \n",devindex,tmp[0]);		
		if(strcmp(tmp[0],"default")==0)		
		{			
			memset(tmp,0,sizeof(tmp));			
			memset(netname,0,sizeof(netname));			
			sscanf(line,"%[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^\n]",tmp[0],tmp[1],tmp[2],tmp[3],netname,tmp[4]);			
			break;		
		}		
		else		
		{			
			memset(tmp,0,sizeof(tmp));			
			memset(netname,0,sizeof(netname));			
			sscanf(line,"%[^ ] %[^ ] %[^ ] %[^\n]",tmp[0],tmp[1],netname,tmp[2]);		
		}				
	}	
	DEBUG_PRINTF("netname = %s\n",netname);	
	netlen = strlen(netname)+1;	
	cplen = netlen < len ? netlen : len;	
	memcpy(pname,netname,cplen);		
	pclose(procpt);	
	
	return 0;
#else
	FILE *procpt = NULL;	
	char line[512]={0};	
	char tmp[10][256]={{0}};	
	char cmdtemp[256]={0};	
	char netname[256]="eth0";	


	sprintf(cmdtemp,"busybox route");	
	procpt = popen(cmdtemp,"r");	
	if(procpt == NULL)
	{
		return -1;
	}
	DEBUG_PRINTF("cmdtemp = %s\n",cmdtemp);		
	while(fgets(line,sizeof(line),procpt))	{		
		DEBUG_PRINTF(">%s",line);				
		if(sscanf(line,"%s %*s  %*s %*s  %*s %*s %*s %s",tmp[0],netname) != 2)
			continue;
		if(strcmp(tmp[0],"default")==0)		
		{			
	
			break;		
		}		
		else		
		{			

		}				
	}	
	DEBUG_PRINTF("netname = %s\n",netname);	
	snprintf(pname,len,"%s",netname);	
	pclose(procpt);	
	return 0;

#endif
}

int  macaddress_func(struct sockaddr *phwaddr)
{
	  int socket_fd;
     struct ifreq ifr;

	char netname[32]={0};
	getCurrentNetname(netname,sizeof(netname));
	
	  if(strlen(netname)==0) return -1;
	 socket_fd = socket(AF_INET,SOCK_DGRAM,0); 
	   if(socket_fd ==-1)
	  {
	     LOG_PRINTF("socket error!\n");
	    return -1;
	  }

	  strcpy(ifr.ifr_name,netname);

	//这里要特别的注意，根据自己的系统的不同会有所差异有可能会是“eth0“。 如果出现错误提示 No such device 可能是这里的问题
	   
	  if(ioctl(socket_fd,SIOCGIFHWADDR,&ifr) < 0)
	  {
	    LOG_PRINTF("ioctl error\n");
		close(socket_fd);
	    return -1;
	  }
	  else
	  {
	      memcpy(phwaddr,(struct sockaddr *)&(ifr.ifr_hwaddr),sizeof(struct sockaddr ));
	  	 DEBUG_PRINTF("current mac = %02x:%02x:%02x:%02x:%02x:%02x\n",  
		  	(unsigned char)ifr.ifr_hwaddr.sa_data[0],
	            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
	            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
	            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
	            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
	            (unsigned char)ifr.ifr_hwaddr.sa_data[5]
            );

	  }
		close(socket_fd);
	  return 0;
}
int getMACAddress(unsigned char *pdata,int datasize)
{
	unsigned char temp[256]={0};
	int templen = 0;
	struct sockaddr hwaddr={0};
	int ret = 0;
//	cpuusage = cpuusage_func();
	ret = macaddress_func(&hwaddr);

	sprintf(temp,"%02X:%02X:%02X:%02X:%02X:%02X",(unsigned char)hwaddr.sa_data[0],
	            (unsigned char)hwaddr.sa_data[1],
	            (unsigned char)hwaddr.sa_data[2],
	            (unsigned char)hwaddr.sa_data[3],
	            (unsigned char)hwaddr.sa_data[4],
	            (unsigned char)hwaddr.sa_data[5]
            );
	templen = strlen(temp)+1;
	if(datasize < templen) return -1;
	memset(pdata,0,datasize);
	strcpy(pdata,temp);
	return ret;
}
int  ipaddress_func(struct sockaddr_in *sin)
{
	  int socket_fd;
     	 struct ifreq ifr;
	
	 char netname[32]={0};
	 getCurrentNetname(netname,sizeof(netname));
	  if(strlen(netname)==0) return -1;
	 socket_fd = socket(AF_INET,SOCK_DGRAM,0); 
	   if(socket_fd ==-1)
	  {
	     LOG_PRINTF("socket error!\n");
	    return -1;
	  }

	  strcpy(ifr.ifr_name,netname);

	//这里要特别的注意，根据自己的系统的不同会有所差异有可能会是“eth0“。 如果出现错误提示 No such device 可能是这里的问题
	   
	  if(ioctl(socket_fd,SIOCGIFADDR,&ifr) < 0)
	  {
	       LOG_PRINTF("ioctl error\n");
	       close(socket_fd);
	       return -1;
	  }
	  else
	  {
	      	  memcpy(sin,(struct sockaddr_in *)&(ifr.ifr_addr),sizeof(struct sockaddr_in ));
	  	  DEBUG_PRINTF("current IP = %s\n",inet_ntoa(sin->sin_addr));

	  }
		close(socket_fd);
	  return 0;
}
int getIPAddress(unsigned char *pdata,int datasize)
{
	unsigned char temp[256]={0};
	int templen = 0;
	int ret = 0;
	struct sockaddr_in sin={0};
//	cpuusage = cpuusage_func();
	ret = ipaddress_func(&sin);
	sprintf(temp,"%s",inet_ntoa(sin.sin_addr));
	templen = strlen(temp)+1;
	if(datasize < templen) return -1;
	memset(pdata,0,datasize);
	strcpy(pdata,temp);
	return ret;
}
int setIPAddress(unsigned char *pdata)
{

	struct ifreq temp;
	struct sockaddr_in *addr;
	int fd = 0;
	int ret = -1;
		
	 char netname[32]={0};
	 getCurrentNetname(netname,sizeof(netname));
	 if(strlen(netname)==0) return -1;
		
	strcpy(temp.ifr_name,netname);
	if ((fd=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		return -1;
	}
	addr = (struct sockaddr_in *)&(temp.ifr_addr);
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(pdata);
	ret = ioctl(fd, SIOCSIFADDR, &temp);
	close(fd);
	if (ret < 0)
		return -1;


	return 0;
}
int netmask_func(struct sockaddr *pnetmask)
{
	  int socket_fd;
     struct ifreq ifr;

	char netname[32]={0};
	getCurrentNetname(netname,sizeof(netname));
	
	  if(strlen(netname)==0) return -1;
	 socket_fd = socket(AF_INET,SOCK_DGRAM,0); 
	   if(socket_fd ==-1)
	  {
	     LOG_PRINTF("socket error!\n");
	    return -1;
	  }

	  strcpy(ifr.ifr_name,netname);

	//这里要特别的注意，根据自己的系统的不同会有所差异有可能会是“eth0“。 如果出现错误提示 No such device 可能是这里的问题
	   
	  if(ioctl(socket_fd,SIOCGIFNETMASK,&ifr) < 0)
	  {
	    LOG_PRINTF("ioctl error\n");
		close(socket_fd);
	    return -1;
	  }
	  else
	  {
			
	      memcpy(pnetmask,(struct sockaddr *)&(ifr.ifr_netmask),sizeof(struct sockaddr ));
	  	 DEBUG_PRINTF("current netmask = %d.%d.%d.%d\n",  
		  	(unsigned char)ifr.ifr_netmask.sa_data[0],
	            (unsigned char)ifr.ifr_netmask.sa_data[1],
	            (unsigned char)ifr.ifr_netmask.sa_data[2],
	            (unsigned char)ifr.ifr_netmask.sa_data[3]

            );

	  }
		close(socket_fd);
	  return 0;
}
int getNetMask(unsigned char *pdata,int datasize)
{
	unsigned char temp[256]={0};
	int templen = 0;
	struct sockaddr netmask={0};
	int ret = 0;
//	cpuusage = cpuusage_func();
	ret = netmask_func(&netmask);

	sprintf(temp,"%d.%d.%d.%d\n",  
		  	(unsigned char)netmask.sa_data[0],
	            (unsigned char)netmask.sa_data[1],
	            (unsigned char)netmask.sa_data[2],
	            (unsigned char)netmask.sa_data[3]
            );
	templen = strlen(temp)+1;
	if(datasize < templen) return -1;
	memset(pdata,0,datasize);
	strcpy(pdata,temp);
	return ret ;
}

int setNetMask(unsigned char *pdata)
{

	struct ifreq temp;
	struct sockaddr *addr;
	int fd = 0;
	int ret = -1;
		
	 char netname[32]={0};
	 getCurrentNetname(netname,sizeof(netname));
	 if(strlen(netname)==0) return -1;
		
	strcpy(temp.ifr_name,netname);
	if ((fd=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		return -1;
	}
	addr = (struct sockaddr *)&(temp.ifr_netmask);
	sscanf(pdata,"%d.%d.%d.%d",&(addr->sa_data[0]),&(addr->sa_data[1]),
		&(addr->sa_data[2]),&(addr->sa_data[3]));

	ret = ioctl(fd, SIOCSIFNETMASK, &temp);
	close(fd);
	if (ret < 0)
		return -1;


	return 0;
}
int getDefaultGateway(unsigned char *pdata,int datasize)
{
#if 0 
	FILE *procpt;	
	char line[512]={0};	
	char tmp[10][256]={0};	
	char cmdtemp[256]={0};	
	char gateway[64]="192.168.1.1";	
	int devindex=0;	
	int waylen=0,cplen=0;		
	char *psubtmp = NULL;
	
	sprintf(cmdtemp,"ip route");	
	procpt = popen(cmdtemp,"r");		
	DEBUG_PRINTF("cmdtemp = %s\n",cmdtemp);		
	while(fgets(line,sizeof(line),procpt))	{		
		DEBUG_PRINTF("# %s\n",line);				
		sscanf(line,"%[^ ] %[^\n]",tmp[0],tmp[1]);		
		devindex=strspn(line,"dev");		
		DEBUG_PRINTF("devindex = %d  tmp[0] = %s \n",devindex,tmp[0]);		
		if(strcmp(tmp[0],"default")==0)		
		{			
			memset(tmp,0,sizeof(tmp));			
			memset(gateway,0,sizeof(gateway));			
		//	sscanf(&line,"%[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^\n]",tmp[0],tmp[1],gateway,tmp[2],tmp[3],tmp[4]);			
		   psubtmp = strstr(line,"via");
		   if(psubtmp != NULL)
		   {
			 	sscanf(psubtmp,"%[^ ] %[^ ] %[^\n]",tmp[0],gateway,tmp[1]);
		   }
			break;		
		}		
		else		
		{			

		}				
	}	
	DEBUG_PRINTF("gateway = %s\n",gateway);	
	waylen = strlen(gateway)+1;	
	cplen = waylen < datasize ? waylen : datasize;	
	memcpy(pdata,gateway,cplen);		
	pclose(procpt);	
	return 0;
#else
	FILE *procpt;	
	char line[512]={0};	
	char tmp[10][256]={0};	
	char cmdtemp[256]={0};	
	char gateway[256]="192.168.1.1";	

	
	sprintf(cmdtemp,"busybox route");	
	procpt = popen(cmdtemp,"r");		
	if(procpt == NULL)
		return -1;
	
	DEBUG_PRINTF("cmdtemp = %s\n",cmdtemp);		
	while(fgets(line,sizeof(line),procpt))	{		
		DEBUG_PRINTF("> %s\n",line);				
		if(sscanf(line,"%s %s  %*s %*s  %*s %*s %*s %*s",tmp[0],gateway) != 2)
			continue;
		if(strcmp(tmp[0],"default")==0)		
		{			
			break;
		}		
		else		
		{			

		}				
	}	
	DEBUG_PRINTF("gateway = %s\n",gateway);	
	snprintf(pdata,datasize,"%s",gateway);
	pclose(procpt);	
	return 0;
#endif
}
/**
 * get dns.
 * @param dns1 a pointer to save first dns.
 * @param dns2 a pointer to save second dns.
 * @return 0 success, or fail.
 */
int getDNSServers(char *dns1,int size1, char *dns2,int size2)
{
    int fd = -1;
    int size = 0;
    char strBuf[100];
    char tmpBuf[100];
    int buf_size = sizeof(strBuf);
    char *p = NULL;
    char *q = NULL;
    int i = 0;
    int j = 0;
    int count = 0;
    
    fd = open("/etc/resolv.conf", O_RDONLY);
    if(-1 == fd)
    {
        LOG_PRINTF("%s open error \n", __func__);
        return -1;
    }
    size = read(fd, strBuf, buf_size);
    if(size < 0)
    {
        LOG_PRINTF("%s read file len error \n", __func__);
        close(fd);
        return -1;
    }
    strBuf[buf_size] = '\0';
    close(fd);

    while(i < buf_size)
    {
        if((p = strstr(&strBuf[i], "nameserver")) != NULL)
        {
            j++;
            p += 1;
            count = 0;
            
            memset(tmpBuf, 0xff, 100);
            memcpy(tmpBuf, p, 100);
            tmpBuf[sizeof(tmpBuf) -1 ] = '\0';
            
            q = p;
            while(*q != '\n')
            {
                q++;
                count++;
            }
            i += (sizeof("nameserver") + count);
            
            if(1 == j)
            {
					if(dns1 != NULL)
					{
		                strncpy(dns1, p, size1);
		                dns1[size1-1]='\0';
					}
            }
            else if(2 == j)
            {
					if(dns2 != NULL)
					{
		                strncpy(dns2, p, size2);
		                dns2[size2-1]='\0';
					}
            }
        } 
        else 
        {
            i++;
        }
    }

    return 0;
}
 int getDNSServers1(unsigned char *pdata,int datasize)
{
	return getDNSServers(pdata,datasize,NULL,0);
}