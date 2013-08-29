#include "accept.h"

#include <errno.h>

#include "../Diagnostic/Log.h"

struct link
{
	const int relay_sockfd;
	struct sockaddr const*const server_addr;
};

static void*socklisten(void*connection)
{
	if(connection==NULL)
	{
		Log(LOGT_NETWORK,LOGL_BUG,"connection was null");
		pthread_exit(NULL);
	}
	else
	{
		const struct link*tlink=(struct link*)connection;
		while(1)
		{
			struct sockaddr client_address;
			socklen_t endpoint_len = (socklen_t)sizeof(client_address);
			int client_sockfd;
			Tunnel*t;

			if((client_sockfd = accept(tlink->relay_sockfd,&client_address,&endpoint_len)) < 1 )
			{
				switch(errno)
				{
					case ENFILE:
					case EMFILE:
					case ECONNABORTED:
						continue;
					case EINTR:
						break;
					case ENOBUFS:
					case ENOMEM:
						Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
					default:
						Log(LOGT_NETWORK,LOGL_BUG,"Cannot accept connections");
				}
				goto die;
			}

			Log(LOGT_NETWORK,LOGL_INFO,"Client connected, opening tunnel");

			t=SpawnTunnel(tlink->server_addr,client_sockfd);
			if(t==NULL)
			{
				Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
				goto die;
			}
		}
		die:
		free(connection);
		pthread_exit(NULL);
	}
}

int AcceptClients(void)
{
	pthread_t netthread;
	int relay_sockfd;
	dictionary*config=iniparser_load(networkinipath());
	if(!config||!iniparser_find_entry(config,"network")||!iniparser_find_entry(config,"server"))
	{
		Log(LOGT_NETWORK,LOGL_NOTICE,"Configuration missing from %s, using defaults where needed.",networkinipath());
	}
	// continue
	{
		const struct sockaddr_in relay_address=
		{
			.sin_family=AF_INET,
			.sin_port = htons((uint16_t)iniparser_getint(config,"network:port",61014)),
			.sin_addr.s_addr = inet_addr(iniparser_getstring(config,"network:address","0.0.0.0")),
			.sin_zero = {0},
		}, server_address=
		{
			.sin_family=AF_INET,
			.sin_port = htons((uint16_t)iniparser_getint(config,"server:port",61014)),
			.sin_addr.s_addr = inet_addr(iniparser_getstring(config,"server:address","127.1")),
			.sin_zero={0},
		};

		// specify and grab a socket we want
		relay_sockfd = socket(AF_INET,SOCK_STREAM,0);

		// did we get one?
		if(relay_sockfd<0)
		{
			switch(errno)
			{
				case EACCES:
					Log(LOGT_NETWORK,LOGL_ERROR,"System prohibits networking");
					break;
				case ENOBUFS:
				case ENOMEM:
					Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
					break;
				default:
					Log(LOGT_NETWORK,LOGL_BUG,"Could not grab socket");
					break;
			}
			goto die;
		}
		
		if(bind(relay_sockfd,(struct sockaddr*)&relay_address,(socklen_t)sizeof(server_address)))
		{
			switch(errno)
			{
				case EADDRINUSE:
					Log(LOGT_NETWORK,LOGL_ERROR,"The relay address %s is already being used.",iniparser_getstring(config,"network:address","0.0.0.0"));
					break;
				case EADDRNOTAVAIL:
					Log(LOGT_NETWORK,LOGL_ERROR,"%s is not available on this machine",iniparser_getstring(config,"network:address","0.0.0.0"));
					break;
				case EACCES:
					Log(LOGT_NETWORK,LOGL_ERROR,"You don't have permission to access %s",iniparser_getstring(config,"network:address","0.0.0.0"));
					break;
				case ELOOP:
				case ENAMETOOLONG:
					Log(LOGT_NETWORK,LOGL_ERROR,"System misconfigured");
					break;
				case ENOBUFS:
					Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
					break;
				default:
					Log(LOGT_NETWORK,LOGL_BUG,"Could not bind network socket!");
					break;
			}
			goto die;
		}

		if(listen(relay_sockfd, iniparser_getint(config,"network:clients",10)))
		{
			switch(errno)
			{
				case EACCES:
					Log(LOGT_NETWORK,LOGL_ERROR,"System does not allow network listeners");
					break;
				case ENOBUFS:
					Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
					break;
				default:
					Log(LOGT_NETWORK,LOGL_BUG,"Could not start listening on socket");
			}
			goto die;
		}

		{
			struct link tlink=
			{
				.relay_sockfd=relay_sockfd,
				.server_addr=(struct sockaddr*)&server_address,
			},*ptlink=malloc(sizeof*ptlink);

			if(ptlink==NULL)
			{
				Log(LOGT_NETWORK,LOGL_ERROR,"Out of memory!");
				goto die;
			}

			memcpy(ptlink,&tlink,sizeof*ptlink);
			
			pthread_create(&netthread,NULL,&socklisten,ptlink);
		}
	}

	iniparser_freedict(config);
	return EXIT_SUCCESS;
	die:
	iniparser_freedict(config);
	return EXIT_FAILURE;
}

