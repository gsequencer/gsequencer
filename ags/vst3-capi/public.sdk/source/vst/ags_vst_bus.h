/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_VST_BUS_H__
#define __AGS_VST_BUS_H__

#include <glib.h>

#include <ags/vst3-capi/base/source/ags_fobject.h>
#include <ags/vst3-capi/base/source/ags_fstring.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_ivst_component.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_ivst_audio_processor.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstBus Bus;

  /**
   * ags_vst_bus_new:
   * @name: the name as string
   * @bus_type: the #AgsVstBusType-enum
   * @flags: the flags
   * 
   * Wrapper of Bus constructor.
   * 
   * Returns: the #AgsVstBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstBus* ags_vst_bus_new(gunichar2 *name, AgsVstBusType bus_type, gint32 flags);

  /**
   * ags_vst_bus_delete:
   * @bus: the #AgsVstBus-struct pointer
   * 
   * Delete @bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_delete(AgsVstBus *bus);

  /**
   * ags_vst_bus_is_active:
   * @bus: the #AgsVstBus-struct pointer
   * 
   * Is @bus active.
   * 
   * Returns: %TRUE if active, otherwise %FALSE
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_bus_is_active(AgsVstBus *bus);

  /**
   * ags_vst_bus_set_active:
   * @bus: the #AgsVstBus-struct pointer
   * @state: %TRUE if active else %FALSE
   * 
   * Set @bus active according to @state.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_set_active(AgsVstBus *bus, gboolean state);

  /**
   * ags_vst_bus_set_name:
   * @bus: the #AgsVstBus-struct pointer
   * @new_name: the new name as string
   * 
   * Set @new_name of @bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_set_name(AgsVstBus *bus, gchar *new_name);

  /**
   * ags_vst_bus_set_bus_type:
   * @bus: the #AgsVstBus-struct pointer
   * @new_bus_type: the #AgsVstBusType-enum
   * 
   * Set @new_bus_type of @bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_set_bus_type(AgsVstBus *bus, AgsVstBusType new_bus_type);

  /**
   * ags_vst_bus_set_flags:
   * @bus: the #AgsVstBus-struct pointer
   * @new_flags: the new flags
   * 
   * Set @new_flags of @bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_set_flags(AgsVstBus *bus, guint32 new_flags);

  /**
   * ags_vst_bus_get_info:
   * @bus: the #AgsVstBus-struct pointer
   * @info: the #AgsVstBusInfo-struct pointer to a pointer
   * 
   * Get @info of @bus.
   * 
   * Returns: %TRUE on success, otherwise %FALSE
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_bus_get_info(AgsVstBus *bus, AgsVstBusInfo **info);

  typedef struct AgsVstEventBus EventBus;

  /**
   * ags_vst_event_bus_new:
   * @name: the name as string
   * @bus_type: the #AgsVstBusType-enum
   * @flags: the flags
   * @channel_count: the channel count
   * 
   * Wrapper of EventBus constructor.
   * 
   * Returns: the #AgsVstEventBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstEventBus* ags_vst_event_bus_new(gunichar2 *name, AgsVstBusType bus_type, gint32 flags, gint32 channel_count);

  /**
   * ags_vst_event_bus_delete:
   * @event_bus: the #AgsVstEventBus-struct pointer
   * 
   * Delete @event_bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_event_bus_delete(AgsVstEventBus *event_bus);

  /**
   * ags_vst_event_bus_get_info:
   * @bus: the #AgsVstEventBus-struct pointer
   * @info: the #AgsVstBusInfo-struct pointer to a pointer
   * 
   * Get @info of @bus.
   * 
   * Returns: %TRUE on success, otherwise %FALSE
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_event_bus_get_info(AgsVstEventBus *event_bus, AgsVstBusInfo **info);

  typedef struct AgsVstAudioBus AudioBus;

  /**
   * ags_vst_audio_bus_new:
   * @name: the name as string
   * @bus_type: the #AgsVstBusType-enum
   * @flags: the flags
   * @arr: the speaker arrangement
   * 
   * Wrapper of AudioBus constructor.
   * 
   * Returns: the #AgsVstAudioBus-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstAudioBus* ags_vst_audio_bus_new(gunichar2 *name, AgsVstBusType bus_type, gint32 flags, AgsVstSpeakerArrangement arr);

  /**
   * ags_vst_audio_bus_delete:
   * @audio_bus: the #AgsVstAudioBus-struct pointer
   * 
   * Delete @audio_bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_audio_bus_delete(AgsVstAudioBus *audio_bus);

  /**
   * ags_vst_audio_bus_get_arrangement:
   * @audio_bus: the #AgsVstAudioBus-struct pointer
   * 
   * Get speaker arrangement of @audio_bus.
   * 
   * Returns: the speaker arrangement 
   * 
   * Since: 2.2.0
   */
  AgsVstSpeakerArrangement ags_vst_audio_bus_get_arrangement(AgsVstAudioBus *audio_bus);

  /**
   * ags_vst_audio_bus_set_arrangement:
   * @audio_bus: the #AgsVstAudioBus-struct pointer
   * @arr: the speaker arrangement location
   * 
   * Set @arr of @audio_bus.
   * 
   * Since: 2.2.0
   */
  void ags_vst_audio_bus_set_arrangement(AgsVstAudioBus *audio_bus, AgsVstSpeakerArrangement *arr);

  /**
   * ags_vst_audio_bus_get_info:
   * @bus: the #AgsVstAudioBus-struct pointer
   * @info: the #AgsVstBusInfo-struct pointer to a pointer
   * 
   * Get @info of @bus.
   * 
   * Returns: %TRUE on success, otherwise %FALSE
   * 
   * Since: 2.2.0
   */
  gboolean ags_vst_audio_bus_get_info(AgsVstAudioBus *audio_bus, AgsVstBusInfo **info);
  
  typedef struct AgsVstBusList BusList;

  /**
   * ags_vst_bus_list_new:
   * 
   * Wrapper of BusList constructor.
   * 
   * Returns: the #AgsVstBusList-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstBusList* ags_vst_bus_list_new(AgsVstMediaType type, AgsVstBusDirection dir);

  /**
   * ags_vst_bus_list_delete:
   * @bus_list: the #AgsVstBusList-struct pointer
   * 
   * Delete @bus_list.
   * 
   * Since: 2.2.0
   */
  void ags_vst_bus_list_delete(AgsVstBusList *bus_list);

  /**
   * ags_vst_bus_list_get_type:
   * @bus_list: the #AgsVstBusList-struct pointer
   * 
   * Get media type of @bus_list.
   * 
   * Returns: the AgsVstMediaType
   * 
   * Since: 2.2.0
   */
  AgsVstMediaType ags_vst_bus_list_get_type(AgsVstBusList *bus_list);

  /**
   * ags_vst_bus_list_get_direction:
   * @bus_list: the #AgsVstBusList-struct pointer
   * 
   * Get bus direction of @bus_list.
   * 
   * Returns: the AgsVstBusDirection
   * 
   * Since: 2.2.0
   */
  AgsVstBusDirection ags_vst_bus_list_get_direction(AgsVstBusList *bus_list);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_BUS_H__*/
