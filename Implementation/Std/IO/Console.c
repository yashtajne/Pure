#include "../../../Interface/Std/IO/Console.h"

/*
#define STREAM_READER_LOCAL_BUFFER_SIZE ONE_KiB

uint64_t
_console_readline(console_t* console, byte_t buffer[])
{
	byte_t local[STREAM_READER_LOCAL_BUFFER_SIZE];
	uint64_t bytesread = 0;

	while ( true )
	{
		uint16_t _bytesread = stream_read( console->stdin, local, STREAM_READER_LOCAL_BUFFER_SIZE );
		if ( !_bytesread )
		{
			buffer[bytesread] = '\0';
			break;
		}

		for ( uint16_t i = 0; i < _bytesread; i++ )
		{
			if (local[i] == '\n')
			{
				buffer[bytesread] = '\0';
				return bytesread;
			}

			if (local[i] == '\r') continue;
			buffer[bytesread++] = local[i];
		}
	}

	return bytesread;
}
*/

/*
#include "../PlatformInc.def"
struct ConsoleDimensions
getConsoleDimensions(console_t* console, err_t* occured)
{
#if IS_PLATFORM_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (console->stdout == INVALID_HANDLE_VALUE) {
		if ( occured != NULL ) { *occured = PURE_ERROR_UNKNOWN; }
		return (struct ConsoleDimensions){ 0, 0 };
	}

	if (!GetConsoleScreenBufferInfo( console->stdout, &info) ) {
		if ( occured != NULL ) { *occured = PURE_ERROR_UNKNOWN; }
		return (struct ConsoleDimensions){ 0, 0 };
	}

	short width = info.srWindow.Right - info.srWindow.Left + 1;
	short height = info.srWindow.Bottom - info.srWindow.Top + 1;

	return (struct ConsoleDimensions){ width, height };
#elif IS_PLATFORM_LINUX
	if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, &w ) == -1 ) {
		if (occured != NULL) {
			*occured = PURE_ERROR_UNKNOWN;
		}
		return (struct ConsoleDimensions){ 0, 0 };
	}

	return (struct ConsoleDimensions){ (short)w.ws_col, (short)w.ws_row };
#else
	#error "Not implemented!"
#endif
}
*/