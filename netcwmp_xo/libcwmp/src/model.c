#include "cwmp/model.h"
#include "xmlet/xmlet.h"
#include "cwmp/log.h"
#include "cwmp/cfg.h"
#include "cwmp_private.h"
#include "paratab.h"

static int cwmp_model_create_parameter(cwmp_t * cwmp,parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count,  pool_t * pool);

inline void DEBUG_DUMP(unsigned char *data,int len) 
{
	int i=0;
	char temp[100]={0};
	char datatemp[10]={0};
	
	for(i=0;i<len;i++)
	{
		//LOG_PRINTF("",data[i]); 
		sprintf(datatemp,"0x%02x ",data[i]);
		strcat(temp,datatemp);
		if((i+1)%16==0 || (i+1)>=len) 
		{
			cwmp_log_debug("%s\n",temp); 
			memset(temp,0,sizeof(temp));
		}
	} 
	cwmp_log_debug(" .END.\n");
}
int cwmp_model_copy_parameter_object(parameter_node_t * object_param, parameter_node_t * new_node, int instance_number,pool_t *pool)
{

    parameter_node_t * tmp_node, *curr_node, *next_newnode;
    if(!object_param || !new_node)
    {
        return CWMP_ERROR;
    }
    if(instance_number > 0)
    {
        memcpy(new_node, object_param, sizeof(parameter_node_t));
        new_node->name = pool_pstrdup(pool,TRitoa(instance_number));
	  new_node->ininame = NULL;
	  new_node->value = NULL;
        new_node->child = NULL;
        new_node->next_sibling = NULL;
        for(tmp_node=object_param; tmp_node; tmp_node = tmp_node->next_sibling)
        {
            if(!tmp_node->next_sibling)
            {
                tmp_node->next_sibling = new_node;
                break;
            }

        }
    }


    for(tmp_node=object_param->child, curr_node=NULL; tmp_node; tmp_node=tmp_node->next_sibling)
    {
        next_newnode = pool_pcalloc(pool,sizeof(parameter_node_t));

        next_newnode->name = pool_pstrdup(pool,tmp_node->name);
	  next_newnode->ininame = pool_pstrdup(pool,tmp_node->ininame);
	  next_newnode->value = pool_pstrdup(pool,tmp_node->value);
	  
        next_newnode->parent = new_node;
        next_newnode->child = NULL;
        next_newnode->next_sibling = NULL;

        if(!curr_node)
        {
            new_node->child = next_newnode;
        }
        else
        {
            curr_node->next_sibling = next_newnode;
        }

        curr_node = next_newnode;

        cwmp_model_copy_parameter_object(tmp_node, next_newnode, 0,pool);


    }

    return CWMP_OK;
}


int cwmp_model_copy_parameter(parameter_node_t * param, parameter_node_t ** new_param, int instance_number,pool_t *pool)
{
    int rv;
    if(!param)
        return CWMP_OK;

    parameter_node_t * object_param = param->child; //object param like InternetGatewayDevice.WANDevice.{i}

    parameter_node_t * new_node = pool_pcalloc(pool,sizeof(parameter_node_t));

    if (!new_node)
    {
        cwmp_log_error("cwmp model copy parameter malloc new node failed ...\n");
        return CWMP_ERROR;
    }

    new_node->name = NULL;
    new_node->rw = 0;
    new_node->type = 0;
    new_node->value = NULL;
    new_node->parent = param;
    new_node->ininame = NULL;
	
    rv = cwmp_model_copy_parameter_object(object_param, new_node, instance_number,pool);
    *new_param = new_node;
    if(rv != CWMP_OK)
    {
        cwmp_log_error("cwmp_model: copy parameter failed");
        cwmp_model_free_parameter(new_node);
        *new_param = NULL;
    }
    return rv;
}

int cwmp_model_free_parameter(parameter_node_t * param,pool_t *pool)
{
    parameter_node_t     *tmp_param = NULL;
    parameter_node_t     *next_param = NULL;

    if(!param)
    {
        return CWMP_OK;
    }

    for(tmp_param=param->child; tmp_param; tmp_param=next_param)
    {
        next_param = tmp_param->next_sibling;
        cwmp_model_delete_parameter(tmp_param,pool);
    }
    pool_pfree(pool,param->name);
    pool_pfree(pool,param->ininame);
    pool_pfree(pool,param->value);
    pool_pfree(pool,param);
	if(param->pucinfo)
	{
		unix_com_detach(param->pucinfo);
		param->pucinfo = NULL;
	}
	

    return CWMP_OK;
}

int cwmp_model_delete_parameter(parameter_node_t * param,pool_t *pool)
{
    parameter_node_t     *tmp_param = NULL;
    parameter_node_t     *next_param = NULL;
    parameter_node_t     *parent = NULL;
    if(!param)
    {
        return CWMP_OK;
    }
    parent = param->parent;
    for(tmp_param=parent->child; tmp_param; tmp_param=tmp_param->next_sibling)
    {
        if(tmp_param->next_sibling == param)
        {
            tmp_param->next_sibling = param->next_sibling;
            break;
        }
    }

    return cwmp_model_free_parameter(param,pool);
}

