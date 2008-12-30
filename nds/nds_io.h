#ifndef _NDS_IO_H_
#define _NDS_IO_H_

/* Derived from libgba */

typedef	unsigned char       u8;
typedef	unsigned short int u16;
typedef	unsigned int       u32;
typedef	signed char         s8;
typedef	signed short int   s16;
typedef	signed int         s32;
typedef	volatile u8        vu8;
typedef	volatile u16      vu16;
typedef	volatile u32      vu32;
typedef	volatile s8        vs8;
typedef	volatile s16      vs16;
typedef	volatile s32      vs32;

#define N2BIT(N) (1 << N)

#define	NDS_RAM     0x02000000
#define	NDS_IOBASE  0x04000000
#define NDS_PALETTE 0x05000000
#define	NDS_VRAM    0x06800000 /* Frame Buffer Mode */

#define GBA_IWRAM_DATA

#endif /* _NDS_IO_H_ */
