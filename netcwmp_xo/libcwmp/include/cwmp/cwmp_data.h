#ifndef __CWMP_DATA_H__
#define __CWMP_DATA_H__
#include <cwmp/cwmp.h>

extern char *  cwmp_data_get_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool);

extern int cwmp_data_set_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool);

#endif // __CWMP_DATA_H__
