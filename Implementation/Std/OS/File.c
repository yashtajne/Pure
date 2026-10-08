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

#if IS_PLATFORM_WINDOWS
	if ( DeleteFileA( pathname ) == 0 )
	{
		DWORD error = GetLastError();
		switch ( error )
		{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				Self.LastError = PURE_ERROR_FILE_NOT_FOUND;
				return;

			case ERROR_ACCESS_DENIED:
				Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;
				return;

			default:
				Self.LastError = PURE_ERROR_UNKNOWN;
				return;
		}
	}
#elif IS_PLATFORM_LINUX
	if ( linux_unlink( pathname ) == -1 )
	{
		switch ( linux_errno )
		{
			case LINUX_ENOENT:
				Self.LastError = PURE_ERROR_FILE_NOT_FOUND;
				return;

			case LINUX_EACCES: // (Permission denied)
			case LINUX_EISDIR: // (Target is a directory, not a file)
				Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;
				return;

			default:
				Self.LastError = PURE_ERROR_UNKNOWN;
				return;
		}
	}
#else
#error "Platform not supported!"
#endif

	Self.LastError = PURE_OK;
}


file_t*
file_open(charseq_t filename, enum FileOpenMode mode)
{
	if ( !filename )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

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
			Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
			return NULL;
	}

	err_t      err;
	voidptr_t  allocated = NULL;
	if ( (err = heap_allocate( sizeof(file_t), &allocated )) )
	{
		Self.LastError = err;
		return NULL;
	}

	file_t* file = allocated;
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
			Self.LastError = PURE_ERROR_FILE_NOT_FOUND;
		else if ( win_err == ERROR_ALREADY_EXISTS || win_err == ERROR_FILE_EXISTS )
			Self.LastError = PURE_ERROR_FILE_ALREADY_EXISTS;
		else
			Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;

		heap_deallocate( file );
		return NULL;
	}

	if ( is_append )
	{
		LARGE_INTEGER zero = { 0 };
		LARGE_INTEGER new_pos;
		SetFilePointerEx( file->stream.handle, zero, &new_pos, FILE_END );
	}

	Self.LastError = PURE_OK;
	return file;

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
			Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
			return NULL;
	}

	err_t     err;
	voidptr_t allocated = NULL;
	if ( (err = heap_allocate( sizeof(file_t), &allocated )) )
	{
		Self.LastError = err;
		return NULL;
	}

	file_t* file = allocated;
	file->descriptor = linux_open( filename, flags, 0666 );
	if ( file->descriptor == -1 )
	{
		switch ( linux_errno )
		{
			case LINUX_ENOENT:
				Self.LastError = PURE_ERROR_FILE_NOT_FOUND;
				break;
			case LINUX_EEXIST:
				Self.LastError = PURE_ERROR_FILE_ALREADY_EXISTS;
				break;
			case LINUX_EACCES:
				Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;
				break;
			default:
				Self.LastError = PURE_ERROR_UNKNOWN;
				break;
		}

		heap_deallocate( file );
		return NULL;
	}

	Self.LastError = PURE_OK;
	return file;

#else
	#error "Not implemented!"
#endif
}

void
file_close(file_t* file)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

#if IS_PLATFORM_WINDOWS
	if ( !CloseHandle( file->stream.handle ) )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}
	Self.LastError = heap_deallocate( file );
#elif IS_PLATFORM_LINUX
	if ( linux_close( file->descriptor ) != 0 )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}
	Self.LastError = heap_deallocate( file );
#else
	#error "Not implemented!"
#endif
}

char_t
file_getchar(file_t* file)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return '\0';
	}

#if IS_PLATFORM_WINDOWS
	char_t character;
	DWORD  bytes_read = 0;

	if ( !ReadFile( file->stream.handle, &character, 1, &bytes_read, NULL ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return '\0';
	}

	if ( bytes_read == 1 )
	{
		Self.LastError = PURE_OK;
		return character;
	}

	if ( bytes_read == 0 ) // EOF
	{
		Self.LastError = PURE_OK;
		return (char_t)-1;
	}

	Self.LastError = PURE_ERROR_FILE_READ_FAILED;
	return '\0';
#elif IS_PLATFORM_LINUX
	char_t        character;
	linux_ssize_t bytes_read;

	do
	{
		bytes_read = linux_read( file->descriptor, &character, 1 );
	} while ( bytes_read == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_read == 1 )
	{
		Self.LastError = PURE_OK;
		return character;
	}

	if ( bytes_read == 0 ) // EOF
	{
		Self.LastError = PURE_OK;
		return (char_t)-1;
	}

	Self.LastError = PURE_ERROR_FILE_READ_FAILED;
	return '\0';
#else
	#error "Not implemented!"
#endif
}

void
file_putchar(file_t* file, char_t character)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

