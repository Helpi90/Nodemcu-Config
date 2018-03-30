#
# Copyright (c) 2015 MaiKe Labs
#
#

ifndef PDIR
PDIR=.
endif

SDK_BASE ?= ../..
TARGET ?= eagle

RTOS_SDK_BASE = $(SDK_BASE)/rtos/esp8266
TOOLCHAIN_DIR ?= $(SDK_BASE)/toolchain

GENIMAGEOPTION = -ff 40m -fm dio -fs 32m

ADDR_FW1 = 0x00000
ADDR_FW2 = 0x09000

FW_FILE_1_ARGS	= -bo $@ -bm dio -bf 40 -bz 4M -bs .text -bs .data -bs .rodata -bc -ec
FW_FILE_2_ARGS	= -es .irom0.text $@ -ec

# esptool path and port
ifeq ($(OS),Windows_NT)
	ESPTOOL		?= $(SDK_BASE)/toolchain/bin/esptool.exe
	FW_TOOL		?= $(SDK_BASE)/toolchain/bin/esptool.py
	ESPPORT		?= COM3
else
	ESPTOOL		?= $(SDK_BASE)/toolchain/bin/esptool
	FW_TOOL		?= $(SDK_BASE)/toolchain/bin/esptool.py

	UNAME_S		:= $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		ESPPORT		?= /dev/cu.SLAB_USBtoUART
		#ESPPORT		?= /dev/cu.usbserial-A601EONX
		#ESPPORT			?= /dev/tty.usbserial-A901J0LC 
	endif
	ifeq ($(UNAME_S),Linux)
		ESPPORT		?= /dev/ttyUSB0
	endif
endif

# select which tools to use as compiler, librarian and linker
CC := $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
AR := $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar
LD := $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
NM := $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-nm
CPP = $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-cpp
OBJCOPY = $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-objcopy
OBJDUMP := $(TOOLCHAIN_DIR)/xtensa-lx106-elf/bin/xtensa-lx106-elf-objdump

CCFLAGS += -std=gnu90 -Wno-pointer-sign -mno-target-align -mno-serialize-volatile -foptimize-register-move
#
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
# https://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/Xtensa-Options.html#Xtensa-Options
#

FIRMWAREDIR := bin
DEFAULTBIN := $(PDIR)/$(FIRMWAREDIR)/esp_init_data_default.bin
DEFAULTADDR := 0x3FC000
BLANKBIN := $(PDIR)/$(FIRMWAREDIR)/blank.bin
BLANKADDR := 0x3FE000

CSRCS ?= $(wildcard *.c)
ASRCs ?= $(wildcard *.s)
ASRCS ?= $(wildcard *.S)
SUBDIRS ?= $(patsubst %/,%,$(dir $(wildcard */Makefile)))

ODIR := .output
OBJODIR := $(ODIR)/$(TARGET)/obj

OBJS := $(CSRCS:%.c=$(OBJODIR)/%.o) \
        $(ASRCs:%.s=$(OBJODIR)/%.o) \
        $(ASRCS:%.S=$(OBJODIR)/%.o)

DEPS := $(CSRCS:%.c=$(OBJODIR)/%.d) \
        $(ASRCs:%.s=$(OBJODIR)/%.d) \
        $(ASRCS:%.S=$(OBJODIR)/%.d)

LIBODIR := $(ODIR)/$(TARGET)/lib
OLIBS := $(GEN_LIBS:%=$(LIBODIR)/%)

IMAGEODIR := $(ODIR)/$(TARGET)/image
OIMAGES := $(GEN_IMAGES:%=$(IMAGEODIR)/%)

BINODIR := $(ODIR)/$(TARGET)/bin
OBINS := $(GEN_BINS:%=$(BINODIR)/%)

#OUTBIN1 := $(ODIR)/$(TARGET)/$(ADDR_FW1).bin
#OUTBIN2 := $(ODIR)/$(TARGET)/$(ADDR_FW2).bin
OUTBIN1 := $(BINODIR)/$(ADDR_FW1).bin
OUTBIN2 := $(BINODIR)/$(ADDR_FW2).bin

CCFLAGS += \
	-Wundef			\
	-Wpointer-arith	\
	-Werror	\
	-Wl,-EL	\
	-fno-inline-functions	\
	-nostdlib	\
	-mlongcalls	\
	-mtext-section-literals

CFLAGS = $(CCFLAGS) $(DEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)
DFLAGS = $(CCFLAGS) $(DDEFINES) $(EXTRA_CCFLAGS) $(INCLUDES)

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

define ShortcutRule
$(1): .subdirs $(2)/$(1)
endef

