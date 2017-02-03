/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_EFFECT_LINE                (ags_effect_line_get_type())
#define AGS_EFFECT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_LINE, AgsEffectLine))
#define AGS_EFFECT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_LINE, AgsEffectLineClass))
#define AGS_IS_EFFECT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_LINE))
#define AGS_IS_EFFECT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_LINE))
#define AGS_EFFECT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_LINE, AgsEffectLineClass))

#define AGS_EFFECT_LINE_DEFAULT_VERSION "0.7.8\0"
#define AGS_EFFECT_LINE_DEFAULT_BUILD_ID "CEST 01-03-2016 00:23\0"

#define AGS_EFFECT_LINE_COLUMNS_COUNT (2)

typedef struct _AgsEffectLine AgsEffectLine;
typedef struct _AgsEffectLineClass AgsEffectLineClass;

typedef enum{
  AGS_EFFECT_LINE_CONNECTED           = 1,
  AGS_EFFECT_LINE_MAPPED_RECALL       = 1 <<  1,
  AGS_EFFECT_LINE_PREMAPPED_RECALL    = 1 <<  2,
}AgsEffectLineFlags;

struct _AgsEffectLine
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsChannel *channel;

  GtkLabel *label;
  
  GtkTable *table;

  GList *queued_drawing;
};

struct _AgsEffectLineClass
{
  GtkVBoxClass vbox;

  void (*set_channel)(AgsEffectLine *effect_line, AgsChannel *channel);

  GList* (*add_effect)(AgsEffectLine *effect_line,
		       GList *control_type_name,
		       gchar *filename,
		       gchar *effect);
  void (*remove_effect)(AgsEffectLine *effect_line,
			guint nth);

  void (*map_recall)(AgsEffectLine *effect_line,
		     guint output_pad_start);
  GList* (*find_port)(AgsEffectLine *effect_line);
};

GType ags_effect_line_get_type(void);

void ags_effect_line_set_channel(AgsEffectLine *effect_line, AgsChannel *channel);

GList* ags_effect_line_add_effect(AgsEffectLine *effect_line,
				  GList *control_type_name,
				  gchar *filename,
				  gchar *effect);
void ags_effect_line_remove_effect(AgsEffectLine *effect_line,
				   guint nth);

void ags_effect_line_map_recall(AgsEffectLine *effect_line,
				guint output_pad_start);
GList* ags_effect_line_find_port(AgsEffectLine *effect_line);

gboolean ags_effect_line_indicator_queue_draw_timeout(GtkWidget *widget);

AgsEffectLine* ags_effect_line_new(AgsChannel *channel);

#endif /*__AGS_EFFECT_LINE_H__*/
