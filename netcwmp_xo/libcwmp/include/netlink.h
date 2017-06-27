#ifndef __NETLINK_H__
#define __NETLINK_H__
enum netmsgtype_e
{
	
	NET_MSG_TYPE_DEL = 1,
	NET_MSG_TYPE_ADD = 2,
	NET_MSG_TYPE_NEWLINK_DOWN = 3,
	NET_MSG_TYPE_NEWLINK_UP = 4,
	NET_MSG_TYPE_ROUTE_DEL=5,
	NET_MSG_TYPE_ROUTE_ADD=6,
};
/*
net check msg callback
input:
	ifname: net device name
	etyp : 1 --up ,0--down
	
output:
	0 : success
	-1 : fail
*/

typedef int (*netlink_msg_callback)(char *ifname,int etype);

/*
net check proc init
input:
	netmsg_cb:  typedef int (*netlink_msg_callback)(char *ifname,int etype);
output:
	0 : success
	-1 : fail
*/

extern int netlink_init(netlink_msg_callback netmsg_cb);

/*
net check status
input:
	ifrname :save name point
	len : ifrname  size
output:
	1: up 
	0: all down 
	-1 : error
*/
extern int netchek_status(char *ifrname,unsigned int len);
#endif
