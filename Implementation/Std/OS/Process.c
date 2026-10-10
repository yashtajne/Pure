#include "../../../Interface/Std/OS/Process.h"
#include "../../../Interface/Std/Charseq.h"
#include "../../../Interface/Std/IO.h"
#include "../../../Platform/Platform.def"

struct _Process
{
#if IS_PLATFORM_WINDOWS
	HANDLE process_handle;
	HANDLE thread_handle;
#elif IS_PLATFORM_LINUX
	uint32_t pid;
#else
#error "Not implemented yet!"
#endif
};

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
process_create(charseq_t invocation)
{
	process_t process = { 0 };

	arrayof(charseq_t)* argv = parseinvocation( invocation );
	for ( size_t i = 0; i < (argv->length - 1); i++ )
	{
		_writecharseq( argv->elements[i] );
		_write( "\n", 1 );
	}

	return NULL;
}