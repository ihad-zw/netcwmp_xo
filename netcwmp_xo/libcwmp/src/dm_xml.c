
#include <cwmp/types.h>
#include <cwmp/util.h>
#include <cwmp/memory.h>
#include <cwmp/buffer.h>
#include <cwmp/event.h>
#include <cwmp/queue.h>

#include <cwmp/log.h>

#include "cwmp/dm_xml.h"

#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/parser.h>


#define LOG_PRINTF cwmp_log_info
#define DEBUG_PRINTF cwmp_log_info

static xmlXPathObjectPtr getnodeset(xmlDocPtr doc, char *xpath) 
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	

	context = xmlXPathNewContext(doc);

	if (context == NULL) {
		LOG_PRINTF("context is NULL\n");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context); 

	if (result == NULL) {
		LOG_PRINTF("xmlXPathexpression_r return NULL\n");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
		xmlXPathFreeObject(result);
		LOG_PRINTF("nodeset is empty addr=%s \n",xpath);
		
		return NULL;
	}

	return result;
}


int dmxml_create(char * dataxmlfile)
{
	int cmdtemp[100]={0};
	int i = 0;	
	char xpathtemp[300]={0};
	char temp[128]={0};
	int failcnt=3;	
	char filepath[256]={0};
	unsigned char datatemp[256]={0};		
	LOG_PRINTF("%s.......enter\n",__FUNCTION__);	
	if(access(CWMPD_DMXML_FILE,F_OK)==0)	
	{		
		LOG_PRINTF("%s   File already exists \n",CWMPD_DMXML_FILE);		
	//	return -1;	
	}	
	sprintf(cmdtemp,"cp -f %s %s",dataxmlfile,CWMPD_DMXML_FILE);	
	//	LOG_PRINTF("cmdtemp == %s \n",cmdtemp);	
	system(cmdtemp);	
	sprintf(cmdtemp,"chmod 766 %s",CWMPD_DMXML_FILE);
	system(cmdtemp);		
	sprintf(filepath,"%s",CWMPD_DMXML_FILE);
	
	xmlDocPtr doc;
	while((doc=xmlReadFile(filepath, NULL, XML_PARSE_NOBLANKS | XML_PARSE_NODICT)) == NULL)
	{
		LOG_PRINTF("DOC == NULL  ERROR fail number : %d\n",failcnt);
		if((failcnt--)<=0)
			return -1;
		sleep(1);
	}
	
	
	sprintf(xpathtemp,"/itms/object");


	xmlXPathObjectPtr result = getnodeset(doc,xpathtemp);	
	
	if (result) 
	{                                                                      
		xmlNodeSetPtr nodeset = result->nodesetval;  
		xmlNodePtr propNodePtr = NULL;  
		
		//LogInfo("nodeset->nodeNr=%d\n",nodeset->nodeNr);                                                  
		for (i=0; i < nodeset->nodeNr; i++) 
		{    
			 
			 xmlNodePtr curNode=  nodeset->nodeTab[i];             
			 
			 propNodePtr = NULL;
			if (xmlHasProp(curNode,BAD_CAST "value") && xmlHasProp(curNode,BAD_CAST "ininame"))
	       	{
	             		propNodePtr = curNode;
	       	}

	      		if(propNodePtr != NULL)
			{
			       xmlChar* szAttrIniname=NULL;
				xmlChar* szAttrValue=NULL;   	
			       xmlAttrPtr attrPtr = propNodePtr->properties;
				 		
			    while (attrPtr != NULL)
			    {
			    	if(szAttrIniname !=NULL && 	szAttrValue !=NULL)
			    		break;
			    		
			       if (!xmlStrcmp(attrPtr->name, BAD_CAST "value"))
			       {
			       	  if(szAttrValue ==NULL)
			          	       szAttrValue = xmlGetProp(propNodePtr,BAD_CAST "value");
			   		 
			       }
			       if (!xmlStrcmp(attrPtr->name, BAD_CAST "ininame"))
			       {
			       	  if(szAttrIniname ==NULL)
			          	        szAttrIniname = xmlGetProp(propNodePtr,BAD_CAST "ininame");
			   		 
			       }
			       attrPtr = attrPtr->next;
			    }  
			    memset(datatemp,0,sizeof(datatemp));
			    if(szAttrIniname && xmlStrlen(szAttrIniname)>0)
			    {
			    	   CH_GetSysData(szAttrIniname,datatemp,sizeof(datatemp));			    	
				    if(szAttrValue)
				   	 	xmlSetProp(propNodePtr,BAD_CAST "value",BAD_CAST datatemp);
			    }
			    if(szAttrIniname)
			    	xmlFree(szAttrIniname);
			    if(szAttrValue)
			    	xmlFree(szAttrValue);
			           
			}
		}
	
		xmlSaveFormatFile(dataxmlfile,doc,1);
		xmlXPathFreeObject (result);   
	}       
	
	if(doc)
		xmlFreeDoc(doc);
	
	
	return 0;
}