void* cwmp_model_find_func(model_func_t * func_list, int func_count, const char * func_name)
{
    if(!func_name || func_count <= 0)
        return NULL;

    model_func_t * f = func_list;
    int i;
    for(i=0; i<func_count; i++, f++)
    {
        if(TRstrcasecmp(func_name, f->name) == 0)
        {
            return f->func;
        }
    }
    return NULL;


}




int cwmp_model_init_parameter(parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count, pool_t * pool)
{
    //	FUNCTION_TRACE();

    param->get = NULL;
    param->set = NULL;
    param->notify = NULL;
    param->add = NULL;
    param->del = NULL;
    param->refresh = NULL;

    if(!node)
    {
        return CWMP_OK;
    }    

    char *nodename = node->nodeName;
    char *name, *value;    
	
    name = cwmp_xml_get_node_attribute(node, DEVICE_MODEL_OBJECT_ATTR_NAME);
    if(!name)
    {
	  param->type = TYPE_OBJECT;
        return CWMP_ERROR;
    }
	
    if(name[TRstrlen(name)-1] == '.')
    {
        param->type = TYPE_OBJECT;
		
    }
    else
    {
        char * type = cwmp_xml_get_node_attribute(node, DEVICE_MODEL_OBJECT_ATTR_TYPE);
        if(!type)
        {
            param->type = TYPE_STRING;
        }
        else
        {
            param->type = cwmp_get_type_value(type);
        } 
    }


    param->name = pool_pstrdup(pool,name);


    value = cwmp_xml_get_node_attribute(node, DEVICE_MODEL_OBJECT_ATTR_RW);
    if(value)
    {
        param->rw = TRatoi(value);
    }

    //value = cwmp_xml_get_node_attribute(node, "value");
    //if(value)
    //{
    //    param->value = pool_pstrdup(pool, value);
    //}

    if(param->type == TYPE_OBJECT)
    {
        value = cwmp_xml_get_node_attribute(node, "add_func");
        if(value)
        {
            param->add = (parameter_add_handler_pt)cwmp_model_find_func(func_list, func_count, value);


        }

        value = cwmp_xml_get_node_attribute(node, "del_func");
        if(value)
        {
            param->del = (parameter_del_handler_pt)cwmp_model_find_func(func_list, func_count, value);
        }

        value = cwmp_xml_get_node_attribute(node, "refresh_func");
        if(value)
        {
            param->refresh = (parameter_refresh_handler_pt)cwmp_model_find_func(func_list, func_count, value);
        }
	
    }
    else
    {
        value = cwmp_xml_get_node_attribute(node, "get_func");
        if(value)
        {
            param->get = (parameter_get_handler_pt) cwmp_model_find_func(func_list, func_count, value);
        }
	  else
	  {
	  	 param->get = (parameter_get_handler_pt) cwmp_model_find_func(func_list, func_count, DEVICE_MODEL_GET_FUNC);
	  }

        value = cwmp_xml_get_node_attribute(node, "set_func");
        if(value)
        {
            param->set = (parameter_set_handler_pt)cwmp_model_find_func(func_list, func_count, value);
        }
	  else
	  {
	  	 param->set = (parameter_set_handler_pt)cwmp_model_find_func(func_list, func_count, DEVICE_MODEL_SET_FUNC);
	  }

        value = cwmp_xml_get_node_attribute(node, "notify_func");
        if(value)
        {
            param->notify = (parameter_notify_handler_pt)cwmp_model_find_func(func_list, func_count, value);
        }
		
	  value = cwmp_xml_get_node_attribute(node, "setp");
        if(value)
        {
            param->setp = (parameter_setp_handler_pt)cwmp_model_find_func(func_list, func_count, value);
        }

	 value = cwmp_xml_get_node_attribute(node, "ininame");
	  if(value)
	  {
	  	param->ininame = pool_pstrdup(pool,value);		
		param->value = cwmp_conf_pool_get(pool,param->ininame);		
	  }
	  else
	  {
	  	value = cwmp_xml_get_node_attribute(node, "value");
		if(value)
		{
			param->value = pool_pstrdup(pool,value);	
		}
	  }
	  
    }

    return CWMP_OK;

}

int cwmp_model_create_child_parameter(cwmp_t * cwmp,parameter_node_t * child_param, xmlnode_t * child_node, model_func_t * func_list, int func_count, pool_t * pool)
{
    //	FUNCTION_TRACE();
    cwmp_model_create_parameter(cwmp,child_param, child_node, func_list, func_count, pool);


    xmlnode_t * next_node =  XmlNodeGetNextSibling(child_node);
    parameter_node_t * last_sibling = child_param;
    while(next_node)
    {
        parameter_node_t * next_param = (parameter_node_t *)pool_pcalloc(pool, sizeof(parameter_node_t));
        cwmp_model_create_parameter(cwmp,next_param, next_node, func_list, func_count, pool);

        next_node = XmlNodeGetNextSibling(next_node);

        next_param->parent = child_param;
        last_sibling->next_sibling = next_param;
        last_sibling = next_param;
    }

    return CWMP_OK;
}


