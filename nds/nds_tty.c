#include "../sim_defs.h"
#include "nds_io.h"
#include "nds_kbd.h"
#include "nds_dma.h"
#include "nds_tty.h"

int gba_tty_X;
int gba_tty_Y;
char gsbuf[1024];

void
gba_tty_inittext(void)
{
    unsigned short *vp;
    struct charmap *cp;
    unsigned char bits;
    int i, j, r, b;

    vp = (unsigned short *) NDS_VRAM;

    for (j = 1024*0; j < 1024*8; j++) {
        *vp++ = 0xFFFF;
    }
    for (j = 1024*8; j < 1024*16; j++) {
        *vp++ = 0x0FF0;
    }
    for (j = 1024*16; j < 1024*24; j++) {
        *vp++ = 0x00FF;
    }
    for (j = 1024*24; j < 1024*32; j++) {
        *vp++ = 0xF00F;
    }
    for (j = 1024*32; j < 1024*40; j++) {
        *vp++ = 0xFF00;
    }

    cp = (struct charmap *) &gbatxt_charmap[0x41];
    vp = (unsigned short *) NDS_VRAM;
    for (i = 0; i < 8; i++) {
       bits = cp->bitmap[i];
       for (b = 0; b < 8; b += 1) {
          *vp++ = (bits & (0x80>>b)) ? 0xFFFF : 0;
       }
       for (b = 0; b < 8; b += 1) {
          *vp++ = (bits & (0x80>>b)) ? 0xFFFF : 0;
       }
       for (b = 0; b < 8; b += 1) {
          *vp++ = (bits & (0x80>>b)) ? 0xFFFF : 0;
       }

       vp += 256-8*3;
    } 
    return;

    for (j = 0; j < 1; j++) {
        cp = (struct charmap *) &gbatxt_charmap[0];

	/* 1 line */
        for (i = 0; i < 32; i++, cp++) {

	    /* 1 char */
            for (r = 0; r < 512; r++) {
                bits = cp->bitmap[r];
                    for (b = 0; b < 8; b += 1) {
                        *vp++ = (bits & (0x80>>b)) ? 0xFFFF : 0;
                    }
                vp =  vp -8 + 256;
            }
        }
    }
}

void
gba_tty_init(void)
{
    int i;
    volatile unsigned short *pp;

    /* SWAP */
    *((unsigned int *) (NDS_IOBASE + 0x0304)) = 0x0; 

    /* Enable mode 0 2D */
    *((unsigned int  *) (NDS_IOBASE + 0x0000)) = 0x20000;
    *((unsigned char *) (NDS_IOBASE + 0x0240)) = 0x80;
    *((unsigned int  *) (NDS_IOBASE + 0x1000)) = 0x0;

//    *((unsigned short *) (0x6800000)) = (127<<10)|(127<<5)|127;
//    *((unsigned short *) (0x6800001)) = (127<<10)|(127<<5)|127;
//    *((unsigned short *) (0x6800002)) = (127<<10)|(127<<5)|127;


    /* Enable VRAMC */
//    *((unsigned char *) (NDS_IOBASE + 0x240)) = 1 | 4; /* VRAM_ENABLE | VRAM_C_SUB_BG */

#if 0
    /* Default palete, everything is white :-) */
    pp = (volatile unsigned short *)NDS_PALETTE;
    for (i = 255; i; i--)
        pp[i] = 0x7fff;
    pp[0] = 0;
#endif

    gba_tty_X = 0;
    gba_tty_Y = 0;

    gba_tty_inittext();
}

void
gba_tty_setxy(int x, int y)
{
    if ((x >= 0) && (x < NDS_XLEN) && (y >= 0) && (y < NDS_YLEN)) {
        gba_tty_X = x;
        gba_tty_Y = y;
    }
}

void
gba_tty_scroll(void)
{
    unsigned short *sp, *dp;
    int len;

    dp = (unsigned short *)(NDS_VRAM + 0x8000);
    sp = dp + NDS_XMAX;
    len = (NDS_XMAX * (NDS_YLEN - 1));

    while (len--)
        *dp++ = *sp++;

    for (len = NDS_XMAX; len; len--)
        *dp++ = 0;
}

void
gba_tty_putc(int c)
{
    unsigned short *vp;

    vp = (unsigned short *)(NDS_VRAM + 0x0000);
    vp += (gba_tty_Y * NDS_XMAX) + gba_tty_X;
    *vp = c;

    gba_tty_X++;
    if ((gba_tty_X >= NDS_XLEN) || (c == '\n')) {
        gba_tty_X = 0;
        gba_tty_Y++;
        if (gba_tty_Y >= NDS_YLEN) {
            gba_tty_scroll();
            gba_tty_Y = NDS_YLEN - 1;
        }
    }

    switch (c) { 
#if 0
        case 9:
            gba_tty_X += 4;
        break;
#endif

        case 11:
            gba_tty_Y += 2;
        break;

        case '\r':
            gba_tty_X = 0;
            gba_tty_Y++;
        break; 
    }
}

void
gba_tty_print(const char *s)
{
    for (; *s; s++)
        gba_tty_putc(*s);	
}

int
gba_puts(char *s)
{
    gba_tty_print(s);
    return 0;
}

int
gba_putsnl(char *s)
{
    gba_tty_print(s);
    gba_tty_print("\n");
    return 0;
}

#ifdef GAMEBOY_KEYPAD_IRQ
extern int readyforinput;
#endif

int
gba_kbd_read(int d, void *_buf, int nbytes)
{
    static int inputidx = 0;
    static int chridx = 0;
#ifndef GAMEBOY_KEYPAD_IRQ
    static int readyforinput = 0;
#endif
    unsigned char *buf = (unsigned char *)_buf;

#ifdef GAMEBOY_KEYPAD_IRQ
    if (!readyforinput)
        return -1;

    if (!gba_kbdinput[inputidx]) { /* commands exhausted */
        printf("\nRebooting system ...\n");
        gba_reset();
    }
#else
    if (gba_kbdinput[inputidx] == NULL)
        return -1;

    if (!readyforinput) {
        if (NDS_ISKEY_DOWN(NDS_KEY_START))
            readyforinput = 1;
        else
            return -1;
    }
#endif

    if (!*(gba_kbdinput[inputidx] + chridx)) {
        inputidx++;
        chridx = 0;
        readyforinput = 0;
        return -1;
    }

    buf[0] = *(gba_kbdinput[inputidx] + chridx);
    chridx++;

    return 1;
}

/* tty */

int32 sim_int_char = 005;

t_stat ttinit (void)
{
    return SCPE_OK;
}

t_stat ttrunstate (void)
{
    return SCPE_OK;
}

t_stat ttcmdstate (void)
{
    return SCPE_OK;
}

t_stat ttclose (void)
{
    return ttcmdstate();
}

t_stat sim_poll_kbd (void)
{
    int status;
    unsigned char buf[1];
    status = gba_kbd_read (0, buf, 1);
    if (status != 1)
        return SCPE_OK;
    else
        return (buf[0] | SCPE_KFLAG);
}

t_stat sim_putchar (int32 out)
{
    char c = out;

    gba_tty_putc(c);

    return SCPE_OK;
}
