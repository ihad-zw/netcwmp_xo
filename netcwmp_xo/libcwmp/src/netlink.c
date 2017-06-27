#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <linux/if.h>

#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>



#include <signal.h>
#include <unistd.h>

#include <pthread.h>

#include "netlink.h"

#define LOG_PRINTF	printf
//#define DEBUG_PRINTF	printf
#define DEBUG_PRINTF do{}while(0);

static int running = 1;

static pthread_t g_netlink_threadid=0;

static netlink_msg_callback g_netmsg_cb=NULL;


static inline void DEBUG_DUMP(unsigned char *data,int len) 
{
	int i=0;
	char temp[100]={0};
	char datatemp[10]={0};
	DEBUG_PRINTF(" .dump.\n");
	for(i=0;i<len;i++)
	{
		//LOG_PRINTF("",data[i]); 
		sprintf(datatemp,"0x%02x ",data[i]);
		strcat(temp,datatemp);
		if((i+1)%16==0 || (i+1)>=len) 
		{
			DEBUG_PRINTF("%s\n",temp); 
			memset(temp,0,sizeof(temp));
		}
	} 
	DEBUG_PRINTF(" .END.\n");
}

void parseNetlinkAddrMsg(struct nlmsghdr *nlh, int newflag)
{
	struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(nlh);
	struct rtattr *rth = IFA_RTA(ifa);
	int rtl = IFA_PAYLOAD(nlh);

	while (rtl && RTA_OK(rth, rtl)) {
	    if (rth->rta_type == IFA_LOCAL) {
			uint32_t ipaddr = htonl(*((uint32_t *)RTA_DATA(rth)));
			char name[IFNAMSIZ];

			if_indextoname(ifa->ifa_index,name);
	
			DEBUG_PRINTF("%s %s address %d.%d.%d.%d\n",
			       name, (newflag != 0)?"add":"del",
			       (ipaddr >> 24) & 0xff,
			       (ipaddr >> 16) & 0xff,
			       (ipaddr >> 8) & 0xff,
			       ipaddr & 0xff);
			int etype = -1;
			
			if(newflag == 1)
				etype = NET_MSG_TYPE_ADD;
			else
				etype = NET_MSG_TYPE_DEL;
				
			if(g_netmsg_cb != NULL)	
			    g_netmsg_cb(name,etype);
	    }
		
		
	    rth = RTA_NEXT(rth, rtl);
	}
}

void parseBinaryNetlinkMessage(struct nlmsghdr *nh,int newflag) {
    int len = nh->nlmsg_len - sizeof(*nh);
    struct ifinfomsg *ifi;

    if (sizeof(*ifi) > (size_t) len) {
        DEBUG_PRINTF("Got a short RTM_NEWLINK message\n");
        return;
    }

    ifi = (struct ifinfomsg *)NLMSG_DATA(nh);
    if ((ifi->ifi_flags & IFF_LOOPBACK) != 0) {
        DEBUG_PRINTF("parseBinaryNetlinkMessage  ifi_flags : 0x%x ,ifi_family : %d\n",ifi->ifi_flags,ifi->ifi_family);
        return;
    }

    struct rtattr *rta = (struct rtattr *)
      ((char *) ifi + NLMSG_ALIGN(sizeof(*ifi)));
    len = NLMSG_PAYLOAD(nh, sizeof(*ifi));

    while(RTA_OK(rta, len)) {
        switch(rta->rta_type) {
            case IFLA_IFNAME:
            {
                char ifname[IFNAMSIZ];
	            char action[32];
                snprintf(ifname, sizeof(ifname), "%s",
                     (char *) RTA_DATA(rta));
                //action = (ifi->ifi_flags & IFF_LOWER_UP) ? "up" : "down";
            	DEBUG_PRINTF("new link  ---ifi_index: 0x%x,ifi_flags : 0x%x,ifi_change : 0x%x , newflag : %d\n",ifi->ifi_index,ifi->ifi_flags,ifi->ifi_change,newflag);
                sprintf(action,"%s",(ifi->ifi_flags & IFF_RUNNING ) && (ifi->ifi_flags & IFF_LOWER_UP)  ? "up" : "down");
                int etype = -2;
                if((ifi->ifi_flags & IFF_RUNNING) && (ifi->ifi_flags & IFF_LOWER_UP) )
                {
                    etype = NET_MSG_TYPE_NEWLINK_UP;
                }
                else
                {
                    etype = NET_MSG_TYPE_NEWLINK_DOWN;
                }
	            DEBUG_PRINTF("%s link %s\n", ifname, action);
	            
	            if(g_netmsg_cb != NULL)	
			        g_netmsg_cb(ifname,etype);
			    break;
		    
            }
            default:
			   // DEBUG_PRINTF("parseBinaryNetlinkMessage  type : %d\n",rta->rta_type);
			    break;
            
        }

        rta = RTA_NEXT(rta, len);
    }
}


