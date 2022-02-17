/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_LADSPA_BRIDGE_H__
#define __AGS_LADSPA_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_LADSPA_BRIDGE                (ags_ladspa_bridge_get_type())
#define AGS_LADSPA_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LADSPA_BRIDGE, AgsLadspaBridge))
#define AGS_LADSPA_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LADSPA_BRIDGE, AgsLadspaBridgeClass))
#define AGS_IS_LADSPA_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LADSPA_BRIDGE))
#define AGS_IS_LADSPA_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LADSPA_BRIDGE))
#define AGS_LADSPA_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LADSPA_BRIDGE, AgsLadspaBridgeClass))

#define AGS_LADSPA_BRIDGE_DEFAULT_VERSION "0.4.3"
#define AGS_LADSPA_BRIDGE_DEFAULT_BUILD_ID "CEST 15-03-2015 13:40"

typedef struct _AgsLadspaBridge AgsLadspaBridge;
typedef struct _AgsLadspaBridgeClass AgsLadspaBridgeClass;

typedef enum{
  AGS_LADSPA_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_LADSPA_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_LADSPA_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_LADSPA_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsLadspaBridgeFlags;

struct _AgsLadspaBridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  AgsRecallContainer *ladspa_play_container;
  AgsRecallContainer *ladspa_recall_container;

  gchar *filename;
  gchar *effect;
  unsigned long effect_index;
};

struct _AgsLadspaBridgeClass
{
  AgsMachineClass machine;
};

GType ags_ladspa_bridge_get_type(void);

void ags_ladspa_bridge_input_map_recall(AgsLadspaBridge *ladspa_bridge,
					guint audio_channel_start,
					guint input_pad_start);
void ags_ladspa_bridge_output_map_recall(AgsLadspaBridge *ladspa_bridge,
					 guint audio_channel_start,
					 guint output_pad_start);

AgsLadspaBridge* ags_ladspa_bridge_new(GObject *soundcard,
				       gchar *filename,
				       gchar *effect);

G_END_DECLS

#endif /*__AGS_LADSPA_BRIDGE_H__*/
