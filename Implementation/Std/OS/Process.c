#include "../../../Interface/Std/OS/Process.h"
#include "../../../Interface/Std/Charseq.h"
#include "../../../Interface/Std/IO.h"
#include "../../../Platform/Platform.def"

struct _Process
{
#if IS_PLATFORM_WINDOWS
	HANDLE processhandle;
	HANDLE threadhandle;
#elif IS_PLATFORM_LINUX
	uint32_t process_id;
#else
#error "Not implemented yet!"
#endif
};

uint8_t process_getexitcode(process_t* process)
{
#if IS_PLATFORM_WINDOWS
	DWORD exitCode = 0;
	if ( !GetExitCodeProcess( process->processhandle, &exitCode ) )
		Self.LastError = PURE_ERROR_FILE_READ_FAILED;

	return exitCode;
#elif IS_PLATFORM_LINUX
#else
#error "Not implemented yet!"
#endif
}

static inline arrayof(charseq_t)* parseinvocation(charseq_t invocation)
{
	size_t count = 0;
	for ( charseq_t p = invocation; *p != '\0'; )
	{
		while ( *p != '\0' && isspace( *p ) ) // skip whitespaces
			++p;

		if ( *p == '\0' ) // break when end of invocation
			break;

		++count;

		while ( *p != '\0' && !isspace( *p ) ) // skip characters
			++p;
	}

	arrayof(charseq_t)* argv = charseqarray_new( count + 1 );
	if ( Self.LastError != PURE_OK ) return NULL;

	charseq_t p = invocation;
	while ( *p != '\0' )
	{
		while ( *p != '\0' && isspace(*p) ) // skip whitespaces again
		{
			*p = '\0';
			++p;
		}

		if ( *p == '\0' ) // break when end of invocation
			break;

		charseqarray_append( argv, &p ); // append character sequence to argv

		while ( *p != '\0' && !isspace( *p ) ) // skip characters
			++p;

		if ( *p != '\0' ) // add null terminator at the end of character sequence
		{
			*p = '\0';
			++p;
		}
	}

	charseq_t sentinel = NULL;
	charseqarray_append( argv, &sentinel ); // null for last element
	return argv;
}

process_t*
process_new(charseq_t invocation)
{
	process_t* process = NULL;

#if IS_PLATFORM_WINDOWS
	PROCESS_INFORMATION processInfo = { 0 };

	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);

	BOOL issuccess = CreateProcessA(
		NULL, invocation,  // lpCommandLine: mutable command-line string
		NULL,              // process security attributes
		NULL,              // thread security attributes
		FALSE,             // inherit handles?
		0,                 // creation flags
		NULL,              // environment (NULL = inherit parent's)
		NULL,              // working directory (NULL = inherit parent's)
		&startupInfo,
		&processInfo
	);

	if ( !issuccess )
	{
		// yet to set the error value to Self.LastError
		return NULL;
	}

	process = allocate( sizeof(struct _Process) );
	*process = (process_t){
		.processhandle = processInfo.hProcess,
		.threadhandle = processInfo.hThread
	};

	Self.LastError = PURE_OK;
	return process;
#elif IS_PLATFORM_LINUX
	arrayof(charseq_t)* argv = parseinvocation( invocation );
#else
#error "Not Implemented yet!"
#endif
}

void process_terminate(process_t* process)
{
#if IS_PLATFORM_WINDOWS
	if ( !TerminateProcess(process->processhandle, 1) )
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}
#elif IS_PLATFORM_LINUX
	if ( linux_kill((linux_pid_t)process->process_id, LINUX_SIGTERM) == -1 && linux_error(0) != LINUX_ESRCH )
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}
#else
#error "Not supported platform."
#endif

	return process_waitforcompletion(process);
}

void
process_waitforcompletion(process_t* process)
{
#if IS_PLATFORM_WINDOWS
	if ( WaitForSingleObject( process->processhandle, INFINITE ) != 0 )
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	DWORD exitCode = 0;
	if ( !GetExitCodeProcess( process->processhandle, &exitCode ) )
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;

#elif IS_PLATFORM_LINUX
#error "Not implemented yet"
#else
#error "Not implemented yet"
#endif
}

void process_cleanup(process_t* process)
{
#if IS_PLATFORM_WINDOWS
	if ( process->processhandle ) CloseHandle( process->processhandle );
	if ( process->threadhandle ) CloseHandle( process->threadhandle );
#endif
	return deallocate( process );
}