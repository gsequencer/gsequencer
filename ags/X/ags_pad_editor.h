/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_PAD_EDITOR_H__
#define __AGS_PAD_EDITOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PAD_EDITOR                (ags_pad_editor_get_type())
#define AGS_PAD_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PAD_EDITOR, AgsPadEditor))
#define AGS_PAD_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))
#define AGS_IS_PAD_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PAD_EDITOR))
#define AGS_IS_PAD_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PAD_EDITOR))
#define AGS_PAD_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PAD_EDITOR, AgsPadEditorClass))

#define AGS_PAD_EDITOR_DEFAULT_VERSION "0.4.2\0"
#define AGS_PAD_EDITOR_DEFAULT_BUILD_ID "CEST 02-10-2014 19:36\0"

typedef struct _AgsPadEditor AgsPadEditor;
typedef struct _AgsPadEditorClass AgsPadEditorClass;

struct _AgsPadEditor
{
  GtkVBox vbox;

  gchar *version;
  gchar *build_id;
  
  AgsChannel *pad;

  GtkExpander *line_editor_expander;
  GtkVBox *line_editor;
};

struct _AgsPadEditorClass
{
  GtkVBoxClass vbox;
};

GType ags_pad_editor_get_type(void);

void ags_pad_editor_set_channel(AgsPadEditor *pad_editor, AgsChannel *channel);

void ags_pad_editor_resize_lines(AgsPadEditor *pad_editor,
				 guint audio_channels, guint audio_channels_old);

AgsPadEditor* ags_pad_editor_new(AgsChannel *channel);

#endif /*__AGS_PAD_EDITOR_H__*/
