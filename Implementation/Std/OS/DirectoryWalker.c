#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/String.h"
#include "../../../Platform/Platform.def"

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
	Self.LastError = _allocate( sizeof(struct DirectoryWalker), (voidptr_t*)&walker );
	return walker;
}

void
directorywalk_start(struct DirectoryWalker* walker, pathname_t dirpath)
{
#if IS_PLATFORM_WINDOWS

	string_fromcharseq( &walker->target, dirpath );
	if ( Self.LastError != PURE_OK ) return;

	charseq_t all = "\\*";
	string_ensurecapacity( &walker->target, charseq_countbytes( all ) );
	if ( Self.LastError != PURE_OK ) return;

	string_appendcharseq( &walker->target, all );
	if ( Self.LastError != PURE_OK ) return;

	// writeln(&walker->target);
	WIN32_FIND_DATAA data = { 0 };
	HANDLE hFind = FindFirstFileA( string_charseq(&walker->target), &data );

	if (hFind == INVALID_HANDLE_VALUE)
	{
		Self.LastError = PURE_ERROR_DIRECTORY_WALK_START_FAILED;
		return;
	}

	walker->handle = hFind;
	walker->data = data;

	Self.LastError = PURE_OK;
	return;
#else
#error "Not implemented yet"
#endif
}

void
directorywalk_next(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	if ( FindNextFileA( walker->handle, &walker->data ) )
	{
		Self.LastError = PURE_OK;
		return;
	}

	DWORD error = GetLastError();
	switch ( error )
	{
		case ERROR_NO_MORE_FILES:
			Self.LastError = PURE_ERROR_DIRECTORY_WALK_FINISHED;
			return;
		default:
			Self.LastError = PURE_ERROR_DIRECTORY_WALK_NEXT_FAILED;
			return;
	}
#else
#error "Not implemented yet"
#endif
}

void
directorywalk_stop(struct DirectoryWalker* walker)
{
#if IS_PLATFORM_WINDOWS
	if ( !FindClose( walker->handle ) )
	{
		Self.LastError = PURE_ERROR_DIRECTORY_WALK_NEXT_FAILED;
		return;
	}

	if ( string_isinline(&walker->target) )
		string_free( &walker->target );
	walker->data = (WIN32_FIND_DATAA){ 0 };

	Self.LastError = PURE_OK;
	return;
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

size_t
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