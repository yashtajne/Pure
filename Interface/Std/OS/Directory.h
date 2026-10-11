#ifndef StdOSDirectory_h
#define StdOSDirectory_h

#include "Path.h"

errcode_t _setcurrent_directory (pathname_t pathname);
errcode_t _getcurrent_directory (pathnameptr_t out);

void       setcurrent_directory (pathname_t);
pathname_t getcurrent_directory (void);

errcode_t _create_directory (charseq_t pathname);
errcode_t _delete_directory (charseq_t pathname);

#define _createdir(d) _create_directory( (d) );
#define _deletedir(d) _delete_directory( (d) );

void create_directory (charseq_t pathname);
void delete_directory (charseq_t pathname);

#define createdir(d) create_directory( (d) );
#define deletedir(d) delete_directory( (d) );

struct DirectoryWalker;
struct DirectoryWalker* directorywalker_new (void);

void directorywalk_start (struct DirectoryWalker* walker, pathname_t dirpath);
void directorywalk_next  (struct DirectoryWalker* walker);
void directorywalk_stop  (struct DirectoryWalker* walker);

#define dirwalk_start(W, P) directorywalk_start((W), (P))
#define dirwalk_next(W)     directorywalk_next((W))
#define dirwalk_stop(W)     directorywalk_stop((W))

charseq_t directorywalker_getfilename(struct DirectoryWalker*);
size_t    directorywalker_getfilesize(struct DirectoryWalker*);
boolean   directorywalker_isdirectory(struct DirectoryWalker*);

#define dirwalker_getfilename(W) directorywalker_getfilename((W))
#define dirwalker_getfilesize(W) directorywalker_getfilesize((W))
#define dirwalker_isdirectory(W) directorywalker_isdirectory((W))

#endif // StdOSDirectory_h