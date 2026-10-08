#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/String.h"
#include "../PlatformInc.def"

struct DirectoryWalker
{
	string_t target;

#if IS_PLATFORM_WINDOWS
	HANDLE handle;
	WIN32_FIND_DATAA data;
#else
#error "Not implemented yet"
#endif
};

struct DirectoryWalker*
directorywalker_new(void)
{
	struct DirectoryWalker* walker = NULL;
	Self.LastError = heap_allocate( sizeof(struct DirectoryWalker), (voidptr_t*)&walker );
	return walker;
}

err_t
directorywalk_start(struct DirectoryWalker* walker, pathname_t dirpath)
{
#if IS_PLATFORM_WINDOWS

	err_t err = string_fromcharseq( &walker->target, dirpath );
	if ( err ) return err;

	charseq_t all = "\\*";
	err = string_ensurecapacity( &walker->target, charseq_countbytes( all ) );
	if ( err ) return err;

	err = string_appendcharseq( &walker->target, all );
	if ( err ) return err;

	// writeln(&walker->target);
	WIN32_FIND_DATAA data = { 0 };
	HANDLE hFind = FindFirstFileA( string_charseq(&walker->target), &data );

	if (hFind == INVALID_HANDLE_VALUE)
		return PURE_ERROR_DIRECTORY_WALK_START_FAILED;

	walker->handle = hFind;
	walker->data = data;

	return PURE_OK;
#else
#error "Not implemented yet"
#endif
}

err_t
directorywalk_next(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	if ( FindNextFileA( walker->handle, &walker->data ) )
		return PURE_OK;

	DWORD error = GetLastError();
	switch ( error )
	{
		case ERROR_NO_MORE_FILES:
			return PURE_ERROR_DIRECTORY_WALK_FINISHED;
		default:
			return PURE_ERROR_DIRECTORY_WALK_NEXT_FAILED;
	}
#else
#error "Not implemented yet"
#endif
}

err_t
directorywalk_stop(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	if ( !FindClose( walker->handle ) )
		return PURE_ERROR_DIRECTORY_WALK_NEXT_FAILED;

	if ( string_isinline(&walker->target) )
		string_free( &walker->target );
	walker->data = (WIN32_FIND_DATAA){ 0 };

	return PURE_OK;
#else
#error "Not implemented yet"
#endif
}

charseq_t
directorywalker_getfilename(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	return walker->data.cFileName;
#else
#error "Not implemented yet"
#endif
}

uint64_t
directorywalker_getfilesize(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	return ((uint64_t)walker->data.nFileSizeHigh << 32) |
			(uint64_t)walker->data.nFileSizeLow;
#else
#error "Not implemented yet"
#endif
}

boolean
directorywalker_isdirectory(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	return (walker->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
#error "Not implemented yet"
#endif
}