void parseRouteMsg(struct nlmsghdr *nlh, int newflag)
{
	struct rtmsg *rtm; 
	int len = 0;
	char tmp[512]={0};
	struct in_addr inaddr;
	char ifname[IF_NAMESIZE]; 
	int etype = -2;
	
	DEBUG_PRINTF("%s  enter ....%d\n",__FUNCTION__,newflag);
	 
	 rtm = (struct rtmsg *)NLMSG_DATA(nlh);
	// len = nlh->nlmsg_len - NLMSG_LENGTH(sizeof(struct rtmsg));
	//len = NLMSG_PAYLOAD(nlh, sizeof(*rt));
	len = RTM_PAYLOAD(nlh);
	 len = RTA_SPACE(len);
	 if(len < 0)
	 {
	    DEBUG_PRINTF("len < 0\n");
	    return ;
	 }
	 if((rtm->rtm_family != AF_INET) || (rtm->rtm_table != RT_TABLE_MAIN))     
	 {
	     DEBUG_PRINTF("rtm_family %d ,rtm_table %d\n",rtm->rtm_family,rtm->rtm_table);
         return;    
     }
         
     struct rtattr *rta = (struct rtattr *)RTM_RTA(rtm);
	//DEBUG_PRINTF("len = %d, rta = 0x%x\n",len,rta);
//	DEBUG_DUMP((unsigned char *)rta,len);
	 while(rta && RTA_OK(rta, len)) {
	     switch(rta->rta_type)
	     {
	        case RTA_DST:
	             inaddr.s_addr =*(u_int *)  RTA_DATA(rta);     
                 DEBUG_PRINTF("RTA_DST:%s\n",(char *)inet_ntoa(inaddr));  
	   
	            break;
	        case RTA_PREFSRC:
	          
	         //   DEBUG_PRINTF("RTA_SRC  :: %s\n",tmp);
	            break;
	        case RTA_GATEWAY:
	            
	         //   DEBUG_PRINTF("RTA_GATEWAY  :: %s\n",tmp);
	            break;
	        case RTA_OIF:
	            if_indextoname(*(int *)RTA_DATA(rta), ifname);   
	            DEBUG_PRINTF("RTA_OIF  :: %s\n",ifname);
	            if(newflag == 1)
	                etype=NET_MSG_TYPE_ROUTE_ADD;
	            else
	                etype= NET_MSG_TYPE_ROUTE_DEL;
	            
	             if(g_netmsg_cb != NULL)	
			        g_netmsg_cb(ifname,etype);
	            break;
			default:
				break;
        	}
         DEBUG_PRINTF("parseRouteMsg  type : %d\n",rta->rta_type);
       	 rta = RTA_NEXT(rta, len);	
	 }
}

void *netlink_core_proc(void *argv)
{
    struct sockaddr_nl addr;
    int sock, len;
    char buffer[4096];
    struct nlmsghdr *nlh;




    if ((sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) == -1) {
        perror("couldn't open NETLINK_ROUTE socket");
        return NULL;
    }
    LOG_PRINTF("netlink start ......\n");
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;  

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("couldn't bind");
        return NULL;
    }

    while (running && (len = recv(sock, buffer, 4096, 0)) > 0) {
        nlh = (struct nlmsghdr *)buffer;
        while ((NLMSG_OK(nlh, len)) && (nlh->nlmsg_type != NLMSG_DONE)) {
			switch(nlh->nlmsg_type)
			{
				case RTM_NEWADDR:
					parseNetlinkAddrMsg(nlh, 1);
					break;
				case RTM_DELADDR:
					parseNetlinkAddrMsg(nlh, 0);
					break;
				case RTM_NEWLINK:
					parseBinaryNetlinkMessage(nlh,1);
					break;
				case RTM_DELLINK:
					parseBinaryNetlinkMessage(nlh,0);
					break;
				case RTM_NEWROUTE:
					parseRouteMsg(nlh,1);
					break;
				case RTM_DELROUTE:
					parseRouteMsg(nlh,0);
					break;
				default:
					break;
					
			}

			DEBUG_PRINTF("nlh->nlmsg_type = %d\n",nlh->nlmsg_type);
            nlh = NLMSG_NEXT(nlh, len);
        }
    }
	close(sock);
	return 0;

}

