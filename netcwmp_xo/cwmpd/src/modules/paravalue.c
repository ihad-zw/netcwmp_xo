
#include "paravalue.h"
#include "cwmp/model.h"
#include "cwmp/cwmp.h"
#include "hashtab.h"
#include "paratab.h"
#include "cwmp/dm_xml.h"

int cpe_get_igd_ms_value(cwmp_t * cwmp, parameter_node_t * node,const char * param_name, char ** value, pool_t * pool)
{    
	//  *value = cwmp_conf_pool_get(pool, "cwmp:acs_username");
	getfunc get_f=NULL;
	unsigned char datatemp[256]={0};
//	    cwmp_log_debug("__%s__@line %d____ param_name:%s \n",__FUNCTION__,__LINE__,param_name);

	get_f = hashtab_search(cwmp->pGetParaTab->table,param_name);

	
	if(get_f != NULL)
	{
		get_f(cwmp,datatemp,sizeof(datatemp));
		if(node->value != NULL)
			pool_pfree(cwmp->pool,node->value);
		node->value = pool_pstrdup(cwmp->pool,datatemp);
	}
	
	*value =node->value;
//	cwmp_log_debug("__%s__@line %d____ node->value:%s ,0x%x\n",__FUNCTION__,__LINE__,node->value,node->value);
    return FAULT_CODE_OK;
}


int cpe_set_igd_ms_value(cwmp_t * cwmp,parameter_node_t * node, const char * param_name, const char * value, int length, callback_register_func_t callback_reg)
{
	setfunc set_f=NULL;
    //save password to database or config file
    cwmp_log_debug("__%s__@line %d____ param_name:%s , value= %s\n",__FUNCTION__,__LINE__,param_name,value);
	set_f = hashtab_search(cwmp->pSetParaTab->table,param_name);
	if(set_f != NULL && value !=NULL)
	{
		set_f(cwmp,value);
	}
	
    if(node->value != NULL)
		pool_pfree(cwmp->pool,node->value);
	
    node->value = pool_pstrdup(cwmp->pool,value);
	
    if(node->ininame != NULL)
		cwmp_conf_set(node->ininame,value);
	
    dmxml_set_value(param_name,value);
	
    return FAULT_CODE_OK;
}
