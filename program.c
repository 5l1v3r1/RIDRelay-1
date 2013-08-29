#include "program.h"

#include <stdlib.h>
#include "network/accept.h"
#include "Diagnostic/Logger.h"

void console(const LOGT lt, const LOGL ll, char const*const le, va_list ap)
{
	(void)vprintf(le,ap);
	printf("\n");
}

int main(void)
{
	(void)Log_subscribe(&console);
	AcceptClients();

	getchar();

	return EXIT_SUCCESS;
}