int getAllifrname(char *pname,int len)
{
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[2048];

 
    int tmplen = 0 ,ifrnamelen =0;
  	char nametemp[256]={0};

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        LOG_PRINTF("socket error\n");
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        LOG_PRINTF("ioctl error\n");
        return -1;
    }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    char szMac[64];
    int count = 0;
    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    count ++ ;
                    unsigned char * ptr ;
                    ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
                    snprintf(szMac,64,"%02X:%02X:%02X:%02X:%02X:%02X",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
                    DEBUG_PRINTF("%d,Interface name : %s , Mac address : %s \n",count,ifr.ifr_name,szMac);
        			ifrnamelen = strlen(ifr.ifr_name);
        			tmplen = strlen(pname);
        			if((ifrnamelen+tmplen+1) < len)
        			{
        				sprintf(nametemp,"%s,",ifr.ifr_name);
        				strcat(pname,nametemp);
        			}
        				
                }
            }
        }else{
            LOG_PRINTF("get mac info error\n");
            if(strlen(pname)>0)
		    {
		    	pname[strlen(pname)-1]='\0';
		    }
	        return -1;
        }
    }
    if(strlen(pname)>0)
    {
    	pname[strlen(pname)-1]='\0';
    }
    
    return 0;
}

int net_detect(char *net_name)
{
	int skfd=0;
	struct ifreq ifr;
	skfd = socket(AF_INET,SOCK_DGRAM,0);
	if(skfd<0){
		LOG_PRINTF("%s:%d open socket error!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	strcpy(ifr.ifr_name,net_name);
	if(ioctl(skfd,SIOCGIFFLAGS,&ifr)<0){
		LOG_PRINTF("%s:%d IOCTL error!\n",__FUNCTION__,__LINE__);
		LOG_PRINTF("Maybe ethernet inferface %s is not valid!",ifr.ifr_name);
		close(skfd);
		return -1;
	}
	if(ifr.ifr_flags & IFF_RUNNING){
		return 1;
	}else{
		return 0;
	}
}
int netchek_status(char *ifrname,unsigned int len)
{
	char tmpname[256]={0};
	int ret = 0;
	char *pifr = NULL;
	char tmp[256]={0};
	
	ret = getAllifrname(tmpname,sizeof(tmpname));
	if(ret == -1) return -1;
	DEBUG_PRINTF("tmpname = %s\n",tmpname);
	pifr=tmpname;
	while(sscanf(pifr,"%[^,\n],",tmp) == 1)
	{
		DEBUG_PRINTF("pifr = %s\n",pifr);
		
	
		if(net_detect(tmp)== 1)
		{
			if(len>strlen(tmp) && ifrname != NULL)
			{
				memset(ifrname,0,len);
				strcat(ifrname,tmp);
			}
			return 1;
		}
		pifr+=strlen(tmp);
		memset(tmp,0,sizeof(tmp));
		
	}
	
	
	
	return 0;
}
int netlink_init(netlink_msg_callback netmsg_cb)
{
	int ret=0;

	
	if(netmsg_cb == NULL)
	{
		LOG_PRINTF("netmsg_cb == NULL \n");
		return -1;
	}
	g_netmsg_cb = netmsg_cb;
	

	ret = pthread_create(&g_netlink_threadid,NULL,netlink_core_proc,(void *)NULL);
	if(ret != 0)
	{
		perror("netlink_core_proc create fail");
		return -1;
	}
	LOG_PRINTF("netlink_init   ....OK  \n");
	

	return 0;
}
