#ifndef DCWLOG_H_INCLUDED
#define DCWLOG_H_INCLUDED


#include <stdio.h>

#define dcwloginfof(FMT, ...) fprintf(stderr, "[DCWINFO] " FMT, __VA_ARGS__)
#define dcwlogwarnf(FMT, ...) fprintf(stderr, "[DCWWARN] " FMT, __VA_ARGS__)
#define dcwlogerrf(FMT, ...)  fprintf(stderr, "[DCWERR] " FMT, __VA_ARGS__)
#define dcwlogdbgf(FMT, ...)  fprintf(stderr, "[DCWDBG] " FMT, __VA_ARGS__)

#endif
