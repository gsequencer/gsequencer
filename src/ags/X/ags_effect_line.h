/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

#define AGS_EFFECT_LINE_DEFAULT_VERSION "0.4.3\0"
#define AGS_EFFECT_LINE_DEFAULT_BUILD_ID "CEST 20-03-2015 08:24\0"

typedef struct _AgsEffectLine AgsEffectLine;
typedef struct _AgsEffectLineClass AgsEffectLineClass;

typedef enum{
  AGS_EFFECT_LINE_CONNECTED        = 1,
}AgsEffectLineFlags;

struct _AgsEffectLine
{
  GtkVBox vbox;

  guint flags;

  gchar *name;

  gchar *version;
  gchar *build_id;
  
  AgsChannel *channel;

  GtkButton *add;
  GtkButton *remove;

  GtkTable *table;
};

struct _AgsEffectLineClass
{
  GtkVBoxClass vbox;

  void (*add_effect)(AgsEffectLine *effect_line,
		     gchar *effect);
  void (*remove_effect)(AgsEffectLine *effect_line,
			guint nth);
};

GType ags_effect_line_get_type(void);

AgsEffectLine* ags_effect_line_new(AgsChannel *channel);

#endif /*__AGS_EFFECT_LINE_H__*/
