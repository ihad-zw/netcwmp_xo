#include "cwmp/cwmp_com.h"
#include "cwmp/types.h"
#include "cwmp/cwmp.h"
#include "cwmp/session.h"
#include <unix_com.h>
#include "cJSON.h"


#define DEBUG_PRINTF cwmp_log_info
#define LOG_PRINTF	cwmp_log_info

typedef struct _cwmp_com_st
{
	const char * name;
	unix_com_callback uccb;
};



static int unix_com_cb(int socket_fd,void *arg,void *pdata,int size,void *para);


struct _cwmp_com_st  cwmp_com_data[]=
{
	{"unix.com.itms.cwmps",unix_com_cb},
	{"unix.com.app.cwmps",unix_com_cb},
	{"unix.com.nativeapi.cwmps",unix_com_cb},
};

static unix_com_info *g_cwmp_puccinfo[sizeof(cwmp_com_data)/sizeof(struct _cwmp_com_st)] = {0};

int cwmp_com_GetParameterValues(unix_com_info * pucinfo,char *pname,char *ptype,char *pvalue,int valuelen)
{
	int ret = -1;
	
	cJSON *jsonroot_2 =NULL;
	char *jsonout = NULL;

	char recivebuf[4096]={0};

	cJSON * parseroot = NULL;

	cJSON *objitem = NULL;
	
	
	if(pname == NULL ||ptype == NULL | pvalue == NULL) return -1;
	
	jsonroot_2 = cJSON_CreateObject();

    	if(jsonroot_2 == NULL)
    	{
		goto JSONEND;
    	}
	cJSON *cwmpobj_2 = NULL;
	cwmpobj_2 = cJSON_CreateObject();
	if(cwmpobj_2 == NULL)
		goto JSONEND;
		
		 
	cJSON_AddItemToObject(jsonroot_2, "cwmp", cwmpobj_2);
      cJSON_AddItemToObject(cwmpobj_2, "cmd", cJSON_CreateString(JSONCMD_GETPARAMETERVALUES));
			 
	cJSON *paralistobj_2 = cJSON_CreateArray();
	if(paralistobj_2 == NULL)
		goto JSONEND;
		
	cJSON_AddItemToObject(cwmpobj_2, "ParameterList", paralistobj_2);
	cJSON *paraobj_2 = cJSON_CreateObject();			
	if(paraobj_2 == NULL)
		goto JSONEND;

			
	cJSON *paraarryobj_2 = cJSON_CreateObject();	
	if(paraarryobj_2 == NULL)
		goto JSONEND;
			
	cJSON_AddStringToObject(paraarryobj_2,"paraname", pname);
	cJSON_AddItemToObject(paraarryobj_2,"paravalue", paraobj_2);
	cJSON_AddItemToArray(paralistobj_2,paraarryobj_2);

	cJSON_AddStringToObject(paraobj_2,"type",ptype); 
	 jsonout = cJSON_PrintUnformatted(jsonroot_2);
	 
	 DEBUG_PRINTF("%s  :   jsonout = %s\n",__FUNCTION__,jsonout);
	unix_com_other(pucinfo,recivebuf,sizeof(recivebuf),jsonout,strlen(jsonout));

	if(strlen(recivebuf) == 0)
		goto JSONEND;
	
	DEBUG_PRINTF("%s  :   recivebuf = %s\n",__FUNCTION__,recivebuf);
	
	parseroot = cJSON_Parse(recivebuf);
	if(parseroot == NULL)
	{
		goto JSONEND;
	}	
	
	cJSON *parsecwmpobj = cJSON_GetObjectItem(parseroot,"cwmp");   
	char *cmdvalue = NULL;
	objitem = cJSON_GetObjectItem(parsecwmpobj,"cmd");
	if(objitem != NULL) cmdvalue = objitem->valuestring;
	
//	LOG_PRINTF("%s  cmdvalue = %s\n",__FUNCTION__,cmdvalue);
	if(strcasecmp(cmdvalue,JSONCMD_GETPARAMETERVALUES_RESPONSE) != 0)
	{
		goto JSONEND;
	}
	
	cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
	if(parseparalistobj == NULL)
		goto JSONEND;
	
	int listobjsize = cJSON_GetArraySize(parseparalistobj);
//	DEBUG_PRINTF("ParameterList size %d\n",listobjsize);
	int i = 0;
	for( i=0;i<listobjsize;i++)
	{
		cJSON *parseparaarryobj = cJSON_GetArrayItem(parseparalistobj,i);
		char *paraname =NULL;
		objitem = cJSON_GetObjectItem(parseparaarryobj,"paraname");
		if(objitem != NULL) paraname = objitem->valuestring;
			
		//DEBUG_PRINTF(">>>>>>>   = %s\n",cJSON_GetObjectItem(paralistarryobj,"paraname")->valuestring);
	//	DEBUG_PRINTF("para name :%s\n",paraname);
			
		if(paraname == NULL)
			continue;
			
		cJSON * parseparaobj = cJSON_GetObjectItem(parseparaarryobj,"paravalue");
		if(parseparaobj  != NULL)
		{
				char * value = NULL;
				objitem = cJSON_GetObjectItem(parseparaobj,"value");
				
				if(objitem != NULL ) value = objitem ->valuestring;

				if(value != NULL)
					snprintf(pvalue,valuelen,"%s",value);
		}
	}
	
JSONEND:
	if(jsonroot_2 != NULL)
	{
		cJSON_Delete(jsonroot_2);
		jsonroot_2 = NULL;
	}
	
	if(jsonout != NULL)
	{
		free(jsonout);
		jsonout = NULL;
	}
	if(parseroot != NULL)
	{
		cJSON_Delete(parseroot);
		parseroot = NULL;
	}
	DEBUG_PRINTF("%s   ret = %d\n",__FUNCTION__,ret);
	return ret;
}
int cwmp_com_SetParameterValues(unix_com_info * pucinfo,char *pname,char *ptype,char *pvalue)
{
	int ret = -1;
	
	cJSON *jsonroot_2 =NULL;
	char *jsonout = NULL;

	char recivebuf[4096]={0};

	cJSON * parseroot = NULL;

	cJSON *objitem = NULL;
	
	
	if(pname == NULL ||ptype == NULL || pvalue == NULL) return -1;
	
	jsonroot_2 = cJSON_CreateObject();

    	if(jsonroot_2 == NULL)
    	{
		goto JSONEND;
    	}
	cJSON *cwmpobj_2 = NULL;
	cwmpobj_2 = cJSON_CreateObject();
	if(cwmpobj_2 == NULL)
		goto JSONEND;
		
		 
	cJSON_AddItemToObject(jsonroot_2, "cwmp", cwmpobj_2);
      cJSON_AddItemToObject(cwmpobj_2, "cmd", cJSON_CreateString(JSONCMD_SETPARAMETERVALUES));
			 
	cJSON *paralistobj_2 = cJSON_CreateArray();
	if(paralistobj_2 == NULL)
		goto JSONEND;
		
	cJSON_AddItemToObject(cwmpobj_2, "ParameterList", paralistobj_2);
	cJSON *paraobj_2 = cJSON_CreateObject();			
	if(paraobj_2 == NULL)
		goto JSONEND;

			
	cJSON *paraarryobj_2 = cJSON_CreateObject();	
	if(paraarryobj_2 == NULL)
		goto JSONEND;
			
	cJSON_AddStringToObject(paraarryobj_2,"paraname", pname);
	cJSON_AddItemToObject(paraarryobj_2,"paravalue", paraobj_2);
	cJSON_AddItemToArray(paralistobj_2,paraarryobj_2);

	cJSON_AddStringToObject(paraobj_2,"value",pvalue); 
	cJSON_AddStringToObject(paraobj_2,"type",ptype); 
	 jsonout = cJSON_PrintUnformatted(jsonroot_2);
	 
	 DEBUG_PRINTF("%s  :   jsonout = %s\n",__FUNCTION__,jsonout);
	unix_com_other(pucinfo,recivebuf,sizeof(recivebuf),jsonout,strlen(jsonout));

	if(strlen(recivebuf) == 0)
		goto JSONEND;
	
	DEBUG_PRINTF("%s  :   recivebuf = %s\n",__FUNCTION__,recivebuf);
	
	parseroot = cJSON_Parse(recivebuf);
	if(parseroot == NULL)
	{
		goto JSONEND;
	}	
	
	cJSON *parsecwmpobj = cJSON_GetObjectItem(parseroot,"cwmp");   
	char *cmdvalue = NULL;
	objitem = cJSON_GetObjectItem(parsecwmpobj,"cmd");
	if(objitem != NULL) cmdvalue = objitem->valuestring;
	
//	LOG_PRINTF("%s  cmdvalue = %s\n",__FUNCTION__,cmdvalue);
	if(strcasecmp(cmdvalue,JSONCMD_SETPARAMETERVALUES_RESPONSE) != 0)
	{
		goto JSONEND;
	}
	
	cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
	if(parseparalistobj == NULL)
		goto JSONEND;
	
	int listobjsize = cJSON_GetArraySize(parseparalistobj);
//	DEBUG_PRINTF("ParameterList size %d\n",listobjsize);
	int i = 0;
	for( i=0;i<listobjsize;i++)
	{
		cJSON *parseparaarryobj = cJSON_GetArrayItem(parseparalistobj,i);
		char *paraname =NULL;
		objitem = cJSON_GetObjectItem(parseparaarryobj,"paraname");
		if(objitem != NULL) paraname = objitem->valuestring;
			
		//DEBUG_PRINTF(">>>>>>>   = %s\n",cJSON_GetObjectItem(paralistarryobj,"paraname")->valuestring);
	//	DEBUG_PRINTF("para name :%s\n",paraname);
			
		if(paraname == NULL)
			continue;
			
		cJSON * parseparaobj = cJSON_GetObjectItem(parseparaarryobj,"paravalue");
		if(parseparaobj  != NULL)
		{
				char * status = NULL;
				objitem = cJSON_GetObjectItem(parseparaobj,"status");
				
				if(objitem != NULL ) status = objitem ->valuestring;
				
				if(status != 0 && strcmp(status,"0") == 0)
				{
					ret = 0;
				}
				else
				{
					ret = -1;
				}
		}
	}
	
JSONEND:
	if(jsonroot_2 != NULL)
	{
		cJSON_Delete(jsonroot_2);
		jsonroot_2 = NULL;
	}
	
	if(jsonout != NULL)
	{
		free(jsonout);
		jsonout = NULL;
	}
	if(parseroot != NULL)
	{
		cJSON_Delete(parseroot);
		parseroot = NULL;
	}
	DEBUG_PRINTF("%s   ret = %d\n",__FUNCTION__,ret);
	return ret;
}
static int unix_com_cb(int socket_fd,void *arg,void *pdata,int size,void *para)
{
	
	
       char *jsonout = NULL;
	cJSON * parseroot = NULL;
	cJSON *jsonroot_2 =NULL;
	cJSON *objitem = NULL;
	
	LOG_PRINTF("unix_com_cb pdata = 0x%x,size = %d,para = 0x%x\n",pdata,size,para);
	
	if(pdata  == NULL )
		return 0;

	cwmp_t *cwmp = (struct cwmp_st *)para;
	
	DEBUG_PRINTF("unix_com_cb  jsonin = %s\n",pdata);
	
	parseroot = cJSON_Parse(pdata);
	if(parseroot == NULL)
	{
		goto JSONEND;
	}	
	
	jsonroot_2 = cJSON_CreateObject();
    	if(jsonroot_2 == NULL)
    	{
		goto JSONEND;
    	}
		
	cJSON *parsecwmpobj = cJSON_GetObjectItem(parseroot,"cwmp");   
	char *cmdvalue = NULL;
	objitem = cJSON_GetObjectItem(parsecwmpobj,"cmd") ;
	
	if(objitem != NULL) cmdvalue =  objitem->valuestring;
	
	cJSON *cwmpobj_2 = NULL;
	
	//LOG_PRINTF("cmdvalue = %s\n",cmdvalue);
	
	if(cmdvalue == NULL)
		goto JSONEND;
	
	if(strcasecmp(cmdvalue,JSONCMD_GETPARAMETERVALUES) == 0)
	{
		cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
		
		//LOG_PRINTF("cjson obj = 0x%x @line %d\n",parseparalistobj,__LINE__);
		
		if(parseparalistobj == NULL)
			goto JSONEND;
		
		cwmpobj_2 = cJSON_CreateObject();
		if(cwmpobj_2 == NULL)
			goto JSONEND;
		
	      cJSON_AddItemToObject(jsonroot_2, "cwmp", cwmpobj_2);
             cJSON_AddItemToObject(cwmpobj_2, "cmd", cJSON_CreateString(JSONCMD_GETPARAMETERVALUES_RESPONSE));
			 
	 	cJSON *paralistobj_2 = cJSON_CreateArray();
		if(paralistobj_2 == NULL)
			goto JSONEND;
		
		cJSON_AddItemToObject(cwmpobj_2, "ParameterList", paralistobj_2);
		
		int listobjsize = cJSON_GetArraySize(parseparalistobj);
		//DEBUG_PRINTF("ParameterList size %d\n",listobjsize);
		int i = 0;
		for( i=0;i<listobjsize;i++)
		{
			cJSON *parseparaarryobj = cJSON_GetArrayItem(parseparalistobj,i);
			char *paraname = NULL;
			objitem = cJSON_GetObjectItem(parseparaarryobj,"paraname"); 
			if(objitem != NULL) paraname = objitem->valuestring;
			
			//DEBUG_PRINTF(">>>>>>>   = %s\n",cJSON_GetObjectItem(paralistarryobj,"paraname")->valuestring);
			//DEBUG_PRINTF("para name :%s\n",paraname);
			
			if(paraname == NULL)
				continue;
			
			cJSON * parseparaobj = cJSON_GetObjectItem(parseparaarryobj,"paravalue");
			
			cJSON *paraobj_2 = cJSON_CreateObject();			
			if(paraobj_2 == NULL)
				goto JSONEND;

			
			cJSON *paraarryobj_2 = cJSON_CreateObject();	
			if(paraarryobj_2 == NULL)
				goto JSONEND;
			
			cJSON_AddStringToObject(paraarryobj_2,"paraname", paraname);
			cJSON_AddItemToObject(paraarryobj_2,"paravalue", paraobj_2);
			cJSON_AddItemToArray(paralistobj_2,paraarryobj_2);
			
			if(parseparaobj  != NULL)
			{
				char * paratype =NULL;				
				objitem = cJSON_GetObjectItem(parseparaobj,"type");
				if(objitem != NULL) paratype = objitem->valuestring;
				
				//char  value[256] = {0};
				//getTR069ParaDB(paraname,value,sizeof(value));
				
				 char *value = cwmp_data_get_parameter_value(cwmp, cwmp->root, paraname, cwmp->pool);	

				//  DEBUG_PRINTF("%s-%s-%s\n",paraname,value,paratype);
				if(value != NULL)
					cJSON_AddStringToObject(paraobj_2,"value",value); 
				else
					cJSON_AddStringToObject(paraobj_2,"value","(null)"); 
				
			}
			
	
		}

	}
	else if(strcasecmp(cmdvalue,JSONCMD_SETPARAMETERVALUES) == 0)
	{
		cJSON *parseparalistobj = cJSON_GetObjectItem(parsecwmpobj,"ParameterList");   
		
		//DEBUG_PRINTF("cjson obj = 0x%x @line %d\n",parseparalistobj,__LINE__);
		
		if(parseparalistobj == NULL)
			goto JSONEND;
		
		cwmpobj_2 = cJSON_CreateObject();
		if(cwmpobj_2 == NULL)
			goto JSONEND;
		
	      cJSON_AddItemToObject(jsonroot_2, "cwmp", cwmpobj_2);
             cJSON_AddItemToObject(cwmpobj_2, "cmd", cJSON_CreateString(JSONCMD_SETPARAMETERVALUES_RESPONSE));
			 
	 	cJSON *paralistobj_2 = cJSON_CreateArray();
		if(paralistobj_2 == NULL)
			goto JSONEND;
		
		cJSON_AddItemToObject(cwmpobj_2, "ParameterList", paralistobj_2);
		
		int listobjsize = cJSON_GetArraySize(parseparalistobj);
		//DEBUG_PRINTF("ParameterList size %d\n",listobjsize);
		int i = 0;
		for( i=0;i<listobjsize;i++)
		{		
			cJSON *parseparaarryobj = cJSON_GetArrayItem(parseparalistobj,i);
			char *paraname = NULL;
			objitem = cJSON_GetObjectItem(parseparaarryobj,"paraname") ;
			if(objitem != NULL) paraname=objitem->valuestring;
			
			//DEBUG_PRINTF(">>>>>>>   = %s\n",cJSON_GetObjectItem(paralistarryobj,"paraname")->valuestring);
		//	DEBUG_PRINTF("para name :%s\n",paraname);
			if(paraname == NULL)
				continue;
			cJSON * parseparaobj = cJSON_GetObjectItem(parseparaarryobj,"paravalue");
			
			cJSON *paraobj_2 = cJSON_CreateObject();			
			if(paraobj_2 == NULL)
				goto JSONEND;
			
			cJSON *paraarryobj_2 = cJSON_CreateObject();	
			if(paraarryobj_2 == NULL)
				goto JSONEND;
			
			cJSON_AddStringToObject(paraarryobj_2,"paraname", paraname);
			cJSON_AddItemToObject(paraarryobj_2,"paravalue", paraobj_2);
			cJSON_AddItemToArray(paralistobj_2,paraarryobj_2);
			if(parseparaobj  != NULL)
			{	
				char *type = NULL;
				char *value = NULL;
				
				objitem = cJSON_GetObjectItem(parseparaobj,"type");
				if(objitem != NULL) type = objitem->valuestring;
				
				objitem = cJSON_GetObjectItem(parseparaobj,"value");
				if(objitem != NULL) value = objitem->valuestring;
				
				
				if(value == NULL)
					goto JSONEND;
				int ret = 0;
				
				//ret = setTR069ParaDB(paraname,value);
				ret = cwmp_data_set_parameter_value(cwmp, cwmp->root, paraname,value, strlen(value),cwmp->pool);	
				
			//      DEBUG_PRINTF("%s-%s-%s\n",paraname,value,type);
				

				if(ret != 0)
					cJSON_AddStringToObject(paraobj_2,"status","-1"); 
				else
					cJSON_AddStringToObject(paraobj_2,"status","0"); 
				
			}
			
	
		}

	}
	
 	 jsonout = cJSON_PrintUnformatted(jsonroot_2);

	unix_com_write(socket_fd,jsonout,strlen(jsonout));

	
JSONEND:
	if(parseroot != NULL)
	{
		cJSON_Delete(parseroot);
		parseroot = NULL;
	}
	if(jsonroot_2 != NULL)
	{
		cJSON_Delete(jsonroot_2);
		jsonroot_2 = NULL;
	}
	if(jsonout != NULL)
	{
		free(jsonout);
		jsonout = NULL;
	}
//	DEBUG_PRINTF("%s  Exit \n",__FUNCTION__);
	return 0;
}


unix_com_info *cwmp_com_create(cwmp_t * cwmp,const char *paramname,pool_t * pool)
{
	unix_com_info * pucinfo = NULL;
	int ret  = 0;
	char *pname =pool_pstrdup(pool,paramname);
	if(pname == NULL)
		return NULL;
	if(pname[TRstrlen(pname)-1] == '.')
	{
		pname[TRstrlen(pname)-1] = '\0';
	}
	else
	{
		pool_pfree(pool, pname);
		  return NULL;
		
	}
	ret = unix_com_create(&pucinfo,pname,UNIX_COM_MASTER,unix_com_cb,cwmp);
	if(ret != 0)
	{
		return NULL;
	}
	if(pname != NULL)
		pool_pfree(pool, pname);
	
	return pucinfo;
}
int cwmp_com_init(cwmp_t * cwmp)
{
	int i = 0;
	unix_com_init();
	
	for(i=0;i<sizeof(cwmp_com_data)/sizeof(struct _cwmp_com_st);i++)
	{
		unix_com_create(&g_cwmp_puccinfo[i],cwmp_com_data[i].name,UNIX_COM_MASTER,cwmp_com_data[i].uccb,cwmp) ;
	}
	return 0;
}

