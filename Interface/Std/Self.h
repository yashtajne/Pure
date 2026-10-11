#ifndef StdSelf_h
#define StdSelf_h

#include "IO/Console.h"
#include "IO/Stream.h"

#define STDOUT_BUFFER_SIZE (ONE_KiB * 8)
#define STDIN_BUFFER_SIZE  (ONE_KiB * 8)

typedef struct
{
	charseq_t ProgramName;
	arrayof(string_t)*    Arguments;
	arrayof(kvcharseq_t)* EnviornmentVariables;

	struct {
		stream_t* stdin;
		stream_t* stderr;

		struct
		{
			stream_t* stream;

			struct
			{
				byte_t bytes[STDOUT_BUFFER_SIZE];
				size_t size;
			}
			buffer;
		}
		stdout;
	}
	Console;

	errorcode_t LastError;
	charseq_t   CurrentDirectory;
}
_Self;

extern _Self Self;

#define __PURESTART \
	_Self Self = { 0 };

void exit(uint8_t exitcode);

#endif // StdSelf_h