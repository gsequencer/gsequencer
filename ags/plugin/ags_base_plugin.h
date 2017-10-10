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

#define AGS_PORT_DESCRIPTOR(ptr) ((AgsPortDescriptor *)(ptr))

typedef struct _AgsBasePlugin AgsBasePlugin;
typedef struct _AgsBasePluginClass AgsBasePluginClass;
typedef struct _AgsPortDescriptor AgsPortDescriptor;

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

/**
 * AgsPortDescriptorFlags:
 * @AGS_PORT_DESCRIPTOR_ATOM: atom port
 * @AGS_PORT_DESCRIPTOR_AUDIO: audio port
 * @AGS_PORT_DESCRIPTOR_CONTROL: control port
 * @AGS_PORT_DESCRIPTOR_MIDI: MIDI port
 * @AGS_PORT_DESCRIPTOR_EVENT: event port
 * @AGS_PORT_DESCRIPTOR_OUTPUT: is output
 * @AGS_PORT_DESCRIPTOR_INPUT: is input
 * @AGS_PORT_DESCRIPTOR_TOGGLED: toggle control
 * @AGS_PORT_DESCRIPTOR_ENUMERATION: enumerated
 * @AGS_PORT_DESCRIPTOR_LOGARITHMIC: logarithmic
 * @AGS_PORT_DESCRIPTOR_INTEGER: integer
 * @AGS_PORT_DESCRIPTOR_SAMPLERATE: samplerate
 * @AGS_PORT_DESCRIPTOR_BOUNDED_BELOW: bounded below
 * @AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE: bounded above
 * @AGS_PORT_DESCRIPTOR_UI_NOTIFICATION: ui notification
 * 
 * Common port attributes.
 */
typedef enum{
  AGS_PORT_DESCRIPTOR_ATOM            = 1,
  AGS_PORT_DESCRIPTOR_AUDIO           = 1 <<  1,
  AGS_PORT_DESCRIPTOR_CONTROL         = 1 <<  2,
  AGS_PORT_DESCRIPTOR_MIDI            = 1 <<  3,
  AGS_PORT_DESCRIPTOR_EVENT           = 1 <<  4,
  AGS_PORT_DESCRIPTOR_OUTPUT          = 1 <<  5,
  AGS_PORT_DESCRIPTOR_INPUT           = 1 <<  6,
  AGS_PORT_DESCRIPTOR_TOGGLED         = 1 <<  7,
  AGS_PORT_DESCRIPTOR_ENUMERATION     = 1 <<  8,
  AGS_PORT_DESCRIPTOR_LOGARITHMIC     = 1 <<  9,
  AGS_PORT_DESCRIPTOR_INTEGER         = 1 << 10,
  AGS_PORT_DESCRIPTOR_SAMPLERATE      = 1 << 11,
  AGS_PORT_DESCRIPTOR_BOUNDED_BELOW   = 1 << 12,
  AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE   = 1 << 13,
  AGS_PORT_DESCRIPTOR_UI_NOTIFICATION = 1 << 14,
}AgsPortDescriptorFlags;

struct _AgsBasePlugin
{
  GObject object;

  guint flags;
  
  gchar *filename;
  gchar *effect;

  guint port_group_count;
  guint *port_group;
  GList *port;
  
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
  GObjectClass object;

  gpointer (*instantiate)(AgsBasePlugin *base_plugin,
			  guint samplerate);

  void (*connect_port)(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location);
  
  void (*activate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);
  void (*deactivate)(AgsBasePlugin *base_plugin, gpointer plugin_handle);

  void (*run)(AgsBasePlugin *base_plugin,
	      gpointer plugin_handle,
	      snd_seq_event_t *seq_event,
	      guint frame_count);
  
  void (*load_plugin)(AgsBasePlugin *base_plugin);
};

struct _AgsPortDescriptor
{
  guint flags;
  
  guint port_index;

  gchar *port_name;
  gchar *port_symbol;

  gint scale_steps;
  gchar **scale_points;
  float *scale_value;
  
  GValue *lower_value;
  GValue *upper_value;

  GValue *default_value;  
  
  gpointer user_data;
};

GType ags_base_plugin_get_type(void);

AgsPortDescriptor* ags_port_descriptor_alloc();
void ags_port_descriptor_free(AgsPortDescriptor *port_descriptor);

GList* ags_port_descriptor_find_symbol(GList *port_descriptor,
				       gchar *port_symbol);

GList* ags_base_plugin_find_filename(GList *base_plugin, gchar *filename);
GList* ags_base_plugin_find_effect(GList *base_plugin, gchar *filename, gchar *effect);

GList* ags_base_plugin_find_ui_effect_index(GList *base_plugin, gchar *ui_filename, guint ui_effect_index);

GList* ags_base_plugin_sort(GList *base_plugin);

void ags_base_plugin_apply_port_group_by_prefix(AgsBasePlugin *base_plugin);

gpointer ags_base_plugin_instantiate(AgsBasePlugin *base_plugin,
				     guint samplerate);

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
