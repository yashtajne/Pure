#include "../../../Interface/Std/OS/Process.h"
#include "../../../Interface/Std/Charseq.h"
#include "../../../Interface/Std/IO.h"
#include "../../../Platform/Platform.def"

#if IS_PLATFORM_LINUX
#include "../../../Platform/LinuxKernel.h"
#endif

struct _Process
{
#if IS_PLATFORM_WINDOWS
	HANDLE processhandle;
	HANDLE threadhandle;
#elif IS_PLATFORM_LINUX
	linux_pid_t process_id;
#else
#error "Not implemented yet!"
#endif
	uint8_t exitcode;
};

void
process_terminate(process_t* process)
{
	Self.LastError = _process_terminate( process );
}

void
process_waitforcompletion(process_t* process)
{
	Self.LastError = _process_waitforcompletion( process );
}

void
process_cleanup(process_t* process)
{
	Self.LastError = _process_cleanup( process );
}

/*
 * Parse a command invocation into argv.
 *
 * The invocation buffer is modified in place.
 * This parser splits on whitespace and does not support
 * quoted arguments or escaped whitespace.
 */
static inline charseq_t*
parseinvocation(charseq_t invocation)
{
	size_t count = 0;

	for ( charseq_t p = invocation; *p != '\0'; )
	{
		while ( *p != '\0' && isspace( *p ) )
			++p;

		if ( *p == '\0' )
			break;

		++count;

		while ( *p != '\0' && !isspace( *p ) )
			++p;
	}

	charseq_t* argv = allocate( (count + 1) * sizeof(charseq_t) );
	if ( argv == NULL ) return NULL;

	charseq_t p = invocation;
	size_t index = 0;

	while ( *p != '\0' )
	{
		while ( *p != '\0' && isspace( *p ) )
		{
			*p = '\0';
			++p;
		}

		if ( *p == '\0' )
			break;

		argv[index++] = p;

		while ( *p != '\0' && !isspace( *p ) )
			++p;

		if ( *p != '\0' )
		{
			*p = '\0';
			++p;
		}
	}

	argv[index] = NULL;

	return argv;
}

uint8_t
process_getexitcode(process_t* process)
{
	if ( process == NULL )
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return 0;
	}

#if IS_PLATFORM_WINDOWS

	DWORD exitCode = 0;

	if (!GetExitCodeProcess(process->processhandle, &exitCode))
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return 0;
	}

	Self.LastError = PURE_OK;
	return (uint8_t)exitCode;

#elif IS_PLATFORM_LINUX

	/*
	 * linux_waitpid() currently performs a blocking wait.
	 * Cache the result so the process is not waited on twice.
	 */
	if ( Self.LastError != PURE_OK )
		return 0;

	return process->exitcode;

#else
#error "Not implemented yet!"
#endif
}

errcode_t
_process_new(process_t* process, charseq_t invocation)
{
#if IS_PLATFORM_WINDOWS

	PROCESS_INFORMATION processInfo = { 0 };

	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);

	BOOL issuccess = CreateProcessA(
		NULL, invocation,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&processInfo
	);

	if ( !issuccess )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	if ( process == NULL )
	{
		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
		return PURE_ERROR_NULL_POINTER;
	}

	process->processhandle = processInfo.hProcess;
	process->threadhandle = processInfo.hThread;

	return PURE_OK;

