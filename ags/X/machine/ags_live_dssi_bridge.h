/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_LIVE_DSSI_BRIDGE_H__
#define __AGS_LIVE_DSSI_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <dssi.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_LIVE_DSSI_BRIDGE                (ags_live_dssi_bridge_get_type())
#define AGS_LIVE_DSSI_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LIVE_DSSI_BRIDGE, AgsLiveDssiBridge))
#define AGS_LIVE_DSSI_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LIVE_DSSI_BRIDGE, AgsLiveDssiBridgeClass))
#define AGS_IS_LIVE_DSSI_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LIVE_DSSI_BRIDGE))
#define AGS_IS_LIVE_DSSI_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LIVE_DSSI_BRIDGE))
#define AGS_LIVE_DSSI_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LIVE_DSSI_BRIDGE, AgsLiveDssiBridgeClass))

#define AGS_LIVE_DSSI_BRIDGE_DEFAULT_VERSION "0.7.134"
#define AGS_LIVE_DSSI_BRIDGE_DEFAULT_BUILD_ID "CEST 14-04-2017 21:10"

typedef struct _AgsLiveDssiBridge AgsLiveDssiBridge;
typedef struct _AgsLiveDssiBridgeClass AgsLiveDssiBridgeClass;

typedef enum{
  AGS_LIVE_DSSI_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_LIVE_DSSI_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_LIVE_DSSI_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_LIVE_DSSI_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsLiveDssiBridgeFlags;

struct _AgsLiveDssiBridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *filename;
  gchar *effect;
  unsigned long effect_index;

  DSSI_Descriptor *dssi_descriptor;
  LADSPA_Handle ladspa_handle;
  
  LADSPA_Data *port_values;
  
  GtkComboBoxText *program;
};

struct _AgsLiveDssiBridgeClass
{
  AgsMachineClass machine;
};

GType ags_live_dssi_bridge_get_type(void);

void ags_live_dssi_bridge_input_map_recall(AgsLiveDssiBridge *live_dssi_bridge, guint audio_channel_start, guint input_pad_start);
void ags_live_dssi_bridge_output_map_recall(AgsLiveDssiBridge *live_dssi_bridge, guint audio_channel_start, guint output_pad_start);

void ags_live_dssi_bridge_load(AgsLiveDssiBridge *live_dssi_bridge);

AgsLiveDssiBridge* ags_live_dssi_bridge_new(GObject *soundcard,
					    gchar *filename,
					    gchar *effect);

#endif /*__AGS_LIVE_DSSI_BRIDGE_H__*/
