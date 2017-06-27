
#include "cwmp/session.h"
#include "cwmp/cfg.h"
#include "cwmp/log.h"
#include "cwmp/cwmp.h"
#include "cwmp_private.h"
#include "cwmp/util.h"
#include "paratab.h"
#include "cwmp/cwmp_data.h"

#define LOG_PRINTF cwmp_log_info

int cwmp_data_set_parameter_value_ex(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool);
int  cwmp_data_get_parameter_value_ex(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool);
char * cwmp_data_get_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool)
{
    parameter_node_t * node;
    char * value = NULL;
    int rc;
	cwmp_data_get_parameter_value_ex(cwmp,root,name,pool);
	
    node = hashtab_search(cwmp->pParaNodeTab->table,name);

	if(node == NULL)
	    	node = cwmp_get_parameter_node(root, name);
	
    if (!node)
        return NULL;

	pthread_mutex_lock(&cwmp->value_mutex);
	
     rc = cwmp_get_parameter_node_value(cwmp, node, name, &value, pool);
	 
	pthread_mutex_unlock(&cwmp->value_mutex);

     if(rc == 0)
     {
		return value;
     }
     else
     {
		return node->value;
     }

}

int cwmp_data_set_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool)
{
    parameter_node_t * node;
    node = hashtab_search(cwmp->pParaNodeTab->table,name);
    int rc = 0;
    if(node == NULL)
		node = cwmp_get_parameter_node(root, name);
	
    if (!node)
        return CWMP_ERROR;
	
	pthread_mutex_lock(&cwmp->value_mutex);
     rc = cwmp_set_parameter_node_value(cwmp, node, name, value, value_length);
	pthread_mutex_unlock(&cwmp->value_mutex);

	cwmp_data_set_parameter_value_ex(cwmp,root,name,value,value_length,pool);
	return rc;
}

int  cwmp_data_get_parameter_value_ex(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool)
{
	parameter_node_t * node;
	int ri = 0;
	char *nametemp= NULL;
	char valuetemp[512]={0};
	int ret = 0;
	
	if(name == NULL) return -1;
	if(pool == NULL) return -1;

	nametemp = pool_pstrdup(pool, name);
	if(nametemp == NULL) return -1;

	ri = strlen(nametemp);
	for(;ri>0; ri--)
	{
		if(nametemp[ri-1] !=  '.')
		{
			continue;
		}
		nametemp[ri]='\0';
		LOG_PRINTF("get---obj : %s\n",nametemp);
		  
	    node = hashtab_search(cwmp->pParaNodeTab->table,nametemp);
	    if(node == NULL)
			node = cwmp_get_parameter_node(root, nametemp);

		if(node == NULL) continue;
		memset(valuetemp,0,sizeof(valuetemp));
		ret = cwmp_com_GetParameterValues(node->pucinfo,name,cwmp_get_type_string(node->type),valuetemp,sizeof(valuetemp)-1);

		if( ret == 0 && strlen(valuetemp) != 0)
		{
			    parameter_node_t * nodeset;
			    nodeset = hashtab_search(cwmp->pParaNodeTab->table,name);
			    int rc = 0;
			    if(nodeset == NULL)
					nodeset = cwmp_get_parameter_node(root, name);
				
		          if(nodeset != NULL)
		          {
					pthread_mutex_lock(&cwmp->value_mutex);
				        rc = cwmp_set_parameter_node_value(cwmp, nodeset, name, valuetemp, strlen(valuetemp));
					pthread_mutex_unlock(&cwmp->value_mutex);
		          }
		}
		
		break;
	}
	pool_pfree(pool, nametemp);

	
	return 0;
}

int cwmp_data_set_parameter_value_ex(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool)
{
	parameter_node_t * node;
	int ri = 0;
	char *nametemp= NULL;
	if(name == NULL) return -1;
	if(pool == NULL) return -1;

	nametemp = pool_pstrdup(pool, name);
	if(nametemp == NULL) return -1;

	ri = strlen(nametemp);
	for(;ri>0; ri--)
	{
		if(nametemp[ri-1] !=  '.')
		{
			continue;
		}
		nametemp[ri]='\0';
		LOG_PRINTF("set---obj : %s\n",nametemp);
		  
	    node = hashtab_search(cwmp->pParaNodeTab->table,nametemp);
	    if(node == NULL)
			node = cwmp_get_parameter_node(root, nametemp);

		if(node == NULL) continue;
		cwmp_com_SetParameterValues(node->pucinfo,name,cwmp_get_type_string(node->type),value);
		break;
	}
	pool_pfree(pool, nametemp);
	return 0;
}


