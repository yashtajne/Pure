#ifndef StdIOConsole_h
#define StdIOConsole_h

#include "Stream.h"
#include "../String.h"
#include "../Charseq.h"
#include "../../Core/Byte.h"

struct ConsoleDimensions
{
	int width;
	int height;
};
struct ConsoleDimensions getConsoleDimensions(void);

#define _console_write(C, B, S) stream_write( (C)->stdout, B, S )
#define  console_write(C, S)    stream_write( (C)->stdout, (byte_t*)(S), charseq_countbytes((S)) );

#define _console_writeline(C, B, S) do { \
	(B)[(S) - 1] = '\n';                 \
	_console_write( C, B, S );           \
	(B)[(S) - 1] = '\0';                 \
} while (0);

#define  console_writeline(C, S) \
	_console_writeline((C), (byte_t*)(S), charseq_countbytes( (S) ));

#define _console_read(C, B, S) stream_read( (C)->stdin, (B), (S) );
// #define  console_read(C, BA, N) _console_read((C), (BA)->elements, N );

// uint64_t _console_readline (console_t*, byte_t buffer[]);
// string_t*  console_readline (console_t*);

#endif // StdIOConsole_h