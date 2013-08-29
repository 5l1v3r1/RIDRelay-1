#include "Tunnel.h"

#include <errno.h>
#include "../Diagnostic/Log.h"


static void*_iLoop(void*rawtunnel)
{
	pthread_exit(rawtunnel);
}

static void*_oLoop(void*rawtunnel)
{
	pthread_exit(rawtunnel);
}

Tunnel*SpawnTunnel(const struct sockaddr*serveraddr,const int client)
{
	pthread_t iloop=0,oloop=0;
	Tunnel c=
	{
		.client=client,
		.server=socket(AF_INET,SOCK_STREAM,0),
		.iloop=iloop,
		.oloop=oloop,
		._queue=NULL,
	},*p=malloc(sizeof*p);
	if(p==NULL){return NULL;}

	if(p->server<0){return NULL;}
	memcpy(p,&c,sizeof*p);

	if(connect(p->server,serveraddr,sizeof*serveraddr)!=0)
	{
		switch(errno)
		{
			case EADDRINUSE:
				Log(LOGT_NETWORK,LOGL_BUG, "Cannot relay more than one tunnel?");
				break;
			case ELOOP:
			case EACCES:
			case EADDRNOTAVAIL:
				Log(LOGT_NETWORK,LOGL_WARNING, "Cannot contact server.");
				break;
			case ENOBUFS:
				Log(LOGT_NETWORK,LOGL_ERROR, "Out of memory!");
				break;
			case ENAMETOOLONG:
				Log(LOGT_NETWORK,LOGL_BUG, "Misconfigured relay host >:(");
				break;
		}
		free(p);
		return NULL;
	}

	pthread_create(&p->iloop,NULL,&_iLoop,p);
	pthread_create(&p->oloop,NULL,&_oLoop,p);
	return p;
}