static int cwmp_model_create_parameter(cwmp_t * cwmp,parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count,  pool_t * pool)
{
    //	FUNCTION_TRACE(); 
    cwmp_model_init_parameter(param, node, func_list, func_count, pool);


    if(param->name != NULL && TRstrlen(param->name) > 0)
    {     
		//cwmp_log_debug("name %s,  addr : 0x%x,value :%s ", param->name,param,param->value);
		hashtab_insert(cwmp->pParaNodeTab->table,param->name,param);
		if(param->type == TYPE_OBJECT )
		{
			param->pucinfo= cwmp_com_create(cwmp,param->name,pool);
		}
    }

    xmlnode_t * child_node = XmlNodeGetFirstChild(node);
    if(!child_node)
    {
        return CWMP_OK;
    }

    parameter_node_t * child_param = (parameter_node_t *)pool_pcalloc(pool, sizeof(parameter_node_t));
    cwmp_model_create_child_parameter(cwmp,child_param, child_node, func_list, func_count,  pool);
    param->child = child_param;
    child_param->parent = param;

    return CWMP_OK;

}


static int cwmp_model_init_object(cwmp_t * cwmp, parameter_node_t *param)
{ 
    parameter_node_t     *node = NULL;

    if(!param)
    {        
        return CWMP_ERROR;
    }

    if(param->type == TYPE_OBJECT && param->refresh)
    {
        //µ÷ÓÃrefreshº¯Êý
        if(param->refresh)
        {
            param->refresh(cwmp, param, callback_register_task);
        }
    }

    for (node = param->child; node; node = node->next_sibling)
    {
        cwmp_model_init_object(cwmp, node);
    }

    return CWMP_OK;
}


int cwmp_model_refresh_object(cwmp_t * cwmp, parameter_node_t *param, int flag, callback_register_func_t callback_reg)
{
    parameter_node_t     *node = NULL;

    if(!param)
    {        
        return CWMP_ERROR;
    }

    if(param->refresh && flag == 1)
    {
        if(param->refresh)
        {
            param->refresh(cwmp, param, callback_reg);
        }
    }

    for (node = param->child; node; node = node->next_sibling)
    {
        if(TRstrcmp(param->name, "{i}") != 0)
        {
            cwmp_model_refresh_object(cwmp, node, 1, callback_reg);
        }
    }

    return CWMP_OK;
}




int cwmp_model_load_parameter(cwmp_t * cwmp, xmldoc_t * doc, model_func_t * func_list, int func_count)
{
    pool_t * pool = cwmp->pool;


    xmlnode_t *  root_node;
    xmlnode_t *  model_node;
    ASSERT(doc != NULL);
    FUNCTION_TRACE();
    root_node = XmlNodeGetFirstChild(& doc->node);
    if (! root_node)
    {
        cwmp_log_error("xml document root is null!");
        return CWMP_ERROR;
    }

    cwmp_log_debug("model load: xml node name is %s\n", root_node->nodeName); 
    model_node = root_node;
    if (model_node == NULL)
    {
        cwmp_log_error("device model node not found!");
        return CWMP_ERROR;
    }

    parameter_node_t * root_param = pool_pcalloc(pool, sizeof(parameter_node_t));
    cwmp->pParaNodeTab = MALLOC(sizeof(struct paratab));
	paratab_init(cwmp->pParaNodeTab,PARATAB_DEFAULT_SIZE);
	
    cwmp_model_create_parameter(cwmp,root_param, model_node, func_list, func_count, pool);

	

    cwmp->root = root_param->child;

    cwmp_model_init_object(cwmp, cwmp->root);
	return CWMP_OK;

}

int cwmp_model_load_xml(cwmp_t * cwmp, const char * xmlfile, model_func_t * func_list, int func_count)
{

    xmldoc_t *  doc;
    size_t xmllen, nread ;

    FILE * fp = fopen(xmlfile, "rb");
    if(!fp)
    {
        cwmp_log_error("xmlfile is NULL\n");
        return CWMP_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    xmllen = ftell(fp);


    char * buf = (char*)MALLOC(sizeof(char)*(xmllen+1));
    if(!buf)
    {
        cwmp_log_error("model load: malloc fail\n");
        goto finish; 
    }
    fseek(fp, 0, SEEK_SET);
    nread = fread(buf, 1, xmllen, fp);
    buf[nread] = 0;
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    doc = XmlParseBuffer(pool, buf);
    if (!doc)
    {
        cwmp_log_error("model load: create doc faild\n");
        goto finish;
    }

    cwmp_model_load_parameter(cwmp, doc, func_list, func_count);
    //cwmp_model_load_inform_info(cwmp, doc);
    //cwmp_model_load_device_info(cwmp, doc);

finish:
    FREE(buf);
    fclose(fp);
    pool_destroy(pool);
	return CWMP_OK;
}





