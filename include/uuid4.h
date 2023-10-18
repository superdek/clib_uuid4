#include <stdint.h>

#ifndef UUID4_H
#define UUID4_H

typedef struct uuid4
{
    uint64_t most;  // left half number of UUID
    uint64_t least; // right half number of UUID
} uuid4_t;

bool uuid4_compare(uuid4_t uuid1, uuid4_t uuid2);

bool uuid4_is_nil(uuid4_t self);

uuid4_t uuid4_create(uint64_t most, uint64_t least);

uuid4_t uuid4_generate(void);

uuid4_t uuid4_copy(uuid4_t self);

uuid4_t uuid4_parse(uint8_t bytes[16]);

void uuid4_unparse(uuid4_t self, uint8_t bytes[16]);

void uuid4_to_string(uuid4_t self, char str[37]);

#endif