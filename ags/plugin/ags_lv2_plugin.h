/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_LV2_PLUGIN_H__
#define __AGS_LV2_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <stdarg.h>

#include <ags/libags.h>

#include <ags/plugin/ags_base_plugin.h>

#include <alsa/seq_midi_event.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_PLUGIN                (ags_lv2_plugin_get_type())
#define AGS_LV2_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_PLUGIN, AgsLv2Plugin))
#define AGS_LV2_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_PLUGIN, AgsLv2PluginClass))
#define AGS_IS_LV2_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_PLUGIN))
#define AGS_IS_LV2_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_PLUGIN))
#define AGS_LV2_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_PLUGIN, AgsLv2PluginClass))

#define AGS_LV2_PLUGIN_DESCRIPTOR(ptr) ((LV2_Descriptor *)(ptr))
  
#define AGS_LV2_EVENT_BUFFER(ptr) ((LV2_Event_Buffer *)(ptr))
#define AGS_LV2_EVENT(ptr) ((LV2_Event *)(ptr))
#define AGS_LV2_EVENT_DATA(ptr) ((void *)(ptr + sizeof(LV2_Event)))

#define AGS_LV2_ATOM_SEQUENCE(ptr) ((LV2_Atom_Sequence *)(ptr))
#define AGS_LV2_ATOM_EVENT(ptr) ((LV2_Atom_Event *)(ptr))

typedef struct _AgsLv2Plugin AgsLv2Plugin;
typedef struct _AgsLv2PluginClass AgsLv2PluginClass;

/**
 * AgsLv2PluginFlags:
 * @AGS_LV2_PLUGIN_IS_SYNTHESIZER: plugin is synthesizer
 * @AGS_LV2_PLUGIN_NEEDS_WORKER: plugin needs worker
 * @AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE: plugin has program interface
 * 
 * Enum values to control the behavior or indicate internal state of #AgsLv2Plugin by
 * enable/disable as flags.
 */
typedef enum{
  AGS_LV2_PLUGIN_IS_SYNTHESIZER            = 1,
  AGS_LV2_PLUGIN_NEEDS_WORKER              = 1 <<  1,
  AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE     = 1 <<  2,
}AgsLv2PluginFlags;

struct _AgsLv2Plugin
{
  AgsBasePlugin base_plugin;

  guint flags;

  gchar *pname;
  gchar *uri;
  gchar *ui_uri;
  
  AgsTurtle *manifest;
  AgsTurtle *turtle;

  gchar *doap_name;
  gchar *foaf_name;
  gchar *foaf_homepage;
  gchar *foaf_mbox;

  LV2_Feature **feature;

  gchar *program;
  
  GList *preset;
};

struct _AgsLv2PluginClass
{
  AgsBasePluginClass base_plugin;

  void (*change_program)(AgsLv2Plugin *lv2_plugin,
			 gpointer lv2_handle,
			 guint bank_index,
			 guint program_index);
};

GType ags_lv2_plugin_get_type(void);
GType ags_lv2_plugin_flags_get_type();

gboolean ags_lv2_plugin_test_flags(AgsLv2Plugin *lv2_plugin, AgsLv2PluginFlags flags);
void ags_lv2_plugin_set_flags(AgsLv2Plugin *lv2_plugin, AgsLv2PluginFlags flags);
void ags_lv2_plugin_unset_flags(AgsLv2Plugin *lv2_plugin, AgsLv2PluginFlags flags);

LV2_Event_Buffer* ags_lv2_plugin_event_buffer_alloc(guint buffer_size);
void ags_lv2_plugin_event_buffer_free(gpointer event_buffer);
void ags_lv2_plugin_event_buffer_realloc_data(LV2_Event_Buffer *event_buffer,
					      guint buffer_size);
LV2_Event_Buffer* ags_lv2_plugin_event_buffer_concat(LV2_Event_Buffer *event_buffer, ...);
						     
gboolean ags_lv2_plugin_event_buffer_append_midi(gpointer event_buffer,
						 guint buffer_size,
						 snd_seq_event_t *events,
						 guint event_count);
gboolean ags_lv2_plugin_event_buffer_remove_midi(gpointer event_buffer,
						 guint buffer_size,
						 guint note);
void ags_lv2_plugin_clear_event_buffer(gpointer event_buffer,
				       guint buffer_size);

gpointer ags_lv2_plugin_alloc_atom_sequence(guint sequence_size);
void ags_lv2_plugin_atom_sequence_free(gpointer atom_sequence);

gboolean ags_lv2_plugin_atom_sequence_append_midi(gpointer atom_sequence,
						  guint sequence_size,
						  snd_seq_event_t *events,
						  guint event_count);
gboolean ags_lv2_plugin_atom_sequence_remove_midi(gpointer atom_sequence,
						  guint sequence_size,
						  guint note);
void ags_lv2_plugin_clear_atom_sequence(gpointer atom_sequence,
					guint sequence_size);

GList* ags_lv2_plugin_find_uri(GList *lv2_plugin,
			       gchar *uri);
GList* ags_lv2_plugin_find_pname(GList *lv2_plugin,
				 gchar *pname);

void ags_lv2_plugin_change_program(AgsLv2Plugin *lv2_plugin,
				   gpointer lv2_handle,
				   guint bank_index,
				   guint program_index);

AgsLv2Plugin* ags_lv2_plugin_new(AgsTurtle *turtle, gchar *filename, gchar *effect, gchar *uri, guint effect_index);

G_END_DECLS

#endif /*__AGS_LV2_PLUGIN_H__*/
