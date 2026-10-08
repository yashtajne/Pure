#ifndef StdIOStream_h
#define StdIOStream_h

#include "../../../Pure.h"

struct _Stream;
typedef struct _Stream stream_t;

uint64_t stream_write (stream_t*, byte_t buffer[], uint64_t buffersize);
uint64_t stream_read  (stream_t*, byte_t buffer[], uint64_t buffersize);

#endif // StdIOStream_h