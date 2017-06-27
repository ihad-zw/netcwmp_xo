


#include <cwmp/http.h>
#include <cwmp/event.h>
#include <cwmp/httpd.h>
#include <cwmp/cfg.h>
#include <cwmp/log.h>
#include "cwmp_private.h"
#include "cwmp/session.h"

#include "cwmp/url.h"
#include "cwmp/urlcmd.h"

int http_urlcmd_proc(cwmp_t * cwmp,http_socket_t * sock, http_request_t * request, pool_t * pool)
{
	int i = 0,size = 0;
	char *out = NULL;
	
	
	
	http_request_t * requst_response = NULL;
	
	cwmp_log_info("http_urlcmd_proc  enter ...\n");
	char * data = pool_pcalloc(request->envpool,request->cont_len);
	if(data == NULL)
		return -1;

	http_request_create(&requst_response, request->envpool);
	socklen_t rsa_len = sizeof(struct sockaddr_in);


	if(sock->addr != NULL)
	{
		char *ip = NULL;
		int port = 0;
		ip = inet_ntoa(sock->addr->sin4.sin_addr);
		port = ntohs(sock->addr->sin4.sin_port);
		cwmp_log_info("client:%s:%d\n",ip,port);
		snprintf(requst_response->dest->host,MAX_HOST_NAME_LEN,"%s:%d",ip,port);
	}
	else
	{
		cwmp_log_info("getpeername error\n");
	}
	
	snprintf(requst_response->dest->uri,MAX_URI_LEN,"cwmp");
	requst_response->cont_type = pool_pstrdup(requst_response->envpool,CONTENT_TYPE_URLENCODED);
	

	
	
URLCMDEND:

	if(requst_response != NULL)
	{
		http_post(sock, requst_response, requst_response->writers, requst_response->envpool);		
		http_request_destroy(requst_response);
	}

	
	return 0;
}
