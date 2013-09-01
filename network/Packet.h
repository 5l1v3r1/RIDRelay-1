#ifndef RPACKET_H
#define RPACKET_H


typedef enum opcode
{
	OPC__UNDEFINED=0x00,
	OPC_LOGIN,
	OPC_PING,
	OPC_UPDATE,
	OPC_GRAPH,
	OPC_ALARM,
	OPC_BOUNDS,
	OPC_VALUE,
	#define OPC__MAXOPCODE OPC_VALUE
} opcode;

typedef enum{unit_undefined,unit_temperature,unit_flow,unit_pressure,unit_fullness,unit_radiation} unittype;

typedef struct
{
	opcode op;
} Packet;

typedef Packet scPacket;
typedef Packet csPacket;

extern void readcsPing(const int, const int);
extern void readcsUpdate(const int, const int);
extern void readcsGraph(const int, const int);
extern void readcsAlarm(const int,const int);

extern void readscLogin(const int, const int);
extern void readscPing(const int, const int);
extern void readscUpdate(const int,const int);
extern void readscGraph(const int, const int);
extern void readscAlarm(const int, const int);

extern void
(*const csreader[OPC__MAXOPCODE+1])(const int,const int),
(*const screader[OPC__MAXOPCODE+1])(const int,const int);

#endif

