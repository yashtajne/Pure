#include "../../../Interface/Std/OS/Path.h"
#include "../../../Platform/Platform.def"
#include "../../../Interface/Std/Self.h"

#define path_charseq(p) (string_charseq((p)))

#if IS_PLATFORM_WINDOWS
static boolean pathname_getattributes(pathname_t pathname, DWORD* attributes)
{
	if (!pathname)
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return false;
	}

	DWORD attr = GetFileAttributesA(pathname);
	if (attr != INVALID_FILE_ATTRIBUTES)
	{
		if (attributes)
			*attributes = attr;

		Self.LastError = PURE_OK;
		return true;
	}

	DWORD win_err = GetLastError();
	if (win_err == ERROR_FILE_NOT_FOUND ||
	    win_err == ERROR_PATH_NOT_FOUND)
	{
		Self.LastError = PURE_OK;
		return false;
	}

	Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;
	return false;
}

#elif IS_PLATFORM_LINUX
static boolean pathname_getstatus(pathname_t pathname, struct stat* status)
{
	if (!pathname)
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return false;
	}

	if (linux_stat(pathname, status) == 0)
	{
		Self.LastError = PURE_OK;
		return true;
	}

	if (linux_error(0) == LINUX_ENOENT ||
	    linux_error(0) == LINUX_ENOTDIR)
	{
		Self.LastError = PURE_OK;
		return false;
	}

	Self.LastError = PURE_ERROR_FILE_ACCESS_DENIED;
	return false;
}
#endif

boolean pathname_isabsolute(pathname_t pathname)
{
	if (!pathname)
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return false;
	}

	Self.LastError = PURE_OK;

#if IS_PLATFORM_LINUX
	return pathname[0] == '/';

#elif IS_PLATFORM_WINDOWS
	return ((pathname[0] >= 'A' && pathname[0] <= 'Z') ||
	        (pathname[0] >= 'a' && pathname[0] <= 'z')) &&
	       pathname[1] == ':' &&
	       (pathname[2] == '\\' || pathname[2] == '/');

#else
	#error "Not implemented!"
#endif
}

boolean pathname_isrelative(pathname_t pathname)
{
	if (!pathname)
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return false;
	}

	return !pathname_isabsolute(pathname);
}

boolean pathname_exists(pathname_t pathname)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;
	return pathname_getattributes(pathname, &attributes);

#elif IS_PLATFORM_LINUX
	struct stat status;
	return pathname_getstatus(pathname, &status);

#else
	#error "Not implemented!"
#endif
}

boolean pathname_isdirectory(pathname_t pathname)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;

	return pathname_getattributes(pathname, &attributes) &&
	       (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

#elif IS_PLATFORM_LINUX
	struct stat status;

	return pathname_getstatus(pathname, &status) &&
	       S_ISDIR(status.st_mode);

#else
	#error "Not implemented!"
#endif
}

boolean pathname_isfile(pathname_t pathname)
{
#if IS_PLATFORM_WINDOWS
	DWORD attributes;

	return pathname_getattributes(pathname, &attributes) &&
	       (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;

#elif IS_PLATFORM_LINUX
	struct stat status;

	return pathname_getstatus(pathname, &status) &&
	       S_ISREG(status.st_mode);

#else
	#error "Not implemented!"
#endif
}