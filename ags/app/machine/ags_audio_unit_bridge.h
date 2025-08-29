/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_BRIDGE_H__
#define __AGS_AUDIO_UNIT_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_BRIDGE                (ags_audio_unit_bridge_get_type())
#define AGS_AUDIO_UNIT_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_BRIDGE, AgsAudioUnitBridge))
#define AGS_AUDIO_UNIT_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_UNIT_BRIDGE, AgsAudioUnitBridgeClass))
#define AGS_IS_AUDIO_UNIT_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_UNIT_BRIDGE))
#define AGS_IS_AUDIO_UNIT_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_UNIT_BRIDGE))
#define AGS_AUDIO_UNIT_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_UNIT_BRIDGE, AgsAudioUnitBridgeClass))

#define AGS_AUDIO_UNIT_BRIDGE_DEFAULT_VERSION "8.1.2"
#define AGS_AUDIO_UNIT_BRIDGE_DEFAULT_BUILD_ID "Sun Aug  3 11:54:45 UTC 2025"

#define AGS_AUDIO_UNIT_BRIDGE_BLOCK_CONTROL_AUDIO_UNIT_UI "audio-unit-ui"
#define AGS_AUDIO_UNIT_BRIDGE_BLOCK_CONTROL_BRIDGE "bridge"

typedef struct _AgsAudioUnitBridge AgsAudioUnitBridge;
typedef struct _AgsAudioUnitBridgeClass AgsAudioUnitBridgeClass;

typedef enum{
  AGS_AUDIO_UNIT_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_AUDIO_UNIT_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_AUDIO_UNIT_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_AUDIO_UNIT_BRIDGE_BULK_INPUT       = 1 <<  3,
  AGS_AUDIO_UNIT_BRIDGE_NO_UPDATE        = 1 <<  4,
  AGS_AUDIO_UNIT_BRIDGE_UI_VISIBLE       = 1 <<  5,
}AgsAudioUnitBridgeFlags;

struct _AgsAudioUnitBridge
{
  AgsMachine machine;

  AgsAudioUnitBridgeFlags flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  AgsRecallContainer *audio_unit_play_container;
  AgsRecallContainer *audio_unit_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;

  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  gchar *filename;
  gchar *effect;
  
  gboolean has_midi;

  AgsAudioUnitPlugin *audio_unit_plugin;

  gpointer av_audio_unit;

  gpointer window;
  
  GtkBox *vbox;
    
  GMenu *audio_unit_menu;

  GHashTable *block_control;
};

struct _AgsAudioUnitBridgeClass
{
  AgsMachineClass machine;
};

GType ags_audio_unit_bridge_get_type(void);

void ags_audio_unit_bridge_input_map_recall(AgsAudioUnitBridge *audio_unit_bridge,
					    guint audio_channel_start,
					    guint input_pad_start);
void ags_audio_unit_bridge_output_map_recall(AgsAudioUnitBridge *audio_unit_bridge,
					     guint audio_channel_start,
					     guint output_pad_start);

void ags_audio_unit_bridge_load(AgsAudioUnitBridge *audio_unit_bridge);
void ags_audio_unit_bridge_reload_port(AgsAudioUnitBridge *audio_unit_bridge);

AgsAudioUnitBridge* ags_audio_unit_bridge_new(GObject *soundcard,
					      gchar *filename,
					      gchar *effect);

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_BRIDGE_H__*/
