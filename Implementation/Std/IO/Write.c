#include "../../../Interface/Std/IO.h"

#define __FLUSH {                         \
	stream_write(                         \
		Self.Console.stdout.stream,       \
		Self.Console.stdout.buffer.bytes, \
		Self.Console.stdout.buffer.size   \
	);                                    \
                                          \
	Self.Console.stdout.buffer.size = 0;  \
}

void _flush(void)
{
	if ( Self.Console.stdout.buffer.size > 0 )
		__FLUSH
}

void _write(byte_t buffer[], uint64_t buffersize)
{
	size_t numberofbyteswritten = 0;
	while ( numberofbyteswritten < buffersize )
	{
		if ( Self.Console.stdout.buffer.size >= STDOUT_BUFFER_SIZE )
			__FLUSH

		size_t remaining_to_write = buffersize - numberofbyteswritten;
		size_t free_buffer_space = STDOUT_BUFFER_SIZE - Self.Console.stdout.buffer.size;
		size_t canwritesize = (remaining_to_write < free_buffer_space) ? remaining_to_write : free_buffer_space;

		memory_copy(
			&buffer[numberofbyteswritten],
			&Self.Console.stdout.buffer.bytes[Self.Console.stdout.buffer.size],
			canwritesize
		);

		Self.Console.stdout.buffer.size += canwritesize;
		numberofbyteswritten += canwritesize;
	}

	_flush();
}

void _writecharseq(char* c_string)
{
	size_t size = charseq_countbytes( c_string );
	_write( (byte_t*)c_string, size );
}