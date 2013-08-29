#ifndef RLINKEDLIST_H
#define RLINKEDLIST_H

#include <stdlib.h>
#include <string.h>

typedef struct llnode LLNODE;

struct llnode 
{
	void*e;
	LLNODE*n;
};

/*@null@*/LLNODE*lle(void*const);

void lladd(LLNODE*const, void*const);

/*@null@*/LLNODE*llrm(/*@only@*/LLNODE*const, void const*const);

#endif
