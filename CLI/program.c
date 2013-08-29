#include <stdlib.h>
#include <stdio.h>

#include "../Diagnostic/Logger.h"

static void console(const LOGL ll,char const*const format,va_list args)
{
	if(ll!=LOGL_UNDEFINED)
		(void)vprintf(format,args);
}

int main(void)
{
	void const*const ticket=Log_subscribe(&console);
	if(ticket==NULL)
	{
		Log(LOGL_ERROR,"ran out of memory during startup!");
		return EXIT_FAILURE;
	}


	Log_unsubscribe(ticket);
	return EXIT_SUCCESS;
}

