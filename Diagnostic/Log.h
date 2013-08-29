#ifndef LOG_H
#define LOG_H

typedef enum
{
	// there should never be a message with loglevel undefined, it may be used internally to mark invalid messages.
	LOGL_UNDEFINED = 0x00,
	// something bad happened; prepare to meet the programs demise.
	LOGL_ERROR,
	// something bad happened, but the program may just survive this. The operator should probably look into this and try and correct it before the next run or iteration.
	LOGL_WARNING,
	// If the program is specifically requested to log something (say for example, --help or --version output) this level will make sure it is not muted.
	LOGL_RESULTS,
	// The operator will probably want to know about this.
	LOGL_NOTICE,
	// Something happened within the system, there is nothing you can or need to do about this.
	LOGL_INFO,
	// The program will proudly announce any data or direction it encounters, this is really exciting, OOH LOOK, I JUST OPENED A FILE WITH EXTENTION .DAT, THE DATA INSIDE LOOKS VALID AND IS STORED IN VERSION THREE FORMAT, WE SUPPORT VERSION THREE, MR. OPERATOR, ISN'T THAT GREAT NEWS!? :D
	LOGL_DEBUG,
	// If you thought debug was exciting, dump may make you feel all warm and fuzzy inside, even though you know you that's bad. The program will produce giant dumps of its internal data whenever the programmers felt like it.Did I just update a value in this tree? Better print out the entire tree so that operator-sensei can review it. Operator-sensei is after all much wiser and more competent than the program itself. note that this log level may have severe security implications for users.
	LOGL_DUMP,
	// The system will admit all of its shortcomings and faults openly through this loglevel. Best left muted in production use to avoid emberrassing situations during presentations or inspections.
	LOGL_BUG,
} LOGL;

// No need for this yet
typedef enum
{
	LOGT_UNDEFINED,
	LOGT_NETWORK,
	LOGT_TUNNEL,
} LOGT;

extern void Log(const LOGT,const LOGL,char const*const, ...);


#endif
