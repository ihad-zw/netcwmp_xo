#include "cwmp/model.h"
#include "data_model.h"
#include "cwmp_module.h"
#include "paratab.h"
#include "paraapi.h"
#include "cwmp/dm_xml.h"

model_func_t ModelFunction[] =
{
	{"run_pingfunc",moduel_ping_func},
	{"cpe_get_igd_ms_value",cpe_get_igd_ms_value},
	{"cpe_set_igd_ms_value",cpe_set_igd_ms_value},
};
struct _parafunc_table ParaFunction[]=
{
	{"Device.LAN.IPAddress",dm_getIPAddress,dm_setIPAddress},
	{"Device.LAN.SubnetMask",dm_getNetMask,dm_setNetMask},
	{"Device.LAN.DefaultGateway",dm_getDefaultGateway,NULL},
	{"Device.LAN.DNSServers",dm_getDNSServers1,NULL},
	{"Device.LAN.MACAddress",dm_getMACAddress,NULL},
	{"Device.ManagementServer.ConnectionRequestURL",dm_getConnectionRequestURL,NULL},
		

	
};
int get_index_after_paramname(parameter_node_t * param, const char * tag_name)
{
    parameter_node_t * parent;
    parameter_node_t * tmp;
    for(parent=param->parent, tmp = param; parent; tmp = parent, parent = parent->parent)
    {
        if(TRstrcmp(parent->name, tag_name) == 0)
        {
             if(is_digit(tmp->name) == 0)
             {
                return TRatoi(tmp->name);   
             }
        }        
    }
    return -1;
}


void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile)
{  
	int i=0;
	cwmp->pGetParaTab= MALLOC(sizeof(struct paratab));
	if(cwmp->pGetParaTab == NULL)
	{
		return -1;
	}
	paratab_init(cwmp->pGetParaTab,PARATAB_DEFAULT_SIZE);

	cwmp->pSetParaTab= MALLOC(sizeof(struct paratab));
	if(cwmp->pSetParaTab == NULL)
	{
		return -1;
	}
	paratab_init(cwmp->pSetParaTab,PARATAB_DEFAULT_SIZE);

	for(i=0;i<sizeof(ParaFunction)/sizeof(struct _parafunc_table);i++)
	{
		if(ParaFunction[i].getfunc != NULL)
			hashtab_insert(cwmp->pGetParaTab->table,ParaFunction[i].paraname,ParaFunction[i].getfunc);

		if(ParaFunction[i].setfunc != NULL)
			hashtab_insert(cwmp->pSetParaTab->table,ParaFunction[i].paraname,ParaFunction[i].setfunc);
	}
	
	dmxml_create(xmlfile);
	
    	cwmp_model_load_xml(cwmp, xmlfile, ModelFunction, sizeof(ModelFunction)/sizeof(model_func_t));
}