define MakeLibrary
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(LIBODIR)/$(1).a: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(LIBODIR)
	$$(if $$(filter %.a,$$?),mkdir -p $$(EXTRACT_DIR)_$(1))
	$$(if $$(filter %.a,$$?),cd $$(EXTRACT_DIR)_$(1); $$(foreach lib,$$(filter %.a,$$?),$$(AR) xo $$(UP_EXTRACT_DIR)/$$(lib);))
	$$(AR) ru $$@ $$(filter %.o,$$?) $$(if $$(filter %.a,$$?),$$(EXTRACT_DIR)_$(1)/*.o)
	$$(if $$(filter %.a,$$?),$$(RM) -r $$(EXTRACT_DIR)_$(1))
endef

define MakeImage
DEP_LIBS_$(1) = $$(foreach lib,$$(filter %.a,$$(COMPONENTS_$(1))),$$(dir $$(lib))$$(LIBODIR)/$$(notdir $$(lib)))
DEP_OBJS_$(1) = $$(foreach obj,$$(filter %.o,$$(COMPONENTS_$(1))),$$(dir $$(obj))$$(OBJODIR)/$$(notdir $$(obj)))
$$(IMAGEODIR)/$(1).out: $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1)) $$(DEPENDS_$(1))
	@mkdir -p $$(IMAGEODIR)
	$$(CC) $$(LDFLAGS) $$(if $$(LINKFLAGS_$(1)),$$(LINKFLAGS_$(1)),$$(LINKFLAGS_DEFAULT) $$(OBJS) $$(DEP_OBJS_$(1)) $$(DEP_LIBS_$(1))) -o $$@
endef

all: .subdirs $(OBJS) $(OLIBS) $(OIMAGES) $(OBINS) $(SPECIAL_MKTARGETS)

clean:
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clean;)
	$(RM) -r $(ODIR)/$(TARGET)

clobber: $(SPECIAL_CLOBBER)
	$(foreach d, $(SUBDIRS), $(MAKE) -C $(d) clobber;)
	$(RM) -r $(ODIR)

.subdirs:
	@set -e; $(foreach d, $(SUBDIRS), $(MAKE) -C $(d);)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@mkdir -p $(OBJODIR);
	$(CC) $(if $(findstring $<,$(DSRCS)),$(DFLAGS),$(CFLAGS)) $(COPTS_$(*F)) -o $@ -c $<

$(OBJODIR)/%.d: %.c
	@mkdir -p $(OBJODIR);
	@echo DEPEND: $(CC) -M $(CFLAGS) $<
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.s
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJODIR)/%.d: %.s
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJODIR)/%.o: %.S
	@mkdir -p $(OBJODIR);
	$(CC) $(CFLAGS) -D__ASSEMBLER__ -o $@ -c $<

$(OBJODIR)/%.d: %.S
	@mkdir -p $(OBJODIR); \
	set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\.o\)[ :]*,$(OBJODIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(foreach lib,$(GEN_LIBS),$(eval $(call ShortcutRule,$(lib),$(LIBODIR))))
$(foreach image,$(GEN_IMAGES),$(eval $(call ShortcutRule,$(image),$(IMAGEODIR))))
$(foreach bin,$(GEN_BINS),$(eval $(call ShortcutRule,$(bin),$(BINODIR))))
$(foreach lib,$(GEN_LIBS),$(eval $(call MakeLibrary,$(basename $(lib)))))
$(foreach image,$(GEN_IMAGES),$(eval $(call MakeImage,$(basename $(image)))))

INCLUDES := $(INCLUDES) -I $(PDIR)/include -I $(PDIR)/include/$(TARGET) -I $(RTOS_SDK_BASE)/extra_include
INCLUDES += -I $(PDIR)/include/lwip -I $(PDIR)/include/lwip/ipv4 -I $(PDIR)/include/lwip/ipv6 -I $(PDIR)/include/espressif

$(OUTBIN2): app/$(IMAGEODIR)/eagle.app.v6.out
	$(vecho) "FW $@"
#	$(Q) mkdir -p $(ODIR)
#	$(Q) $(FW_TOOL) -eo $< $(FW_FILE_1_ARGS)

$(OUTBIN1): app/$(IMAGEODIR)/eagle.app.v6.out
	$(vecho) "FW $@"
#	$(Q) mkdir -p $(ODIR)
#	$(Q) $(FW_TOOL) -eo $< $(FW_FILE_2_ARGS)

flash: $(OUTBIN1) $(USERFBIN) $(OUTBIN2) $(DEFAULTBIN) $(BLANKBIN) zero
	$(ESPTOOL) -cd nodemcu -cb 115200 -cp $(ESPPORT) -ca $(ADDR_FW1) -cf app/$(OUTBIN1) -ca $(ADDR_FW2) -cf app/$(OUTBIN2) -ca $(DEFAULTADDR) -cf $(DEFAULTBIN) -ca $(BLANKADDR) -cf $(BLANKBIN)

zero: $(CLREEPBIN) $(DEFAULTBIN) $(BLANKBIN)
	$(ESPTOOL) -cd nodemcu -cb 115200 -cp $(ESPPORT) -ca $(DEFAULTADDR) -cf $(DEFAULTBIN) -ca $(BLANKADDR) -cf $(BLANKBIN)

$(BINODIR)/%.bin:
	@echo Generating image file!
	@mkdir -p $(BINODIR)
	@$(FW_TOOL) elf2image -es user_start_trampoline -o $(BINODIR)/ $(GENIMAGEOPTION) $(shell pwd)/$(IMAGEODIR)/eagle.app.v6.out
