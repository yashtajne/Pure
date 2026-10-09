#ifndef Path_h
#define Path_h
#include "../../Core/String.h"

typedef string_t  path_t;
typedef charseq_t pathname_t;

// errcode_t path_getCurrentDirectory(pathname_t* pathname);
// pathname_t pathname_getcurrentdirectory(errcode_t* occured);

boolean pathname_isabsolute (pathname_t, errcode_t* occured);
boolean pathname_isrelative (pathname_t, errcode_t* occured);

boolean pathname_isdirectory (pathname_t path, errcode_t* occured);
boolean pathname_isfile      (pathname_t path, errcode_t* occured);

boolean pathname_exists (pathname_t path, errcode_t* occured);

#define isabsolute(P, E)                                      \
	_Generic((P),                                             \
		path_t: path_isabsolute( string_charseq((P)), (E) ),  \
		path_t*: path_isabsolute( string_charseq((P)), (E) ), \
		pathname_t: path_isabsolute( (P), (E) )               \
	)

#define isrelative(path, occured)                             \
	_Generic((P),                                             \
		path_t: path_isrelative( string_charseq((P)), (E) ),  \
		path_t*: path_isrelative( string_charseq((P)), (E) ), \
		pathname_t: path_isrelative( (P), (E) )               \
	)

#define isdirectory(path, occured)                             \
	_Generic((P),                                              \
		path_t: path_isdirectory( string_charseq((P)), (E) ),  \
		path_t*: path_isdirectory( string_charseq((P)), (E) ), \
		pathname_t: path_isdirectory( (P), (E) )               \
	)

#define isfile(path, occured)                             \
	_Generic((P),                                         \
		path_t: path_isfile( string_charseq((P)), (E) ),  \
		path_t*: path_isfile( string_charseq((P)), (E) ), \
		pathname_t: path_isfile( (P), (E) )               \
	)

#define exists(path, occured)                             \
	_Generic((P),                                         \
		path_t: path_exists( string_charseq((P)), (E) ),  \
		path_t*: path_exists( string_charseq((P)), (E) ), \
		pathname_t: path_exists( (P), (E) )               \
	)

#endif // Path_h
