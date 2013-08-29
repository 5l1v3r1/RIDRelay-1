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
} opcode;

typedef enum{unit_undefined,unit_temperature,unit_flow,unit_pressure,unit_fullness,unit_radiation} unittype;

typedef struct
{
	opcode op;
} Packet;

typedef Packet iPacket;

struct LoginPacket
{
	Packet base;
	int zero;
};

struct iGraph
{
	iPacket base;
	char*name;
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

struct serverGraph
{
};

ssize_t writeUpdate(const int, struct Update*);
ssize_t writeGraph(const int,struct serverGraph*);
ssize_t writeLogin(const int,struct LoginPacket*);

struct iGraph*readGraph(const int);
struct iBounds*readBounds(const int);
struct Update*readUpdate(const int);

void destroyiGraph(struct iGraph*);
void destroyoGraph(struct serverGraph*);
void destroyiBounds(struct iBounds*);
void destroyUpdate(struct Update*);
void destroyLogin(struct LoginPacket*);
#endif
