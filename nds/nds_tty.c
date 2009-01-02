#include "../sim_defs.h"
#include "nds_io.h"
#include "nds_kbd.h"
#include "nds_dma.h"
#include "nds_tty.h"

#define	RGB15(r,g,b)  ((r)|((g)<<5)|((b)<<10))

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

    vp = (unsigned short *) (NDS_VRAM+0x4000); /* BG TILE 0 */

    for (j = 0; (j < 1); j++) {
        cp = (struct charmap *) &gbatxt_charmap[0];
        for (i = 0; (i < 128); i++, cp++) {
            for (r = 0; (r < 8); r++) {
                bits = cp->bitmap[r];
                for (b = 0; (b < 8); b += 2) {
                    *vp++ = ((bits & (0x80>>b)) ? 2 : 1) |
                    ((bits & (0x40>>b)) ? 0x200 : 0x100);
                }
            }
        }
    }
}

void
gba_tty_init(void)
{
    int i;
    volatile unsigned short *pp;

    /* video */
    *((unsigned int   *) (NDS_IOBASE + 0x0000)) = 0x10100; /* DISPCNT   : Display Mode=Graphics Display, BG Mode=0, BG0 Active*/
    *((unsigned char  *) (NDS_IOBASE + 0x0240)) = 0x81;    /* VRAMCNT_A : VRAM Enable, VRAM Offset=0,MST=1 */
    *((unsigned short *) (NDS_IOBASE + 0x0008)) = 0x0084;  /* BG0CNT    : Colors/Palettes=256/1, Character Base Block=1, Screen Base Block=0 */

    /* palette */
    *((unsigned short *) (NDS_PALETTE)+1) = RGB15(0,0,0);     /* black */
    *((unsigned short *) (NDS_PALETTE)+2) = RGB15(31,31,31);  /* white */

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

    dp = (unsigned short *)(NDS_VRAM + 0x0000);
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
