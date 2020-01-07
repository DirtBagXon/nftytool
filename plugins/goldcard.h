/*
 * Definitions for the gold card/goldwafer
 * 
 */

//
// sizes in bytes
//
#define GC_EXT_EEPROM_SIZE      0x800

#define GC_ROM_SIZE             0x800
#define GC_EEPROM_SIZE          0x80
#define GC_IDENT_SIZE           0x08
#define GC_CONFIG_SIZE          0x02

//
// positions in hexfile
// 
#define GC_ROM_POS              0x0000
#define GC_IDENT_POS            0x4000
#define GC_CONFIG_POS           0x400e
#define GC_EEPROM_POS           0x4200

//
// loader for writing the ext. eeprom.
// taken directly from original infinity usb software by WB Electronics
//
unsigned char loader_code[] = {
  0x25, 0x06, 0x61, 0xbe, 0x01, 0x0c, 0x2d, 0x06, 0x61, 0xbe, 0x01, 0x0c, 
  0x50, 0x0e, 0x27, 0x02, 0x25, 0x06, 0x50, 0x14, 0x50, 0x16, 0x00, 0x00, 
  0x3f, 0x0c, 0x50, 0x20, 0x61, 0xbe, 0x50, 0x24, 0x24, 0x0c, 0x61, 0x9e, 
  0x2d, 0x06, 0x01, 0x0c, 0x25, 0x06, 0x3f, 0x0c, 0x50, 0x32, 0x2d, 0x0c, 
  0x50, 0x36, 0x25, 0x0c, 0x00, 0x00, 0x50, 0x38, 0x00, 0x00, 0x50, 0x3c, 
  0x37, 0x0c, 0x50, 0x46, 0x24, 0x0c, 0x60, 0x9e, 0x50, 0x4c, 0x2c, 0x0c, 
  0x60, 0xbe, 0x00, 0x00, 0x2d, 0x06, 0x01, 0x0c, 0x25, 0x06, 0x27, 0x0c, 
  0x34, 0x0c, 0x2f, 0x0c, 0x50, 0x5a, 0x2d, 0x06, 0x2f, 0x0c, 0x25, 0x06, 
  0x50, 0x62, 0x50, 0x64, 0x00, 0x00, 0x3f, 0x0c, 0x50, 0x6e, 0x2d, 0x0c, 
  0x50, 0x72, 0x25, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x50, 0x18 
};

unsigned char loader_cfg[] = {
  0x7f, 0xf2
};
