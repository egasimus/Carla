/*
 * Carla Plugin Host
 * Copyright (C) 2011-2014 Filipe Coelho <falktx@falktx.com>
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

#include "CarlaEngineInternal.hpp"
#include "CarlaPlugin.hpp"

CARLA_BACKEND_START_NAMESPACE

// -----------------------------------------------------------------------
// Engine Internal helper macro, sets lastError and returns false/NULL

#define CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(cond, err)  if (! (cond)) { carla_safe_assert(#cond, __FILE__, __LINE__); lastError = err; return false;   }
#define CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERRN(cond, err) if (! (cond)) { carla_safe_assert(#cond, __FILE__, __LINE__); lastError = err; return nullptr; }

// -----------------------------------------------------------------------
// InternalEvents

EngineInternalEvents::EngineInternalEvents() noexcept
    : in(nullptr),
      out(nullptr) {}

EngineInternalEvents::~EngineInternalEvents() noexcept
{
    CARLA_SAFE_ASSERT(in == nullptr);
    CARLA_SAFE_ASSERT(out == nullptr);
}

void EngineInternalEvents::clear() noexcept
{
    if (in != nullptr)
    {
        delete[] in;
        in = nullptr;
    }

    if (out != nullptr)
    {
        delete[] out;
        out = nullptr;
    }
}

// -----------------------------------------------------------------------
// InternalTime

EngineInternalTime::EngineInternalTime() noexcept
    : playing(false),
      frame(0) {}

// -----------------------------------------------------------------------
// NextAction

EngineNextAction::EngineNextAction() noexcept
    : opcode(kEnginePostActionNull),
      pluginId(0),
      value(0),
      mutex() {}

EngineNextAction::~EngineNextAction() noexcept
{
    CARLA_SAFE_ASSERT(opcode == kEnginePostActionNull);
}

void EngineNextAction::ready() const noexcept
{
    mutex.lock();
    mutex.unlock();
}

void EngineNextAction::clearAndReset() noexcept
{
    mutex.lock();
    opcode   = kEnginePostActionNull;
    pluginId = 0;
    value    = 0;
    mutex.unlock();
}

// -----------------------------------------------------------------------
// CarlaEngine::ProtectedData

CarlaEngine::ProtectedData::ProtectedData(CarlaEngine* const engine) noexcept
    : thread(engine),
#ifdef HAVE_LIBLO
      osc(engine),
      oscData(nullptr),
#endif
      callback(nullptr),
      callbackPtr(nullptr),
      fileCallback(nullptr),
      fileCallbackPtr(nullptr),
      hints(0x0),
      bufferSize(0),
      sampleRate(0.0),
      aboutToClose(false),
      isIdling(0),
      curPluginCount(0),
      maxPluginNumber(0),
      nextPluginId(0),
      envMutex(),
      lastError(),
      name(),
      options(),
      timeInfo(),
#ifndef BUILD_BRIDGE
      plugins(nullptr),
#endif
      events(),
#ifndef BUILD_BRIDGE
      graph(engine),
#endif
      time(),
      nextAction()
{
#ifdef BUILD_BRIDGE
    carla_zeroStruct(plugins, 1);
#endif
}

CarlaEngine::ProtectedData::~ProtectedData() noexcept
{
    CARLA_SAFE_ASSERT(curPluginCount == 0);
    CARLA_SAFE_ASSERT(maxPluginNumber == 0);
    CARLA_SAFE_ASSERT(nextPluginId == 0);
    CARLA_SAFE_ASSERT(isIdling == 0);
#ifndef BUILD_BRIDGE
    CARLA_SAFE_ASSERT(plugins == nullptr);
#endif
}

// -----------------------------------------------------------------------

bool CarlaEngine::ProtectedData::init(const char* const clientName)
{
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(name.isEmpty(), "Invalid engine internal data (err #1)");
#ifdef HAVE_LIBLO
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(oscData == nullptr, "Invalid engine internal data (err #2)");
#endif
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(events.in  == nullptr, "Invalid engine internal data (err #4)");
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(events.out == nullptr, "Invalid engine internal data (err #5)");
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(clientName != nullptr && clientName[0] != '\0', "Invalid client name");
#ifndef BUILD_BRIDGE
    CARLA_SAFE_ASSERT_RETURN_INTERNAL_ERR(plugins == nullptr, "Invalid engine internal data (err #3)");
#endif

    aboutToClose   = false;
    curPluginCount = 0;
    nextPluginId   = 0;

    switch (options.processMode)
    {
    case ENGINE_PROCESS_MODE_CONTINUOUS_RACK:
        maxPluginNumber = MAX_RACK_PLUGINS;
        options.forceStereo = true; // just in case
        break;
    case ENGINE_PROCESS_MODE_PATCHBAY:
        maxPluginNumber = MAX_PATCHBAY_PLUGINS;
        break;
    case ENGINE_PROCESS_MODE_BRIDGE:
        maxPluginNumber = 1;
        break;
    default:
        maxPluginNumber = MAX_DEFAULT_PLUGINS;
        break;
    }

    switch (options.processMode)
    {
    case ENGINE_PROCESS_MODE_CONTINUOUS_RACK:
    case ENGINE_PROCESS_MODE_PATCHBAY:
    case ENGINE_PROCESS_MODE_BRIDGE:
        events.in  = new EngineEvent[kMaxEngineEventInternalCount];
        events.out = new EngineEvent[kMaxEngineEventInternalCount];
        break;
    default:
        break;
    }

    nextPluginId = maxPluginNumber;

    name = clientName;
    name.toBasic();

    timeInfo.clear();

#ifdef HAVE_LIBLO
    osc.init(clientName);
# ifndef BUILD_BRIDGE
    oscData = osc.getControlData();
# endif
#endif

#ifndef BUILD_BRIDGE
    plugins = new EnginePluginData[maxPluginNumber];
    carla_zeroStruct(plugins, maxPluginNumber);
#endif

    nextAction.ready();
    thread.startThread();

    return true;
}

void CarlaEngine::ProtectedData::close()
{
    CARLA_SAFE_ASSERT(name.isNotEmpty());
    CARLA_SAFE_ASSERT(plugins != nullptr);
    CARLA_SAFE_ASSERT(nextPluginId == maxPluginNumber);
    CARLA_SAFE_ASSERT(nextAction.opcode == kEnginePostActionNull);

    aboutToClose = true;

    thread.stopThread(500);
    nextAction.ready();

#ifdef HAVE_LIBLO
    osc.close();
    oscData = nullptr;
#endif

    aboutToClose    = false;
    curPluginCount  = 0;
    maxPluginNumber = 0;
    nextPluginId    = 0;

#ifndef BUILD_BRIDGE
    if (plugins != nullptr)
    {
        delete[] plugins;
        plugins = nullptr;
    }
#endif

    events.clear();
    name.clear();
}

// -----------------------------------------------------------------------

#ifndef BUILD_BRIDGE
void CarlaEngine::ProtectedData::doPluginRemove() noexcept
{
    CARLA_SAFE_ASSERT_RETURN(curPluginCount > 0,);
    CARLA_SAFE_ASSERT_RETURN(nextAction.pluginId < curPluginCount,);
    --curPluginCount;

    // move all plugins 1 spot backwards
    for (uint i=nextAction.pluginId; i < curPluginCount; ++i)
    {
        CarlaPlugin* const plugin(plugins[i+1].plugin);

        CARLA_SAFE_ASSERT_BREAK(plugin != nullptr);

        plugin->setId(i);

        plugins[i].plugin      = plugin;
        plugins[i].insPeak[0]  = 0.0f;
        plugins[i].insPeak[1]  = 0.0f;
        plugins[i].outsPeak[0] = 0.0f;
        plugins[i].outsPeak[1] = 0.0f;
    }

    const uint id(curPluginCount);

    // reset last plugin (now removed)
    plugins[id].plugin      = nullptr;
    plugins[id].insPeak[0]  = 0.0f;
    plugins[id].insPeak[1]  = 0.0f;
    plugins[id].outsPeak[0] = 0.0f;
    plugins[id].outsPeak[1] = 0.0f;
}

void CarlaEngine::ProtectedData::doPluginsSwitch() noexcept
{
    CARLA_SAFE_ASSERT_RETURN(curPluginCount >= 2,);

    const uint idA(nextAction.pluginId);
    const uint idB(nextAction.value);

    CARLA_SAFE_ASSERT_RETURN(idA < curPluginCount,);
    CARLA_SAFE_ASSERT_RETURN(idB < curPluginCount,);
    CARLA_SAFE_ASSERT_RETURN(plugins[idA].plugin != nullptr,);
    CARLA_SAFE_ASSERT_RETURN(plugins[idB].plugin != nullptr,);

#if 0
    std::swap(plugins[idA].plugin, plugins[idB].plugin);
#else
    CarlaPlugin* const tmp(plugins[idA].plugin);

    plugins[idA].plugin = plugins[idB].plugin;
    plugins[idB].plugin = tmp;
#endif
}
#endif

void CarlaEngine::ProtectedData::doNextPluginAction(const bool unlock) noexcept
{
    switch (nextAction.opcode)
    {
    case kEnginePostActionNull:
        break;
    case kEnginePostActionZeroCount:
        curPluginCount = 0;
        break;
#ifndef BUILD_BRIDGE
    case kEnginePostActionRemovePlugin:
        doPluginRemove();
        break;
    case kEnginePostActionSwitchPlugins:
        doPluginsSwitch();
        break;
#endif
    }

    nextAction.opcode   = kEnginePostActionNull;
    nextAction.pluginId = 0;
    nextAction.value    = 0;

    if (unlock)
    {
        nextAction.mutex.tryLock();
        nextAction.mutex.unlock();
    }
}

// -----------------------------------------------------------------------
// PendingRtEventsRunner

PendingRtEventsRunner::PendingRtEventsRunner(CarlaEngine* const engine) noexcept
    : pData(engine->pData) {}

PendingRtEventsRunner::~PendingRtEventsRunner() noexcept
{
    pData->doNextPluginAction(true);

    if (pData->time.playing)
        pData->time.frame += pData->bufferSize;

    if (pData->options.transportMode == ENGINE_TRANSPORT_MODE_INTERNAL)
    {
        pData->timeInfo.playing = pData->time.playing;
        pData->timeInfo.frame   = pData->time.frame;
    }
}

// -----------------------------------------------------------------------
// ScopedActionLock

ScopedActionLock::ScopedActionLock(CarlaEngine* const engine, const EnginePostAction action, const uint pluginId, const uint value, const bool lockWait) noexcept
    : pData(engine->pData)
{
    CARLA_SAFE_ASSERT_RETURN(action != kEnginePostActionNull,);

    pData->nextAction.mutex.lock();

    CARLA_SAFE_ASSERT_RETURN(pData->nextAction.opcode == kEnginePostActionNull,);

    pData->nextAction.opcode   = action;
    pData->nextAction.pluginId = pluginId;
    pData->nextAction.value    = value;

    if (lockWait)
    {
        // block wait for unlock on processing side
        carla_stdout("ScopedPluginAction(%i) - blocking START", pluginId);
        pData->nextAction.mutex.lock();
        carla_stdout("ScopedPluginAction(%i) - blocking DONE", pluginId);
    }
    else
    {
        pData->doNextPluginAction(false);
    }
}

ScopedActionLock::~ScopedActionLock() noexcept
{
    CARLA_SAFE_ASSERT(pData->nextAction.opcode == kEnginePostActionNull);
    pData->nextAction.mutex.tryLock();
    pData->nextAction.mutex.unlock();
}

// -----------------------------------------------------------------------
// ScopedEngineEnvironmentLocker

ScopedEngineEnvironmentLocker::ScopedEngineEnvironmentLocker(CarlaEngine* const engine) noexcept
    : pData(engine->pData)
{
    pData->envMutex.lock();
}

ScopedEngineEnvironmentLocker::~ScopedEngineEnvironmentLocker() noexcept
{
    pData->envMutex.unlock();
}

// -----------------------------------------------------------------------

CARLA_BACKEND_END_NAMESPACE
