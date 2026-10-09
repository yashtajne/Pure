#include "../../../Interface/Std/String.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/OS/Env.h"
#include "../../../Interface/Std/OS/File.h"
#include "../../../Interface/Std/Self.h"
#include "../../../Platform/Platform.def"

charseq_t
environment_variable_get(charseq_t key)
{
	if ( !key )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return NULL;
	}

#if IS_PLATFORM_WINDOWS
	uint32_t size_of_env = GetEnvironmentVariableA( key, NULL, 0 );
	if ( !size_of_env )
	{
		Self.LastError = PURE_ERROR_ENV_VAR_NOT_FOUND;
		return NULL;
	}

	if ( size_of_env > INT16_MAX + 1 )
	{
		Self.LastError = PURE_ERROR_STRING_TOO_LONG;
		return NULL;
	}

	char* buffer = NULL;
	if ( _allocate( size_of_env, (voidptr_t*)&buffer ) )
	{
		Self.LastError = PURE_ERROR_OUT_OF_MEMORY;
		return NULL;
	}

	if ( GetEnvironmentVariableA( key, buffer, size_of_env ) != (size_of_env - 1) )
	{
		_deallocate( buffer );
		Self.LastError = PURE_ERROR_ENV_VAR_NOT_FOUND;
		return NULL;
	}

	Self.LastError = PURE_OK;
	return buffer;
#elif IS_PLATFORM_LINUX
#error "Not Implemented yet!"
#else
#error "Not Implemented yet!"
#endif
}

void
environment_variable_set(charseq_t key, charseq_t value)
{
	if ( !key || !value )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}
#if IS_PLATFORM_WINDOWS
	if ( SetEnvironmentVariableA( key, value ) )
	{
		Self.LastError = PURE_OK;
		return;
	}

	Self.LastError = PURE_ERROR_INVALID_ARGUMENT;
#elif IS_PLATFORM_LINUX
#error "Not Implemented yet!"
#else
#error "Not Implemented yet!"
#endif
}

void
environment_variable_unset(charseq_t key)
{
	if ( !key )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

#if IS_PLATFORM_WINDOWS
	if ( SetEnvironmentVariableA( key, NULL ) )
	{
		Self.LastError = PURE_OK;
		return;
	}
	Self.LastError = PURE_ERROR_ENV_VAR_NOT_FOUND;
	return;
#elif IS_PLATFORM_LINUX
#error "Not Implemented yet!"
#else
#error "Not Implemented yet!"
#endif
}

static uint64_t
process_env_line(char* buffer, uint32_t len, boolean is_load)
{
	buffer[len] = '\0';
	if ( len == 0 ) return 0;

	char* eq = NULL;
	for ( uint32_t i = 0; i < len; i++ )
	{
		if ( buffer[i] == '=' )
		{
			eq = &buffer[i];
			break;
		}
	}

	if ( eq && eq > buffer )
	{
		*eq = '\0';
		char* key = buffer;
		char* value = eq + 1;

		while ( *key == ' ' || *key == '\t' ) key++;
		while ( *value == ' ' || *value == '\t' ) value++;

		if ( *key != '\0' )
		{
			if ( is_load )
			{
				environment_variable_set( key, value );
				if ( Self.LastError == PURE_OK ) return 1;
			}
			else
			{
				environment_variable_unset( key );
				if ( Self.LastError == PURE_OK ) return 1;
			}
		}
	}
	return 0;
}

static number_t
process_env_file(charseq_t filename, boolean is_load)
{
	if ( !filename )
		return PURE_ERROR_NULL_POINTER;

	file_t* file = file_open( filename, READ_ONLY );
	if ( !file )
	{
		return PURE_ERROR_FILE_NOT_FOUND;
	}

	uint64_t count = 0;
	char buffer[1024];
	uint32_t buf_idx = 0;
	char_t ch;

	while ( (ch = file_getchar( file )) != (char_t)-1 )
	{
		if ( ch == '\n' || ch == '\r' )
		{
			count += process_env_line( buffer, buf_idx, is_load );
			buf_idx = 0;
		}
		else if ( buf_idx < sizeof(buffer) - 1 )
		{
			buffer[buf_idx++] = ch;
		}
	}

	if ( buf_idx > 0 )
	{
		count += process_env_line( buffer, buf_idx, is_load );
	}

	file_close( file );
	return (number_t)count;
}

number_t
environment_variables_load(charseq_t filename)
{
	return process_env_file( filename, true );
}

number_t
environment_variables_unload(charseq_t filename)
{
	return process_env_file( filename, false );
}