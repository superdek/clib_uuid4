#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <utils.h>

#include <uuid4.h>

__attribute__((constructor))
static void constructor(void)
{
    srand(time(NULL));
    return;
}

/*
 * most: a number that specifies the left half of the UUID
 * least: a number that specifies the right half of the UUID
 */
static inline void _check(uint64_t most, uint64_t least)
{
    assert(most != 0 || least != 0);

    uint64_t version = (most & 0x000000000000F000) >> 12;
    // printf("version: %lu\n", version);
    assert(version == 4);

    uint64_t variant = (least & 0xF000000000000000) >> 60;
    assert(0x8 <= variant && variant <= 0xB);

    return;
}

bool uuid4_is_nil(uuid4_t self)
{
    assert(self != NULL);
    
    _check(*(self + 0), *(self + 1));

    return *(self + 0) == 0 && *(self + 1) == 0;
}

bool uuid4_compare(uuid4_t self, uuid4_t target_uuid)
{
    assert(self != NULL);
    assert(target_uuid != NULL);

    _check(*(self + 0), *(self + 1));

    return *(self + 0) == *(target_uuid + 0) && 
        *(self + 1) == *(target_uuid + 1);
}

// uint64_t uuid4_get_varint(uuid4_t self)
// {

// }

static inline uuid4_t _create(uint64_t most, uint64_t least)
{
    uuid4_t uuid = (uuid4_t)memory_alloc(sizeof(uint64_t) * 2);
    *(uuid + 0) = most;
    *(uuid + 1) = least;

    return uuid;
}

uuid4_t uuid4_create(uint64_t most, uint64_t least)
{
    _check(most, least);

    return _create(most, least);
}

uuid4_t uuid4_generate(void)
{
    uint64_t most = ((uint64_t)rand() << 32) | (uint64_t)rand();
    uint64_t least = ((uint64_t)rand() << 32) | (uint64_t)rand();

    most &= 0xFFFFFFFFFFFF4FFF;
    most |= 0x0000000000004000;
    least &= 0xBFFFFFFFFFFFFFFF;
    least |= 0x8000000000000000;

    return _create(most, least);
}

uuid4_t uuid4_copy(uuid4_t self)
{
    assert(self != NULL);

    _check(*(self + 0), *(self + 1));

    return _create(*(self + 0), *(self + 1));
}

char *uuid4_to_string(uuid4_t self)
{
    assert(self != NULL);

    _check(*(self + 0), *(self + 1));

    char *str = (char *)memory_alloc(37);

    uint64_t value = *(self + 0);
    uint64_t a = 0;
    for (uint64_t i = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            str[i] = '-';
            continue;
        }

        if (a == 8)
        {
            value = *(self + 1);
            a = 0;
        }

        uint64_t j = (7 - a++) * 8;
        uint8_t byte = (uint8_t)((value >> j) & 0xFF);

        sprintf(str + i++, "%02X", byte);
    }

    str[36] = '\0';
    return str;
}

void uuid4_destroy(uuid4_t self)
{
    assert(self != NULL);

    memory_dealloc(self);

    return;
}