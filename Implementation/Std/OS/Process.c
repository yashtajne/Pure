#include "../../../Interface/Std/OS/Process.h"
#include "../../../Interface/Std/Memory.h"

#include "../../../Interface/Core/ASCII.h"
#include "../../../Interface/Core/Char.h"

#include "../PlatformInc.def"

struct _Process
{
#if IS_PLATFORM_WINDOWS
	HANDLE process_handle;
	HANDLE thread_handle;
	uint32_t process_id;
	charseq_t raw_invocation;
#elif IS_PLATFORM_LINUX
	uint32_t process_id;
#else
	#error "Not yet Implemented!"
#endif

	charseq_t invocation; // Owned
	charseq_t* arguments;
	uint64_t argument_count;
	boolean has_started;
	boolean has_completed;
	uint8_t exitcode;
};

#define PROCESS_ALL_FLAGS (PROCESS_CAPTURE_OUTPUT | PROCESS_INHERIT_STDIN | PROCESS_INHERIT_STDOUT | PROCESS_INHERIT_STDERR)

static boolean process_tokenize_invocation(charseq_t invocation, charseq_t* arguments, uint64_t* argument_count)
{
	charseq_t read = invocation;
	charseq_t write = invocation;
	uint64_t count = 0;
	while ( *read != '\0' )
	{
		while ( ascii_isspace(*read) ) read++;
		if ( *read == '\0' ) break;
		if ( arguments ) arguments[count] = write;
		count++;
		char_t quote = '\0';
		while ( *read != '\0' )
		{
			char_t character = *read++;
			if ( quote != '\0' && character == quote ) { quote = '\0'; continue; }
			if ( quote == '\0' )
			{
				if ( character == '\'' || character == '"' ) { quote = character; continue; }
				if ( ascii_isspace(character) ) break;
			}
			if ( character == '\\' && *read != '\0' ) character = *read++;
			if ( arguments ) *write++ = character;
		}
		if ( quote != '\0' ) return false;
		if ( arguments ) *write++ = '\0';
	}
	if ( arguments ) arguments[count] = NULL;
	if ( argument_count ) *argument_count = count;
	return count != 0;
}

process_t* process_create(charseq_t invocation, err_t* occured)
{
	if ( !invocation )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

	uint64_t invocation_size = charseq_countbytes(invocation);
	if ( !invocation_size )
	{
		if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	voidptr_t allocation = NULL;
	err_t err = heap_allocate(sizeof(process_t), &allocation);
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	process_t* process = allocation;
	*process = (process_t){ 0 };

	err = heap_allocate(invocation_size + 1, (voidptr_t*)&process->invocation);
	if ( err )
	{
		if ( occured ) *occured = err;
		return process;
	}

	err = memory_copy(invocation, process->invocation, invocation_size + 1);
	if ( err )
	{
		if ( occured ) *occured = err;
		return process;
	}

#if IS_PLATFORM_WINDOWS
	err = heap_allocate(invocation_size + 1, (voidptr_t*)&process->raw_invocation);
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}
	for ( uint64_t i = 0; i <= invocation_size; i++ ) process->raw_invocation[i] = invocation[i];
#endif

	uint64_t argument_count = 0;
	if ( !process_tokenize_invocation(process->invocation, NULL, &argument_count) )
	{
		if ( occured ) *occured = PURE_ERROR_PROCESS_CREATE_FAILED;
		return process;
	}

	err = heap_allocate((argument_count + 1) * sizeof(charseq_t), (voidptr_t*)&process->arguments);
	if ( err )
	{
		if ( occured ) *occured = err;
		return process;
	}

	if ( !process_tokenize_invocation(process->invocation, process->arguments, &process->argument_count) )
	{
		if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
		return process;
	}

	if ( occured ) *occured = PURE_OK;
	return process;
}

