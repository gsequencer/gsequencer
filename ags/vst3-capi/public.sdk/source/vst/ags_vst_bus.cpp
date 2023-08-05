/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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
  /**
   * Wrapper of bus constructor.
   * 
   * @param name the name as string
   * @param bus_type the bus type
   * @param flags the flags
   * @return the bus
   * 
   * @since 2.2.0
   */
  AgsVstBus* ags_vst_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags)
  {
    return((AgsVstBus* ) new Steinberg::Vst::Bus());
  }
#endif
  
  /**
   * Delete bus.
   * 
   * @param bus the bus
   * 
   * @since 2.2.0
   */
  void ags_vst_bus_delete(AgsVstBus *bus)
  {
    delete bus;
  }

  /**
   * Is bus active.
   * 
   * @param bus the bus
   * @return true if active, otherwise false
   * 
   * @since 2.2.0
   */
  gboolean ags_vst_bus_is_active(AgsVstBus *bus)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::Bus*>((void *) bus)->isActive()));
  }

  /**
   * Set bus active according to state.
   * 
   * @param bus the bus
   * @param state true if active else false
   * 
   * @since 2.2.0
   */
  void ags_vst_bus_set_active(AgsVstBus *bus, gboolean state)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setActive(static_cast<Steinberg::TBool>(state));
  }

  /**
   * Set new name of bus.
   * 
   * @param bus the bus
   * @param new_name the new name as string
   * 
   * @since 2.2.0
   */
  void ags_vst_bus_set_name(AgsVstBus *bus, AgsVstString new_name)
  {
    char16_t *wstr = g_utf8_to_utf16(new_name,
				     -1,
				     NULL,
				     NULL,
				     NULL);
    
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setName(wstr);
  }

  /**
   * Set new bus type of bus.
   * 
   * @param bus the bus
   * @param new_bus_type the bus type
   * 
   * @since 2.2.0
   */
  void ags_vst_bus_set_bus_type(AgsVstBus *bus, AgsVstBusType new_bus_type)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setBusType(static_cast<Steinberg::Vst::BusType>(new_bus_type));
  }

  /**
   * Set new flags of bus.
   * 
   * @param bus the bus
   * @param new_flags the new flags
   * 
   * @since 2.2.0
   */
  void ags_vst_bus_set_flags(AgsVstBus *bus, guint32 new_flags)
  {
    static_cast<Steinberg::Vst::Bus*>((void *) bus)->setFlags(new_flags);
  }

  /**
   * Get info of bus.
   * 
   * @param bus the bus
   * @param info the bus info
   * @return true on success, otherwise false
   * 
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_bus_get_info(AgsVstBus *bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::Bus*>((void *) bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  /**
   * Instantiate new Steinberg::Vst::EventBus the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @param name the name as string
   * @param bus_type the bus type
   * @param flags the flags
   * @param channel_count the channel count
   * @return the new instance of event bus
   * 
   * @since 2.2.0
   */
  AgsVstEventBus* ags_vst_event_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, gint32 channel_count)
  {
    return((AgsVstEventBus *) new Steinberg::Vst::EventBus((char16_t *) name, static_cast<Steinberg::Vst::BusType>(bus_type), flags, channel_count));
  }

  /**
   * Delete.
   *
   * @param event_bus the event bus
   *
   * @since 2.2.0
   */
  void ags_vst_event_bus_delete(AgsVstEventBus *event_bus)
  {
    delete event_bus;
  }

  /**
   * Get info of bus.
   * 
   * @param bus the event bus
   * @param info the bus info
   * @return true on success, otherwise false
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_event_bus_get_info(AgsVstEventBus *event_bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::EventBus*>((void *) event_bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  /**
   * Instantiate new Steinberg::Vst::AudioBus the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @param name the name as string
   * @param bus_type the bus type
   * @param flags the flags
   * @param arr the speaker arrangement
   * @return the new instance of audio bus
   * 
   * @since 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, AgsVstSpeakerArrangement arr)
  {
    return((AgsVstAudioBus *) new Steinberg::Vst::AudioBus((char16_t *) name, static_cast<Steinberg::Vst::BusType>(bus_type), flags, arr));
  }

  /**
   * Delete.
   *
   * @param audio_bus the audio bus
   *
   * @since 2.2.0
   */
  void ags_vst_audio_bus_delete(AgsVstAudioBus *audio_bus)
  {
    delete audio_bus;
  }

  /**
   * Get speaker arrangement of audio bus.
   * 
   * @param audio_bus the audio bus
   * @return the speaker arrangement 
   * 
   * @since 2.2.0
   */
  AgsVstSpeakerArrangement ags_vst_audio_bus_get_arrangement(AgsVstAudioBus *audio_bus)
  {
    return((AgsVstSpeakerArrangement) static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->getArrangement());
  }

  /**
   * Set speaker arrangement of audio bus.
   * 
   * @param audio_bus the audio bus
   * @param arr the speaker arrangement 
   * 
   * @since 2.2.0
   */
  void ags_vst_audio_bus_set_arrangement(AgsVstAudioBus *audio_bus, AgsVstSpeakerArrangement arr)
  {
    static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->setArrangement(static_cast<Steinberg::Vst::SpeakerArrangement>(arr));
  }

  /**
   * Get @info of @bus.
   * 
   * @param bus the audio bus
   * @param info the bus info
   * #return true on success, otherwise false
   * 
   * @since 2.2.0
   */
  gboolean ags_vst_audio_bus_get_info(AgsVstAudioBus *audio_bus, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<gboolean>(static_cast<Steinberg::Vst::AudioBus*>((void *) audio_bus)->getInfo(const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1))));
  }

  /**
   * Instantiate new Steinberg::Vst::BusList the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @param type the media type
   * @param dir the bus direction
   * @return the new instance of bus list
   * 
   * @since 2.2.0
   */
  AgsVstBusList* ags_vst_bus_list_new(AgsVstMediaType type, AgsVstBusDirection dir)
  {
    return((AgsVstBusList *) new Steinberg::Vst::BusList(type, dir));
  }
  
  /**
   * Delete.
   *
   * @param bus_list the bus list
   *
   * @since 2.2.0
   */
  void ags_vst_bus_list_delete(AgsVstBusList *bus_list)
  {
    delete bus_list;
  }

  /**
   * Get media type of bus list.
   * 
   * @param bus_list the bus list
   * @return the media type 
   * 
   * @since 2.2.0
   */
  AgsVstMediaType ags_vst_bus_list_get_type(AgsVstBusList *bus_list)
  {
    return(static_cast<Steinberg::Vst::BusList*>((void *) bus_list)->getType());
  }

  /**
   * Get bus direction of bus list.
   * 
   * @param bus_list the bus list
   * @return the bus direction
   * 
   * @ince 2.2.0
   */
  AgsVstBusDirection ags_vst_bus_list_get_direction(AgsVstBusList *bus_list)
  {
    return(static_cast<Steinberg::Vst::BusList*>((void *) bus_list)->getDirection());
  }
  
}
