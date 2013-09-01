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

void(*const csreader[OPC__MAXOPCODE+1])(const int,const int)=
{
	NULL,
	NULL,
	&readcsPing,
	&readcsUpdate,
	&readcsGraph,
	&readcsAlarm,
	NULL,
	NULL,
},(*const screader[OPC__MAXOPCODE+1])(const int,const int)=
{
	NULL,
	&readscLogin,
	&readscPing,
	&readscUpdate,
	&readscGraph,
	&readscAlarm,
	NULL,
	NULL,
};

static uint8_t readByte(const int source,const int destination)
{
	uint8_t i=0;
	(void)recv(source, &i, sizeof(uint8_t),MSG_WAITALL);
	(void)write(destination, &i, sizeof(uint8_t));
	return i;
}

static uint32_t readInt(const int source,const int destination)
{
	uint32_t i=0;
	(void)recv(source, &i, sizeof(uint32_t),MSG_WAITALL);
	(void)write(destination, &i, sizeof(uint32_t));
	// uncomment if in netbyteorder instead of hostbyteorder
	i=ntohl(i);
	return i;
}

void readcsPing(const int client, const int server)
{
	(void)client;
	(void)server;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n> 0x%02x ; PONG",OPC_PING);
}

void readcsUpdate(const int client, const int server)
{
	(void)client;
	(void)server;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n> 0x%02x ; update request",OPC_UPDATE);
}

void readcsGraph(const int client, const int server)
{
	uint32_t x;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n> 0x%02x ; graph request",OPC_GRAPH);
	x=readInt(client,server);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; name length: %d",x,x);
	while(x-->0)
	{
		uint8_t c=readByte(client,server);
		Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; '%c'",c,c);
	}
}

void readscLogin(const int server, const int client)
{
	int x;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n< 0x%02x ; opcode: Login",OPC_LOGIN);
	x=(int)readInt(server,client);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; protocol version: %d",x,x);
}

void readscPing(const int server, const int client)
{
	(void)client;
	(void)server;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n< 0x%02x ; PING",OPC_PING);
}

void readscUpdate(const int server,const int client)
{
	int x;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n< 0x%02x ; opcode: Update",OPC_UPDATE);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; unittype",readInt(server,client));
	x=(int)readInt(server,client);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; amount of sensors: %d",x,x);
	while(x-->0)
	{
		Log(LOGT_TUNNEL,LOGL_RESULT,"\t\t0x%08x",readInt(server,client));
	}

}

void readscGraph(const int server,const int client)
{
	int x;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n< 0x%02x ; opcode: Graph",OPC_GRAPH);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; unit code", readInt(server,client));
	x=(int)readInt(server,client);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; %d datapoints inbound", x,x);
	while(x-->0)
	{
		Log(LOGT_TUNNEL,LOGL_RESULT,"\t\t0x%08x", readInt(server, client));
	}
	
}

void readscAlarm(const int server,const int client)
{
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n< 0x%02x ; opcode: Alarm",OPC_ALARM);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; unit code", readInt(server,client));
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; current sensor value", readInt(server,client));
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; counteraction code",readInt(server,client));
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; sensor number",readInt(server,client));
}

void readcsAlarm(const int client, const int server)
{
	int x;
	Log(LOGT_TUNNEL,LOGL_RESULT,"\n> 0x%02x ; opcode: Alarm",OPC_ALARM);
	x=(int)readInt(client, server);
	Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; namelength: %d", x,x);
	
	while(x-->0)
	{
		Log(LOGT_TUNNEL,LOGL_RESULT,"\t0x%08x ; name", readByte(client, server));
	}
}

