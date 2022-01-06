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

#ifndef __AGS_VST3_BRIDGE_H__
#define __AGS_VST3_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_VST3_BRIDGE                (ags_vst3_bridge_get_type())
#define AGS_VST3_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VST3_BRIDGE, AgsVst3Bridge))
#define AGS_VST3_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VST3_BRIDGE, AgsVst3BridgeClass))
#define AGS_IS_VST3_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_VST3_BRIDGE))
#define AGS_IS_VST3_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_VST3_BRIDGE))
#define AGS_VST3_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_VST3_BRIDGE, AgsVst3BridgeClass))

#define AGS_VST3_BRIDGE_DEFAULT_VERSION "3.10.5"
#define AGS_VST3_BRIDGE_DEFAULT_BUILD_ID "Fri Aug 27 16:44:29 CEST 2021"

#define AGS_VST3_BRIDGE_BLOCK_CONTROL_VST3_UI "vst3-ui"
#define AGS_VST3_BRIDGE_BLOCK_CONTROL_BRIDGE "bridge"

typedef struct _AgsVst3Bridge AgsVst3Bridge;
typedef struct _AgsVst3BridgeClass AgsVst3BridgeClass;

typedef enum{
  AGS_VST3_BRIDGE_DISPLAY_INPUT    = 1,
  AGS_VST3_BRIDGE_BULK_OUTPUT      = 1 <<  1,
  AGS_VST3_BRIDGE_DISPLAY_OUTPUT   = 1 <<  2,
  AGS_VST3_BRIDGE_BULK_INPUT       = 1 <<  3,
  AGS_VST3_BRIDGE_NO_UPDATE        = 1 <<  4,
  AGS_VST3_BRIDGE_UI_VISIBLE       = 1 <<  5,
}AgsVst3BridgeFlags;

struct _AgsVst3Bridge
{
  AgsMachine machine;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;

  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  AgsRecallContainer *vst3_play_container;
  AgsRecallContainer *vst3_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;

  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  gchar *filename;
  gchar *effect;
  gint32 effect_index;
  
  gboolean has_midi;

  AgsVst3Plugin *vst3_plugin;
  
  GtkBox *vbox;
  
  GtkComboBox *program;  

  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;
  AgsVstIEditControllerHostEditing *iedit_controller_host_editing;

  AgsVstComponentHandler *icomponent_handler;

  int perform_edit_handler;
  int restart_component_handler;
  
  AgsVstIPlugView *iplug_view;

  gpointer ns_window;
  gpointer ns_view;
  
  GtkMenu *vst3_menu;

  GHashTable *block_control;
};

struct _AgsVst3BridgeClass
{
  AgsMachineClass machine;
};

GType ags_vst3_bridge_get_type(void);

void ags_vst3_bridge_input_map_recall(AgsVst3Bridge *vst3_bridge,
				      guint audio_channel_start,
				      guint input_pad_start);
void ags_vst3_bridge_output_map_recall(AgsVst3Bridge *vst3_bridge,
				       guint audio_channel_start,
				       guint output_pad_start);

void ags_vst3_bridge_load(AgsVst3Bridge *vst3_bridge);
void ags_vst3_bridge_reload_port(AgsVst3Bridge *vst3_bridge);

AgsVst3Bridge* ags_vst3_bridge_new(GObject *soundcard,
				   gchar *filename,
				   gchar *effect);

G_END_DECLS

#endif /*__AGS_VST3_BRIDGE_H__*/
