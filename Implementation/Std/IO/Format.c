/*

#include "../../../Interface/Core/Char.h"
#include "../../../Interface/Core/ASCII.h"
#include "../../../Interface/Std/String.h"
#include "../../../Interface/Std/IO.h"
#include "../PlatformInc.def"

static inline uintptr_t _fmt_word_ptr   (const void* p)          { return (uintptr_t)p; }
static inline uintptr_t _fmt_word_i32   (int v)                  { return (uintptr_t)(int64_t)v; }
static inline uintptr_t _fmt_word_u32   (unsigned int v)         { return (uintptr_t)v; }
static inline uintptr_t _fmt_word_long  (long v)                 { return (uintptr_t)(int64_t)v; }
static inline uintptr_t _fmt_word_ulong (unsigned long v)        { return (uintptr_t)v; }
static inline uintptr_t _fmt_word_i64   (long long v)            { return (uintptr_t)v; }
static inline uintptr_t _fmt_word_u64   (unsigned long long v)   { return (uintptr_t)v; }

#define _fmt_box(x) \
	_Generic((x)+0, \
		char*:              _fmt_word_ptr,   \
		void*:              _fmt_word_ptr,   \
		int:                _fmt_word_i32,   \
		unsigned int:       _fmt_word_u32,   \
		long:               _fmt_word_long,  \
		unsigned long:      _fmt_word_ulong, \
		long long:          _fmt_word_i64,   \
		unsigned long long: _fmt_word_u64    \
	)(x)

#define _fmt_concat(a, b) a##b
#define _fmt_concat2(a, b) _fmt_concat(a, b)
#define _fmt_narg(...) _fmt_narg_(__VA_ARGS__, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _fmt_narg_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N, ...) N

#define _fmt_map_1(a) _fmt_box(a)
#define _fmt_map_2(a, ...) _fmt_box(a), _fmt_map_1(__VA_ARGS__)
#define _fmt_map_3(a, ...) _fmt_box(a), _fmt_map_2(__VA_ARGS__)
#define _fmt_map_4(a, ...) _fmt_box(a), _fmt_map_3(__VA_ARGS__)
#define _fmt_map_5(a, ...) _fmt_box(a), _fmt_map_4(__VA_ARGS__)
#define _fmt_map_6(a, ...) _fmt_box(a), _fmt_map_5(__VA_ARGS__)
#define _fmt_map_7(a, ...) _fmt_box(a), _fmt_map_6(__VA_ARGS__)
#define _fmt_map_8(a, ...) _fmt_box(a), _fmt_map_7(__VA_ARGS__)
#define _fmt_map_9(a, ...) _fmt_box(a), _fmt_map_8(__VA_ARGS__)
#define _fmt_map_10(a, ...) _fmt_box(a), _fmt_map_9(__VA_ARGS__)
#define _fmt_map_11(a, ...) _fmt_box(a), _fmt_map_10(__VA_ARGS__)
#define _fmt_map_12(a, ...) _fmt_box(a), _fmt_map_11(__VA_ARGS__)

#define _fmt_args(...) _fmt_concat2(_fmt_map_, _fmt_narg(__VA_ARGS__))(__VA_ARGS__)

#define format(E, F, ...)                                                    \
	__format(                                                                \
		F,                                                                   \
		(uintptr_t[]){ _fmt_args(__VA_ARGS__) },                             \
		sizeof((uintptr_t[]){ _fmt_args(__VA_ARGS__) }) / sizeof(uintptr_t), \
		E                                                                    \
	)

string_t* __format (charseq_t format, uintptr_t* values, uint64_t count, err_t* occured);


static err_t
format_appendchar(string_t* string, char ch)
{
	err_t err = ensureStringCapacity( string, 1 );
	if ( err )
		return err;
	return string_appendchar( string, ch );
}

static err_t
format_append(string_t* string, charseq_t seq)
{
	if ( !seq )
		seq = "";

	uint64_t length = charseq_countbytes( seq );
	err_t err = ensureStringCapacity( string, length );
	if ( err )
		return err;

	return string_appendcharseq( string, seq );
}

string_t*
__format(charseq_t format, uintptr_t* values, uint64_t count, err_t* occured)
{
	string_t* string = NULL;
	uint64_t index = 0;
	err_t err = PURE_OK;

	if ( occured )
		*occured = PURE_OK;

	uint64_t format_length = charseq_countbytes( format );
	if ( !format_length )
	{
		if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
		return NULL;
	}

	uint32_t alloc_capacity = (uint32_t)(format_length + (count * 24) + 1);
	string = string_new( alloc_capacity, &err );
	if ( err )
	{
		if ( occured ) *occured = err;
		return NULL;
	}

	for ( uint64_t i = 0; format[i] != '\0'; i++ )
	{
		if ( format[i] != '%' )
		{
			err = format_appendchar( string, format[i] );
			if ( err )
			{
				if ( occured ) *occured = err;
				return string;
			}
			continue;
		}

		char spec = format[i + 1];
		if ( spec == '\0' )
		{
			if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
			return string;
		}

		if ( spec == '%' )
		{
			err = format_appendchar( string, '%' );
			if ( err )
			{
				if ( occured ) *occured = err;
				return string;
			}
			i++;
			continue;
		}

		if ( index >= count )
		{
			if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
			return string;
		}

		switch ( spec )
		{
			case 'd':
			{
				char_t buffer[32];
				err = ascii_frominteger( (int64_t)values[index], (byte_t*)buffer, sizeof(buffer) );
				if ( err )
				{
					if ( occured ) *occured = err;
					return string;
				}

				err = format_append( string, buffer );
				break;
			}

			case 's':
			{
				err = format_append( string, (charseq_t)values[index] );
				break;
			}

			default:
			{
				if ( occured ) *occured = PURE_ERROR_INVALID_ARGUMENT;
				return string;
			}
		}

		if ( err )
		{
			if ( occured ) *occured = err;
			return string;
		}

		i++;
		index++;
	}

	if ( (index != count) && occured )
		*occured = PURE_ERROR_INVALID_ARGUMENT;

	return string;
}

*/