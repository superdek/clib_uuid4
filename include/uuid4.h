#include <stdint.h>

#ifndef UUID4_H
#define UUID4_H

typedef uint64_t *uuid4_t;

bool uuid4_is_nil(uuid4_t self);

bool uuid4_compare(uuid4_t self, uuid4_t target_uuid);

uuid4_t uuid4_create(uint64_t most, uint64_t least);

uuid4_t uuid4_generate(void);

uuid4_t uuid4_copy(uuid4_t self);

char *uuid4_to_string(uuid4_t self);

void uuid4_destroy(uuid4_t self);

#endif