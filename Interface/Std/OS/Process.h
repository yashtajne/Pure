#ifndef StdOSProcess_H
#define StdOSProcess_H
#include "../Self.h"
#include "../../Core/Error.h"

struct _Process;
typedef struct _Process process_t;

enum ProcessExecutionFlag
{
	PROCESS_CAPTURE_OUTPUT  = 1u << 0,
	PROCESS_INHERIT_STDIN   = 1u << 1,
	PROCESS_INHERIT_STDOUT  = 1u << 2,
	PROCESS_INHERIT_STDERR  = 1u << 3,
};

#define PROCESS_INHERIT_ALL \
	PROCESS_INHERIT_STDIN | PROCESS_INHERIT_STDOUT | PROCESS_INHERIT_STDERR

errcode_t  _process_new (process_t*, charseq_t invocation);
process_t*  process_new (charseq_t invocation);

errcode_t _process_terminate         (process_t*);
errcode_t _process_waitforcompletion (process_t*);
errcode_t _process_cleanup           (process_t*);

void process_terminate         (process_t*);
void process_waitforcompletion (process_t*);
void process_cleanup           (process_t*);

// thread_t* process_getthread (process_t*);

uint8_t process_getexitcode (process_t*);

#endif // StdOSProcess_h