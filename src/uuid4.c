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

uuid4_t uuid4_parse(uint8_t bytes[16])
{
    assert(bytes != NULL);

    uint64_t most =
        ((uint64_t)*(bytes + 0) << 56) |
        ((uint64_t)*(bytes + 1) << 48) |
        ((uint64_t)*(bytes + 2) << 40) |
        ((uint64_t)*(bytes + 3) << 32) |
        ((uint64_t)*(bytes + 4) << 24) |
        ((uint64_t)*(bytes + 5) << 16) |
        ((uint64_t)*(bytes + 6) << 8) |
        ((uint64_t)*(bytes + 7) << 0);
    uint64_t least =
        ((uint64_t)*(bytes + 8) << 56) |
        ((uint64_t)*(bytes + 9) << 48) |
        ((uint64_t)*(bytes + 10) << 40) |
        ((uint64_t)*(bytes + 11) << 32) |
        ((uint64_t)*(bytes + 12) << 24) |
        ((uint64_t)*(bytes + 13) << 16) |
        ((uint64_t)*(bytes + 14) << 8) |
        ((uint64_t)*(bytes + 15) << 0);

    _check(most, least);

    return (uuid4_t){most, least};
}

void uuid4_unparse(uuid4_t self, uint8_t bytes[16])
{
    assert(bytes != NULL);

    bytes[0] = (uint8_t)((self.most >> 56) & 0xFF);
    bytes[1] = (uint8_t)((self.most >> 48) & 0xFF);
    bytes[2] = (uint8_t)((self.most >> 40) & 0xFF);
    bytes[3] = (uint8_t)((self.most >> 32) & 0xFF);
    bytes[4] = (uint8_t)((self.most >> 24) & 0xFF);
    bytes[5] = (uint8_t)((self.most >> 16) & 0xFF);
    bytes[6] = (uint8_t)((self.most >> 8) & 0xFF);
    bytes[7] = (uint8_t)((self.most >> 0) & 0xFF);
    bytes[8] = (uint8_t)((self.least >> 56) & 0xFF);
    bytes[9] = (uint8_t)((self.least >> 48) & 0xFF);
    bytes[10] = (uint8_t)((self.least >> 40) & 0xFF);
    bytes[11] = (uint8_t)((self.least >> 32) & 0xFF);
    bytes[12] = (uint8_t)((self.least >> 24) & 0xFF);
    bytes[13] = (uint8_t)((self.least >> 16) & 0xFF);
    bytes[14] = (uint8_t)((self.least >> 8) & 0xFF);
    bytes[15] = (uint8_t)((self.least >> 0) & 0xFF);

    return;
}

void uuid4_to_string(uuid4_t self, char str[37])
{
    assert(str != NULL);

    _check(self.most, self.least);

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
    return;
}