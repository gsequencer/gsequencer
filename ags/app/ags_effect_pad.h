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

#ifndef __AGS_EFFECT_PAD_H__
#define __AGS_EFFECT_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_effect_line.h>

G_BEGIN_DECLS

#define AGS_TYPE_EFFECT_PAD                (ags_effect_pad_get_type())
#define AGS_EFFECT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_PAD, AgsEffectPad))
#define AGS_EFFECT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_PAD, AgsEffectPadClass))
#define AGS_IS_EFFECT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_PAD))
#define AGS_IS_EFFECT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_PAD))
#define AGS_EFFECT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_PAD, AgsEffectPadClass))

#define AGS_EFFECT_PAD_DEFAULT_VERSION "0.7.8"
#define AGS_EFFECT_PAD_DEFAULT_BUILD_ID "CEST 01-03-2016 00:23"

#define AGS_EFFECT_PAD_COLUMNS_COUNT (2)

typedef struct _AgsEffectPad AgsEffectPad;
typedef struct _AgsEffectPadClass AgsEffectPadClass;

typedef enum{
  AGS_EFFECT_PAD_MAPPED_RECALL       = 1,
  AGS_EFFECT_PAD_PREMAPPED_RECALL    = 1 <<  1,
  AGS_EFFECT_PAD_CONNECTED           = 1 <<  2,
  AGS_EFFECT_PAD_SHOW_GROUPING       = 1 <<  3,
  AGS_EFFECT_PAD_GROUP_ALL           = 1 <<  4,
  AGS_EFFECT_PAD_GROUP_LINE          = 1 <<  5,
}AgsEffectPadFlags;

struct _AgsEffectPad
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
  
  GtkWidget *parent_effect_bridge;

  guint cols;

  GList *effect_line;
  
  GtkGrid *effect_line_grid;
};

struct _AgsEffectPadClass
{
  GtkBoxClass box;

  void (*samplerate_changed)(AgsEffectPad *effect_pad,
			     guint samplerate, guint old_samplerate);
  void (*buffer_size_changed)(AgsEffectPad *effect_pad,
			      guint buffer_size, guint old_buffer_size);
  void (*format_changed)(AgsEffectPad *effect_pad,
			 guint format, guint old_format);

  void (*set_channel)(AgsEffectPad *effect_pad, AgsChannel *channel);

  void (*resize_lines)(AgsEffectPad *effect_pad, GType line_type,
		       guint audio_channels, guint audio_channels_old);

  void (*map_recall)(AgsEffectPad *effect_pad);
  GList* (*find_port)(AgsEffectPad *effect_pad);

  void (*refresh_port)(AgsEffectPad *effect_pad);
};

GType ags_effect_pad_get_type(void);

void ags_effect_pad_samplerate_changed(AgsEffectPad *effect_pad,
				       guint samplerate, guint old_samplerate);
void ags_effect_pad_buffer_size_changed(AgsEffectPad *effect_pad,
					guint buffer_size, guint old_buffer_size);
void ags_effect_pad_format_changed(AgsEffectPad *effect_pad,
				   guint format, guint old_format);

void ags_effect_pad_set_channel(AgsEffectPad *effect_pad, AgsChannel *channel);

void ags_effect_pad_resize_lines(AgsEffectPad *effect_pad, GType line_type,
				 guint audio_channels, guint audio_channels_old);

GList* ags_effect_pad_get_effect_line(AgsEffectPad *effect_pad);
void ags_effect_pad_add_effect_line(AgsEffectPad *effect_pad,
				    AgsEffectLine *effect_line,
				    guint x, guint y,
				    guint width, guint height);
void ags_effect_pad_remove_effect_line(AgsEffectPad *effect_pad,
				       AgsEffectLine *effect_line);

void ags_effect_pad_map_recall(AgsEffectPad *effect_pad);
GList* ags_effect_pad_find_port(AgsEffectPad *effect_pad);

void ags_effect_pad_refresh_port(AgsEffectPad *effect_pad);

AgsEffectPad* ags_effect_pad_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_EFFECT_PAD_H__*/
