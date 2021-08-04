/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h>

#include <public.sdk/source/vst/vstbus.h>

extern "C" {
  
  AgsVstBus* ags_vst_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags)
  {
    return(new Steinberg::Bus());
  }
  
  void ags_vst_bus_delete(AgsVstBus *bus)
  {
    delete bus;
  }

  gboolean ags_vst_bus_is_active(AgsVstBus *bus)
  {
    return(static_cast<gboolean>(bus->isActive()));
  }

  void ags_vst_bus_set_active(AgsVstBus *bus, gboolean state)
  {
    bus->setActive(static_cast<TBool>(state));
  }

  void ags_vst_bus_set_name(AgsVstBus *bus, AgsVstString new_name)
  {
    bus->setName(reinterpret_cast<String>(new_name));
  }

  void ags_vst_bus_set_bus_type(AgsVstBus *bus, AgsVstBusType new_bus_type)
  {
    bus->setBusType(static_cast<BusType>(new_bus_type));
  }

  void ags_vst_bus_set_flags(AgsVstBus *bus, guint32 new_flags)
  {
    bus->setFlags(static_cast<uint32>(new_flags));
  }

  gboolean ags_vst_bus_get_info(AgsVstBus *bus, AgsVstBusInfo **info)
  {
    return(static_cast<gboolean>(bus->getInfo(static_cast<BusInfo&>(info[0])));
  }

  AgsVstEventBus* ags_vst_event_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, gint32 channel_count)
  {
    return(new EventBus(static_cast<TChar*>(name), static_cast<BusType>(bus_type), static_cast<int32>(flags), static_cast<int32>(channel_count)));
  }

  void ags_vst_event_bus_delete(AgsVstEventBus *event_bus)
  {
    delete event_bus;
  }

  gboolean ags_vst_event_bus_get_info(AgsVstEventBus *event_bus, AgsVstBusInfo **info)
  {
    return(static_cast<gboolean>(event_bus->getInfo(static_cast<BusInfo&>(info[0]))));
  }

  AgsVstAudioBus* ags_vst_audio_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, AgsVstSpeakerArrangement arr)
  {
    return(new AudioBus(static_cast<TChar*>(name), static_cast<BusType>(bus_type), static_cast<int32>(flags), arr));
  }

  void ags_vst_audio_bus_delete(AgsVstAudioBus *audio_bus)
  {
    delete audio_bus;
  }

  AgsVstSpeakerArrangement ags_vst_audio_bus_get_arrangement(AgsVstAudioBus *audio_bus)
  {
    return(audio_bus->getArrangement());
  }

  void ags_vst_audio_bus_set_arrangement(AgsVstAudioBus *audio_bus, AgsVstSpeakerArrangement *arr)
  {
    audio_bus->setArrangement(arr);
  }

  gboolean ags_vst_audio_bus_get_info(AgsVstAudioBus *audio_bus, AgsVstBusInfo **info)
  {
    return(static_cast<gboolean>(audio_bus->getInfo(static_cast<BusInfo&>(info[0]))));
  }

  AgsVstBusList* ags_vst_bus_list_new(AgsVstMediaType type, AgsVstBusDirection dir)
  {
    return(new BusList(type, dir));
  }
  
  void ags_vst_bus_list_delete(AgsVstBusList *bus_list)
  {
    delete bus_list;
  }

  AgsVstMediaType ags_vst_bus_list_get_type(AgsVstBusList *bus_list)
  {
    return(static_cast<guint>(bus_list->getType()));
  }

  AgsVstBusDirection ags_vst_bus_list_get_direction(AgsVstBusList *bus_list)
  {
    return(static_cast<guint>(bus_list->getDirection()));
  }
  
}
