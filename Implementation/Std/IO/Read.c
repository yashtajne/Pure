#include "../../../Interface/Std/IO.h"

size_t
_read(byte_t buffer[], uint64_t buffersize)
{
	size_t bytesread = stream_read( Self.Console.stdin, buffer, buffersize );
	if ( bytesread > 0 )
		buffer[bytesread] = '\0';

	return bytesread;
}

size_t
_readline(byte_t buffer[], uint64_t buffersize)
{
	if ( !buffer || buffersize == 0 )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

	size_t total_bytes = 0;
	while ( total_bytes < buffersize - 1 )
	{
		byte_t ch;
		size_t read_bytes = stream_read( Self.Console.stdin, &ch, 1 );
		if ( read_bytes == 0 || ch == '\n' )
		{
			if ( ch == '\n' )
			{
				buffer[total_bytes++] = '\n';
			}
			break;
		}
		if ( ch == '\r' ) continue;
		buffer[total_bytes++] = ch;
	}

	buffer[total_bytes] = '\0';
	return total_bytes;
}