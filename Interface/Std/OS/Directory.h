#ifndef Directory_h
#define Directory_h
#include "../Self.h"

#include "Path.h"
#include "../../Core/Error.h"

err_t create_directory (charseq_t pathname);
err_t delete_directory (charseq_t pathname);

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

#endif // Directory_h