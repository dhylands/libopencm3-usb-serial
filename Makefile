BOARD ?= 1bitsy
#BOARD ?= discovery
$(info BOARD = $(BOARD))

TARGET ?= usb-serial
FLASH_ADDR ?= 0x8000000

# Turn on increased build verbosity by defining BUILD_VERBOSE in your main
# Makefile or in your environment. You can also use V=1 on the make command
# line.

ifeq ("$(origin V)", "command line")
BUILD_VERBOSE=$(V)
endif
ifndef BUILD_VERBOSE
BUILD_VERBOSE = 0
endif
ifeq ($(BUILD_VERBOSE),0)
Q = @
else
Q =
endif
# Since this is a new feature, advertise it
ifeq ($(BUILD_VERBOSE),0)
$(info Use make V=1 or set BUILD_VERBOSE in your environment to increase build verbosity.)
endif

BUILD ?= build-$(BOARD)

RM = rm
ECHO = @echo

CROSS_COMPILE = arm-none-eabi-

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size

LIBOPENCM3_DIR = libopencm3
LIBOPENCM3_LIBDIR = $(LIBOPENCM3_DIR)/lib
LIBOPENCM3_LIBNAME = opencm3_stm32f4
LIBOPENCM3_LIBNAME_FULL = $(LIBOPENCM3_LIBDIR)/lib$(LIBOPENCM3_LIBNAME).a

INC =  -I.
INC += -I$(LIBOPENCM3_DIR)/include

CFLAGS_CORTEX_M4 = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion -Werror

CFLAGS =  -DSTM32F4 $(INC)
CFLAGS += -Wextra -Wshadow -Wredundant-decls -Wall -Wmissing-prototypes -Wstrict-prototypes
CFLAGS += -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)

CFLAGS_1bitsy    = -DBOARD_1BITSY
CFLAGS_discovery = -DBOARD_STM32F4DISC

LDSCRIPT_1bitsy    = stm32f4-1bitsy.ld
LDSCRIPT_discovery = stm32f4-discovery.ld

OBJ_1bitsy = $(BUILD)/button_boot.o
OBJ_discovery =

CFLAGS += $(CFLAGS_$(BOARD))

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

LDFLAGS = --static -nostartfiles -T $(LDSCRIPT_$(BOARD)) -Wl,-Map=$(@:.elf=.map),--cref

LIBS = -L$(LIBOPENCM3_LIBDIR) -l$(LIBOPENCM3_LIBNAME)

OBJ = $(BUILD)/$(TARGET).o \
      $(BUILD)/led.o \
      $(BUILD)/systick.o \
      $(BUILD)/uart.o \
      $(BUILD)/usb.o \
      $(BUILD)/StrPrintf.o \
      $(OBJ_$(BOARD))

all: $(BUILD)/$(TARGET).elf

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@cp $(@:.o=.d) $(@:.o=.P); \
  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  rm -f $(@:.o=.d)
endef

$(OBJ): | $(BUILD)
$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: %.c
	$(call compile_c)

pgm: $(BUILD)/$(TARGET).dfu
ifeq ($(USE_PYDFU),1)
	$(Q)./pydfu.py -u $^
else
	$(Q)dfu-util -a 0 -D $^ -s:leave
endif

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $^ $@

$(BUILD)/$(TARGET).dfu: $(BUILD)/$(TARGET).bin
	$(Q)./dfu.py -b $(FLASH_ADDR):$^ $@

$(LIBOPENCM3_LIBNAME_FULL):
	git submodule update --init
	make -C $(LIBOPENCM3_DIR) TARGETS=stm32/f4

# Building the library generates a bunch of header files, so make each object
# depend on the library to ensure that the headers gets built.
$(OBJ): $(LIBOPENCM3_LIBNAME_FULL)

$(BUILD)/$(TARGET).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(CFLAGS_CORTEX_M4) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

stlink: $(BUILD)/$(TARGET).bin
	$(Q)st-flash --reset write $^ $(FLASH_ADDR)

uart: $(BUILD)/$(TARGET).bin
	$(Q)./stm32loader.py -p /dev/ttyUSB0 -evw $^

run: $(BUILD)/$(TARGET).elf
	$(GDB) -ex 'target extended-remote /dev/ttyACM0' -x gdbinit $<

# Unprotect does a MASS erase, so it shouldn't try to flash as well.
# And on the STM32F103, the ACK never gets received
uart-unprotect:
	$(Q)./stm32loader.py -p /dev/ttyUSB0 -uV

clean:
	$(RM) -rf $(BUILD)
.PHONY: clean

-include $(OBJ:.o=.P)
