#include "Stream.h"

uint64_t
stream_write(stream_t* stream, byte_t buffer[], uint64_t buffersize)
{
/*
	if ( !stream || !buffer )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

	if ( !buffersize )
	{
		Self.LastError = PURE_ERROR_BUFFER_TOO_SMALL;
		return 0;
	}
*/

#if IS_PLATFORM_WINDOWS
	DWORD written = 0;
    BOOL s = WriteFile( stream->handle, buffer, (DWORD)buffersize, &written, NULL );
    return s ? (uint64_t)written : 0;
#elif IS_PLATFORM_LINUX
	return linux_write( stream->descriptor, buffer, buffersize );
#else
#error "Not implemented yet"
#endif
}

uint64_t
stream_read(stream_t* stream, byte_t buffer[], uint64_t buffersize)
{
	// if ( buffer == NULL || buffersize == 0 )
	// 	return 0; // error

#if IS_PLATFORM_WINDOWS
	DWORD written = 0;
    BOOL s = ReadFile( stream->handle, buffer, (DWORD)buffersize, &written, NULL );
    return s ? (uint64_t)written : 0;
#elif IS_PLATFORM_LINUX
	linux_ssize_t result = linux_read( stream->descriptor, buffer, buffersize );
	if ( result < 0 && linux_error(result) == LINUX_EINTR )
		return 0; // error
	return result;
#else
#error "Not implemented yet"
#endif
}