
#include <cwmp/http.h>
#include <cwmp/event.h>
#include <cwmp/httpd.h>
#include <cwmp/cfg.h>
#include <cwmp/log.h>
#include "cwmp_private.h"
#include "cwmp/session.h"

#include "cwmp/httpjson.h"
#include "cJSON.h"
#include "unix_com.h"

int http_json_proc(cwmp_t * cwmp,http_socket_t * sock, http_request_t * request, pool_t * pool)
{
	int i = 0,size = 0;
	char *out = NULL;
	
	cJSON * root = NULL;
	cJSON * response_root = NULL;
	
	http_request_t * requst_response = NULL;
							 
	char * pdata = pool_pcalloc(request->envpool,request->cont_len);
	if(pdata == NULL)
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
	requst_response->cont_type = pool_pstrdup(requst_response->envpool,"application/json");
	
	cwmp_chunk_copy(pdata,request->readers,request->cont_len);
	
	cwmp_chunk_print(request->readers);
	

	char *jsonout = NULL;
	cJSON * parseroot = cJSON_Parse(pdata);

	
	if(parseroot == NULL)
	{
		goto JSONEND;
	}	
	cJSON *jsonroot = cJSON_CreateObject();

    	if(jsonroot == NULL)
    	{
		goto JSONEND;
    	}
	cJSON *parsecwmpobj = cJSON_GetObjectItem(parseroot,"cwmp");   
	char *cmdvalue = cJSON_GetObjectItem(parsecwmpobj,"cmd")->valuestring;
	cJSON *cwmpobj = NULL;
	
	cwmp_log_info("cmdvalue = %s\n",cmdvalue);
	
	if(TRstrcasecmp(cmdvalue,JSONCMD_GETPARAMETERVALUES) == 0)
	{
		cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
		
		//cwmp_log_info("cjson obj = 0x%x @line %d\n",parseparalistobj,__LINE__);
		
		if(parseparalistobj == NULL)
			goto JSONEND;
		
		cwmpobj = cJSON_CreateObject();
		if(cwmpobj == NULL)
			goto JSONEND;
		
	      cJSON_AddItemToObject(jsonroot, "cwmp", cwmpobj);
             cJSON_AddItemToObject(cwmpobj, "cmd", cJSON_CreateString(JSONCMD_GETPARAMETERVALUES_RESPONSE));
			 
	 	cJSON *paralistobj = cJSON_CreateObject();
		if(paralistobj == NULL)
			goto JSONEND;
		
		cJSON_AddItemToObject(cwmpobj, "ParameterList", paralistobj);
		
		int listobjsize = cJSON_GetArraySize(parseparalistobj);
		cwmp_log_info("ParameterList size %d\n",listobjsize);
		int i = 0;
		for( i=0;i<listobjsize;i++)
		{
			char *listname = cJSON_GetArrayItem(parseparalistobj,i)->string;
			
			cwmp_log_info("para name :%s\n",listname);
			if(listname == NULL)
				continue;
			
			cJSON * parseparaobj = cJSON_GetArrayItem(parseparalistobj,i);
			
			cJSON *paraobj = cJSON_CreateObject();			
			if(paraobj == NULL)
				goto JSONEND;
			cJSON_AddItemToObject(paralistobj,listname, paraobj);
			
			if(parseparaobj  != NULL)
			{
				char * paratype = cJSON_GetObjectItem(parseparaobj,"type")->valuestring;
				char * paravalue = cwmp_data_get_parameter_value(cwmp, cwmp->root, listname, cwmp->pool);	
				
			      cwmp_log_info("%s-%s-%s",listname,paravalue,paratype);
				

				if(paravalue != NULL)
					cJSON_AddStringToObject(paraobj,"value",paravalue); 
				else
					cJSON_AddStringToObject(paraobj,"value","(null)"); 
				
			}
			
	
		}

	}
	else if(TRstrcasecmp(cmdvalue,JSONCMD_SETPARAMETERVALUES) == 0)
	{
		cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
		
		//cwmp_log_info("cjson obj = 0x%x @line %d\n",parseparalistobj,__LINE__);
		
		if(parseparalistobj == NULL)
			goto JSONEND;
		
		cwmpobj = cJSON_CreateObject();
		if(cwmpobj == NULL)
			goto JSONEND;
		
	      cJSON_AddItemToObject(jsonroot, "cwmp", cwmpobj);
             cJSON_AddItemToObject(cwmpobj, "cmd", cJSON_CreateString(JSONCMD_SETPARAMETERVALUES_RESPONSE));
			 
	 	cJSON *paralistobj = cJSON_CreateObject();
		if(paralistobj == NULL)
			goto JSONEND;
		
		cJSON_AddItemToObject(cwmpobj, "ParameterList", paralistobj);
		
		int listobjsize = cJSON_GetArraySize(parseparalistobj);
		cwmp_log_info("ParameterList size %d\n",listobjsize);
		int i = 0;
		for( i=0;i<listobjsize;i++)
		{
			char *listname = cJSON_GetArrayItem(parseparalistobj,i)->string;
			
			cwmp_log_info("para name :%s\n",listname);
			if(listname == NULL)
				continue;
			
			cJSON * parseparaobj = cJSON_GetArrayItem(parseparalistobj,i);
			
			cJSON *paraobj = cJSON_CreateObject();			
			if(paraobj == NULL)
				goto JSONEND;
			cJSON_AddItemToObject(paralistobj,listname, paraobj);
			
			if(parseparaobj  != NULL)
			{
				char * paratype = cJSON_GetObjectItem(parseparaobj,"type")->valuestring;
				char *paravalue = cJSON_GetObjectItem(parseparaobj,"value")->valuestring;
				if(paravalue == NULL)
					goto JSONEND;
				
				int ret = cwmp_data_set_parameter_value(cwmp, cwmp->root, listname,paravalue, strlen(paravalue),cwmp->pool);	
				
			      cwmp_log_info("%s-%s-%s",listname,paravalue,paratype);
				

				if(ret != 0)
					cJSON_AddStringToObject(paraobj,"status","-1"); 
				else
					cJSON_AddStringToObject(paraobj,"status","0"); 
				
			}
			
	
		}

	}
	
 	 jsonout = cJSON_Print(jsonroot);

	cwmp_chunk_write_string(requst_response->writers,jsonout,TRstrlen(jsonout),requst_response->envpool);
JSONEND:
	if(parseroot != NULL)
	{
		cJSON_Delete(parseroot);
		parseroot = NULL;
	}
	if(jsonroot != NULL)
	{
		cJSON_Delete(jsonroot);
		jsonroot = NULL;
	}
	if(jsonout != NULL)
	{
		free(jsonout);
		jsonout = NULL;
	}
	if(requst_response != NULL)
	{
		http_post(sock, requst_response, requst_response->writers, requst_response->envpool);		
		http_request_destroy(requst_response);
	}

	return 0;
}

