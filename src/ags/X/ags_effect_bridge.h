/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_EFFECT_BRIDGE_H__
#define __AGS_EFFECT_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_EFFECT_BRIDGE                (ags_effect_bridge_get_type())
#define AGS_EFFECT_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridge))
#define AGS_EFFECT_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridgeClass))
#define AGS_IS_EFFECT_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_BRIDGE))
#define AGS_IS_EFFECT_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_BRIDGE))
#define AGS_EFFECT_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_BRIDGE, AgsEffectBridgeClass))

#define AGS_EFFECT_BRIDGE_DEFAULT_VERSION "0.4.3\0"
#define AGS_EFFECT_BRIDGE_DEFAULT_BUILD_ID "CEST 15-03-2015 13:40\0"

#define AGS_EFFECT_BRIDGE_MAX_COLUMNS (2)

typedef struct _AgsEffectBridge AgsEffectBridge;
typedef struct _AgsEffectBridgeClass AgsEffectBridgeClass;

typedef enum{
  AGS_EFFECT_BRIDGE_CONNECTED        = 1,
  AGS_EFFECT_BRIDGE_DISPLAY_INPUT    = 1 <<  1,
  AGS_EFFECT_BRIDGE_BULK_OUTPUT      = 1 <<  2,
  AGS_EFFECT_BRIDGE_DISPLAY_OUTPUT   = 1 <<  3,
  AGS_EFFECT_BRIDGE_BULK_INPUT       = 1 <<  4,
}AgsEffectBridgeFlags;

struct _AgsEffectBridge
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsAudio *audio;

  GType *bulk_output_type;
  GtkWidget *bulk_output;
  
  GType *output_pad_type;
  GType *output_line_type;
  GtkHBox *output;
  
  GType *bulk_input_type;
  GtkWidget *bulk_input;

  GType *input_pad_type;
  GType *input_line_type;
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
};

GType ags_effect_bridge_get_type(void);

void ags_effect_bridge_resize_audio_channels(AgsEffectBridge *effect_bridge,
					     guint new_size, guint old_size);
void ags_effect_bridge_resize_pads(AgsEffectBridge *effect_bridge,
				   GType channel_type,
				   guint new_size, guint old_size);

AgsEffectBridge* ags_effect_bridge_new(AgsAudio *audio);

#endif /*__AGS_EFFECT_BRIDGE_H__*/
