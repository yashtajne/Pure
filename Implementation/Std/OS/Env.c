#include "../../../Interface/Std/OS/Env.h"
#include "../../../Interface/Std/String.h"
#include "../../../Interface/Std/Memory.h"
#include "../../../Interface/Std/OS/File.h"
#include "../../../Interface/Std/Self.h"
#include "../PlatformInc.def"

err_t
environment_variable_get(charseq_t key, string_t* out_string)
{
	if ( !key || !out_string )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	uint32_t size_of_env = GetEnvironmentVariableA( key, NULL, 0 );
	if ( !size_of_env )
		return PURE_ERROR_ENV_VAR_NOT_FOUND;

	if ( size_of_env > INT16_MAX + 1 )
		return PURE_ERROR_STRING_TOO_LONG;

	char* buffer = NULL;
	if ( heap_allocate( size_of_env, (voidptr_t*)&buffer ) )
		return PURE_ERROR_OUT_OF_MEMORY;

	if ( GetEnvironmentVariableA( key, buffer, size_of_env ) != (size_of_env - 1) )
	{
		heap_deallocate( buffer );
		return PURE_ERROR_ENV_VAR_NOT_FOUND;
	}

	err_t res = _string_fromcharseq( out_string, buffer, (size_of_env - 1) );
	heap_deallocate( buffer );
	return res;
#elif IS_PLATFORM_LINUX
	char* val = getenv( key );
	if ( !val )
		return PURE_ERROR_ENV_VAR_NOT_FOUND;

	uint64_t len = charseq_countbytes( val );
	if ( len > UINT32_MAX )
		return PURE_ERROR_STRING_TOO_LONG;

	return string_fromcharseqwithsize( out_string, val, (uint32_t)len );
#else
	#error "Not Implemented yet!"
#endif
}

err_t
environment_variable_set(charseq_t key, charseq_t value)
{
	if ( !key || !value )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( SetEnvironmentVariableA( key, value ) )
		return PURE_OK;
	return PURE_ERROR_INVALID_ARGUMENT;
#elif IS_PLATFORM_LINUX
	if ( setenv( key, value, 1 ) == 0 )
		return PURE_OK;
	return PURE_ERROR_INVALID_ARGUMENT;
#else
	#error "Not Implemented yet!"
#endif
}

err_t
environment_variable_unset(charseq_t key)
{
	if ( !key )
		return PURE_ERROR_NULL_POINTER;

#if IS_PLATFORM_WINDOWS
	if ( SetEnvironmentVariableA( key, NULL ) )
		return PURE_OK;
	return PURE_ERROR_ENV_VAR_NOT_FOUND;
#elif IS_PLATFORM_LINUX
	if ( unsetenv( key ) == 0 )
		return PURE_OK;
	return PURE_ERROR_ENV_VAR_NOT_FOUND;
#else
	#error "Not Implemented yet!"
#endif
}

static void
process_env_line(char* buffer, uint32_t len, boolean is_load, uint64_t* out_count)
{
	buffer[len] = '\0';
	if ( len == 0 ) return;

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
				if ( environment_variable_set( key, value ) == PURE_OK )
				{
					if ( out_count ) (*out_count)++;
				}
			}
			else
			{
				if ( environment_variable_unset( key ) == PURE_OK )
				{
					if ( out_count ) (*out_count)++;
				}
			}
		}
	}
}

static err_t
process_env_file(charseq_t filename, boolean is_load, uint64_t* out_count)
{
	if ( !filename )
		return PURE_ERROR_NULL_POINTER;

	err_t err = PURE_OK;
	file_t* file = file_open( filename, READ_ONLY );
	if ( err != PURE_OK || !file )
	{
		if ( out_count ) *out_count = 0;
		return err != PURE_OK ? err : PURE_ERROR_FILE_NOT_FOUND;
	}

	uint64_t count = 0;
	char buffer[1024];
	uint32_t buf_idx = 0;
	char_t ch;

	while ( (ch = file_getchar( file )) != (char_t)-1 && Self.LastError == PURE_OK )
	{
		if ( ch == '\n' || ch == '\r' )
		{
			process_env_line( buffer, buf_idx, is_load, &count );
			buf_idx = 0;
		}
		else if ( buf_idx < sizeof(buffer) - 1 )
		{
			buffer[buf_idx++] = ch;
		}
	}

	if ( buf_idx > 0 )
	{
		process_env_line( buffer, buf_idx, is_load, &count );
	}

	file_close( file );
	if ( out_count ) *out_count = count;
	return PURE_OK;
}

err_t
environment_variables_load(charseq_t filename, uint64_t* out_loaded_count)
{
	if ( !out_loaded_count )
		return PURE_ERROR_NULL_POINTER;
	return process_env_file( filename, true, out_loaded_count );
}

err_t
environment_variables_unload(charseq_t filename)
{
	return process_env_file( filename, false, NULL );
}