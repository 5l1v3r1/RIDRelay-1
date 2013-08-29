#include "Packet.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

#include "../Diagnostic/Log.h"

struct iGraph*readGraph(const int source)
{
	struct iGraph u=
	{
		.base={.op=OPC_UNDEFINED,},
		.name=NULL,
	},*p=malloc(sizeof(struct iGraph));

	uint32_t requestsize=0;
	size_t wanted;
	ssize_t expected;
	char*sensor;
	unsigned int i;

	if(!p)return NULL;
	memcpy(p,&u,sizeof*p);

	wanted=sizeof(int);
	expected=(ssize_t)wanted;
	if(recv(source, &requestsize, wanted,MSG_WAITALL)!=expected)
	{
		return p;
	}
	// only needed for int sizes
	requestsize=ntohl(requestsize);
	if(requestsize<1)
	{
		return p;
	}

	wanted=(sizeof(char)*requestsize);
	expected=(ssize_t)wanted;
	sensor=malloc(wanted+sizeof(char));
	if(!sensor)return NULL;
	for(i=(unsigned int)wanted+1;--i;)
	{
		sensor[i]=0;
	}
	if(recv(source, sensor, wanted,MSG_WAITALL)!=expected)
	{
		return p;
	}

	p->base.op=OPC_GRAPH;
	p->name=sensor;
	return p;
}

struct iBounds*readBounds(const int source)
{
	struct iBounds b=
	{
		.base={.op=OPC_UNDEFINED,},
		.name=NULL,
		.lbound=INT_MIN,
		.ubound=INT_MAX,
	},*p=malloc(sizeof*p);
	size_t wanted;
	uint32_t namelen,lbound,ubound;
	char*sensor;

	if(!p)return NULL;
	memcpy(p,&b,sizeof*p);

	wanted=sizeof(int);
	if(recv(source,&namelen,wanted,MSG_WAITALL)==-1)
	{
		return p;
	}
	namelen=ntohl(namelen);

	if(namelen<1)
	{
		return p;
	}

	wanted=sizeof(char)*namelen;
	sensor=malloc(wanted+sizeof(char));
	if(!sensor)
	{
		free(p);
		return NULL;
	}
	{unsigned int i;for(i=(unsigned int)wanted+1;--i;)
	{
		sensor[i]=0;
	}}

	if(recv(source,sensor,wanted,MSG_WAITALL)==-1)
	{
		return p;
	}

	wanted=sizeof(int);
	if(recv(source,&lbound,wanted,MSG_WAITALL)==-1)
	{
		return p;
	}
	lbound=ntohl(lbound);
	if(recv(source,&ubound,wanted,MSG_WAITALL)==-1)
	{
		return p;
	}
	ubound=ntohl(ubound);

	p->base.op=OPC_BOUNDS;
	p->name=sensor;
	p->lbound=(int)lbound;
	p->ubound=(int)ubound;
	return p;
}

void destroyLogin(struct LoginPacket*l)
{
	free(l);
}

void destroyUpdate(struct Update*u)
{
	free(u->sensors);
	u->sensors=NULL;
	free(u);
	u=NULL;
}

void destroyiGraph(struct iGraph*g)
{
	free(g->name);
	g->name=NULL;
	free(g);
	g=NULL;
}

void destroyiBounds(struct iBounds*b)
{
	free(b->name);
	b->name=NULL;
	free(b);
	b=NULL;
}

