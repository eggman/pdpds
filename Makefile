# Amit Singh
# http://www.kernelthread.com
#
# Kohei SEKINE
# http://d.hatena.ne.jp/eggman/
#
# 5th Edition UNIX on the Nintendo Gameboy Advance
#

# Host: Windows
include $(DEVKITARM)\ds_rules

export DEPSDIR   := $(CURDIR)/build

CROSSTOOLS_PREFIX = arm-eabi
CROSSTOOLS_PATH   = c:/devkitpro/devkitARM
AS                = $(CROSSTOOLS_PREFIX)-as
CC                = $(CROSSTOOLS_PREFIX)-gcc
LD                = $(CROSSTOOLS_PREFIX)-ld
OBJCOPY           = $(CROSSTOOLS_PREFIX)-objcopy
INCLUDES          = -I$(CROSSTOOLS_PATH)/$(CROSSTOOLS_PREFIX)/include
LOCAL_INCLUDES    = -I$(CROSSTOOLS_PATH)/../libnds/include
ASFLAGS           =  $(INCLUDES)/

# -DGAMEBOY       mandatory
# -DGAMEBOY_DMA
# -DGAMEBOY_FASTDATA
# -DGAMEBOY_KEYPAD_IRQ
# -DGAMEBOY_PDP_CIS
# -DGAMEBOY_WCACHE
#
# GAMEBOY_FLAGS     = -DGAMEBOY -DGAMEBOY_DMA -DGAMEBOY_KEYPAD_IRQ
GAMEBOY_FLAGS     = -DGAMEBOY -DNDS

# COPTFLAG        = -O3 -fnew-ra -funit-at-a-time
# COPTFLAG        = -Os
COPTFLAG          = -O3
CFLAGS            = $(GAMEBOY_FLAGS) $(INCLUDES) $(LOCAL_INCLUDES) $(COPTFLAG) \
                    -specs=ds_arm9.specs  -DARM9
CFLAGS_ARM        = $(GAMEBOY_FLAGS) $(INCLUDES) $(LOCAL_INCLUDES) $(COPTFLAG) \
                    -marm -mthumb-interwork -specs=ds_arm9.specs -DARM9
LDFLAGS           = -Map unixv5.map -lm -Lc:/devkitpro/libnds/lib -lnds9 
#LDFLAGS           = -lm
OBJCOPYFLAGS      = -v -O binary

GBAUNIXV5_ELF     = unixv5
GBAUNIXV5_TMP     = unixv5.tmp
GBAUNIXV5_ROM     = unixv5.nds
UNIXV5_DISK       = disks/unixv5.dsk

OBJS              = pdp11_cpu.o pdp11_fp.o pdp11_lp.o pdp11_rk.o pdp11_rl.o \
                    pdp11_rp.o pdp11_rx.o pdp11_stddev.o pdp11_sys.o \
                    pdp11_tm.o scp.o nds/nds_fsio.o nds/nds_fsio_core.o \
                    nds/nds_intr.o nds/nds_single_intr.o nds/nds_sys.o \
                    nds/nds_tty.o nds/nds_tty_charmap.o

default: all

nds/nds_fsio_core.o: nds/nds_fsio_core.c nds/nds_fsio.h nds/nds_dma.h \
                     nds/nds_unix.h
	$(CC) $(CFLAGS_ARM) -c -o $@ $<

nds/nds_fsio.o: nds/nds_fsio.c nds/nds_fsio.h nds/nds_dma.h nds/nds_sys.h \
                     nds/nds_unix.h

nds/nds_intr.o: nds/nds_intr.c nds/nds_intr.h nds/nds_gfx.h

nds/nds_single_intr.o: nds/nds_single_intr.s

nds/nds_sys.o: nds/nds_sys.c nds/nds_intr.h nds/nds_sys.h

nds/nds_tty.o: nds/nds_tty.c sim_defs.h nds/nds_io.h nds/nds_kbd.h \
               nds/nds_dma.h nds/nds_tty.h

nds/nds_tty_charmap.o: nds/nds_tty_charmap.c nds/nds_tty.h

# --

nds/nds_dma.h: nds/nds_io.h nds/nds_intr.h

nds/nds_gfx.h: nds/nds_io.h

nds/nds_intr.h: nds/nds_io.h

nds/nds_tty.h: nds/nds_io.h

# --

pdp11_cpu.o: pdp11_cpu.c pdp11_defs.h nds/nds_io.h
	mkdir -p build
	mkdir -p build\\nds
	$(CC) $(CFLAGS) -c -o $@ $<

pdp11_fp.o: pdp11_fp.c pdp11_defs.h nds/nds_io.h

pdp11_lp.o: pdp11_lp.c pdp11_defs.h

pdp11_rk.o: pdp11_rk.c pdp11_defs.h

pdp11_rl.o: pdp11_rl.c pdp11_defs.h

pdp11_rp.o: pdp11_rp.c pdp11_defs.h

pdp11_rx.o: pdp11_rx.c pdp11_defs.h

pdp11_stddev.o: pdp11_stddev.c pdp11_defs.h

pdp11_sys.o: pdp11_sys.c pdp11_defs.h

pdp11_tm.o: pdp11_tm.c pdp11_defs.h

scp.o: scp.c nds/nds_fsio.h nds/nds_intr.h sim_defs.h

# --

pdp11_defs.h: sim_defs.h

sim_defs.h: nds/nds_fsio.h

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.s.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(GBAUNIXV5_ROM): $(GBAUNIXV5_TMP) $(UNIXV5_DISK)
	@rm nds/nds_fsio_core.o
	@echo "expr 512 + `cat $(GBAUNIXV5_TMP) | wc -c`" > offset.sh
	@echo "#define NDS_UNIXOFFSET `bash offset.sh `" > nds/nds_unix.h
	@rm offset.sh
	@echo "#define NDS_UNIXSIZE `cat $(UNIXV5_DISK) | wc -c`" >> nds/nds_unix.h
	$(CC) $(CFLAGS) -c nds/nds_fsio_core.c -o nds/nds_fsio_core.o
	$(CC) $(CFLAGS) -o $(GBAUNIXV5_ELF) $(OBJS) -Wl,$(LDFLAGS)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(GBAUNIXV5_ELF) $(GBAUNIXV5_TMP)
	cat $(GBAUNIXV5_TMP) $(UNIXV5_DISK) > $(GBAUNIXV5_TMP).bin
	ndstool -c $@ -9 $(GBAUNIXV5_TMP).bin
	@du -h $@

$(GBAUNIXV5_TMP): $(GBAUNIXV5_ELF)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(GBAUNIXV5_ELF): $(OBJS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) -Wl,$(LDFLAGS)

.PHONY: all clean realclean

all: $(GBAUNIXV5_ROM)

clean:
	rm -f $(OBJS) *.d *.i *.s $(GBAUNIXV5_ELF) $(GBAUNIXV5_TMP) $(GBAUNIXV5_TMP).bin
	rm -rf build

realclean: clean
	rm -f $(GBAUNIXV5_ROM)
