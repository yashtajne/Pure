#include "../../../Interface/Std/OS/File.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/Self.h"
#include "../IO/Stream.h"

struct _File
{
	stream_t stream;
	byte_t   pathname[];
};

void
file_delete(pathname_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}

	Self.LastError = _file_delete( pathname );
}

file_t*
file_open(charseq_t filename, enum FileOpenMode mode)
{
	if ( !filename )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	file_t* file = allocate( sizeof(*file) );
	if ( !file )
		return NULL;

	Self.LastError = _file_open( file, filename, mode );
	if ( Self.LastError != PURE_OK )
	{
		_deallocate( file );
		return NULL;
	}

	return file;
}

void
file_close(file_t* file)
{
	Self.LastError = _file_close( file );
}

char_t
file_getchar(file_t* file)
{
	char_t character = '\0';
	Self.LastError = _file_getchar( file, &character );
	return character;
}

void
file_putchar(file_t* file, char_t character)
{
	Self.LastError = _file_putchar( file, character );
}

void
file_setcursorposition(file_t* file, uint64_t position)
{
	Self.LastError = _file_setcursorposition( file, position );
}

uint64_t
file_getcursorposition(file_t* file)
{
	uint64_t position = 0;
	Self.LastError = _file_getcursorposition( file, &position );
	return position;
}

size_t
file_getsize(file_t* file)
{
	size_t size = 0;
	Self.LastError = _file_getsize( file, &size );
	return size;
}

errcode_t
_file_delete(pathname_t pathname)
{
	if ( !pathname )
		return PURE_ERROR_INVALID_ARGUMENT;

#if IS_PLATFORM_WINDOWS
	if ( DeleteFileA( pathname ) == 0 )
	{
		DWORD error = GetLastError();
		switch ( error )
		{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				return PURE_ERROR_FILE_NOT_FOUND;

			case ERROR_ACCESS_DENIED:
				return PURE_ERROR_FILE_ACCESS_DENIED;

			default:
				return PURE_ERROR_UNKNOWN;
		}
	}
#elif IS_PLATFORM_LINUX
	if ( linux_unlink( pathname ) == -1 )
	{
		switch ( linux_errno )
		{
			case LINUX_ENOENT:
				return PURE_ERROR_FILE_NOT_FOUND;

			case LINUX_EACCES: // (Permission denied)
			case LINUX_EISDIR: // (Target is a directory, not a file)
				return PURE_ERROR_FILE_ACCESS_DENIED;

			default:
				return PURE_ERROR_UNKNOWN;
		}
	}
#else
#error "Platform not supported!"
#endif

	return PURE_OK;
}

