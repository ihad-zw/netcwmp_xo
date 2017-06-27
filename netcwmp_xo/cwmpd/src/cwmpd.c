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

#include "cwmpd.h"
#include "getopt.h"
#include "pool/pool.h"
#include "netlink.h"

#ifdef ANDROID
#include <cutils/properties.h>
#endif

#define CWMP_VALUE_UNSET -1
#define CWMP_MEM_PAGE_SIZE	(1024*1024*16)/4096  // page size = 4096


int              cwmp_argc;
char           **cwmp_argv;


static pool_t * cwmp_global_pool;

extern char  *optarg;

void cwmp_daemon()
{
    //daemon(0, 1);
}







static int cwmp_save_argv( int argc, char *const *argv)
{
    cwmp_argv = (char **) argv;
    cwmp_argc = argc;

    return 0;
}


int cwmp_set_var(cwmp_t * cwmp)
{
	char temp[256]={0};
	int netstat = 0;
    	FUNCTION_TRACE();


    cwmp_bzero(cwmp, sizeof(cwmp_t));
	netstat = netchek_status(temp,sizeof(temp));
	
	if(netstat == 1)
	  	cwmp->new_request = CWMP_TRUE;
	else
		cwmp->new_request = CWMP_FALSE;
    

    cwmp->pool = cwmp_global_pool;


    cwmp_event_init(cwmp);
	
    cwmp->queue = queue_create(cwmp->pool);
	
	
	
    pthread_mutex_init(&cwmp->value_mutex, NULL);

    return CWMP_OK;
}




#ifdef USE_CWMP_OPENSSL
void cwmp_init_ssl(cwmp_t * cwmp)
{
    char * cafile = cwmp_conf_pool_get(cwmp_global_pool, "cwmp:ca_file");
    char * capasswd = cwmp_conf_pool_get(cwmp_global_pool, "cwmp:ca_password");   
	if(cafile !=NULL && capasswd != NULL)
	    cwmp->ssl_ctx = openssl_initialize_ctx(cafile, capasswd);
}
#endif



void usage()
{
	printf("cwmpd -D|-d log_lever:CWMP_LOG_STDERR=0~CWMP_LOG_ERROR =4~CWMP_LOG_DEBUG=9\n\n");

}

int main(int argc, char **argv)
{

	cwmp_pid_t pid;
    cwmp_t * cwmp;



	
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	int opt =0;	
	int loglevel = CWMP_LOG_WARN;
	char * plogfile=NULL;


   	 pid = getpid();


	while ((opt = getopt(argc, argv, "d:Df:")) != -1) {
		
		switch (opt) {
		case 'd':
			loglevel += atoi(optarg);
			break;
		case 'D':
			loglevel = CWMP_LOG_DEBUG;
			break;
		case 'f':
			if(plogfile!=NULL)
				plogfile = strdup(optarg);
			break;
		case '?':
		default:
			usage();
			break;
		}
	}

    cwmp_log_init(plogfile, loglevel);
	


    cwmp_global_pool = pool_create(POOL_DEFAULT_SIZE);
	
	cwmp_log_debug("cwmp_global_pool=0x%x,0x%x\n",cwmp_global_pool,sizeof(pool_t));




	cwmp = pool_pcalloc(cwmp_global_pool,sizeof(cwmp_t));

    cwmp_conf_open(CWMPD_CONF_FILE);

    //cwmp_init_db();    

    cwmp_set_var(cwmp);
    cwmp_daemon();
    
    cwmp_conf_init(cwmp);

#ifdef USE_CWMP_OPENSSL
    cwmp_init_ssl(cwmp);
#endif
	
    cwmp_model_load(cwmp, CWMPD_DATA_XML_FILE);

	cwmp_com_init(cwmp);
	
    cwmp_process_start_master(cwmp);


	
    while(1)
    {
		sleep(1);
    }
    return 0;
}



