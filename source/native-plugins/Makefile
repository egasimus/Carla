#!/usr/bin/make -f
# Makefile for native-plugins #
# --------------------------- #
# Created by falkTX
#

CWD=..
MODULENAME=native-plugins
include ../modules/Makefile.mk

# ----------------------------------------------------------------------------------------------------------------------------

BUILD_C_FLAGS   += -I..
BUILD_CXX_FLAGS += -I.. -isystem $(CWD)/modules -I$(CWD)/modules/distrho

# ----------------------------------------------------------------------------------------------------------------------------
# Flags for ZynAddSubFX

ifeq ($(HAVE_ZYN_DEPS),true)
ZYN_CXX_FLAGS  = $(BUILD_CXX_FLAGS) -isystem zynaddsubfx -Izynaddsubfx/rtosc
ZYN_CXX_FLAGS += $(shell pkg-config --cflags fftw3 mxml zlib)
ZYN_LD_FLAGS   = $(shell pkg-config --libs liblo)
ifeq ($(HAVE_X11),true)
ZYN_CXX_FLAGS += $(shell pkg-config --cflags x11)
ZYN_LD_FLAGS  += $(shell pkg-config --libs x11)
endif
ifeq ($(HAVE_ZYN_UI_DEPS),true)
ifeq ($(HAVE_NTK),true)
FLUID          = ntk-fluid
ZYN_CXX_FLAGS += -DNTK_GUI
ZYN_CXX_FLAGS += $(shell pkg-config --cflags ntk_images ntk)
ZYN_LD_FLAGS  += $(shell pkg-config --libs ntk_images ntk)
else # HAVE_NTK
FLUID          = fluid
ZYN_CXX_FLAGS += -DFLTK_GUI
ZYN_CXX_FLAGS += $(shell fltk-config --use-images --cxxflags)
ZYN_LD_FLAGS  += $(shell fltk-config --use-images --ldstaticflags)
endif # HAVE_NTK
else  # HAVE_ZYN_UI_DEPS
ZYN_CXX_FLAGS += -DNO_UI
endif # HAVE_ZYN_UI_DEPS
endif # HAVE_ZYN_DEPS

# ----------------------------------------------------------------------------------------------------------------------------
# Set targets

TARGETS = $(MODULEDIR)/$(MODULENAME).a

# ----------------------------------------------------------------------------------------------------------------------------
# Set objects

OBJS = \
	$(OBJDIR)/_all.c.o \
	$(OBJDIR)/_data.cpp.o

# ----------------------------------------------------------------------------------------------------------------------------
# Simple plugins

OBJS += \
	$(OBJDIR)/bypass.c.o \
	$(OBJDIR)/lfo.c.o \
	$(OBJDIR)/midi-channel-filter.c.o \
	$(OBJDIR)/midi-gain.c.o \
	$(OBJDIR)/midi-join.c.o \
	$(OBJDIR)/midi-split.c.o \
	$(OBJDIR)/midi-through.c.o \
	$(OBJDIR)/midi-transpose.c.o \
	$(OBJDIR)/nekofilter.c.o

OBJS += \
	$(OBJDIR)/audio-file.cpp.o \
	$(OBJDIR)/bigmeter.cpp.o \
	$(OBJDIR)/midi-file.cpp.o \
	$(OBJDIR)/midi-sequencer.cpp.o \
	$(OBJDIR)/notes.cpp.o

# ----------------------------------------------------------------------------------------------------------------------------
# DISTRHO plugins

OBJS += \
	$(OBJDIR)/distrho-3bandeq.cpp.o \
	$(OBJDIR)/distrho-3bandsplitter.cpp.o \
	$(OBJDIR)/distrho-mverb.cpp.o \
	$(OBJDIR)/distrho-nekobi.cpp.o \
	$(OBJDIR)/distrho-pingpongpan.cpp.o

ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_PROJECTM),true)
OBJS += $(OBJDIR)/distrho-prom.cpp.o
endif
endif

# ----------------------------------------------------------------------------------------------------------------------------
# DISTRHO plugins (Juice)

