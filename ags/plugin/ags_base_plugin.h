/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_BASE_PLUGIN_H__
#define __AGS_BASE_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <alsa/seq_event.h>

#define AGS_TYPE_BASE_PLUGIN                (ags_base_plugin_get_type())
#define AGS_BASE_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BASE_PLUGIN, AgsBasePlugin))
#define AGS_BASE_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BASE_PLUGIN, AgsBasePluginClass))
#define AGS_IS_BASE_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BASE_PLUGIN))
#define AGS_IS_BASE_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BASE_PLUGIN))
#define AGS_BASE_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BASE_PLUGIN, AgsBasePluginClass))

typedef struct _AgsBasePlugin AgsBasePlugin;
typedef struct _AgsBasePluginClass AgsBasePluginClass;

/**
 * AgsBasePluginFlags:
 * @AGS_BASE_PLUGIN_IS_INSTRUMENT: plugin is instrument
 * 
 * Enum values to control the behavior or indicate internal state of #AgsBasePlugin by
 * enable/disable as flags.
*/
typedef enum{
  AGS_BASE_PLUGIN_IS_INSTRUMENT      =  1,
}AgsBasePluginFlags;

struct _AgsBasePlugin
{
  GObject gobject;

  guint flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  gchar *filename;
  gchar *effect;

  guint port_group_count;
  guint *port_group;
  GList *plugin_port;
  
  guint effect_index;
  void *plugin_so;
  void *plugin_descriptor;
  void *plugin_handle;

  gchar *ui_filename;
  gchar *ui_effect;
  
  guint ui_effect_index;
  void *ui_plugin_so;
  void *ui_plugin_descriptor;
  void *ui_plugin_handle;
  
  GObject *ui_plugin;
};

struct _AgsBasePluginClass
{
  GObjectClass gobject;

  gpointer (*instantiate)(AgsBasePlugin *base_plugin,
			  guint samplerate, guint buffer_size);

  void (*connect_port)(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location);
  
  void (*activate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);
  void (*deactivate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);

  void (*run)(AgsBasePlugin *base_plugin,
	      gpointer plugin_handle,
	      snd_seq_event_t *seq_event,
	      guint frame_count);
  
  void (*load_plugin)(AgsBasePlugin *base_plugin);
};

GType ags_base_plugin_get_type(void);

pthread_mutex_t* ags_base_plugin_get_class_mutex();

GList* ags_base_plugin_find_filename(GList *base_plugin, gchar *filename);
GList* ags_base_plugin_find_effect(GList *base_plugin, gchar *filename, gchar *effect);

GList* ags_base_plugin_find_ui_effect_index(GList *base_plugin, gchar *ui_filename, guint ui_effect_index);

GList* ags_base_plugin_sort(GList *base_plugin);

void ags_base_plugin_apply_port_group_by_prefix(AgsBasePlugin *base_plugin);

gpointer ags_base_plugin_instantiate(AgsBasePlugin *base_plugin,
				     guint samplerate, guint buffer_size);

void ags_base_plugin_connect_port(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location);

void ags_base_plugin_activate(AgsBasePlugin *base_plugin, gpointer plugin_handle);
void ags_base_plugin_deactivate(AgsBasePlugin *base_plugin, gpointer plugin_handle);

void ags_base_plugin_run(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle,
			 snd_seq_event_t *seq_event,
			 guint frame_count);

void ags_base_plugin_load_plugin(AgsBasePlugin *base_plugin);

AgsBasePlugin* ags_base_plugin_new(gchar *filename, gchar *effect, guint effect_index);

#endif /*__AGS_BASE_PLUGIN_H__*/
