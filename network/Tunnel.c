#include "Tunnel.h"

#include <errno.h>
#include <string.h>
#include "../Diagnostic/Log.h"
#include "Packet.h"

#include <unistd.h>

static void*_iLoop(void*rawtunnel)
{
	if(rawtunnel==NULL)
	{
		pthread_exit(NULL);
	}
	else
	{
		Tunnel*tunnel=rawtunnel;
		bool speakprotocol=true;
		while(true)
		{
			unsigned char byte=0;
			if(read(tunnel->client, &byte, 1)!=1)
			{
				Log(LOGT_TUNNEL,LOGL_INFO,"client disconnected");
				goto die;
			}
			else
			{
				if(write(tunnel->server,&byte,1)!=1)
				{
					Log(LOGT_TUNNEL,LOGL_WARNING,"Cannot send to server!");
					goto die;
				}
				if(!speakprotocol)
				{
					Log(LOGT_TUNNEL,LOGL_DUMP,"--> 0x%02x",byte);
				}
				else
				{
					opcode op=byte;
					switch(op)
					{
						case OPC_PING:
							readcsPing(tunnel->client,tunnel->server);
							break;
						case OPC_UPDATE:
							readcsUpdate(tunnel->client,tunnel->server);
							break;
						case OPC_GRAPH:
							readcsGraph(tunnel->client,tunnel->server);
							break;
						default:
							Log(LOGT_TUNNEL,LOGL_WARNING,"Unsupported packet received");
							Log(LOGT_TUNNEL,LOGL_RESULT,"\n;; Client goes unparsed from here ;;\n> 0x%02x",byte);
							speakprotocol=false;
							break;
					}
				}
			}
		}
		die:
		// cleanup by oLoop
		pthread_exit(NULL);
	}
}

static void*_oLoop(void*rawtunnel)
{
	if(rawtunnel==NULL)
	{
		pthread_exit(NULL);
	}
	else
	{
		Tunnel*tunnel=rawtunnel;
		bool speakprotocol=true;
		while(true)
		{
			unsigned char byte=0;
			if(read(tunnel->server,&byte,1)!=1)
			{
				Log(LOGT_TUNNEL,LOGL_INFO,"Server disconnected");
				goto die;
			}
			else
			{
				if(write(tunnel->client,&byte,1)!=1)
				{
					Log(LOGT_TUNNEL,LOGL_WARNING,"Cannot send to client!");
					goto die;
				}
				
				if(!speakprotocol)
				{
					Log(LOGT_TUNNEL,LOGL_DUMP,"<-- 0x%02x",byte);
				}
				else
				{
					#if RELAY_USE_DISPATCHER
					if(screader[byte]==NULL)
					{
						Log(LOGT_TUNNEL,LOGL_RESULT,"\n;; Server goes unparsed from here ;;\n< 0x%02x",byte);
						speakprotocol=false;
					}
					else
					{
						(screader[byte])(tunnel->server,tunnel->client);
					}
					#else
					opcode op=byte;
					switch(op)
					{
						case OPC_LOGIN:
							readscLogin(tunnel->server,tunnel->client);
							break;
						case OPC_PING:
							readscPing(tunnel->server,tunnel->client);
							break;
						case OPC_UPDATE:
							readscUpdate(tunnel->server,tunnel->client);
							break;
						case OPC_GRAPH:
							readscGraph(tunnel->server,tunnel->client);
							break;
						default:
							Log(LOGT_TUNNEL,LOGL_RESULT,"\n;; Server goes unparsed from here ;;\n> 0x%02x",byte);
							speakprotocol=false;
							break;
					}
					#endif
				}
			}
		}
		die:
		// TODO: close tunnel
		pthread_exit(NULL);
	}
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

	if(connect(p->server,serveraddr,(socklen_t)sizeof(struct sockaddr_in))!=0)
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
			default:
				Log(LOGT_NETWORK,LOGL_BUG, "cannot connect to server (%s)", strerror(errno));
				break;
		}
		free(p);
		return NULL;
	}

	Log(LOGT_TUNNEL,LOGL_DEBUG,"connected tunnel to server");

	pthread_create(&p->iloop,NULL,&_iLoop,p);
	pthread_create(&p->oloop,NULL,&_oLoop,p);


	return p;
}

