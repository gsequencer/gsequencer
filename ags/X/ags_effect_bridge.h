/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_EFFECT_BRIDGE_H__
#define __AGS_EFFECT_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_EFFECT_BRIDGE                (ags_effect_bridge_get_type())
#define AGS_EFFECT_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridge))
#define AGS_EFFECT_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridgeClass))
#define AGS_IS_EFFECT_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_BRIDGE))
#define AGS_IS_EFFECT_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_BRIDGE))
#define AGS_EFFECT_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridgeClass))

#define AGS_EFFECT_BRIDGE_DEFAULT_VERSION "0.7.8"
#define AGS_EFFECT_BRIDGE_DEFAULT_BUILD_ID "CEST 01-03-2016 00:23"

#define AGS_EFFECT_BRIDGE_MAX_COLUMNS (2)

typedef struct _AgsEffectBridge AgsEffectBridge;
typedef struct _AgsEffectBridgeClass AgsEffectBridgeClass;

typedef enum{
  AGS_EFFECT_BRIDGE_MAPPED_RECALL     = 1,
  AGS_EFFECT_BRIDGE_PREMAPPED_RECALL  = 1 <<  1,
  AGS_EFFECT_BRIDGE_CONNECTED         = 1 <<  2, 
  AGS_EFFECT_BRIDGE_DISPLAY_INPUT     = 1 <<  3,
  AGS_EFFECT_BRIDGE_BULK_OUTPUT       = 1 <<  4,
  AGS_EFFECT_BRIDGE_DISPLAY_OUTPUT    = 1 <<  5,
  AGS_EFFECT_BRIDGE_BULK_INPUT        = 1 <<  6,
}AgsEffectBridgeFlags;

struct _AgsEffectBridge
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsAudio *audio;
  
  GType bulk_output_type;
  GtkWidget *bulk_output;
  
  GType output_pad_type;
  GType output_line_type;
  GtkHBox *output;
  
  GType bulk_input_type;
  GtkWidget *bulk_input;

  GType input_pad_type;
  GType input_line_type;
  GtkHBox *input;
};

struct _AgsEffectBridgeClass
{
  GtkVBoxClass vbox;

  void (*resize_audio_channels)(AgsEffectBridge *effect_bridge,
				guint new_size, guint old_size);
  void (*resize_pads)(AgsEffectBridge *effect_bridge,
		      GType channel_type,
		      guint new_size, guint old_size);

  void (*map_recall)(AgsEffectBridge *effect_bridge);
  GList* (*find_port)(AgsEffectBridge *effect_bridge);
};

GType ags_effect_bridge_get_type(void);

void ags_effect_bridge_resize_audio_channels(AgsEffectBridge *effect_bridge,
					     guint new_size, guint old_size);
void ags_effect_bridge_resize_pads(AgsEffectBridge *effect_bridge,
				   GType channel_type,
				   guint new_size, guint old_size);

void ags_effect_bridge_map_recall(AgsEffectBridge *effect_bridge);
GList* ags_effect_bridge_find_port(AgsEffectBridge *effect_bridge);

AgsEffectBridge* ags_effect_bridge_new(AgsAudio *audio);

#endif /*__AGS_EFFECT_BRIDGE_H__*/
