#include "Logger.h"

#include <stdio.h>
#include <string.h>

#include "../Util/LinkedList.h"

/*@abstract@*/struct LOGCB
{
	void(*log)(const LOGT,const LOGL,char const*const,va_list);
};

/*@null@*//*@only@*/static LLNODE*subs;

void const*Log_subscribe(void(*ls)(const LOGT,const LOGL, char const*const, va_list))
/*@globals subs@*/
/*@modifies subs@*/
{
	// Not subscribing nothing, thank you very much.
	if(ls==NULL){return NULL;}
	{
		struct LOGCB cb=
		{
			.log=ls,
		},*p=malloc(sizeof*p);

		if(p==NULL){return NULL;}
		memcpy(p,&cb,sizeof*p);

		if(subs==NULL)
		{
			subs=lle(p);
		}
		else
		{
			lladd(subs,p);
		}
		return p;
	}
}

void Log_unsubscribe(void const*const ticket)
{
	if(subs==NULL||ticket==NULL){return;}
	subs=llrm(subs,ticket);
	free((void*)ticket);
}

void Log(const LOGT lt,const LOGL ll, char const*const format, ...)
{
	va_list ap;
	LLNODE const*n = subs;
	va_start(ap,format);
	if(n!=NULL)do
			if(n->e!=NULL)
				((struct LOGCB*)n->e)->log(lt,ll,format,ap);
		while((n=n->n)!=NULL);
	va_end(ap);
}
