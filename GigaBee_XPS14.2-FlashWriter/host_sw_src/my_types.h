#ifndef MY_TYPES_H
#define MY_TYPES_H

#define BYTE_SWAP4(x) \
    (((x & 0xFF000000) >> 24) | \
     ((x & 0x00FF0000) >> 8)  | \
     ((x & 0x0000FF00) << 8)  | \
     ((x & 0x000000FF) << 24))

#define BYTE_SWAP2(x) \
    (((x & 0xFF00) >> 8) | \
     ((x & 0x00FF) << 8))

struct gigabee_device_t {
    unsigned char mac[6];
    qint32 ip;
    qint32 test_result;
    int list_position;

};
enum {
    GFW_OP_PING_CMD = 1,
    GFW_OP_PING_ACK,
    GFW_OP_FLASH_DATA_TRANSFER_CMD,
    GFW_OP_FLASH_DATA_TRANSFER_ACK,

    GFW_OP_FLASH_DATA_RESEND_CMD,

    GFW_OP_FLASH_WRITE_CMD,
    GFW_OP_FLASH_WRITE_ACK

};

enum {
    SELF_TEST_ALL_OK = 0,
    SELF_TEST_RAM = 1,
    SELF_TEST_FLASH = 2,
    SELF_TEST_EEPROM = 4,
    SELF_TEST_ETHERNET = 8
};

typedef struct __attribute__ ((__packed__)) {
    unsigned char magic1; // G
    unsigned char magic2; // F
    unsigned char magic3; // W
    unsigned char op; // operation
    quint32 id; // id / sequence number
    quint32 data_length;
    quint32 param1;

    quint32 address_offset;
    quint32 number_of_packets;

    unsigned char data[0];
} gfw_datagram_t;

#endif // MY_TYPES_H
