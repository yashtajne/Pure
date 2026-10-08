#ifndef WindowsProcessCalls_h
#define WindowsProcessCalls_h

#include "Kernel32.h"

/* Environment Variable Operations */
__declspec(dllimport)
DWORD __stdcall GetEnvironmentVariableA(
	const char* lpName,
	char* lpBuffer,
	DWORD nSize
);

__declspec(dllimport)
BOOL __stdcall SetEnvironmentVariableA(
	const char* lpName,
	const char* lpValue
);


/* Process Operations */
typedef unsigned int UINT;

typedef struct _STARTUPINFOA {
	DWORD cb;
	char* lpReserved;
	char* lpDesktop;
	char* lpTitle;
	DWORD dwX;
	DWORD dwY;
	DWORD dwXSize;
	DWORD dwYSize;
	DWORD dwXCountChars;
	DWORD dwYCountChars;
	DWORD dwFillAttribute;
	DWORD dwFlags;
	unsigned short wShowWindow;
	unsigned short cbReserved2;
	unsigned char* lpReserved2;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
} STARTUPINFOA;

typedef struct _PROCESS_INFORMATION {
	HANDLE hProcess;
	HANDLE hThread;
	DWORD dwProcessId;
	DWORD dwThreadId;
} PROCESS_INFORMATION;

#define INFINITE 0xFFFFFFFF

__declspec(dllimport)
BOOL __stdcall CreateProcessA(
	const char* lpApplicationName,
	char* lpCommandLine,
	void* lpProcessAttributes,
	void* lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	void* lpEnvironment,
	const char* lpCurrentDirectory,
	STARTUPINFOA* lpStartupInfo,
	PROCESS_INFORMATION* lpProcessInformation
);

__declspec(dllimport)
DWORD __stdcall WaitForSingleObject(
	HANDLE hHandle,
	DWORD dwMilliseconds
);

__declspec(dllimport)
BOOL __stdcall GetExitCodeProcess(
	HANDLE hProcess,
	DWORD* lpExitCode
);

__declspec(dllimport)
BOOL __stdcall TerminateProcess(
    HANDLE hProcess,
    UINT uExitCode
);

#endif // WindowsProcessCalls_h