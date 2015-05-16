/*
 * Carla Native Plugins
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "CarlaDefines.h"

#ifdef CARLA_OS_WIN
#define errx(...)
#define warnx(...)
#endif

#define PLUGINVERSION
#define SOURCE_DIR "/usr/share/zynaddsubfx/examples"
#undef override

// base c-style headers
#include "zynaddsubfx/tlsf/tlsf.h"
#include "zynaddsubfx/rtosc/rtosc.h"

// C-code includes
extern "C" {
#include "zynaddsubfx/tlsf/tlsf.c"
#include "zynaddsubfx/rtosc/dispatch.c"
#include "zynaddsubfx/rtosc/rtosc.c"
}

// rtosc includes
#include "zynaddsubfx/rtosc/cpp/midimapper.cpp"
#include "zynaddsubfx/rtosc/cpp/miditable.cpp"
#include "zynaddsubfx/rtosc/cpp/ports.cpp"
#include "zynaddsubfx/rtosc/cpp/subtree-serialize.cpp"
#include "zynaddsubfx/rtosc/cpp/thread-link.cpp"
#include "zynaddsubfx/rtosc/cpp/undo-history.cpp"

// zynaddsubfx includes
#include "zynaddsubfx/DSP/AnalogFilter.cpp"
#include "zynaddsubfx/DSP/FFTwrapper.cpp"
#include "zynaddsubfx/DSP/Filter.cpp"
#include "zynaddsubfx/DSP/FormantFilter.cpp"
#include "zynaddsubfx/DSP/SVFilter.cpp"
#include "zynaddsubfx/DSP/Unison.cpp"
#include "zynaddsubfx/Effects/Alienwah.cpp"
#include "zynaddsubfx/Effects/Chorus.cpp"
#include "zynaddsubfx/Effects/Distorsion.cpp"
#include "zynaddsubfx/Effects/DynamicFilter.cpp"
#include "zynaddsubfx/Effects/Echo.cpp"
#include "zynaddsubfx/Effects/Effect.cpp"
#include "zynaddsubfx/Effects/EffectLFO.cpp"
#include "zynaddsubfx/Effects/EffectMgr.cpp"
#include "zynaddsubfx/Effects/EQ.cpp"
#include "zynaddsubfx/Effects/Phaser.cpp"
#include "zynaddsubfx/Effects/Reverb.cpp"
#include "zynaddsubfx/Misc/Allocator.cpp"
#include "zynaddsubfx/Misc/Bank.cpp"
#include "zynaddsubfx/Misc/Config.cpp"
#include "zynaddsubfx/Misc/Master.cpp"
#include "zynaddsubfx/Misc/Microtonal.cpp"
#include "zynaddsubfx/Misc/MiddleWare.cpp"
#include "zynaddsubfx/Misc/Part.cpp"
#include "zynaddsubfx/Misc/PresetExtractor.cpp"
#include "zynaddsubfx/Misc/Recorder.cpp"
//#include "zynaddsubfx/Misc/Stereo.cpp"
#include "zynaddsubfx/Misc/Util.cpp"
#include "zynaddsubfx/Misc/WavFile.cpp"
#include "zynaddsubfx/Misc/WaveShapeSmps.cpp"
#include "zynaddsubfx/Misc/XMLwrapper.cpp"
#include "zynaddsubfx/Params/ADnoteParameters.cpp"
#include "zynaddsubfx/Params/Controller.cpp"
#include "zynaddsubfx/Params/EnvelopeParams.cpp"
#include "zynaddsubfx/Params/FilterParams.cpp"
#include "zynaddsubfx/Params/LFOParams.cpp"
#include "zynaddsubfx/Params/PADnoteParameters.cpp"
#include "zynaddsubfx/Params/Presets.cpp"
#include "zynaddsubfx/Params/PresetsArray.cpp"
#include "zynaddsubfx/Params/PresetsStore.cpp"
#include "zynaddsubfx/Params/SUBnoteParameters.cpp"
#include "zynaddsubfx/Synth/ADnote.cpp"
#include "zynaddsubfx/Synth/Envelope.cpp"
#include "zynaddsubfx/Synth/LFO.cpp"
#include "zynaddsubfx/Synth/OscilGen.cpp"
#include "zynaddsubfx/Synth/PADnote.cpp"
#include "zynaddsubfx/Synth/Resonance.cpp"
#include "zynaddsubfx/Synth/SUBnote.cpp"
#include "zynaddsubfx/Synth/SynthNote.cpp"
#include "zynaddsubfx/UI/ConnectionDummy.cpp"

// Dummy variables and functions for linking purposes
// const char* instance_name = nullptr;
// SYNTH_T* synth = nullptr;
class WavFile;
namespace Nio {
   void masterSwap(Master*){}
//    bool start(void){return 1;}
//    void stop(void){}
//    bool setSource(std::string){return true;}
//    bool setSink(std::string){return true;}
//    std::set<std::string> getSources(void){return std::set<std::string>();}
//    std::set<std::string> getSinks(void){return std::set<std::string>();}
//    std::string getSource(void){return "";}
//    std::string getSink(void){return "";}
   void waveNew(WavFile*){}
   void waveStart(){}
   void waveStop(){}
//    void waveEnd(void){}
}
