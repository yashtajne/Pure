#ifndef File_h
#define File_h

#include "../../Core/Error.h"
#include "Path.h"

struct _File;
typedef struct _File file_t;

enum FileOpenMode        //   . - - - - - - - - - - - - - - - - - - - - - - - - - - .
{                        //   |  If file exists            | If file does not exist |
                         //   | - - - - - - - - - - - - - -|- - - - - - - - - - - - |
	READ_ONLY,           //   |  OK (keeps data)           | ERROR_FILE_NOT_FOUND   |
	WRITE_ONLY,          //   |  OK (truncates)            | OK (Creates new file)  |
	READ_AND_WRITE,      //   |  OK (keeps data)           | OK (Creates new file)  |
	WRITE_ONLY_APPEND,   //   |  OK (keeps data)           | ERROR_FILE_NOT_FOUND   |
	READ_WRITE_APPEND,   //   |  OK (keeps data)           | OK (creates new file)  |
	WRITE_ONLY_EXISTING, //   |  ERROR_FILE_ALREADY_EXISTS | OK (creates new file)  |
	READ_WRITE_EXISTING, //   |  ERROR_FILE_ALREADY_EXISTS | OK (creates new file)  |
                         //   ' - - - - - - - - - - - - - -'- - - - - - - - - - - - '
};

file_t* file_open  (charseq_t filename, enum FileOpenMode mode);
void    file_close (file_t* file);

#define fileopen(file, mode) file_open( (file), (mode) )
#define fileclose(file)      file_close(file)

/*
#define fileopen(F, M, E)                                    \
	_Generic((F),                                            \
		path_t: file_open( string_charseq(&(F)), (M), (E) ), \
		path_t*: file_open( string_charseq((F)), (M), (E) ), \
		charseq_t: file_open( (F), (M), (E) )                \
	)

#define fileclose(F, M, E)                                    \
	_Generic((F),                                             \
		path_t: file_close( string_charseq(&(F)), (M), (E) ), \
		path_t*: file_close( string_charseq((F)), (M), (E) ), \
		charseq_t: file_close( (F), (M), (E) )                \
	)

err_t file_delete (pathname_t pathname);

#define filedelete(P)                                      \
	_Generic((P),                                          \
		file_t: file_delete( string_charseq(&(P)) ),       \
		path_t: file_delete( string_charseq(&(P)) ),       \
		path_t*: file_delete( string_charseq((P)) ),       \
		pathname_t: file_delete( (P) )                     \
	)
*/

char_t file_getchar (file_t* file);
void   file_putchar (file_t* file, char_t character);

#define filegetc(file)            file_getchar( (file) )
#define fileputc(file, character) file_putchar( (file), (character) )

#define fgetc(file) filegetc((file))
#define fputc(file) fileputc((file))

uint64_t file_getcursorposition  (file_t* file);
void     file_setcursorposition  (file_t* file, uint64_t position);
void     file_movecursorposition (file_t* file, int64_t offset);

#define filegetcurpos(file)            file_getcursorposition( (file) )
#define filesetcurpos(file, position)  file_setcursorposition( (file), (position) )
#define filemovecursor(file, position) file_movecursorposition( (file), (position) )

size_t _file_read  (file_t* file, byte_t buffer[], size_t buffersize);
size_t _file_write (file_t* file, byte_t buffer[], size_t buffersize);

#define _fileread(file, buffer, buffersize)  _file_read( (file), (buffer), (buffersize) )
#define _filewrite(file, buffer, buffersize) _file_write( (file), (buffer), (buffersize) )

size_t _file_readline  (file_t* file, byte_t buffer[], size_t buffersize);
size_t _file_writeline (file_t* file, byte_t buffer[], size_t buffersize);

#define filereadline(file, buffer, buffersize)  _file_readline( (file), (buffer), (buffersize) )
#define filewriteline(file, buffer, buffersize) _file_writeline( (file), (buffer), (buffersize) )

size_t file_getsize (file_t* file);

#define filegetsize(file) file_getsize( (file) )

#endif // File_h