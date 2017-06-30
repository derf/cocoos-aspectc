# MSP430 Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
# License: Unlicense
#
#######################################
# user configuration:
#######################################
# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = msp430fr5969
# SOURCES: list of input source sources
SOURCES = os_assert.c  os_cbk.c  os_event.c  os_kernel.c  os_msgqueue.c  os_sem.c  os_task.c main.c
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -Iinc -I/opt/msp430/ti/gcc/include
# OUTDIR: directory to use for output
OUTDIR = build
# define flags
CFLAGS = -mmcu=$(MCU) -std=c++11 -g -Os -Wall -Wextra -Wunused -ffunction-sections $(INCLUDES)
ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(OUTDIR)/$(TARGET).map -Wl,--library-path=/opt/msp430/ti/gcc/include -Wl,--gc-sections
#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC      	= /opt/msp430/ti/gcc/bin/msp430-elf-g++
LD      	= /opt/msp430/ti/gcc/bin/msp430-elf-ld
AR      	= /opt/msp430/ti/gcc/bin/msp430-elf-ar
AS      	= /opt/msp430/ti/gcc/bin/msp430-elf-gcc
GASP    	= /opt/msp430/ti/gcc/bin/msp430-elf-gasp
NM      	= /opt/msp430/ti/gcc/bin/msp430-elf-nm
OBJCOPY 	= /opt/msp430/ti/gcc/bin/msp430-elf-objcopy
MAKETXT 	= srec_cat
UNIX2DOS	= unix2dos
RM      	= rm -f
MKDIR		= mkdir -p
#######################################

# file that includes all dependencies
DEPEND = $(SOURCES:.c=.d)

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o)))

# default: build hex file and TI TXT file
all: $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).txt

# TI TXT file
$(OUTDIR)/%.txt: $(OUTDIR)/%.hex
	$(MAKETXT) -O $@ -TITXT $< -I
	$(UNIX2DOS) $(OUTDIR)/$(TARGET).txt

# intel hex file
$(OUTDIR)/%.hex: $(OUTDIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

# elf file
$(OUTDIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	ag++ -r build/repo.acp -v 0 --c_compiler $(CC) --keep_woven -p src -c $(CFLAGS) -o $@ $<

# assembly listing
%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

# remove build artifacts and executables
clean:
	-$(RM) $(OUTDIR)/*

program:
	LD_LIBRARY_PATH=/home/derf/var/projects/msp430/MSP430Flasher_1.3.7 \
	/home/derf/var/projects/msp430/MSP430Flasher_1.3.7/MSP430Flasher \
	-w build/main.hex -v -g -z '[VCC]'

.PHONY: all clean program
