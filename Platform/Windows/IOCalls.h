#ifndef WindowsIOCalls_h
#define WindowsIOCalls_h

#include "Kernel32.h"

#define STD_INPUT_HANDLE  ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE  ((DWORD)-12)

__declspec(dllimport)
HANDLE __stdcall GetStdHandle(
    DWORD nStdHandle
);

#endif // WindowsIOCalls_h