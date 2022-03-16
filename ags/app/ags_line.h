/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_LINE_H__
#define __AGS_LINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_line_member.h>
#include <ags/app/ags_effect_separator.h>

G_BEGIN_DECLS

#define AGS_TYPE_LINE                (ags_line_get_type())
#define AGS_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINE, AgsLine))
#define AGS_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINE, AgsLineClass))
#define AGS_IS_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LINE))
#define AGS_IS_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LINE))
#define AGS_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LINE, AgsLineClass))

#define AGS_LINE_PLUGIN(ptr) ((AgsLinePlugin *)(ptr))

#define AGS_LINE_DEFAULT_VERSION "4.0.0"
#define AGS_LINE_DEFAULT_BUILD_ID "Mon Feb 28 01:22:56 UTC 2022"

#define AGS_LINE_COLUMNS_COUNT (2)
#define AGS_LINE_SEPARATOR_FILENAME "ags-line-separator-filename"
#define AGS_LINE_SEPARATOR_EFFECT "ags-line-separator-effect"

typedef struct _AgsLine AgsLine;
typedef struct _AgsLinePlugin AgsLinePlugin;
typedef struct _AgsLineClass AgsLineClass;

typedef enum{
  AGS_LINE_GROUPED           = 1,
  AGS_LINE_MAPPED_RECALL     = 1 << 1,
  AGS_LINE_PREMAPPED_RECALL  = 1 << 2,
}AgsLineFlags;

struct _AgsLine
{
  GtkBox box;

  guint flags;
  guint connectable_flags;

  gchar *version;
  gchar *build_id;

  guint samplerate;
  guint buffer_size;
  guint format;

  gchar *name;

  AgsChannel *channel;
  
  GtkWidget *pad;

  GtkLabel *label;
  GtkToggleButton *group;

  GList *line_member;
  GList *effect_separator;

  AgsExpander *line_member_expander;

  GtkWidget *indicator;

  GList *plugin;

  GList *queued_drawing;
};

struct _AgsLineClass
{
  GtkBoxClass box;

  void (*samplerate_changed)(AgsLine *line,
			     guint samplerate, guint old_samplerate);
  void (*buffer_size_changed)(AgsLine *line,
			      guint buffer_size, guint old_buffer_size);
  void (*format_changed)(AgsLine *line,
			 guint format, guint old_format);
  
  void (*set_channel)(AgsLine *line, AgsChannel *channel);

  void (*group_changed)(AgsLine *line);
  
  void (*add_plugin)(AgsLine *line,
		     GList *control_type_name,
		     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		     gchar *plugin_name,
		     gchar *filename,
		     gchar *effect,
		     guint start_audio_channel, guint stop_audio_channel,
		     guint start_pad, guint stop_pad,
		     gint position,
		     guint create_flags, guint recall_flags);
  void (*remove_plugin)(AgsLine *line,
			guint nth);
  
  void (*map_recall)(AgsLine *line,
		     guint output_pad_start);
  GList* (*find_port)(AgsLine *line);

  void (*stop)(AgsLine *line,
	       GList *recall_id, gint sound_scope);
};

struct _AgsLinePlugin
{  
  AgsRecallContainer *play_container;
  AgsRecallContainer *recall_container;

  gchar *plugin_name;
  
  gchar *filename;
  gchar *effect;
  
  GList *control_type_name;

  guint control_count;
};

GType ags_line_get_type(void);

AgsLinePlugin* ags_line_plugin_alloc(AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect);
void ags_line_plugin_free(AgsLinePlugin *line_plugin);

void ags_line_samplerate_changed(AgsLine *line,
				 guint samplerate, guint old_samplerate);
void ags_line_buffer_size_changed(AgsLine *line,
				  guint buffer_size, guint old_buffer_size);
void ags_line_format_changed(AgsLine *line,
			     guint format, guint old_format);

void ags_line_set_channel(AgsLine *line, AgsChannel *channel);

void ags_line_group_changed(AgsLine *line);

GList* ags_line_get_line_member(AgsLine *line);
void ags_line_add_line_member(AgsLine *line,
			      AgsLineMember *line_member,
			      guint x, guint y,
			      guint width, guint height);
void ags_line_remove_line_member(AgsLine *line,
				 AgsLineMember *line_member);

GList* ags_line_get_effect_separator(AgsLine *line);
void ags_line_add_effect_separator(AgsLine *line,
				   AgsEffectSeparator *effect_separator,
				   guint x, guint y,
				   guint width, guint height);
void ags_line_remove_effect_separator(AgsLine *line,
				      AgsEffectSeparator *effect_separator);

void ags_line_add_plugin(AgsLine *line,
			 GList *control_type_name,
			 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			 gchar *plugin_name,
			 gchar *filename,
			 gchar *effect,
			 guint start_audio_channel, guint stop_audio_channel,
			 guint start_pad, guint stop_pad,
			 gint position,
			 guint create_flags, guint recall_flags);
void ags_line_remove_plugin(AgsLine *line,
			    guint nth);

void ags_line_map_recall(AgsLine *line,
			 guint output_pad_start);
GList* ags_line_find_port(AgsLine *line);

void ags_line_stop(AgsLine *line,
		   GList *recall_id, gint sound_scope);

GList* ags_line_find_next_grouped(GList *line);

void ags_line_check_message(AgsLine *line);

gboolean ags_line_indicator_queue_draw_timeout(GtkWidget *widget);

AgsLine* ags_line_new(GtkWidget *pad, AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_LINE_H__*/
