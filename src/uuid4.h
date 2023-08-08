#include <stdint.h>

#ifndef UUID4_H
#define UUID4_H

typedef struct uuid4
{
    uint64_t most;  // a number that specifies the left half of the UUID
    uint64_t least; // a number that specifies the right half of the UUID
} uuid4_t;

bool uuid4_is_nil(uuid4_t self);

bool uuid4_compare(uuid4_t self, uuid4_t target);

uuid4_t uuid4_generate(uint64_t most, uint64_t least);

uuid4_t uuid4_generate_rand(void);

uuid4_t uuid4_copy(uuid4_t self);

void uuid4_to_string(uuid4_t self, char str[37]);

#endif