#ifndef __CWMP_MODEL_H__
#define __CWMP_MODEL_H__

#include <cwmp/types.h>
#include <cwmp/cwmp.h>



#define DEVICE_MODEL_NODE   "itms"
#define DEVICE_MODEL_OBJECT_NODE   "object"
#define DEVICE_MODEL_PARAM_NODE   "param"
#define DEVICE_MODEL_OBJECT_ATTR_NAME	"name"
#define DEVICE_MODEL_OBJECT_ATTR_VALUE	"value"
#define DEVICE_MODEL_OBJECT_ATTR_RW	"rw"
#define DEVICE_MODEL_OBJECT_ATTR_ATTRIBUTE	"attribute"
#define DEVICE_MODEL_OBJECT_ATTR_TYPE	"type"
#define DEVICE_MODEL_OBJECT_ATTR_ININAME		"ininame"

#define DEVICE_MODEL_SET_FUNC		 "cpe_set_igd_ms_value"
#define DEVICE_MODEL_GET_FUNC		"cpe_get_igd_ms_value"

struct model_func_st
{
   const char * name;
   void * func;
};

typedef struct _parafunc_table
{
	const char *paraname;
	int (*getfunc)(cwmp_t * cwmp,unsigned char *pdata,int datasize);
	int (*setfunc)(cwmp_t * cwmp,unsigned char *pdata);
}parafunc_table_st;

typedef	int (*getfunc)(cwmp_t * cwmp,unsigned char *pdata,int datasize);
typedef	int (*setfunc)(cwmp_t * cwmp,unsigned char *pdata);

int cwmp_model_load_xml(cwmp_t * cwmp, const char * xmlfile, model_func_t * func_list, int func_count);
int cwmp_model_copy_parameter(parameter_node_t * param, parameter_node_t ** new_param, int instance_number,pool_t *pool);
int cwmp_model_refresh_object(cwmp_t * cwmp, parameter_node_t *param, int flag, callback_register_func_t callback_reg);



#endif
