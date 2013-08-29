#ifndef TUNNEL_H
#define TUNNEL_H

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../Util/queue.h"

typedef struct
{
	const int client,server;
	pthread_t iloop,oloop;
	queue*_queue;
} Tunnel;

extern Tunnel*SpawnTunnel(const struct sockaddr*,const int);

#endif