#elif IS_PLATFORM_LINUX

	if ( process == NULL )
		return PURE_ERROR_NULL_POINTER;

	if ( invocation == NULL || *invocation == '\0' )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	/*
	 * Parse the invocation before fork().
	 * argv points into the caller-provided invocation buffer.
	 */
	charseq_t* argv = parseinvocation( invocation );

	if ( argv == NULL )
		return PURE_ERROR_OUT_OF_MEMORY;

	if ( argv[0] == NULL )
	{
		_deallocate(argv);
		return PURE_ERROR_PROCESS_WAIT_FAILED;
	}

	linux_pid_t pid = linux_fork();

	if ( pid == -1 )
	{
		_deallocate(argv);

		return PURE_ERROR_PROCESS_WAIT_FAILED;
	}

	if ( pid == 0 )
	{
		/*
		 * Child process.
		 *
		 * execve requires a pathname. argv[0] must therefore
		 * contain an explicit executable path, such as:
		 *
		 *     /bin/ls -l
		 *
		 * No PATH lookup is performed here.
		 */
		linux_execve(argv[0], argv, NULL);

		/*
		 * execve returns only when it fails.
		 * Do not return into the caller's code in the child.
		 */
		linux_exit(127);
	}

	/*
	 * Parent process.
	 *
	 * After fork(), the child has its own address space, so the
	 * parent's argv allocation can be released independently.
	 */
	_deallocate(argv);

	process->process_id = pid;

	return PURE_OK;

#else
#error "Not implemented yet!"
#endif
}

errcode_t
_process_terminate(process_t* process)
{
	if ( process == NULL )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

#if IS_PLATFORM_WINDOWS

	if ( !TerminateProcess( process->processhandle, 1 ) )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

#elif IS_PLATFORM_LINUX

	if ( linux_kill( process->process_id, LINUX_SIGTERM ) == -1 )
	{
		/*
		 * ESRCH means the process no longer exists.
		 * Attempt to collect its status below in that case.
		 */
		if ( linux_error(0) != LINUX_ESRCH )
			return PURE_ERROR_PROCESS_WAIT_FAILED;
	}

#else
#error "Not implemented yet!"
#endif

	return _process_waitforcompletion( process );
}

errcode_t
_process_waitforcompletion(process_t* process)
{
	if (process == NULL)
		return PURE_ERROR_PROCESS_WAIT_FAILED;

#if IS_PLATFORM_WINDOWS

	if ( WaitForSingleObject( process->processhandle, INFINITE ) != 0 )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	DWORD exitCode = 0;

	if ( !GetExitCodeProcess( process->processhandle, &exitCode ) )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	return PURE_OK;

#elif IS_PLATFORM_LINUX

	int status = 0;
	linux_pid_t result;

	/*
	 * waitpid can be interrupted by a signal.
	 * Retry when the syscall reports EINTR.
	 */
	do
	{
		result = linux_waitpid( process->process_id, &status );
	}
	while ( result == -1 && linux_error(0) == LINUX_EINTR );

	if ( result == -1 )
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	if (result != process->process_id)
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	if ( LINUX_WIFEXITED(status) )
	{
		process->exitcode = (uint8_t)LINUX_WEXITSTATUS(status);
	}
	else if ( LINUX_WIFSIGNALED(status) )
	{
		/*
		 * Conventional shell-style exit status for a process
		 * terminated by a signal.
		 */
		process->exitcode =
			(uint8_t)(128 + LINUX_WTERMSIG(status));
	}
	else
		return PURE_ERROR_PROCESS_WAIT_FAILED;

	return PURE_OK;

#else
#error "Not implemented yet!"
#endif
}

errcode_t
_process_cleanup(process_t* process)
{
	if ( process == NULL )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( process->processhandle )
		CloseHandle( process->processhandle );

	if ( process->threadhandle )
		CloseHandle( process->threadhandle );
#elif IS_PLATFORM_LINUX
	/*
	 * No kernel handle needs to be closed for a PID.
	 * The caller should wait for a running child before cleanup
	 * if it needs to collect its exit status.
	 */
#endif

	return _deallocate(process);
}

/* ~ ~ Non-underscore wrappers (set Self.LastError, cf. allocate/deallocate) ~ ~ */

process_t*
process_new(charseq_t invocation)
{
	process_t* process = allocate( sizeof(struct _Process) );
	if ( process == NULL )
		return NULL;

	Self.LastError = _process_new( process, invocation );
	if ( Self.LastError != PURE_OK )
	{
		deallocate( process );
		return NULL;
	}

	return process;
}