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

#ifndef __AGS_NOTATION_META_H__
#define __AGS_NOTATION_META_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTATION_META                (ags_notation_meta_get_type())
#define AGS_NOTATION_META(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION_META, AgsNotationMeta))
#define AGS_NOTATION_META_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION_META, AgsNotationMetaClass))
#define AGS_IS_NOTATION_META(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTATION_META))
#define AGS_IS_NOTATION_META_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTATION_META))
#define AGS_NOTATION_META_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTATION_META, AgsNotationMetaClass))

typedef struct _AgsNotationMeta AgsNotationMeta;
typedef struct _AgsNotationMetaClass AgsNotationMetaClass;

typedef enum{
  AGS_NOTATION_META_CONNECTED   = 1,
}AgsNotationMetaFlags;

struct _AgsNotationMeta
{
  GtkVBox vbox;

  guint flags;

  GtkLabel *machine_type;
  GtkLabel *machine_name;

  GtkLabel *audio_channels;
  GtkLabel *output_pads;
  GtkLabel *input_pads;
  
  GtkLabel *editor_tool;

  GtkLabel *active_audio_channel;
  
  GtkLabel *cursor_x_position;
  GtkLabel *cursor_y_position;

  GtkLabel *current_note;
};

struct _AgsNotationMetaClass
{
  GtkVBoxClass vbox;
};

GType ags_notation_meta_get_type(void);

void ags_notation_meta_refresh(AgsNotationMeta *notation_meta);

AgsNotationMeta* ags_notation_meta_new();

G_END_DECLS

#endif /*__AGS_NOTATION_META_H__*/
