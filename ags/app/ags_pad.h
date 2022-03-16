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

#ifndef __AGS_PAD_H__
#define __AGS_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_line.h>

G_BEGIN_DECLS

#define AGS_TYPE_PAD                (ags_pad_get_type())
#define AGS_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD, AgsPad))
#define AGS_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD, AgsPadClass))
#define AGS_IS_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PAD))
#define AGS_IS_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PAD))
#define AGS_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PAD, AgsPadClass))

#define AGS_PAD_DEFAULT_VERSION "0.4.2"
#define AGS_PAD_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36"

typedef struct _AgsPad AgsPad;
typedef struct _AgsPadClass AgsPadClass;

typedef enum{
  AGS_PAD_SHOW_GROUPING       = 1,
  AGS_PAD_GROUP_ALL           = 1 <<  1,
  AGS_PAD_GROUP_LINE          = 1 <<  2,
  AGS_PAD_MAPPED_RECALL       = 1 <<  3,
  AGS_PAD_PREMAPPED_RECALL    = 1 <<  4,
  AGS_PAD_BLOCK_PLAY          = 1 <<  5,
  AGS_PAD_BLOCK_STOP          = 1 <<  6,
}AgsPadFlags;

struct _AgsPad
{
  GtkBox box;

  guint flags;
  guint connectable_flags;
  
  gchar *name;

  gchar *version;
  gchar *build_id;

  guint samplerate;
  guint buffer_size;
  guint format;

  AgsChannel *channel;

  guint cols;
  
  GList *line;
  
  AgsExpanderSet *line_expander_set;

  GtkToggleButton *group;
  GtkToggleButton *mute;
  GtkToggleButton *solo;

  GtkToggleButton *play;
};

struct _AgsPadClass
{
  GtkBoxClass box;

  void (*samplerate_changed)(AgsPad *pad,
			     guint samplerate, guint old_samplerate);
  void (*buffer_size_changed)(AgsPad *pad,
			      guint buffer_size, guint old_buffer_size);
  void (*format_changed)(AgsPad *pad,
			 guint format, guint old_format);

  void (*set_channel)(AgsPad *pad, AgsChannel *channel);

  void (*resize_lines)(AgsPad *pad, GType line_type,
		       guint audio_channels, guint audio_channels_old);
  void (*map_recall)(AgsPad *pad,
		     guint output_pad_start);
  GList* (*find_port)(AgsPad *pad);
};

GType ags_pad_get_type(void);

void ags_pad_samplerate_changed(AgsPad *pad,
				guint samplerate, guint old_samplerate);
void ags_pad_buffer_size_changed(AgsPad *pad,
				 guint buffer_size, guint old_buffer_size);
void ags_pad_format_changed(AgsPad *pad,
			    guint format, guint old_format);

void ags_pad_set_channel(AgsPad *pad, AgsChannel *channel);

GList* ags_pad_get_line(AgsPad *pad);
void ags_pad_add_line(AgsPad *pad,
		      AgsLine *line,
		      guint x, guint y,
		      guint width, guint height);
void ags_pad_remove_line(AgsPad *pad,
			 AgsLine *line);

void ags_pad_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old);

void ags_pad_map_recall(AgsPad *pad,
			guint output_pad_start);

GList* ags_pad_find_port(AgsPad *pad);

void ags_pad_play(AgsPad *pad);

AgsPad* ags_pad_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_PAD_H__*/
