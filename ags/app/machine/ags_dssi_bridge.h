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

#ifndef __AGS_DSSI_BRIDGE_H__
#define __AGS_DSSI_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <dssi.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_DSSI_BRIDGE                (ags_dssi_bridge_get_type())
#define AGS_DSSI_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridge))
#define AGS_DSSI_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridgeClass))
#define AGS_IS_DSSI_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DSSI_BRIDGE))
#define AGS_IS_DSSI_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DSSI_BRIDGE))
#define AGS_DSSI_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DSSI_BRIDGE, AgsDssiBridgeClass))

#define AGS_DSSI_BRIDGE_DEFAULT_VERSION "0.7.4"
#define AGS_DSSI_BRIDGE_DEFAULT_BUILD_ID "CEST 16-02-2016 23:36"

typedef struct _AgsDssiBridge AgsDssiBridge;
typedef struct _AgsDssiBridgeClass AgsDssiBridgeClass;

typedef enum{
  AGS_DSSI_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_DSSI_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_DSSI_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_DSSI_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsDssiBridgeFlags;

struct _AgsDssiBridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  AgsRecallContainer *dssi_play_container;
  AgsRecallContainer *dssi_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;

  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  gchar *filename;
  gchar *effect;
  unsigned long effect_index;

  DSSI_Descriptor *dssi_descriptor;
  LADSPA_Handle ladspa_handle;
  
  LADSPA_Data *port_values;
  
  GtkComboBox *program;
};

struct _AgsDssiBridgeClass
{
  AgsMachineClass machine;
};

GType ags_dssi_bridge_get_type(void);

void ags_dssi_bridge_input_map_recall(AgsDssiBridge *dssi_bridge,
				      guint audio_channel_start,
				      guint input_pad_start);
void ags_dssi_bridge_output_map_recall(AgsDssiBridge *dssi_bridge,
				       guint audio_channel_start,
				       guint output_pad_start);

void ags_dssi_bridge_load(AgsDssiBridge *dssi_bridge);

AgsDssiBridge* ags_dssi_bridge_new(GObject *soundcard,
				   gchar *filename,
				   gchar *effect);

G_END_DECLS

#endif /*__AGS_DSSI_BRIDGE_H__*/