OBJS += \
	$(OBJDIR)/distrho-vectorjuice.cpp.o \
	$(OBJDIR)/distrho-wobblejuice.cpp.o

# ----------------------------------------------------------------------------------------------------------------------------
# ZynAddSubFX

ifeq ($(HAVE_ZYN_DEPS),true)
OBJS += \
	$(OBJDIR)/zynaddsubfx-fx.cpp.o \
	$(OBJDIR)/zynaddsubfx-synth.cpp.o \
	$(OBJDIR)/zynaddsubfx-src.cpp.o

ifeq ($(HAVE_ZYN_UI_DEPS),true)
TARGETS += resources/zynaddsubfx-ui$(APP_EXT)

ZYN_UI_FILES_CPP = \
	zynaddsubfx/UI/ADnoteUI.cpp \
	zynaddsubfx/UI/BankUI.cpp \
	zynaddsubfx/UI/ConfigUI.cpp \
	zynaddsubfx/UI/EffUI.cpp \
	zynaddsubfx/UI/EnvelopeUI.cpp \
	zynaddsubfx/UI/FilterUI.cpp \
	zynaddsubfx/UI/LFOUI.cpp \
	zynaddsubfx/UI/MasterUI.cpp \
	zynaddsubfx/UI/MicrotonalUI.cpp \
	zynaddsubfx/UI/OscilGenUI.cpp \
	zynaddsubfx/UI/PADnoteUI.cpp \
	zynaddsubfx/UI/PartUI.cpp \
	zynaddsubfx/UI/PresetsUI.cpp \
	zynaddsubfx/UI/ResonanceUI.cpp \
	zynaddsubfx/UI/SUBnoteUI.cpp \
	zynaddsubfx/UI/VirKeyboard.cpp

ZYN_UI_FILES_H = \
	zynaddsubfx/UI/ADnoteUI.h \
	zynaddsubfx/UI/BankUI.h \
	zynaddsubfx/UI/ConfigUI.h \
	zynaddsubfx/UI/EffUI.h \
	zynaddsubfx/UI/EnvelopeUI.h \
	zynaddsubfx/UI/FilterUI.h \
	zynaddsubfx/UI/LFOUI.h \
	zynaddsubfx/UI/MasterUI.h \
	zynaddsubfx/UI/MicrotonalUI.h \
	zynaddsubfx/UI/OscilGenUI.h \
	zynaddsubfx/UI/PADnoteUI.h \
	zynaddsubfx/UI/PartUI.h \
	zynaddsubfx/UI/PresetsUI.h \
	zynaddsubfx/UI/ResonanceUI.h \
	zynaddsubfx/UI/SUBnoteUI.h \
	zynaddsubfx/UI/VirKeyboard.h
endif
endif

# ----------------------------------------------------------------------------------------------------------------------------
# Experimental plugins

ifeq ($(EXPERIMENTAL_PLUGINS),true)
OBJS += \
	$(OBJDIR)/zita-at1.cpp.o \
	$(OBJDIR)/zita-bls1.cpp.o \
	$(OBJDIR)/zita-rev1.cpp.o

TARGETS += resources/rev1-ui$(APP_EXT)
endif

# ----------------------------------------------------------------------------------------------------------------------------

all: $(TARGETS)

# ----------------------------------------------------------------------------------------------------------------------------

