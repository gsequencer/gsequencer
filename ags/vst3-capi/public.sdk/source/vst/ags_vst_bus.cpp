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

#if 0  
  AgsVstBus* ags_vst_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags)
  {
    return((AgsVstBus* ) new Steinberg::Vst::Bus());
  }
#endif
  
  void ags_vst_bus_delete(AgsVstBus *bus)
  {
    delete bus;
  }

  gboolean ags_vst_bus_is_active(AgsVstBus *bus)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::Bus*>((void *) bus)->isActive()));
  }

  void ags_vst_bus_set_active(AgsVstBus *bus, gboolean state)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setActive(static_cast<Steinberg::TBool>(state));
  }

  void ags_vst_bus_set_name(AgsVstBus *bus, AgsVstString new_name)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setName(static_cast<Steinberg::String>((gchar *) new_name));
  }

  void ags_vst_bus_set_bus_type(AgsVstBus *bus, AgsVstBusType new_bus_type)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setBusType(static_cast<Steinberg::Vst::BusType>(new_bus_type));
  }

  void ags_vst_bus_set_flags(AgsVstBus *bus, guint32 new_flags)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setFlags(new_flags);
  }

  gboolean ags_vst_bus_get_info(AgsVstBus *bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::Bus*>((void *) bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  AgsVstEventBus* ags_vst_event_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, gint32 channel_count)
  {
    return((AgsVstEventBus *) new Steinberg::Vst::EventBus((char16_t *) name, static_cast<Steinberg::Vst::BusType>(bus_type), flags, channel_count));
  }

  void ags_vst_event_bus_delete(AgsVstEventBus *event_bus)
  {
    delete event_bus;
  }

  gboolean ags_vst_event_bus_get_info(AgsVstEventBus *event_bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::EventBus*>((void *) event_bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  AgsVstAudioBus* ags_vst_audio_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, AgsVstSpeakerArrangement arr)
  {
    return((AgsVstAudioBus *) new Steinberg::Vst::AudioBus((char16_t *) name, static_cast<Steinberg::Vst::BusType>(bus_type), flags, arr));
  }

  void ags_vst_audio_bus_delete(AgsVstAudioBus *audio_bus)
  {
    delete audio_bus;
  }

  AgsVstSpeakerArrangement ags_vst_audio_bus_get_arrangement(AgsVstAudioBus *audio_bus)
  {
    return((AgsVstSpeakerArrangement) static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->getArrangement());
  }

  void ags_vst_audio_bus_set_arrangement(AgsVstAudioBus *audio_bus, AgsVstSpeakerArrangement arr)
  {
    static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->setArrangement(static_cast<Steinberg::Vst::SpeakerArrangement>(arr));
  }

  gboolean ags_vst_audio_bus_get_info(AgsVstAudioBus *audio_bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  AgsVstBusList* ags_vst_bus_list_new(AgsVstMediaType type, AgsVstBusDirection dir)
  {
    return((AgsVstBusList *) new Steinberg::Vst::BusList(type, dir));
  }
  
  void ags_vst_bus_list_delete(AgsVstBusList *bus_list)
  {
    delete bus_list;
  }

  AgsVstMediaType ags_vst_bus_list_get_type(AgsVstBusList *bus_list)
  {
    return(static_cast<Steinberg::Vst::BusList*>((void *) bus_list)->getType());
  }

  AgsVstBusDirection ags_vst_bus_list_get_direction(AgsVstBusList *bus_list)
  {
    return(static_cast<Steinberg::Vst::BusList*>((void *) bus_list)->getDirection());
  }
  
}
