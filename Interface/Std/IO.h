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

/*
static inline void _write_charseq (charseq_t x) { console_write( &Self.Console, x ); }
static inline void _write_string  (string_t* x) { _write_charseq( string_charseq(x) ); }

static inline void _writeline_charseq (charseq_t x) { console_writeline( &Self.Console, x ); }
static inline void _writeline_string  (string_t* x) { _writeline_charseq( string_charseq(x) ); }

#define write(x) _Generic((x), \
	charseq_t: _write_charseq, \
	string_t*: _write_string   \
)(x)

#define writeln(x) _Generic((x), \
charseq_t: _writeline_charseq,   \
string_t*: _writeline_string     \
)(x)
*/

/*
static inline void _swrite(charseq_t seq) {
	stream_write(
		Self.Console.stdout,
		(byte_t*)seq,
		charseq_countbytes(seq)
	);
}

static inline void _swriteln(charseq_t seq) {
	uint64_t size = charseq_countbytes(seq);
	(seq)[(size) + 1] = '\n'; _write( (byte_t*)seq, size + 1 ); (seq)[(size) + 1] = '\0';
}

static inline void _input(byte_t buffer[], uint64_t buffersize)
{
	_read( buffer, buffersize );
	for ( uint64_t i = 0; i < buffersize; i++ )
	{
		if ( buffer[i] == '\n' )
		{
			buffer[i] = '\0';
			return;
		}
	}
}
*/
#endif // StdIO_h