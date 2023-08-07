#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <syserr.h>

#include "uuid4.h"

static void __attribute__((constructor)) constructor(void)
{
    srand(time(NULL));

    return;
}

static void __attribute__((destructor)) destructor(void)
{
    return;
}

bool uuid4_is_nil(uuid4_t self)
{
    return self.most == 0 && self.least == 0;
}

bool uuid4_compare(uuid4_t self, uuid4_t target)
{
    return self.most == target.most && self.least == target.least;
}

// uint64_t uuid4_get_varint(uuid4_t self)
// {

// }

static inline uuid4_t _fit(uint64_t most, uint64_t least)
{
    most &= 0xFFFFFFFFFFFF4FFF;
    most |= 0x0000000000004000;
    least &= 0xBFFFFFFFFFFFFFFF;
    least |= 0x8000000000000000;

    uuid4_t uuid = {most, least};
    return uuid;
}

uuid4_t uuid4_generate(uint64_t most, uint64_t least)
{
    return _fit(most, least);
}

uuid4_t uuid4_generate_rand(void)
{
    uint64_t rand_num1 = (uint64_t)rand();
    uint64_t rand_num2 = (uint64_t)rand();
    uint64_t rand_num3 = (uint64_t)rand();
    uint64_t rand_num4 = (uint64_t)rand();

    uint64_t most = (rand_num1 << 32) | rand_num2;
    uint64_t least = (rand_num3 << 32) | rand_num4;

    return _fit(most, least);
}

uuid4_t uuid4_copy(uuid4_t self)
{
    uuid4_t uuid = {self.most, self.least};
    return uuid;
}

uuid4_t uuid4_parse(uint8_t *bytes)
{
    if (bytes == NULL)
        call_nullarg();

    uint64_t most =
        ((uint64_t)bytes[0] << 56) |
        ((uint64_t)bytes[1] << 48) |
        ((uint64_t)bytes[2] << 40) |
        ((uint64_t)bytes[3] << 32) |
        ((uint64_t)bytes[4] << 24) |
        ((uint64_t)bytes[5] << 16) |
        ((uint64_t)bytes[6] << 8) |
        ((uint64_t)bytes[7]);
    uint64_t least =
        ((uint64_t)bytes[8] << 56) |
        ((uint64_t)bytes[9] << 48) |
        ((uint64_t)bytes[10] << 40) |
        ((uint64_t)bytes[11] << 32) |
        ((uint64_t)bytes[12] << 24) |
        ((uint64_t)bytes[13] << 16) |
        ((uint64_t)bytes[14] << 8) |
        ((uint64_t)bytes[15]);

    uint64_t version = (most & 0x000000000000F000) >> 12;
    // printf("version: %lu\n", version);
    if (version != 4)
        call_syserr("invalid version");

    uint64_t variant = (least & 0xF000000000000000) >> 60;
    if (variant < 0x8 || 0xB < variant)
        call_syserr("invalid variant");

    uuid4_t uuid = {most, least};
    return uuid;
}

void uuid4_unparse(uuid4_t self, uint8_t *bytes)
{
    if (bytes == NULL)
        call_nullarg();

    uint64_t most = self.most;
    bytes[0] = (uint8_t)((most >> 56) & 0xFF);
    bytes[1] = (uint8_t)((most >> 48) & 0xFF);
    bytes[2] = (uint8_t)((most >> 40) & 0xFF);
    bytes[3] = (uint8_t)((most >> 32) & 0xFF);
    bytes[4] = (uint8_t)((most >> 24) & 0xFF);
    bytes[5] = (uint8_t)((most >> 16) & 0xFF);
    bytes[6] = (uint8_t)((most >> 8) & 0xFF);
    bytes[7] = (uint8_t)(most & 0xFF);

    uint64_t least = self.least;
    bytes[8] = (uint8_t)((least >> 56) & 0xFF);
    bytes[9] = (uint8_t)((least >> 48) & 0xFF);
    bytes[10] = (uint8_t)((least >> 40) & 0xFF);
    bytes[11] = (uint8_t)((least >> 32) & 0xFF);
    bytes[12] = (uint8_t)((least >> 24) & 0xFF);
    bytes[13] = (uint8_t)((least >> 16) & 0xFF);
    bytes[14] = (uint8_t)((least >> 8) & 0xFF);
    bytes[15] = (uint8_t)(least & 0xFF);

    return;
}

void uuid4_to_string(uuid4_t self, char str[37])
{
    if (str == NULL)
        call_nullarg();

    uint64_t most = self.most, least = self.least;

    uint64_t value = most;
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
            value = least;
            a = 0;
        }

        uint64_t j = (7 - a++) * 8;
        uint8_t byte = (uint8_t)((value >> j) & 0xFF);

        sprintf(str + i, "%02X", byte);
        i++;
    }

    str[36] = '\0';
    return;
}