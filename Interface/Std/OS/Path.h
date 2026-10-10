#ifndef StdOSPath_h
#define StdOSPath_h
#include "../../Core/String.h"

typedef string_t  path_t;
typedef charseq_t pathname_t;

// errcode_t path_getCurrentDirectory(pathname_t* pathname);
// pathname_t pathname_getcurrentdirectory(errcode_t* occured);

boolean pathname_exists (pathname_t);

boolean pathname_isabsolute (pathname_t);
boolean pathname_isrelative (pathname_t);

boolean pathname_isdirectory (pathname_t);
boolean pathname_isfile      (pathname_t);

#define isabsolute(P) \
	_Generic((P), \
		path_t: pathname_isabsolute(string_charseq((P))), \
		path_t*: pathname_isabsolute(string_charseq((P))), \
		pathname_t: pathname_isabsolute((P)))

#define isrelative(P) \
	_Generic((P), \
		path_t: pathname_isrelative(string_charseq((P))), \
		path_t*: pathname_isrelative(string_charseq((P))), \
		pathname_t: pathname_isrelative((P)))

#define isdirectory(P) \
	_Generic((P), \
		path_t: pathname_isdirectory(string_charseq((P))), \
		path_t*: pathname_isdirectory(string_charseq((P))), \
		pathname_t: pathname_isdirectory((P)))

#define isfile(P) \
	_Generic((P), \
		path_t: pathname_isfile(string_charseq((P))), \
		path_t*: pathname_isfile(string_charseq((P))), \
		pathname_t: pathname_isfile((P)))

#define exists(P) \
	_Generic((P), \
		path_t: pathname_exists(string_charseq((P))), \
		path_t*: pathname_exists(string_charseq((P))), \
		pathname_t: pathname_exists((P)))

#endif // StdOSPath_h