clean:
	rm -f $(OBJDIR)/*.o $(MODULEDIR)/$(MODULENAME)*.a $(ZYN_UI_FILES_H) $(ZYN_UI_FILES_CPP) $(TARGETS)

debug:
	$(MAKE) DEBUG=true

# ----------------------------------------------------------------------------------------------------------------------------

$(MODULEDIR)/$(MODULENAME).a: $(OBJS)
	-@mkdir -p $(MODULEDIR)
	@echo "Creating $(MODULENAME).a"
	@rm -f $@
	@$(AR) crs $@ $^

resources/rev1-ui$(APP_EXT): $(OBJDIR)/zita-rev1-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking zita-rev1-ui"
	@$(CXX) $^ $(LINK_FLAGS) -lclxclient -lclthreads $(shell pkg-config --libs cairo libpng12 x11 xft zlib) -ldl -lpthread -lrt -o $@

resources/zynaddsubfx-ui$(APP_EXT): $(OBJDIR)/zynaddsubfx-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking zynaddsubfx-ui"
	@$(CXX) $^ $(ZYN_LD_FLAGS) -ldl -lpthread -o $@

# ----------------------------------------------------------------------------------------------------------------------------

zynaddsubfx/UI/%.cpp: zynaddsubfx/UI/%.fl
	@echo "Generating $@/h"
	@$(FLUID) -c -o zynaddsubfx/UI/$*.cpp -h zynaddsubfx/UI/$*.h $<

zynaddsubfx/UI/%.h: zynaddsubfx/UI/%.fl
	@echo "Generating $@/cpp"
	@$(FLUID) -c -o zynaddsubfx/UI/$*.cpp -h zynaddsubfx/UI/$*.h $<

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/_all.c.o: _all.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(OBJDIR)/%.c.o: %.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CC) $< $(BUILD_C_FLAGS) -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/distrho-3bandeq.cpp.o: distrho-3bandeq.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_3BandEQ -Idistrho-3bandeq -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-3bandsplitter.cpp.o: distrho-3bandsplitter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_3BandSplitter -Idistrho-3bandsplitter -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-mverb.cpp.o: distrho-mverb.cpp
	-@mkdir -p $(OBJDIR)
	# FIXME - fix mverb strict warnings
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_MVerb -Idistrho-mverb -I$(CWD)/modules/dgl -Wno-conversion -Wno-effc++ -Wno-shadow -c -o $@

$(OBJDIR)/distrho-nekobi.cpp.o: distrho-nekobi.cpp
	-@mkdir -p $(OBJDIR)
	# FIXME - fix nekobi strict warnings
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_Nekobi -Idistrho-nekobi -I$(CWD)/modules/dgl -w -c -o $@

$(OBJDIR)/distrho-pingpongpan.cpp.o: distrho-pingpongpan.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_PingPongPan -Idistrho-pingpongpan -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-prom.cpp.o: distrho-prom.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(PROJECTM_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_ProM -Idistrho-prom -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/distrho-vectorjuice.cpp.o: distrho-vectorjuice.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_VectorJuice -Idistrho-vectorjuice -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-wobblejuice.cpp.o: distrho-wobblejuice.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_WobbleJuice -Idistrho-wobblejuice -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/zynaddsubfx-fx.cpp.o: zynaddsubfx-fx.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -c -o $@

$(OBJDIR)/zynaddsubfx-synth.cpp.o: zynaddsubfx-synth.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -c -o $@

$(OBJDIR)/zynaddsubfx-src.cpp.o: zynaddsubfx-src.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -w -c -o $@

$(OBJDIR)/zynaddsubfx-ui.cpp.o: zynaddsubfx-ui.cpp $(ZYN_UI_FILES_H) $(ZYN_UI_FILES_CPP)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -w -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/zita-at1.cpp.o: zita-at1.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(shell pkg-config --cflags fftw3f x11 xft) -Wno-unused-parameter -Wno-unused-result -c -o $@

$(OBJDIR)/zita-bls1.cpp.o: zita-bls1.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(shell pkg-config --cflags fftw3f x11 xft) -Wno-unused-parameter -Wno-unused-result -c -o $@

$(OBJDIR)/zita-rev1.cpp.o: zita-rev1.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(shell pkg-config --cflags fftw3f) -Wno-unused-parameter -c -o $@

$(OBJDIR)/zita-rev1-ui.cpp.o: zita-rev1-ui.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(shell pkg-config --cflags cairo libpng12 x11 xft zlib) -Wno-unused-parameter -Wno-ignored-qualifiers -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/%.cpp.o: %.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

# ----------------------------------------------------------------------------------------------------------------------------

-include $(OBJS:%.o=%.d)
-include $(OBJDIR)/zita-rev-ui.cpp.d
-include $(OBJDIR)/zynaddsubfx-ui.cpp.d

# ----------------------------------------------------------------------------------------------------------------------------