int dmxml_set_value(char *paraname,char *paravalue)
{
	xmlDocPtr doc;
	int i = 0;
	int len=0;
	char xpathtemp[300]={0};
	char temp[128]={0};

	xmlChar* szAttr=NULL;
	

	char filepath[256]={0};
	if(paraname == NULL || paravalue == NULL){LOG_PRINTF("Itms_setsysxml xpath = 0x%x content 0x%x \n",paraname,paravalue); return -1;}
			
	if(strlen(paraname)>256){LOG_PRINTF("xpath too len max length = 256\n"); return -1;}
		
	sprintf(filepath,"%s",CWMPD_DMXML_FILE);

	doc=xmlReadFile(filepath, NULL,XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
	if(doc == NULL){LOG_PRINTF("Set doc == NULL  Error \n"); return -1;}
	
	sprintf(xpathtemp,"/itms/object[@name='%s']",paraname);
	//LOG_PRINTF("set--xpathtemp = %s\n",xpathtemp);

	xmlXPathObjectPtr result = getnodeset(doc,xpathtemp);	
	
	if(result == NULL)
	{
		sprintf(xpathtemp,"/itms/object/parameter/[@name='%s']",paraname);
//		LOG_PRINTF("set--xpathtemp = %s\n",xpathtemp);
		result = getnodeset(doc,xpathtemp);	
	}
	
	if (result) 
	{                                                                      
		xmlNodeSetPtr nodeset = result->nodesetval;  
		xmlNodePtr propNodePtr = NULL;  
		
		
		//LOG_PRINTF("nodeset->nodeNr=%d\n",nodeset->nodeNr)
;                                                  
		for (i=0; i < nodeset->nodeNr; i++) 
		{    
			 LOG_PRINTF("keyword: %s\n", nodeset->nodeTab[i]->name);   
			 xmlNodePtr curNode=  nodeset->nodeTab[i];             
			             
			if (xmlHasProp(curNode,BAD_CAST "value"))
	       	{
	          		 propNodePtr = curNode;
	       	}
	       	break;
      
		}
		if(propNodePtr != NULL)
		{
		     xmlAttrPtr attrPtr = propNodePtr->properties;
			
		    while (attrPtr != NULL)
		    {
		       if (!xmlStrcmp(attrPtr->name, BAD_CAST "value"))
		       {
		      //    szAttr = xmlGetProp(propNodePtr,BAD_CAST "ininame");
		      		//DEBUG_PRINTF("paravalue : %s\n",paravalue);
			   	xmlSetProp(propNodePtr,BAD_CAST "value",BAD_CAST paravalue);
			       break;
		       }
		       attrPtr = attrPtr->next;
		   }
		}
		xmlSaveFormatFile(filepath,doc,1);
		xmlXPathFreeObject (result);   
	}       
	
	if(doc)
		xmlFreeDoc(doc);
	sync();
	return 0;
}
