#ifndef _NDS_TTY_H_
#define _NDS_TTY_H_

#include "nds_io.h"

struct charmap {
    unsigned char bitmap[8];
};

/* Coordinates */
#define NDS_XMAX       32
#define NDS_XLEN       30
#define NDS_YMAX       32
#define NDS_YLEN       20

#define NDS_KEYBASE    *(volatile unsigned short *)(0x4000130)
#define NDS_KEY_A      0x1
#define NDS_KEY_B      0x2
#define NDS_KEY_SELECT 0x4
#define NDS_KEY_START  0x8
#define NDS_KEY_RIGHT  0x10
#define NDS_KEY_LEFT   0x20
#define NDS_KEY_UP     0x40
#define NDS_KEY_DOWN   0x80
#define NDS_KEY_R      0x100
#define NDS_KEY_L      0x200
#define NDS_KEY_NONE   0x0
#define NDS_KEY_ALL    0x3ff

#define NDS_ISKEY_UP(key)   !(~(NDS_KEYBASE | 0xfc00) & (key))
#define NDS_ISKEY_DOWN(key) ~(NDS_KEYBASE | 0xfc00) & (key)
#define NDS_KEY_GET         (unsigned long)(~(NDS_KEYBASE | 0xfc00))

extern const struct charmap gbatxt_charmap[];

void nds_tty_init(void);
void nds_tty_print(const char *s);
void nds_tty_inittext(void);
void nds_tty_setxy(int x, int y);
void nds_tty_scroll(void);
void nds_tty_putc(int c);

#endif /* _NDS_TTY_H_ */
