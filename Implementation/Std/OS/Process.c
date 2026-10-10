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
};

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

	for (charseq_t p = invocation; *p != '\0';)
	{
		while (*p != '\0' && isspace(*p))
			++p;

		if (*p == '\0')
			break;

		++count;

		while (*p != '\0' && !isspace(*p))
			++p;
	}

	charseq_t* argv = allocate((count + 1) * sizeof(charseq_t));

	if (argv == NULL || Self.LastError != PURE_OK)
		return NULL;

	charseq_t p = invocation;
	size_t index = 0;

	while (*p != '\0')
	{
		while (*p != '\0' && isspace(*p))
		{
			*p = '\0';
			++p;
		}

		if (*p == '\0')
			break;

		argv[index++] = p;

		while (*p != '\0' && !isspace(*p))
			++p;

		if (*p != '\0')
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
	if (process == NULL)
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

	return process->exit_code;

#else
#error "Not implemented yet!"
#endif
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

	if (!issuccess)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return NULL;
	}

	process = allocate(sizeof(struct _Process));

	if (process == NULL || Self.LastError != PURE_OK)
	{
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return NULL;
	}

	*process = (process_t){
		.processhandle = processInfo.hProcess,
		.threadhandle = processInfo.hThread
	};

	Self.LastError = PURE_OK;
	return process;

#elif IS_PLATFORM_LINUX

	if (invocation == NULL || *invocation == '\0')
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return NULL;
	}

	/*
	 * Parse the invocation before fork().
	 * argv points into the caller-provided invocation buffer.
	 */
	charseq_t* argv = parseinvocation(invocation);

	if (argv == NULL)
		return NULL;

	if (argv[0] == NULL)
	{
		deallocate(argv);
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return NULL;
	}

	/*
	 * Allocate the process object before creating the child,
	 * avoiding an allocation failure after a child is running.
	 */
	process = allocate(sizeof(struct _Process));

	if (process == NULL || Self.LastError != PURE_OK)
	{
		deallocate(argv);
		return NULL;
	}

	linux_pid_t pid = linux_fork();

	if (pid == -1)
	{
		deallocate(argv);
		deallocate(process);

		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return NULL;
	}

	if (pid == 0)
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
	deallocate(argv);

	*process = (struct _Process){ .process_id = pid };

	Self.LastError = PURE_OK;
	return process;

#else
#error "Not implemented yet!"
#endif
}

void
process_terminate(process_t* process)
{
	if (process == NULL)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

#if IS_PLATFORM_WINDOWS

	if (!TerminateProcess(process->processhandle, 1))
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

#elif IS_PLATFORM_LINUX

	if (linux_kill(process->process_id, LINUX_SIGTERM) == -1)
	{
		/*
		 * ESRCH means the process no longer exists.
		 * Attempt to collect its status below in that case.
		 */
		if ( linux_error(0) != LINUX_ESRCH )
		{
			Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
			return;
		}
	}

#else
#error "Not implemented yet!"
#endif

	process_waitforcompletion(process);
}

void
process_waitforcompletion(process_t* process)
{
	if (process == NULL)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

#if IS_PLATFORM_WINDOWS

	if (WaitForSingleObject(process->processhandle, INFINITE) != 0)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	DWORD exitCode = 0;

	if (!GetExitCodeProcess(process->processhandle, &exitCode))
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	Self.LastError = PURE_OK;

#elif IS_PLATFORM_LINUX

	int status = 0;
	linux_pid_t result;

	/*
	 * waitpid can be interrupted by a signal.
	 * Retry when the syscall reports EINTR.
	 */
	do
	{
		result = linux_waitpid(process->process_id, &status);
	}
	while (result == -1 && linux_error(0) == LINUX_EINTR);

	if (result == -1)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	if (result != process->process_id)
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	if (LINUX_WIFEXITED(status))
	{
		process->exit_code = (uint8_t)LINUX_WEXITSTATUS(status);
	}
	else if (LINUX_WIFSIGNALED(status))
	{
		/*
		 * Conventional shell-style exit status for a process
		 * terminated by a signal.
		 */
		process->exit_code =
			(uint8_t)(128 + LINUX_WTERMSIG(status));
	}
	else
	{
		Self.LastError = PURE_ERROR_PROCESS_WAIT_FAILED;
		return;
	}

	Self.LastError = PURE_OK;

#else
#error "Not implemented yet!"
#endif
}

void
process_cleanup(process_t* process)
{
	if ( process == NULL )
		return;

#if IS_PLATFORM_WINDOWS
	if (process->processhandle)
		CloseHandle(process->processhandle);

	if (process->threadhandle)
		CloseHandle(process->threadhandle);
#elif IS_PLATFORM_LINUX
	/*
	 * No kernel handle needs to be closed for a PID.
	 * The caller should wait for a running child before cleanup
	 * if it needs to collect its exit status.
	 */
#endif

	deallocate(process);
}