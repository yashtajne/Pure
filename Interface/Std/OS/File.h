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

errcode_t _file_delete (pathname_t pathname);
void       file_delete (pathname_t pathname);

#define _filedelete(f) _file_delete((f));
#define  filedelete(f)  file_delete((f));

errcode_t _file_open   (file_t* file, charseq_t filename, enum FileOpenMode mode);
errcode_t _file_close  (file_t* file);

#define _fileopen(file, mode) _file_open( (file), (mode) )
#define _fileclose(file)      _file_close(file)

file_t* file_open   (charseq_t filename, enum FileOpenMode mode);
void    file_close  (file_t* file);

#define fileopen(file, mode) file_open( (file), (mode) )
#define fileclose(file)      file_close(file)

errcode_t _file_getchar (file_t* file, char_t* character);
errcode_t _file_putchar (file_t* file, char_t character);

char_t file_getchar (file_t* file);
void   file_putchar (file_t* file, char_t character);

#define _filegetc(file, character) _file_getchar( (file), (character) )
#define _fileputc(file, character) _file_putchar( (file), (character) )

#define _fgetc(file, character) _filegetc( (file), (character) )
#define _fputc(file, character) _fileputc( (file), (character) )

#define filegetc(file)            file_getchar( (file) )
#define fileputc(file, character) file_putchar( (file), (character) )

#define fgetc(file) filegetc( (file) )
#define fputc(file, character) fileputc( (file), (character) )

errcode_t _file_getcursorposition  (file_t* file, uint64_t* position);
errcode_t _file_setcursorposition  (file_t* file, uint64_t position);
errcode_t _file_movecursorposition (file_t* file, int64_t offset);

#define _filegetcurpos(file, position) _file_getcursorposition( (file), (position) )
#define _filesetcurpos(file, position) _file_setcursorposition( (file), (position) )
#define _filemovecursor(file, offset)  _file_movecursorposition( (file), (offset) )

uint64_t file_getcursorposition  (file_t* file);
void     file_setcursorposition  (file_t* file, uint64_t position);
void     file_movecursorposition (file_t* file, int64_t offset);

#define filegetcurpos(file)           file_getcursorposition( (file) )
#define filesetcurpos(file, position) file_setcursorposition( (file), (position) )
#define filemovecursor(file, offset)  file_movecursorposition( (file), (offset) )

errcode_t _file_read  (file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbytesread);
errcode_t _file_write (file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbyteswritten);

#define _fileread(file, buffer, buffersize, numberofbytesread)  _file_read( (file), (buffer), (buffersize), (numberofbytesread) )
#define _filewrite(file, buffer, buffersize, numberofbyteswritten) _file_write( (file), (buffer), (buffersize), (numberofbyteswritten) )

errcode_t _file_readline  (file_t* file, byte_t buffer[], size_t buffersize, size_t* numberofbytesread);
#define   _filereadline(file, buffer, buffersize, numberofbytesread)  _file_readline( (file), (buffer), (buffersize), (numberofbytesread) )

// errcode_t _file_writeline (file_t* file, byte_t buffer[], size_t buffersize, size_t numberofbytesread);
// #define   _filewriteline(file, buffer, buffersize, numberofbytesread) _file_writeline( (file), (buffer), (buffersize), (numberofbytesread) )

errcode_t _file_getsize (file_t* file, size_t* size);
size_t     file_getsize (file_t* file);

#define _filegetsize(file, size) _file_getsize( (file), (size) )
#define  filegetsize(file) file_getsize( (file) )

#endif // File_h