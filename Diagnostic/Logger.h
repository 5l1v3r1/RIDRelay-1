#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

#include "Log.h"

/*@null@*//*@only@*/void const*
Log_subscribe(void(*)(const LOGT,const LOGL,char const*const,va_list))/*@globals internalState@*//*@modifies internalState@*/;

void Log_unsubscribe(/*@only@*/void const*const);

#endif

