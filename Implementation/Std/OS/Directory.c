#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/Self.h"
#include "../../../Platform/Platform.def"

errcode_t
_create_directory(charseq_t pathname)
{
#if IS_PLATFORM_WINDOWS
	if ( !CreateDirectoryA( pathname, NULL ) )
		return PURE_ERROR_DIRECTORY_CREATE_FAILED;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( linux_mkdir( pathname, 0777 ) != 0 )
		return PURE_ERROR_DIRECTORY_CREATE_FAILED;
	return PURE_OK;
#else
#error "yet to implement"
#endif
}

errcode_t
_delete_directory(charseq_t pathname)
{
#if IS_PLATFORM_WINDOWS
	if ( !RemoveDirectoryA( pathname ) )
		return PURE_ERROR_DIRECTORY_DELETE_FAILED;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( linux_rmdir( pathname ) != 0 )
		return PURE_ERROR_DIRECTORY_DELETE_FAILED;
	return PURE_OK;
#else
#error "yet to implement"
#endif
}

void
create_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	Self.LastError = _create_directory(pathname);
}

void
delete_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	Self.LastError = _delete_directory( pathname );
}

errcode_t
_setcurrent_directory(pathname_t pathname)
{
	if ( !pathname )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( SetCurrentDirectoryA( pathname ) )
		return PURE_OK;

	DWORD error = GetLastError();
	switch ( error )
	{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			return PURE_ERROR_FILE_NOT_FOUND;

		case ERROR_ACCESS_DENIED:
			return PURE_ERROR_FILE_ACCESS_DENIED;

		default:
			return PURE_ERROR_DIRECTORY_SET_CURRENT_FAILED;
	}
#elif IS_PLATFORM_LINUX
	if ( linux_chdir( pathname ) == 0 )
		return PURE_OK;

	switch ( linux_errno )
	{
		case LINUX_ENOENT:
		case LINUX_ENOTDIR:
			return PURE_ERROR_FILE_NOT_FOUND;

		case LINUX_EACCES:
			return PURE_ERROR_FILE_ACCESS_DENIED;

		default:
			return PURE_ERROR_DIRECTORY_SET_CURRENT_FAILED;
	}
#else
#error "yet to implement"
#endif
}

errcode_t
_getcurrent_directory(pathnameptr_t out)
{
	if ( !out )
		return PURE_ERROR_NULL_POINTER;

	*out = NULL;

#if IS_PLATFORM_WINDOWS
	DWORD needed = GetCurrentDirectoryA( 0, NULL );
	if ( needed == 0 )
		return PURE_ERROR_DIRECTORY_GET_CURRENT_FAILED;

	charseq_t buffer = NULL;
	if ( _allocate( needed, (voidptr_t*)&buffer ) != PURE_OK )
		return PURE_ERROR_OUT_OF_MEMORY;

	DWORD written = GetCurrentDirectoryA( needed, buffer );
	if ( written == 0 || written >= needed )
	{
		_deallocate( buffer );
		return PURE_ERROR_DIRECTORY_GET_CURRENT_FAILED;
	}

	*out = buffer;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	size_t size = 256;
	while ( size <= ONE_MiB )
	{
		charseq_t buffer = NULL;
		if ( _allocate( size, (voidptr_t*)&buffer ) != PURE_OK )
			return PURE_ERROR_OUT_OF_MEMORY;

		if ( linux_getcwd( buffer, size ) != -1 )
		{
			*out = buffer;
			return PURE_OK;
		}

		int reason = linux_errno;
		_deallocate( buffer );

		if ( reason == LINUX_ENOMEM )
			return PURE_ERROR_OUT_OF_MEMORY;
		if ( reason != LINUX_ERANGE )
			return PURE_ERROR_DIRECTORY_GET_CURRENT_FAILED;

		size *= 2;
	}
	return PURE_ERROR_DIRECTORY_GET_CURRENT_FAILED;
#else
#error "yet to implement"
#endif
}

void
setcurrent_directory(pathname_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	errcode_t err = _setcurrent_directory( pathname );
	if ( err != PURE_OK )
	{
		Self.LastError = err;
		return;
	}

	pathname_t fresh = NULL;
	if ( _getcurrent_directory( &fresh ) != PURE_OK || !fresh )
	{
		uint64_t length = charseq_countbytes( pathname );
		if ( _allocate( length + 1, (voidptr_t*)&fresh ) != PURE_OK )
		{
			Self.LastError = PURE_ERROR_OUT_OF_MEMORY;
			return;
		}
		if ( memory_copy( pathname, fresh, length + 1 ) != PURE_OK )
		{
			_deallocate( fresh );
			Self.LastError = PURE_ERROR_UNKNOWN;
			return;
		}
	}

	if ( Self.CurrentDirectory )
		_deallocate( Self.CurrentDirectory );

	Self.CurrentDirectory = fresh;
	Self.LastError = PURE_OK;
}

pathname_t
getcurrent_directory(void)
{
	pathname_t cwd = NULL;
	Self.LastError = _getcurrent_directory( &cwd );
	return cwd;
}