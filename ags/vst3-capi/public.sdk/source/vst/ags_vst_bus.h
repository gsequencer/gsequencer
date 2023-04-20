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

#ifndef __AGS_VST_BUS_H__
#define __AGS_VST_BUS_H__

#include <glib.h>

#include <ags/vst3-capi/base/source/ags_vst_fobject.h>
#include <ags/vst3-capi/base/source/ags_vst_fstring.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iaudio_processor.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstBus Bus;

  AgsVstBus* ags_vst_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags);

  void ags_vst_bus_delete(AgsVstBus *bus);

  gboolean ags_vst_bus_is_active(AgsVstBus *bus);
  void ags_vst_bus_set_active(AgsVstBus *bus, gboolean state);

  void ags_vst_bus_set_name(AgsVstBus *bus, AgsVstString new_name);

  void ags_vst_bus_set_bus_type(AgsVstBus *bus, AgsVstBusType new_bus_type);

  void ags_vst_bus_set_flags(AgsVstBus *bus, guint32 new_flags);

  gboolean ags_vst_bus_get_info(AgsVstBus *bus, AgsVstBusInfo *info);

  typedef struct AgsVstEventBus EventBus;

  AgsVstEventBus* ags_vst_event_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, gint32 channel_count);

  void ags_vst_event_bus_delete(AgsVstEventBus *event_bus);

  gboolean ags_vst_event_bus_get_info(AgsVstEventBus *event_bus, AgsVstBusInfo *info);

  typedef struct AgsVstAudioBus AudioBus;

  AgsVstAudioBus* ags_vst_audio_bus_new(AgsVstTChar *name, AgsVstBusType bus_type, gint32 flags, AgsVstSpeakerArrangement arr);

  void ags_vst_audio_bus_delete(AgsVstAudioBus *audio_bus);

  AgsVstSpeakerArrangement ags_vst_audio_bus_get_arrangement(AgsVstAudioBus *audio_bus);
  void ags_vst_audio_bus_set_arrangement(AgsVstAudioBus *audio_bus, AgsVstSpeakerArrangement arr);

  gboolean ags_vst_audio_bus_get_info(AgsVstAudioBus *audio_bus, AgsVstBusInfo *info);
  
  typedef struct AgsVstBusList BusList;

  AgsVstBusList* ags_vst_bus_list_new(AgsVstMediaType type, AgsVstBusDirection dir);

  void ags_vst_bus_list_delete(AgsVstBusList *bus_list);

  AgsVstMediaType ags_vst_bus_list_get_type(AgsVstBusList *bus_list);

  AgsVstBusDirection ags_vst_bus_list_get_direction(AgsVstBusList *bus_list);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_BUS_H__*/
