#define EBML_LENGTH_1 0x80 // 10000000
#define EBML_LENGTH_2 0x40 // 01000000
#define EBML_LENGTH_3 0x20 // 00100000
#define EBML_LENGTH_4 0x10 // 00010000
#define EBML_LENGTH_5 0x08 // 00001000
#define EBML_LENGTH_6 0x04 // 00000100
#define EBML_LENGTH_7 0x02 // 00000010
#define EBML_LENGTH_8 0x01 // 00000001

#define EBML_DATA_SIZE_UNKNOWN          0x01FFFFFFFFFFFFFFLLU
#define EBML_DATA_SIZE_UNKNOWN_UNPACKED 0x00FFFFFFFFFFFFFFLLU
#define EBML_DATA_SIZE_UNKNOWN_LENGTH   8

#define EID_HEADER                      0x1A45DFA3
#define EID_VERSION                     0x4286
#define EID_READVERSION                 0x42F7
#define EID_MAXIDLENGTH                 0x42F2
#define EID_MAXSIZELENGTH               0x42F3
#define EID_DOCTYPE                     0x4282
#define EID_DOCTYPEVERSION              0x4287
#define EID_DOCTYPEREADVERSION          0x4285

#define EID_VOID               0xEC
#define EID_CRC32              0xBF
