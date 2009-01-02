#include "nds_sys.h"

void
nds_reset(void)
{
    swiSoftReset();
}

void
nds_stop(void)
{
    //nds_syscall(0x3);
}

void
nds_abort(char *msg)
{
    gba_tty_print(msg);
    nds_stop();
}
