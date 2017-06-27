#ifndef __PARAVALUE_H__
#define __PARAVALUE_H__
#include "cwmp/types.h"
#include "cwmp/cwmp.h"

extern int cpe_get_igd_ms_value(cwmp_t * cwmp, parameter_node_t * node,const char * param_name, char ** value, pool_t * pool);


extern int cpe_set_igd_ms_value(cwmp_t * cwmp,parameter_node_t * node, const char * param_name, const char * value, int length, callback_register_func_t callback_reg);

#endif
