BUILDROOT ?= ./

ifndef CONFIG
  CONFIG=Release
endif

ifeq ($(CONFIG),Debug)
CPPFLAGS     = -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
EMCCFLAGS   ?= -g
ASFLAGS      = -g
endif

ifeq ($(CONFIG),Release)
CPPFLAGS     = -O2
EMCCFLAGS   ?= -Oz # optimise for size
endif

DEPS       = $(BUILD)/patch.cpp $(BUILD)/patch.h

ifdef FAUST
# options for FAUST compilation
PATCHNAME   ?= $(FAUST)
PATCHCLASS  ?= FaustPatch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
DEPS        += $(BUILD)/$(PATCHFILE)
else ifdef HEAVY
# options for Heavy PD compilation
PATCHNAME   ?= $(HEAVY)
PATCHCLASS  ?= HeavyPatch
PATCHFILE   ?= HeavyPatch.hpp
DEPS        += $(HEAVYDIR)/Heavy_owl.h
HEAVYFILE   ?= $(HEAVY).pd
HEAVYNAME   ?= owl
HEAVYDIR    ?= $(BUILD)/HeavySource
HEAVYSRC    ?= $(BUILDROOT)/HeavySource
CPPFLAGS    += -I$(HEAVYDIR)
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
vpath %.c $(HEAVYDIR)
ifdef HEAVYTOKEN
HEAVYARGS   = -t $(HEAVYTOKEN)
endif
else
# options for C++ compilation
PATCHNAME   ?= "Template"
PATCHCLASS  ?= $(PATCHNAME)Patch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
endif

PATCHIN     ?= 2
PATCHOUT    ?= 2
SLOT        ?= 0
OWLDEVICE   ?= "OWL-MIDI"
BUILD       ?= $(BUILDROOT)/Build

CPPFLAGS += -DARM_CORTEX
CPPFLAGS += -DEXTERNAL_SRAM
CPPFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding
CPPFLAGS += -mtune=cortex-m4
CPPFLAGS += -fpic
CPPFLAGS += -fpie
CPPFLAGS += -fdata-sections
CPPFLAGS += -ffunction-sections
# CPPFLAGS += -munaligned-access
CPPFLAGS += -mno-unaligned-access
# CPPFLAGS += -mlong-calls

# CPPFLAGS += -mpic-data-is-text-relative
CPPFLAGS += -fno-omit-frame-pointer
CPPFLAGS += -flto

LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

LDLIBS   ?= -lm
LDSCRIPT ?= Source/flash.ld
FIRMWARESENDER = Tools/FirmwareSender

C_SRC   = basicmaths.c
CPP_SRC = main.cpp operators.cpp message.cpp StompBox.cpp PatchProcessor.cpp
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp FastFourierTransform.cpp 
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += PatchProgram.cpp

