#include "../../../Interface/Std/OS/Path.h"
#include "../PlatformInc.def"

#define path_charseq(p) (string_charseq((p)))

#if IS_PLATFORM_WINDOWS
static boolean pathname_getattributes(pathname_t pathname, DWORD* attributes, err_t* occured)
{
	if ( !pathname )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return false;
	}

	DWORD attr = GetFileAttributesA( pathname );
	if ( attr != INVALID_FILE_ATTRIBUTES )
	{
		if ( attributes ) *attributes = attr;
		if ( occured ) *occured = PURE_OK;
		return true;
	}

	DWORD win_err = GetLastError();
	if ( win_err == ERROR_FILE_NOT_FOUND || win_err == ERROR_PATH_NOT_FOUND )
	{
		if ( occured ) *occured = PURE_OK;
		return false;
	}

	if ( occured ) *occured = PURE_ERROR_FILE_ACCESS_DENIED;
	return false;
}
#elif IS_PLATFORM_LINUX
static boolean pathname_getstatus(pathname_t pathname, struct stat* status, err_t* occured)
{
	if ( !pathname )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return false;
	}

	if ( linux_stat( pathname, status ) == 0 )
	{
		if ( occured ) *occured = PURE_OK;
		return true;
	}
	if ( linux_error(0) == LINUX_ENOENT || linux_error(0) == LINUX_ENOTDIR )
	{
		if ( occured ) *occured = PURE_OK;
		return false;
	}
	if ( occured ) *occured = PURE_ERROR_FILE_ACCESS_DENIED;
	return false;
}
#endif

/*
pathname_t
pathname_getCurrentWorkingDirectory(err_t* occured)
{
#if IS_PLATFORM_WINDOWS
	DWORD capacity = GetCurrentDirectoryA( 0, NULL );
	if ( !capacity )
	{
		if ( occured ) *occured = PURE_ERROR_FILE_READ_FAILED;
		return NULL;
	}

	pathname_t allocated = NULL;
	err_t err = heap_allocate( capacity, (voidptr_t*)&allocated );
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	DWORD written = GetCurrentDirectoryA( capacity, allocated );
	if ( !written || written >= capacity )
	{
		if ( occured ) *occured = PURE_ERROR_FILE_READ_FAILED;
		return NULL;
	}

	if ( occured ) *occured = PURE_OK;
	return allocated;
#elif IS_PLATFORM_LINUX
	uint64_t capacity = 64;
	pathname_t allocated = NULL;

	err_t err = heap_allocate(capacity, (voidptr_t*)&allocated);
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	};

	while ( !linux_getcwd(allocated, capacity) )
	{
		if ( linux_error(0) != LINUX_ERANGE )
		{
			heap_deallocate(allocated);
			if ( occured ) *occured = PURE_ERROR_FILE_READ_FAILED;
		}

		if ( capacity > UINT64_MAX / 2 )
		{
			heap_deallocate(allocated);
			if ( occured ) *occured = PURE_ERROR_OUT_OF_MEMORY;
		}

		capacity *= 2;
		voidptr_t reallocated = NULL;

		err = heap_reallocate(allocated, capacity, &reallocated);
		if ( err )
		{
			heap_deallocate(allocated);
			if ( occured ) *occured = err;
		}

		allocated = reallocated;
	}
	if ( occured ) *occured = PURE_OK;
	return allocated;
#else
	#error "Not implemented!"
#endif
}
*/

boolean pathname_isabsolute(pathname_t pathname, err_t* occured)
{
	if ( !pathname )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return false;
	}
	if ( occured ) *occured = PURE_OK;
#if IS_PLATFORM_LINUX
	return pathname[0] == '/';
#elif IS_PLATFORM_WINDOWS
	return ((pathname[0] >= 'A' && pathname[0] <= 'Z') || (pathname[0] >= 'a' && pathname[0] <= 'z')) && pathname[1] == ':' && (pathname[2] == '\\' || pathname[2] == '/');
#else
	#error "Not implemented!"
#endif
}

boolean pathname_isrelative(pathname_t pathname, err_t* occured)
{
	if ( !pathname )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return false;
	}
	if ( occured ) *occured = PURE_OK;
	return !pathname_isabsolute(pathname, NULL);
}

boolean pathname_exists(pathname_t pathname, err_t* occured)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;
	return pathname_getattributes(pathname, &attributes, occured);
#elif IS_PLATFORM_LINUX
	struct stat status;
	return pathname_getstatus(pathname, &status, occured);
#else
	#error "Not implemented!"
#endif
}

boolean pathname_isdirectory(pathname_t pathname, err_t* occured)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;
	return pathname_getattributes(pathname, &attributes, occured) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
#elif IS_PLATFORM_LINUX
	struct stat status;
	return pathname_getStatus(pathname, &status, occured) && S_ISDIR(status.st_mode);
#else
	#error "Not implemented!"
#endif
}

boolean pathname_isfile(pathname_t pathname, err_t* occured)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;
	return pathname_getattributes(pathname, &attributes, occured) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
#elif IS_PLATFORM_LINUX
	struct stat status;
	return pathname_getStatus(pathname, &status, occured) && S_ISREG(status.st_mode);
#else
	#error "Not implemented!"
#endif
}