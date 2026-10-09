#ifndef StdIO_h
#define StdIO_h

#include "IO/Console.h"
#include "IO/Stream.h"
#include "Memory.h"
#include "String.h"
#include "Self.h"

void _flush (void);

void _write (byte_t buffer[], size_t buffersize);
// void _writeline (byte_t buffer[], uint64_t buffersize);

size_t _read     (byte_t buffer[], uint64_t buffersize);
size_t _readline (byte_t buffer[], uint64_t buffersize);

void _writecharseq (charseq_t seq);

#endif // StdIO_h