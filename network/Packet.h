#ifndef RPACKET_H
#define RPACKET_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

typedef enum opcode
{
	OPC_UNDEFINED=0x00,
	OPC_LOGIN,
	OPC_PING,
	OPC_UPDATE,
	OPC_GRAPH,
	OPC_ALARM,
	OPC_BOUNDS,
	OPC_VALUE,
} opcode;

typedef enum{unit_undefined,unit_temperature,unit_flow,unit_pressure,unit_fullness,unit_radiation} unittype;

typedef struct
{
	opcode op;
} Packet;

typedef Packet scPacket;
typedef Packet csPacket;

struct csPing{csPacket base;};
struct scPing{scPacket base;};

struct csUpdate{csPacket base;};

struct scUpdate
{
	scPacket base;
	unittype type;
	unsigned int sensorcount;
	int*sensors;
};

struct csGraph
{
	csPacket base;
	unsigned int namelen;
	char*name;
};

struct scGraph
{
	scPacket base;
};

struct iBounds
{
	Packet base;
	char*name;
	int lbound,ubound;
};

struct Update
{
	Packet base;
	unittype unit;
	int sensorlen;
	int*sensors;
};

extern void readcsPing(const int, const int);
extern void readcsUpdate(const int, const int);
extern void readcsGraph(const int, const int);

extern void readscLogin(const int, const int);
extern void readscPing(const int, const int);
extern void readscUpdate(const int,const int);
extern void readscGraph(const int, const int);


#endif

