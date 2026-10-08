#ifndef WindowsKernelCalls_h
#define WindowsKernelCalls_h

typedef void *LPVOID;
typedef char* LPCH;
typedef short SHORT;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned __int8  BOOL;
typedef unsigned __int64 SIZE_T;

#define TRUE  1
#define FALSE 0

typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)(__int64)-1)
// #define NULL ((void *)0)

#define MEM_COMMIT       0x1000
#define MEM_RESERVE      0x2000
#define MEM_RELEASE      0x8000
#define PAGE_READWRITE   0x04

__declspec(dllimport)
void __stdcall ExitProcess(unsigned int uExitCode);

__declspec(dllimport)
char* __stdcall GetCommandLineA(void);

__declspec(dllimport)
LPVOID __stdcall VirtualAlloc(
    LPVOID address,
    SIZE_T size,
    DWORD allocation_type,
    DWORD protection
);

__declspec(dllimport)
BOOL __stdcall VirtualFree(
    LPVOID address,
    SIZE_T size,
    DWORD free_type
);

__declspec(dllimport)
LPCH __stdcall GetEnvironmentStringsA(void);

__declspec(dllimport)
BOOL __stdcall FreeEnvironmentStringsA(
    LPCH penv
);

#endif // WindowsKernelCalls_h