SOURCE       = $(BUILDROOT)/Source
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
LIBSOURCE    = $(BUILDROOT)/LibSource
TESTPATCHES  = $(BUILDROOT)/TestPatches
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -I$(BUILD)
CPPFLAGS += -IOwlPatches
PATCH_C_SRC = $(wildcard $(PATCHSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)
vpath %.c Libraries/syscalls

# emscripten
EMCC      ?= emcc
EMCCFLAGS ?= -fno-rtti -fno-exceptions # -std=c++11
EMCCFLAGS += -IOwlPatches -I$(SOURCE) -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(BUILD) -I$(TESTPATCHES)
EMCCFLAGS += -I$(BUILD)/HeavySource
EMCCFLAGS +=  -ILibraries -ILibraries/KissFFT -DHV_SIMD_NONE
EMCCFLAGS += -Wno-warn-absolute-paths
EMCCFLAGS += -Wno-unknown-warning-option
EMCCFLAGS += -Wno-c++11-extensions
EMCCFLAGS += --memory-init-file 0 # don't create separate memory init file .mem
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus','_WEB_getButtons','_WEB_setButtons']"""
EMCC_SRC   = $(SOURCE)/PatchProgram.cpp $(SOURCE)/PatchProcessor.cpp $(SOURCE)/message.cpp
EMCC_SRC  += WebSource/web.cpp
EMCC_SRC  += $(LIBSOURCE)/basicmaths.c $(LIBSOURCE)/StompBox.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp $(LIBSOURCE)/FastFourierTransform.cpp $(LIBSOURCE)/Envelope.cpp $(LIBSOURCE)/VoltsPerOctave.cpp $(LIBSOURCE)/WavetableOscillator.cpp $(LIBSOURCE)/PolyBlepOscillator.cpp
EMCC_SRC  += $(PATCH_CPP_SRC) $(PATCH_C_SRC)
EMCC_SRC  += Libraries/KissFFT/kiss_fft.c
EMCC_SRC  += $(wildcard $(HEAVYDIR)/*.c)
WEBDIR     = $(BUILD)/web

# JavaScript minifiers
#CLOSURE = java -jar Tools/node_modules/google-closure-compiler/compiler.jar --language_in=ECMASCRIPT5
UGLIFYJS = Tools/node_modules/uglifyjs/bin/uglifyjs

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11

# object files
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)
OBJS += $(BUILD)/startup.o
OBJS += $(BUILD)/libnosys_gnu.o

all: patch

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/common.mk

.PHONY: .FORCE clean realclean run store online docs

.FORCE:
	@echo Building patch $(PATCHNAME)

$(BUILD)/patch.cpp: .FORCE
	@echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $(BUILD)/patch.cpp

$(BUILD)/patch.h: .FORCE
	@echo "#include \"$(PATCHFILE)\"" > $(BUILD)/patch.h

$(BUILD)/startup.o: .FORCE
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/progname.s
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SOURCE)/startup.s -o $@

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SOURCE)/PatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

$(BUILD)/patch.elf: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

as: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

map: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

$(BUILD)/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@faust -I $(PATCHSOURCE) -i -inpl -a owl.cpp -cn $(PATCHNAME)Patch $< -o $@

size: $(BUILD)/patch.elf $(BUILD)/patch.bin
	@$(SIZE) $(BUILD)/patch.elf
	@ls -s --block-size=1 $(BUILD)/patch.bin

patch: $(BUILD)/patch.bin

sysex: patch $(BUILD)/patch.syx

run: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -run

store: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	@doxygen Doxyfile

online:
	@echo "$(ONLINE_INCLUDES)" > $(BUILD)/patch.h
	@echo "$(ONLINE_REGISTER)" > $(BUILD)/patch.cpp
	@echo '.string "OnlineCompiler"' > $(BUILD)/progname.s
	@$(MAKE) $(BUILD)/patch.syx
	@cp $(BUILD)/patch.syx $(BUILD)/online.syx

$(WEBDIR)/patch.js: $(EMCC_SRC) $(DEPS)
	@mkdir -p $(WEBDIR)
	@$(EMCC) $(EMCCFLAGS) $(EMCC_SRC) -o $(WEBDIR)/patch.js
	@cp WebSource/*.js WebSource/*.html WebSource/*.mp3 $(WEBDIR)

$(WEBDIR)/%.min.js: $(WEBDIR)/%.js
	@$(UGLIFYJS) -o $@ $<
#	$(CLOSURE) --js_output_file=$@ $<

web: $(WEBDIR)/patch.js
minify: $(WEBDIR)/patch.min.js

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(HEAVYDIR)/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python ./Tools/Heavy/uploader.py $(HEAVYDIR) -g c -n $(HEAVYNAME) -o $(HEAVYDIR) $(HEAVYARGS)
	@cp $(HEAVYSRC)/HvUtils.h $(HEAVYDIR)

heavy: $(HEAVYDIR)/Heavy_owl.h
	@$(eval HEAVY_SRC = $(wildcard $(HEAVYDIR)/*.c) )
	@$(eval PATCH_OBJS += $(addprefix $(HEAVYDIR)/, $(notdir $(HEAVY_SRC:.c=.o))))
	@make $(PATCH_OBJS)
