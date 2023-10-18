#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <utils.h>

#include "../src/uuid4.c"

// static const uint64_t UUID_BYTE_SIZE = 16;

int main()
{
    for (uint64_t i = 0; i < 100; ++i)
    {
        uuid4_t uuid = uuid4_generate();
        // uuid4_t uuid = 
        //     uuid4_create(8445831224288885050, 5789347015909345661);
        char str[37];
        uuid4_to_string(uuid, str);

        printf("uuid's str: %s\n", str);
    }

    printf("test uuid4_parse\n");
    {
        // target's uuid: 9c6241f0-e74e-40ce-b9a6-953413d2af80
        uint8_t bytes[16] =
            {0x9C, 0x62, 0x41, 0xF0, 0xE7, 0x4E, 0x40, 0xCE,
             0xB9, 0xA6, 0x95, 0x34, 0x13, 0xD2, 0xAF, 0x80};

        uuid4_t expected_uuid =
            uuid4_create(11268641720563024078UL, 13377543794054049664UL);
        uuid4_t uuid = uuid4_parse(bytes);

        if (uuid4_compare(expected_uuid, uuid) == false)
            printf("ERROR: unexpected result\n");
    }
    printf("pass\n");

    printf("test uuid4_unparse\n");
    {
        uuid4_t uuid =
            uuid4_create(11268641720563024078UL, 13377543794054049664UL);

        uint8_t expected_bytes[16] =
            {0x9C, 0x62, 0x41, 0xF0, 0xE7, 0x4E, 0x40, 0xCE,
            0xB9, 0xA6, 0x95, 0x34, 0x13, 0xD2, 0xAF, 0x80};    
        uint8_t bytes[16];
        uuid4_unparse(uuid, bytes);
    
        if (memcmp(expected_bytes, bytes, 16) != 0)
            printf("ERROR: unexpected result\n");
    }
    printf("pass\n");

    return 0;
}