errcode_t
_file_open(file_t* file, charseq_t filename, enum FileOpenMode mode)
{
	if ( !file )
		return PURE_ERROR_NULL_POINTER;
	if ( !filename )
		return PURE_ERROR_INVALID_ARGUMENT;

#if IS_PLATFORM_WINDOWS
	DWORD   desired_access       = 0;
	DWORD   creation_disposition = 0;
	boolean is_append            = false;

	switch ( mode )
	{
		case READ_ONLY:
			desired_access       = GENERIC_READ;
			creation_disposition = OPEN_EXISTING;
			break;

		case WRITE_ONLY:
			desired_access       = GENERIC_WRITE;
			creation_disposition = CREATE_ALWAYS;
			break;

		case READ_AND_WRITE:
			desired_access       = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = OPEN_ALWAYS;
			break;

		case WRITE_ONLY_APPEND:
			desired_access       = GENERIC_WRITE;
			creation_disposition = OPEN_EXISTING;
			is_append            = true;
			break;

		case READ_WRITE_APPEND:
			desired_access       = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = OPEN_ALWAYS;
			is_append            = true;
			break;

		case WRITE_ONLY_EXISTING:
			desired_access       = GENERIC_WRITE;
			creation_disposition = CREATE_NEW;
			break;

		case READ_WRITE_EXISTING:
			desired_access       = GENERIC_READ | GENERIC_WRITE;
			creation_disposition = CREATE_NEW;
			break;

		default:
			return PURE_ERROR_INVALID_ARGUMENT;
	}

	file->stream.handle = CreateFileA(
		filename,
		desired_access,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		creation_disposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if ( file->stream.handle == INVALID_HANDLE_VALUE )
	{
		DWORD win_err = GetLastError();

		if ( win_err == ERROR_FILE_NOT_FOUND || win_err == ERROR_PATH_NOT_FOUND )
			return PURE_ERROR_FILE_NOT_FOUND;
		else if ( win_err == ERROR_ALREADY_EXISTS || win_err == ERROR_FILE_EXISTS )
			return PURE_ERROR_FILE_ALREADY_EXISTS;
		else
			return PURE_ERROR_FILE_ACCESS_DENIED;
	}

	if ( is_append )
	{
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER new_pos;
		SetFilePointerEx( file->stream.handle, zero, &new_pos, FILE_END );
	}

	return PURE_OK;

#elif IS_PLATFORM_LINUX
	int flags;

	switch ( mode )
	{
		case READ_ONLY:
			flags = LINUX_O_RDONLY;
			break;

		case WRITE_ONLY:
			flags = LINUX_O_WRONLY | LINUX_O_CREAT | LINUX_O_TRUNC;
			break;

		case READ_AND_WRITE:
			flags = LINUX_O_RDWR | LINUX_O_CREAT;
			break;

		case WRITE_ONLY_APPEND:
			flags = LINUX_O_WRONLY | LINUX_O_APPEND;
			break;

		case READ_WRITE_APPEND:
			flags = LINUX_O_RDWR | LINUX_O_CREAT | LINUX_O_APPEND;
			break;

		case WRITE_ONLY_EXISTING:
			flags = LINUX_O_WRONLY | LINUX_O_CREAT | LINUX_O_EXCL;
			break;

		case READ_WRITE_EXISTING:
			flags = LINUX_O_RDWR | LINUX_O_CREAT | LINUX_O_EXCL;
			break;

		default:
			return PURE_ERROR_INVALID_ARGUMENT;
	}

	file->descriptor = linux_open( filename, flags, 0666 );
	if ( file->descriptor == -1 )
	{
		switch ( linux_errno )
		{
			case LINUX_ENOENT:
				return PURE_ERROR_FILE_NOT_FOUND;
			case LINUX_EEXIST:
				return PURE_ERROR_FILE_ALREADY_EXISTS;
			case LINUX_EACCES:
				return PURE_ERROR_FILE_ACCESS_DENIED;
			default:
				return PURE_ERROR_UNKNOWN;
		}
	}

	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_close(file_t* file)
{
	if ( !file )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( !CloseHandle( file->stream.handle ) )
		return PURE_ERROR_INVALID_ARGUMENT;

	return _deallocate( file );
#elif IS_PLATFORM_LINUX
	if ( linux_close( file->descriptor ) != 0 )
		return PURE_ERROR_INVALID_ARGUMENT;
	return _deallocate( file );
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_getchar(file_t* file, char_t* ch)
{
#if IS_PLATFORM_WINDOWS
	DWORD  bytes_read = 0;

	if ( !ReadFile( file->stream.handle, ch, 1, &bytes_read, NULL ) )
		return PURE_ERROR_FILE_READ_FAILED;

	if ( bytes_read == 1 )
		return PURE_OK;

	if ( bytes_read == 0 ) // EOF
	{
		*ch = (char_t)-1;
		return PURE_OK;
	}

	return PURE_ERROR_FILE_READ_FAILED;
#elif IS_PLATFORM_LINUX
	linux_ssize_t bytes_read;
	do
	{
		bytes_read = linux_read( file->descriptor, ch, 1 );
	} while ( bytes_read == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_read == 1 )
		return PURE_OK;

	if ( bytes_read == 0 ) // EOF
	{
		*ch = (char_t)-1;
		return PURE_OK;
	}

	return PURE_ERROR_FILE_READ_FAILED;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_putchar(file_t* file, char_t character)
{
	if ( !file )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	DWORD bytes_written = 0;

	if ( !WriteFile( file->stream.handle, &character, 1, &bytes_written, NULL ) || bytes_written != 1 )
		return PURE_ERROR_FILE_WRITE_FAILED;

	return PURE_OK;
#elif IS_PLATFORM_LINUX
	linux_ssize_t bytes_written;

	do
	{
		bytes_written = linux_write( file->descriptor, &character, 1 );
	} while ( bytes_written == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_written != 1 )
		return PURE_ERROR_FILE_WRITE_FAILED;

	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}


errcode_t
_file_getcursorposition(file_t* file, uint64_t* position)
{
	if ( !file || !position )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER zero = { 0 };
	LARGE_INTEGER current_position;

	if ( !SetFilePointerEx( file->stream.handle, zero, &current_position, FILE_CURRENT ) )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	*position = (uint64_t)current_position.QuadPart;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	int64_t current_position = linux_lseek( file->descriptor, 0, LINUX_SEEK_CUR );
	if ( current_position == -1 )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	*position = (uint64_t)current_position;
	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_setcursorposition(file_t* file, uint64_t position)
{
	if ( !file )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( position > INT64_MAX )
	{
		return PURE_ERROR_INVALID_ARGUMENT;
	}

	LARGE_INTEGER dist;
	dist.QuadPart = (long long)position;
	LARGE_INTEGER new_pos;

	if ( !SetFilePointerEx( file->stream.handle, dist, &new_pos, FILE_BEGIN ) )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( position > INT64_MAX )
	{
		return PURE_ERROR_INVALID_ARGUMENT;
	}

	int64_t new_position = linux_lseek( file->descriptor, (int64_t)position, LINUX_SEEK_SET );
	if ( new_position == -1 )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

void
file_movecursorposition(file_t* file, int64_t offset)
{
	Self.LastError = _file_movecursorposition( file, offset );
}

errcode_t
_file_movecursorposition(file_t* file, int64_t offset)
{
	if ( !file )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER dist;
	dist.QuadPart = (long long)offset;
	LARGE_INTEGER new_pos;

	if ( !SetFilePointerEx( file->stream.handle, dist, &new_pos, FILE_CURRENT ) )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	return PURE_OK;
#elif IS_PLATFORM_LINUX
	int64_t new_position = linux_lseek( file->descriptor, offset, LINUX_SEEK_CUR );
	if ( new_position == -1 )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_read(file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbytesread)
{
	*numberofbytesread = 0;

#if IS_PLATFORM_WINDOWS
	if ( buffersize > UINT32_MAX )
		return PURE_ERROR_INVALID_ARGUMENT;

	DWORD tmp = 0;
	if ( !ReadFile( file->stream.handle, buffer, (DWORD)buffersize, &tmp, NULL ) )
		return PURE_ERROR_FILE_READ_FAILED;

	*numberofbytesread = tmp;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( buffersize > INT64_MAX )
		return PURE_ERROR_INVALID_ARGUMENT;

	linux_ssize_t tmp;
	do
	{
		tmp = linux_read( file->descriptor, buffer, buffersize );
	} while ( tmp == -1 && linux_error(0) == LINUX_EINTR );

	if ( tmp == -1 )
		return PURE_ERROR_FILE_READ_FAILED;

	*numberofbytesread = (size_t)tmp;
	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_write(file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbyteswritten)
{
	*numberofbyteswritten = 0;

#if IS_PLATFORM_WINDOWS
	if ( buffersize > UINT32_MAX )
		return PURE_ERROR_INVALID_ARGUMENT;

	DWORD tmp = 0;
	if ( !WriteFile( file->stream.handle, buffer, (DWORD)buffersize, &tmp, NULL ) )
		return PURE_ERROR_FILE_WRITE_FAILED;

	*numberofbyteswritten = tmp;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( buffersize > INT64_MAX )
		return PURE_ERROR_INVALID_ARGUMENT;

	linux_ssize_t tmp;
	do
	{
		tmp = linux_write( file->descriptor, buffer, buffersize );
	} while ( tmp == -1 && linux_error(0) == LINUX_EINTR );

	if ( tmp == -1 )
		return PURE_ERROR_FILE_WRITE_FAILED;

	*numberofbyteswritten = (size_t)tmp;
	return PURE_OK;
#else
	#error "Not implemented!"
#endif
}

errcode_t
_file_readline(file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbytesread)
{
	if ( buffersize == 0 )
		return PURE_ERROR_INVALID_ARGUMENT;

	*numberofbytesread = 0;
	byte_t chunk[256];
	boolean saw_newline = false;
	boolean at_eof = false;
	boolean did_read = false;

	while ( *numberofbytesread < buffersize - 1 )
	{
		size_t space_remaining = buffersize - 1 - *numberofbytesread;
		size_t to_read = (space_remaining < sizeof(chunk)) ? space_remaining : sizeof(chunk);

		size_t bytes_read = 0;
		errcode_t err = _file_read( file, chunk, to_read, &bytes_read );
		if ( err ) return err;
		did_read = true;

		if ( bytes_read == 0 )
		{
			at_eof = true;
			break;
		}

		boolean found_newline = false;
		size_t line_bytes = 0;
		size_t i = 0;

		for ( i = 0; i < bytes_read; i++ )
		{
			if ( chunk[i] == '\n' )
			{
				found_newline = true;
				break;
			}

			if ( chunk[i] != '\r' )
			{
				buffer[*numberofbytesread + line_bytes] = chunk[i];
				line_bytes++;
			}
		}

		*numberofbytesread += line_bytes;

		if ( found_newline )
		{
			size_t unconsumed = bytes_read - (i + 1);
			if ( unconsumed > 0 )
			{
				errcode_t err = _file_movecursorposition( file, -(int64_t)unconsumed );
				if ( err ) return err;
			}
			saw_newline = true;
			break;
		}
	}

	buffer[*numberofbytesread] = '\0';

	if ( did_read && !saw_newline && !at_eof && *numberofbytesread == buffersize - 1 )
		return PURE_ERROR_BUFFER_TOO_SMALL;

	return PURE_OK;
}

errcode_t
_file_getsize(file_t* file, size_t* size)
{
	if ( !file || !size )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER file_size;
	if ( !GetFileSizeEx( file->stream.handle, &file_size ) )
	{
		return PURE_ERROR_FILE_READ_FAILED;
	}

	*size = (size_t)file_size.QuadPart;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	int64_t current_position = linux_lseek( file->descriptor, 0, LINUX_SEEK_CUR );
	if ( current_position == -1 )
		return PURE_ERROR_FILE_READ_FAILED;

	int64_t file_size = linux_lseek( file->descriptor, 0, LINUX_SEEK_END );
	if ( file_size == -1 )
		return PURE_ERROR_FILE_READ_FAILED;

	// Restore cursor position
	if ( linux_lseek( file->descriptor, current_position, LINUX_SEEK_SET ) == -1 )
		return PURE_ERROR_FILE_READ_FAILED;

	*size = (size_t)file_size;
	return PURE_OK;
#else
	#error "Not Implemented!"
#endif
}