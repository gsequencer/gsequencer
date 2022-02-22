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

#ifndef __AGS_EFFECT_LINE_H__
#define __AGS_EFFECT_LINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_EFFECT_LINE                (ags_effect_line_get_type())
#define AGS_EFFECT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_LINE, AgsEffectLine))
#define AGS_EFFECT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_LINE, AgsEffectLineClass))
#define AGS_IS_EFFECT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_LINE))
#define AGS_IS_EFFECT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_LINE))
#define AGS_EFFECT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_LINE, AgsEffectLineClass))

#define AGS_EFFECT_LINE_PLUGIN(ptr) ((AgsEffectLinePlugin *)(ptr))

#define AGS_EFFECT_LINE_DEFAULT_VERSION "0.7.8"
#define AGS_EFFECT_LINE_DEFAULT_BUILD_ID "CEST 01-03-2016 00:23"

#define AGS_EFFECT_LINE_COLUMNS_COUNT (2)
#define AGS_EFFECT_LINE_SEPARATOR_FILENAME "ags-effect-line-separator-filename"
#define AGS_EFFECT_LINE_SEPARATOR_EFFECT "ags-effect-line-separator-effect"

typedef struct _AgsEffectLine AgsEffectLine;
typedef struct _AgsEffectLinePlugin AgsEffectLinePlugin;
typedef struct _AgsEffectLineClass AgsEffectLineClass;

typedef enum{
  AGS_EFFECT_LINE_CONNECTED           = 1,
  AGS_EFFECT_LINE_MAPPED_RECALL       = 1 <<  1,
  AGS_EFFECT_LINE_PREMAPPED_RECALL    = 1 <<  2,
}AgsEffectLineFlags;

struct _AgsEffectLine
{
  GtkBox box;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  guint samplerate;
  guint buffer_size;
  guint format;

  AgsChannel *channel;

  GtkLabel *label;
  GtkToggleButton *group;
  
  GtkGrid *grid;

  GList *plugin;

  GList *queued_drawing;
};

struct _AgsEffectLineClass
{
  GtkBoxClass box;

  void (*samplerate_changed)(AgsEffectLine *effect_line,
			     guint samplerate, guint old_samplerate);
  void (*buffer_size_changed)(AgsEffectLine *effect_line,
			      guint buffer_size, guint old_buffer_size);
  void (*format_changed)(AgsEffectLine *effect_line,
			 guint format, guint old_format);
  
  void (*set_channel)(AgsEffectLine *effect_line, AgsChannel *channel);

  void (*add_plugin)(AgsEffectLine *effect_line,
		     GList *control_type_name,
		     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		     gchar *plugin_name,
		     gchar *filename,
		     gchar *effect,
		     guint start_audio_channel, guint stop_audio_channel,
		     guint start_pad, guint stop_pad,
		     gint position,
		     guint create_flags, guint recall_flags);
  void (*remove_plugin)(AgsEffectLine *effect_line,
			guint nth);

  void (*map_recall)(AgsEffectLine *effect_line,
		     guint output_pad_start);
  GList* (*find_port)(AgsEffectLine *effect_line);

  void (*done)(AgsEffectLine *effect_line,
	       GObject *recall_id);
};

struct _AgsEffectLinePlugin
{  
  AgsRecallContainer *play_container;
  AgsRecallContainer *recall_container;

  gchar *plugin_name;
  
  gchar *filename;
  gchar *effect;
  
  GList *control_type_name;

  guint control_count;
};

GType ags_effect_line_get_type(void);

AgsEffectLinePlugin* ags_effect_line_plugin_alloc(AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
						  gchar *plugin_name,
						  gchar *filename,
						  gchar *effect);
void ags_effect_line_plugin_free(AgsEffectLinePlugin *effect_line_plugin);

void ags_effect_line_samplerate_changed(AgsEffectLine *effect_line,
					guint samplerate, guint old_samplerate);
void ags_effect_line_buffer_size_changed(AgsEffectLine *effect_line,
					 guint buffer_size, guint old_buffer_size);
void ags_effect_line_format_changed(AgsEffectLine *effect_line,
				    guint format, guint old_format);

void ags_effect_line_set_channel(AgsEffectLine *effect_line, AgsChannel *channel);

void ags_effect_line_add_plugin(AgsEffectLine *effect_line,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags);
void ags_effect_line_remove_plugin(AgsEffectLine *effect_line,
				   guint nth);

void ags_effect_line_map_recall(AgsEffectLine *effect_line,
				guint output_pad_start);
GList* ags_effect_line_find_port(AgsEffectLine *effect_line);

void ags_effect_line_done(AgsEffectLine *effect_line,
			  GObject *recall_id);

GList* ags_effect_line_find_next_grouped(GList *effect_line);

void ags_effect_line_check_message(AgsEffectLine *effect_line);

gboolean ags_effect_line_indicator_queue_draw_timeout(GtkWidget *widget);

AgsEffectLine* ags_effect_line_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_EFFECT_LINE_H__*/