#if IS_PLATFORM_WINDOWS
	DWORD bytes_written = 0;

	if ( !WriteFile( file->stream.handle, &character, 1, &bytes_written, NULL ) || bytes_written != 1 )
	{
		Self.LastError = PURE_ERROR_FILE_WRITE_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#elif IS_PLATFORM_LINUX
	linux_ssize_t bytes_written;

	do
	{
		bytes_written = linux_write( file->descriptor, &character, 1 );
	} while ( bytes_written == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_written != 1 )
	{
		Self.LastError = PURE_ERROR_FILE_WRITE_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#else
	#error "Not implemented!"
#endif
}

uint64_t
file_getcursorposition(file_t* file)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER zero = { 0 };
	LARGE_INTEGER position;

	if ( !SetFilePointerEx( file->stream.handle, zero, &position, FILE_CURRENT ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (uint64_t)position.QuadPart;
#elif IS_PLATFORM_LINUX
	int64_t position = linux_lseek( file->descriptor, 0, LINUX_SEEK_CUR );
	if ( position == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (uint64_t)position;
#else
	#error "Not implemented!"
#endif
}

void
file_setcursorposition(file_t* file, uint64_t position)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

#if IS_PLATFORM_WINDOWS
	if ( position > INT64_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}

	LARGE_INTEGER dist;
	dist.QuadPart = (long long)position;
	LARGE_INTEGER new_pos;

	if ( !SetFilePointerEx( file->stream.handle, dist, &new_pos, FILE_BEGIN ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( position > INT64_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return;
	}

	int64_t new_position = linux_lseek( file->descriptor, (int64_t)position, LINUX_SEEK_SET );
	if ( new_position == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#else
	#error "Not implemented!"
#endif
}

void
file_movecursorposition(file_t* file, int64_t offset)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER dist;
	dist.QuadPart = (long long)offset;
	LARGE_INTEGER new_pos;

	if ( !SetFilePointerEx( file->stream.handle, dist, &new_pos, FILE_CURRENT ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#elif IS_PLATFORM_LINUX
	int64_t new_position = linux_lseek( file->descriptor, offset, LINUX_SEEK_CUR );
	if ( new_position == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return;
	}

	Self.LastError = PURE_OK;
#else
	#error "Not implemented!"
#endif
}

size_t
_file_read(file_t* file, byte_t buffer[], uint64_t buffersize)
{
	if ( !file || !buffer )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

#if IS_PLATFORM_WINDOWS
	if ( buffersize > UINT32_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return 0;
	}

	DWORD bytes_read = 0;
	if ( !ReadFile( file->stream.handle, buffer, (DWORD)buffersize, &bytes_read, NULL ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (size_t)bytes_read;
#elif IS_PLATFORM_LINUX
	if ( buffersize > INT64_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return 0;
	}

	linux_ssize_t bytes_read;
	do
	{
		bytes_read = linux_read( file->descriptor, buffer, buffersize );
	} while ( bytes_read == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_read == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (size_t)bytes_read;
#else
	#error "Not implemented!"
#endif
}

size_t
_file_write(file_t* file, byte_t buffer[], uint64_t buffersize)
{
	if ( !file || !buffer )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

#if IS_PLATFORM_WINDOWS
	if ( buffersize > UINT32_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return 0;
	}

	DWORD bytes_written = 0;
	if ( !WriteFile( file->stream.handle, buffer, (DWORD)buffersize, &bytes_written, NULL ) )
	{
		Self.LastError = PURE_ERROR_FILE_WRITE_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (size_t)bytes_written;
#elif IS_PLATFORM_LINUX
	if ( buffersize > INT64_MAX )
	{
		Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
		return 0;
	}

	linux_ssize_t bytes_written;
	do
	{
		bytes_written = linux_write( file->descriptor, buffer, buffersize );
	} while ( bytes_written == -1 && linux_error(0) == LINUX_EINTR );

	if ( bytes_written == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_WRITE_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (size_t)bytes_written;
#else
	#error "Not implemented!"
#endif
}

size_t
_file_readline(file_t* file, byte_t buffer[], size_t buffersize)
{
	if ( !file || !buffer || buffersize == 0 )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

	size_t total_bytes_read = 0;
	byte_t chunk[256];

	while ( total_bytes_read < buffersize - 1 )
	{
		size_t space_remaining = buffersize - 1 - total_bytes_read;
		size_t to_read = (space_remaining < sizeof(chunk)) ? space_remaining : sizeof(chunk);

		size_t bytes_read = _file_read( file, chunk, to_read );
		if ( bytes_read == 0 )
			break;

		size_t line_bytes = 0;
		size_t i = 0;
		boolean found_newline = false;

		for ( i = 0; i < bytes_read; i++ )
		{
			if ( chunk[i] == '\n' )
			{
				found_newline = true;
				break;
			}

			if ( chunk[i] != '\r' && chunk[i] != '\n' )
			{
				if ( total_bytes_read + line_bytes < buffersize - 1 )
				{
					buffer[total_bytes_read + line_bytes] = chunk[i];
					line_bytes++;
				}
			}
		}

		total_bytes_read += line_bytes;

		if ( found_newline )
		{
			size_t unconsumed = bytes_read - (i + 1);
			if ( unconsumed > 0 )
			{
				file_movecursorposition( file, -(int64_t)unconsumed );
			}
			break;
		}
	}

	buffer[total_bytes_read] = '\0';
	Self.LastError = PURE_OK;
	return total_bytes_read;
}

size_t
_file_writeline(file_t* file, byte_t buffer[], size_t buffersize)
{
	if ( !file || !buffer )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

	size_t bytes_written = _file_write( file, buffer, buffersize );

	if ( buffersize == 0 || buffer[buffersize - 1] != '\n' )
	{
		byte_t newline = '\n';
		bytes_written += _file_write( file, &newline, 1 );
	}

	Self.LastError = PURE_OK;
	return bytes_written;
}

size_t
file_getsize(file_t* file)
{
	if ( !file )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return 0;
	}

#if IS_PLATFORM_WINDOWS
	LARGE_INTEGER size;
	if ( !GetFileSizeEx( file->stream.handle, &size ) )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (size_t)size.QuadPart;
#elif IS_PLATFORM_LINUX
	int64_t current_position = linux_lseek( file->descriptor, 0, LINUX_SEEK_CUR );
	if ( current_position == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	int64_t size = linux_lseek( file->descriptor, 0, LINUX_SEEK_END );
	if ( size == -1 )
	{
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;
		return 0;
	}

	// Restore cursor position
	linux_lseek( file->descriptor, current_position, LINUX_SEEK_SET );

	Self.LastError = PURE_OK;
	return (size_t)size;
#else
	#error "Not Implemented!"
#endif
}