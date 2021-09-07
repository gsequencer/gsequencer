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

#ifndef __AGS_VST3_PLUGIN_H__
#define __AGS_VST3_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-vst.h>

#include <ags/plugin/ags_base_plugin.h>

G_BEGIN_DECLS

#define AGS_TYPE_VST3_PLUGIN                (ags_vst3_plugin_get_type())
#define AGS_VST3_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VST3_PLUGIN, AgsVst3Plugin))
#define AGS_VST3_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VST3_PLUGIN, AgsVst3PluginClass))
#define AGS_IS_VST3_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VST3_PLUGIN))
#define AGS_IS_VST3_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VST3_PLUGIN))
#define AGS_VST3_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VST3_PLUGIN, AgsVst3PluginClass))

typedef struct _AgsVst3Plugin AgsVst3Plugin;
typedef struct _AgsVst3PluginClass AgsVst3PluginClass;

struct _AgsVst3Plugin
{
  AgsBasePlugin base_plugin;

  gpointer get_plugin_factory;
  
  AgsVstHostContext *host_context;

  char *cid;

  gpointer (*plugin_init)(gpointer plugin_so);
  gpointer (*plugin_exit)();
  
  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;

  GHashTable *program;
};

struct _AgsVst3PluginClass
{
  AgsBasePluginClass base_plugin;
};

GType ags_vst3_plugin_get_type(void);

guint ags_vst3_plugin_get_audio_output_bus_count(AgsVst3Plugin *vst3_plugin);
guint ags_vst3_plugin_get_audio_input_bus_count(AgsVst3Plugin *vst3_plugin);

guint ags_vst3_plugin_get_event_output_bus_count(AgsVst3Plugin *vst3_plugin);
guint ags_vst3_plugin_get_event_input_bus_count(AgsVst3Plugin *vst3_plugin);

guint ags_vst3_plugin_get_audio_output_port_count(AgsVst3Plugin *vst3_plugin,
						  guint bus_index);
guint ags_vst3_plugin_get_audio_input_port_count(AgsVst3Plugin *vst3_plugin,
						 guint bus_index);

AgsVstProcessData* ags_vst3_plugin_process_data_lookup(AgsVstIComponent *icomponent);
void ags_vst3_plugin_process_data_insert(AgsVstIComponent *icomponent,
					 AgsVstProcessData *data);
void ags_vst3_plugin_process_data_remove(AgsVstIComponent *icomponent);

AgsVst3Plugin* ags_vst3_plugin_new(gchar *filename, gchar *effect, guint effect_index);

G_END_DECLS

#endif /*__AGS_VST3_PLUGIN_H__*/
