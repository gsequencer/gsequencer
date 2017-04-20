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

#ifndef __AGS_LV2_BRIDGE_H__
#define __AGS_LV2_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#define AGS_TYPE_LV2_BRIDGE                (ags_lv2_bridge_get_type())
#define AGS_LV2_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_BRIDGE, AgsLv2Bridge))
#define AGS_LV2_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_BRIDGE, AgsLv2BridgeClass))
#define AGS_IS_LV2_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LV2_BRIDGE))
#define AGS_IS_LV2_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LV2_BRIDGE))
#define AGS_LV2_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LV2_BRIDGE, AgsLv2BridgeClass))

#define AGS_LV2_BRIDGE_DEFAULT_VERSION "0.4.3\0"
#define AGS_LV2_BRIDGE_DEFAULT_BUILD_ID "CEST 13-05-2015 13:40\0"

typedef struct _AgsLv2Bridge AgsLv2Bridge;
typedef struct _AgsLv2BridgeClass AgsLv2BridgeClass;

typedef enum{
  AGS_LV2_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_LV2_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_LV2_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_LV2_BRIDGE_BULK_INPUT       = 1 <<  3,
}AgsLv2BridgeFlags;

struct _AgsLv2Bridge
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
  gchar *uri;
  guint uri_index;
  
  gboolean has_midi;
  
  gboolean has_gui;
  gchar *gui_filename;
  gchar *gui_uri;

  GtkVBox *vbox;

  LV2_Descriptor *lv2_descriptor;
  LV2_Handle *lv2_handle;
  float *port_value;
  
  GtkComboBoxText *program;
  GtkComboBoxText *preset;

  LV2_Feature **ui_feature;
  LV2UI_Handle ui_handle;

  GtkWidget *lv2_gui;
  GtkWidget *ui_widget;

  GtkMenu *lv2_menu;
};

struct _AgsLv2BridgeClass
{
  AgsMachineClass machine;
};

GType ags_lv2_bridge_get_type(void);

void ags_lv2_bridge_input_map_recall(AgsLv2Bridge *lv2_bridge, guint audio_channel_start, guint input_pad_start);
void ags_lv2_bridge_output_map_recall(AgsLv2Bridge *lv2_bridge, guint audio_channel_start, guint output_pad_start);

void ags_lv2_bridge_load_program(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_load_preset(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_load_midi(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_load_gui(AgsLv2Bridge *lv2_bridge);
void ags_lv2_bridge_load(AgsLv2Bridge *lv2_bridge);

gboolean ags_lv2_bridge_lv2ui_idle_timeout(GtkWidget *widget);

AgsLv2Bridge* ags_lv2_bridge_new(GObject *soundcard,
				 gchar *filename,
				 gchar *effect);

#endif /*__AGS_LV2_BRIDGE_H__*/
