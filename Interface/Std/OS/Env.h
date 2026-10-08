#ifndef Env_h
#define Env_h
#include "../../../Interface/Core/String.h"

charseq_t environment_variable_get (charseq_t key);
void environment_variable_set      (charseq_t key, charseq_t value);
void environment_variable_unset    (charseq_t key);

#define envget(k, out) environment_variable_get((k), (out))
#define envset(k, v)   environment_variable_set((k), (v))
#define envunset(k)    environment_variable_unset((k))

err_t environment_variables_load   (charseq_t filename, uint64_t* out_loaded_count);
err_t environment_variables_unload (charseq_t filename);

#define envload(f, out) (environment_variables_load((f), (out)))
#define envunload(f)    (environment_variables_unload((f)))

#endif // Env_h