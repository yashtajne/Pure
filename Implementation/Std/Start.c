#include "../../Pure.h"
#include "IO/Stream.h"

#if IS_PLATFORM_WINDOWS
#include "../../Interface/Std/Self.h"
#include "../../Interface/Std/String.h"
#include "../../Interface/Std/Charseq.h"
#include "../../Interface/Std/OS/Directory.h"

static inline stringarray_t* parse_args(void);
static inline arrayof(kvcharseq_t)* parse_envp(void);

__PURESTART int main(void);
void _start(void)
{
	Self.Arguments = parse_args();
	Self.EnviornmentVariables = parse_envp();
	Self.LastError = _getcurrent_directory( &Self.CurrentDirectory );

	HANDLE hstdin  = GetStdHandle( STD_INPUT_HANDLE );
	HANDLE hstdout = GetStdHandle( STD_OUTPUT_HANDLE );
	HANDLE hstderr = GetStdHandle( STD_ERROR_HANDLE );

	Self.Console.stdin  = &((stream_t){ .handle = hstdin });
	Self.Console.stderr = &((stream_t){ .handle = hstderr });
	Self.Console.stdout.stream = &((stream_t){ .handle = hstdout });

	int result = main();

	_deallocate( Self.EnviornmentVariables );
	ExitProcess( (unsigned int)result );
}

/*
No Error handling added for now.
*/
static inline stringarray_t* parse_args(void)
{
	char* cmd = GetCommandLineA();
	if ( !cmd ) return NULL;

	while ( *cmd == ' ' || *cmd == '\t' )
		cmd++;

	if ( *cmd == '\0' )
		return stringarray_new( 1 );

	if ( *cmd == '"' )
	{
		cmd++;
		char* start = cmd;
		while ( *cmd != '\0' && *cmd != '"' )
			cmd++;

		uint64_t len = (uint64_t)(cmd - start);
		Self.ProgramName = charseq_fromcharseqwithsize( start, len );
		if ( *cmd == '"' )
			cmd++;
	}
	else
	{
		char* start = cmd;
		while ( *cmd != '\0' && *cmd != ' ' && *cmd != '\t' )
			cmd++;

		uint64_t len = (uint64_t)(cmd - start);
		Self.ProgramName = charseq_fromcharseqwithsize( start, len );
	}

	stringarray_t* sa = stringarray_new( 1 );
	while ( *cmd != '\0' )
	{
		while ( *cmd == ' ' || *cmd == '\t' )
			cmd++;

		if ( *cmd == '\0' )
			break;

		if ( *cmd == '"' )
		{
			cmd++;
			char* start = cmd;

			while ( *cmd != '\0' && *cmd != '"' )
				cmd++;

			uint64_t len = (uint64_t)(cmd - start);
			string_t s;

			_string_fromcharseq( &s, start, (uint32_t)len );
			stringarray_append( sa, &s );

			if ( *cmd == '"' )
				cmd++;
		}
		else
		{
			char* start = cmd;
			while ( *cmd != '\0' && *cmd != ' ' && *cmd != '\t' )
				cmd++;

			uint64_t len = (uint64_t)(cmd - start);
			string_t s;

			_string_fromcharseq( &s, start, (uint32_t)len );
			stringarray_append( sa, &s );
		}
	}

	return sa;
}

static inline arrayof(kvcharseq_t)*
parse_envp(void)
{
	char* env_block = GetEnvironmentStringsA();
	if ( !env_block ) return NULL;

	arrayof(kvcharseq_t)* envarr = kvcharseqarray_new( 1 );
	char* var = env_block;

	while ( *var != '\0' )
	{
		uint64_t total_len = charseq_countbytes( var );
		kvcharseq_t pair = { 0 };

		uint64_t search_offset = (var[0] == '=') ? 1 : 0;
		uint64_t rel_pos = 0;

		if ( charseq_find( var + search_offset, "=", &rel_pos ) )
		{
			uint64_t eq_pos = search_offset + rel_pos;
			pair.key = charseq_fromcharseqwithsize( var, (uint32_t)eq_pos );
			pair.value = charseq_fromcharseq( var + eq_pos + 1 );
		}
		else
		{
			pair.key = charseq_fromcharseq( var );
			pair.value = "";
		}

		kvcharseqarray_append( envarr, &pair );
		var += total_len + 1;
	}

	FreeEnvironmentStringsA( env_block );
	return envarr;
}
#elif IS_PLATFORM_LINUX
#include "../../Interface/Std/Self.h"
#include "../../Interface/Std/OS/Directory.h"

__attribute__((used)) static void self_init_current_directory(void)
{
	Self.LastError = _getcurrent_directory( &Self.CurrentDirectory );
}

__PURESTART int main(void);
__attribute__((naked)) void _start(void)
{
	__asm__(
		"mov (%rsp), %edi\n"
		"lea 8(%rsp), %rsi\n"
		"lea 16(%rsp,%rdi,8), %rdx\n"
		"mov %rdx, Self(%rip)\n"
		"call self_init_current_directory\n"
		"call main\n"
		"mov %eax, %edi\n"
		"mov $60, %eax\n"
		"syscall\n"
		"hlt\n"
	);
}
#endif