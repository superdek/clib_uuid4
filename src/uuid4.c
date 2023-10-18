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

bool uuid4_compare(uuid4_t uuid1, uuid4_t uuid2)
{
    _check(uuid1.most, uuid1.least);
    _check(uuid2.most, uuid2.least);

    return uuid1.most == uuid2.most && 
        uuid1.least == uuid2.least;
}

bool uuid4_is_nil(uuid4_t self)
{
    
    _check(self.most, self.least);

    return self.most == 0 && self.least == 0;
}

// uint64_t uuid4_get_varint(uuid4_t self)
// {

// }

uuid4_t uuid4_create(uint64_t most, uint64_t least)
{
    _check(most, least);

    return (uuid4_t){most, least};
}

uuid4_t uuid4_generate(void)
{
    uint64_t most = ((uint64_t)rand() << 32) | (uint64_t)rand();
    uint64_t least = ((uint64_t)rand() << 32) | (uint64_t)rand();

    most &= 0xFFFFFFFFFFFF4FFF;
    most |= 0x0000000000004000;
    least &= 0xBFFFFFFFFFFFFFFF;
    least |= 0x8000000000000000;

    return (uuid4_t){most, least};
}

uuid4_t uuid4_copy(uuid4_t self)
{
    _check(self.most, self.least);

    return (uuid4_t){self.most, self.least};
}

char *uuid4_to_string(uuid4_t self)
{
    _check(self.most, self.least);

    char *str = (char *)memory_alloc(37);

    uint64_t value = self.most;
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
            value = self.least;
            a = 0;
        }

        uint64_t j = (7 - a++) * 8;
        uint8_t byte = (uint8_t)((value >> j) & 0xFF);

        sprintf(str + i++, "%02X", byte);
    }

    str[36] = '\0';
    return str;
}