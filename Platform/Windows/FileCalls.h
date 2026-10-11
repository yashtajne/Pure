#ifndef WindowsFileCalls_h
#define WindowsFileCalls_h

#include "Kernel32.h"

typedef char CHAR;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		long HighPart;
	} DUMMYSTRUCTNAME;
	struct {
		DWORD LowPart;
		long HighPart;
	} u;
	long long QuadPart;
} LARGE_INTEGER;

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME;

typedef struct _WIN32_FIND_DATAA {
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	CHAR cFileName[260];
	CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA;

/* File access & creation flags */
#define GENERIC_READ    0x80000000L
#define GENERIC_WRITE   0x40000000L

#define FILE_SHARE_READ   0x00000001
#define FILE_SHARE_WRITE  0x00000002
#define FILE_SHARE_DELETE 0x00000004

#define CREATE_NEW        1
#define CREATE_ALWAYS     2
#define OPEN_EXISTING     3
#define OPEN_ALWAYS       4
#define TRUNCATE_EXISTING 5

#define FILE_ATTRIBUTE_NORMAL    0x00000080
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010
#define INVALID_FILE_ATTRIBUTES  ((DWORD)-1)

#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

#define ERROR_FILE_NOT_FOUND 2L
#define ERROR_PATH_NOT_FOUND 3L
#define ERROR_ACCESS_DENIED  5L
#define ERROR_INVALID_HANDLE 6L
#define ERROR_ALREADY_EXISTS 183L
#define ERROR_FILE_EXISTS 80
#define ERROR_NO_MORE_FILES 18L

/* File Operations */
__declspec(dllimport)
HANDLE __stdcall CreateFileA(
	const char* lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	void* lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);

__declspec(dllimport)
BOOL __stdcall CloseHandle(
	HANDLE hObject
);

__declspec(dllimport)
BOOL __stdcall ReadFile(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	DWORD* lpNumberOfBytesRead,
	void* lpOverlapped
);

__declspec(dllimport)
BOOL __stdcall WriteFile(
	HANDLE hFile,
	const void* lpBuffer,
	DWORD nNumberOfBytesToWrite,
	DWORD* lpNumberOfBytesWritten,
	void* lpOverlapped
);

__declspec(dllimport)
BOOL __stdcall SetFilePointerEx(
	HANDLE hFile,
	LARGE_INTEGER liDistanceToMove,
	LARGE_INTEGER* lpNewFilePointer,
	DWORD dwMoveMethod
);

__declspec(dllimport)
BOOL __stdcall GetFileSizeEx(
	HANDLE hFile,
	LARGE_INTEGER* lpFileSize
);

__declspec(dllimport)
DWORD __stdcall GetLastError(void);

/* Directory Operations */
__declspec(dllimport)
BOOL __stdcall CreateDirectoryA(
	const char* lpPathName,
	void* lpSecurityAttributes
);

__declspec(dllimport)
BOOL __stdcall RemoveDirectoryA(
	const char* lpPathName
);

__declspec(dllimport)
BOOL __stdcall DeleteFileA(
	const char* lpFileName
);

__declspec(dllimport)
HANDLE __stdcall FindFirstFileA(
	const char* lpFileName,
	WIN32_FIND_DATAA* lpFindFileData
);

__declspec(dllimport)
BOOL __stdcall FindNextFileA(
	HANDLE hFindFile,
	WIN32_FIND_DATAA* lpFindFileData
);

__declspec(dllimport)
BOOL __stdcall FindClose(
	HANDLE hFindFile
);

/* Path Operations */
__declspec(dllimport)
DWORD __stdcall GetFileAttributesA(
	const char* lpFileName
);

__declspec(dllimport)
BOOL __stdcall SetCurrentDirectoryA(
	const char* lpPathName
);

__declspec(dllimport)
DWORD __stdcall GetCurrentDirectoryA(
	DWORD nBufferLength,
	char* lpBuffer
);

#endif // WindowsFileCalls_h