err_t
process_execute(process_t* process, uint8_t flags )
{
	if ( !process || !process->invocation ) return PURE_ERROR_NULL_POINTER;
	if ( flags & ~PROCESS_ALL_FLAGS ) return PURE_ERROR_INVALID_ARGUMENT;
	if ( process->has_started ) return PURE_ERROR_INVALID_ARGUMENT;
	if ( !process->arguments || !process->arguments[0] ) return PURE_ERROR_INVALID_ARGUMENT;
#if IS_PLATFORM_WINDOWS
	// On Windows, CreateProcessA with lpApplicationName=NULL resolves bare names via PATH,
	// so we only pre-check existence when arguments[0] contains an explicit path separator.
	{
		charseq_t exe = process->arguments[0];
		boolean is_explicit_path = false;
		for ( charseq_t p = exe; *p != '\0'; p++ )
		{
			if ( *p == '/' || *p == '\\' ) { is_explicit_path = true; break; }
		}
		if ( is_explicit_path )
		{
			DWORD attrs = GetFileAttributesA(exe);
			if ( attrs == INVALID_FILE_ATTRIBUTES )
			{
				DWORD win_err = GetLastError();
				if ( win_err == ERROR_FILE_NOT_FOUND || win_err == ERROR_PATH_NOT_FOUND )
					return PURE_ERROR_FILE_NOT_FOUND;
				return PURE_ERROR_PROCESS_CREATE_FAILED;
			}
			if ( attrs & FILE_ATTRIBUTE_DIRECTORY )
				return PURE_ERROR_INVALID_ARGUMENT;
		}
	}
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(STARTUPINFOA);
	PROCESS_INFORMATION pi = { 0 };
	BOOL s = CreateProcessA(NULL, process->raw_invocation, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if ( !s )
	{
		DWORD win_err = GetLastError();
		if ( win_err == ERROR_FILE_NOT_FOUND || win_err == ERROR_PATH_NOT_FOUND )
			return PURE_ERROR_FILE_NOT_FOUND;
		return PURE_ERROR_PROCESS_CREATE_FAILED;
	}
	process->process_handle = pi.hProcess;
	process->thread_handle = pi.hThread;
	process->process_id = (uint32_t)pi.dwProcessId;
#elif IS_PLATFORM_LINUX
	// linux_execve does not search PATH, so arguments[0] must be an absolute or relative path.
	// Always check that the target file exists and is a regular file before forking.
	{
		struct stat st;
		if ( linux_stat(process->arguments[0], &st) == -1 )
		{
			if ( linux_error(0) == LINUX_ENOENT ) return PURE_ERROR_FILE_NOT_FOUND;
			return PURE_ERROR_PROCESS_CREATE_FAILED;
		}
		if ( !S_ISREG(st.st_mode) )
			return PURE_ERROR_INVALID_ARGUMENT;
	}
	linux_pid_t child_pid = linux_fork();
	if ( child_pid == 0 )
	{
		linux_execve(process->arguments[0], process->arguments, Self.enviornment );
		linux_exit(127);
	}
	if ( child_pid < 0 ) return PURE_ERROR_PROCESS_CREATE_FAILED;
	process->process_id = (uint32_t)child_pid;
#else
	return PURE_ERROR_PROCESS_CREATE_FAILED;
#endif
	process->exitcode = 0;
	process->has_started = true;
	process->has_completed = false;
	return PURE_OK;
}

err_t
process_waitforcompletion(process_t* process)
{
	if ( !process ) return PURE_ERROR_NULL_POINTER;
	if ( process->has_completed ) return PURE_OK;
	if ( !process->has_started ) return PURE_ERROR_INVALID_ARGUMENT;
#if IS_PLATFORM_WINDOWS
	if ( WaitForSingleObject(process->process_handle, INFINITE) != 0 ) return PURE_ERROR_PROCESS_WAIT_FAILED;
	DWORD exit_code = 0;
	if ( !GetExitCodeProcess(process->process_handle, &exit_code) ) return PURE_ERROR_PROCESS_WAIT_FAILED;
	process->exitcode = (uint8_t)exit_code;
#elif IS_PLATFORM_LINUX
	int32_t status;
	linux_pid_t result;
	do { result = linux_waitpid((linux_pid_t)process->process_id, &status); }
	while ( result == -1 && linux_error(0) == LINUX_EINTR );
	if ( result == -1 ) return PURE_ERROR_PROCESS_WAIT_FAILED;
	if ( LINUX_WIFEXITED(status) ) process->exitcode = (uint8_t)LINUX_WEXITSTATUS(status);
	else if ( LINUX_WIFSIGNALED(status) ) process->exitcode = (uint8_t)(128 + LINUX_WTERMSIG(status));
	else return PURE_ERROR_PROCESS_WAIT_FAILED;
#else
	return PURE_ERROR_PROCESS_WAIT_FAILED;
#endif
	process->has_completed = true;
	process->has_started = false;
#if IS_PLATFORM_WINDOWS
	if ( process->thread_handle ) { CloseHandle(process->thread_handle); process->thread_handle = NULL; }
#endif
	return PURE_OK;
}

err_t process_terminate(process_t* process)
{
	if ( !process )
		return PURE_ERROR_NULL_POINTER;

	if ( process->has_completed )
		return PURE_OK;

	if ( !process->has_started )
		return PURE_ERROR_INVALID_ARGUMENT;

#if IS_PLATFORM_WINDOWS
	if ( !TerminateProcess(process->process_handle, 1) ) return PURE_ERROR_PROCESS_WAIT_FAILED;
#elif IS_PLATFORM_LINUX
	if ( linux_kill((linux_pid_t)process->process_id, LINUX_SIGTERM) == -1 && linux_error(0) != LINUX_ESRCH ) return PURE_ERROR_PROCESS_WAIT_FAILED;
#else
#error "Not supported platform."
#endif

	return process_waitforcompletion(process);
}

uint8_t process_getexitcode(process_t* process, err_t* occured)
{
	if ( !process )
	{
		if ( occured ) *occured = PURE_ERROR_NULL_POINTER;
		return 0;
	}

	if ( !process->has_completed )
	{
		if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
		return 0;
	}

	if ( occured ) *occured = PURE_OK;
	return process->exitcode;
}

err_t process_cleanup(process_t* process)
{
	if ( !process )
		return PURE_ERROR_NULL_POINTER;

	if ( process->has_started )
	{
		err_t err = process_waitforcompletion(process);
		if ( err ) return err;
	}

#if IS_PLATFORM_WINDOWS
	if ( process->process_handle ) CloseHandle(process->process_handle);
	if ( process->raw_invocation ) heap_deallocate(process->raw_invocation);
#endif

	err_t err = heap_deallocate(process->arguments);
	if ( err ) return err;

	err = heap_deallocate(process->invocation);
	if ( err ) return err;

	return heap_deallocate(process);
}