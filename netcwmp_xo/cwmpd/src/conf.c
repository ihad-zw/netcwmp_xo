/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include "cwmp_module.h"
#include <cwmp/cfg.h>



void cwmp_conf_init(cwmp_t * cwmp)
{
    pool_t * pool;
    FUNCTION_TRACE();

    pool = cwmp->pool;
    cwmp->httpd_port =  7750;

    cwmp->acs_auth  =   1;
    cwmp->cpe_auth  =   1;

    if(cwmp->acs_auth)
    {
        cwmp->acs_user = cwmp_conf_pool_get(pool, "MSERVER:Username");
        cwmp->acs_pwd = cwmp_conf_pool_get(pool, "MSERVER:Password");
    }

	if(cwmp->cpe_auth)
	{
	    cwmp->cpe_user = cwmp_conf_pool_get(pool, "MSERVER:ConnectionRequestUsername");
    		cwmp->cpe_pwd = cwmp_conf_pool_get(pool, "MSERVER:ConnectionRequestPassword");

	}

    cwmp->acs_url   =   cwmp_conf_pool_get(pool, "MSERVER:URL"); //  "http://192.168.0.69:8000/otnms/acs/webservice.action";//cwmp_nvram_getdup(pool, "cwmp:acs_url");

    cwmp->cpe_mf    =   cwmp_conf_pool_get(pool, "SYSTEM:Manufacturer"); //     "ZTE"; //cwmp_nvram_getdup(pool, "cwmp:cpe_manufacture");

    cwmp->cpe_oui   =   cwmp_conf_pool_get(pool, "SYSTEM:ManufacturerOUI"); //   "00D0D0";cwmp_nvram_getdup(pool, "cwmp:cpe_oui");

    cwmp->cpe_sn    =   cwmp_conf_pool_get(pool, "SYSTEM:SerialNumber"); //    "0410400AA11AA2255"; //cwmp_nvram_getdup(pool, "cwmp:cpe_sn");

    cwmp->cpe_name  =   cwmp_conf_pool_get(pool, "SYSTEM:ManufacturerOUI"); //  "00D0D0"; //cwmp_nvram_getdup(pool, "cwmp:cpe_name");

    cwmp->cpe_pc    =   cwmp_conf_pool_get(pool, "SYSTEM:ProductClass"); //      "ZXECS EBG2100";

    cwmp_log_debug("url:%s\nmf:%s\noui:%s\nsn:%s\nname:%s\npc:%s\nhttpd port:%d\n",    cwmp->acs_url, cwmp->cpe_mf, cwmp->cpe_oui, cwmp->cpe_sn, cwmp->cpe_name, cwmp->cpe_pc,
                   cwmp->httpd_port);

#ifdef ANDROID
    cwmp->event_filename =pool_pstrdup(pool,"/data/setting/cwmpevent.bin");
#else
    cwmp->event_filename =pool_pstrdup(pool,"./cwmpevent.bin");
#endif

    cwmp->http_timeout = 60;	//s 

}

