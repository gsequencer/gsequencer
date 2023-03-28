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

#ifndef __AGS_CONNECTION_EDITOR_LINE_H__
#define __AGS_CONNECTION_EDITOR_LINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONNECTION_EDITOR_LINE                (ags_connection_editor_line_get_type())
#define AGS_CONNECTION_EDITOR_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_EDITOR_LINE, AgsConnectionEditorLine))
#define AGS_CONNECTION_EDITOR_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_EDITOR_LINE, AgsConnectionEditorLineClass))
#define AGS_IS_CONNECTION_EDITOR_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_EDITOR_LINE))
#define AGS_IS_CONNECTION_EDITOR_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_EDITOR_LINE))
#define AGS_CONNECTION_EDITOR_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONNECTION_EDITOR_LINE, AgsConnectionEditorLineClass))

typedef struct _AgsConnectionEditorLine AgsConnectionEditorLine;
typedef struct _AgsConnectionEditorLineClass AgsConnectionEditorLineClass;

struct _AgsConnectionEditorLine
{
  GtkBox box;

  guint connectable_flags;

  GtkWidget *parent_connection_editor_pad;
  
  AgsChannel *channel;

  GtkBox *output_box;

  GtkLabel *output_label;  
  GtkComboBox *output_soundcard;
  GtkSpinButton *output_line;
  
  GtkBox *input_box;

  GtkLabel *input_label;  
  GtkComboBox *input_soundcard;
  GtkSpinButton *input_line;
};

struct _AgsConnectionEditorLineClass
{
  GtkBoxClass box;
};

GType ags_connection_editor_line_get_type(void);

AgsConnectionEditorLine* ags_connection_editor_line_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_CONNECTION_EDITOR_LINE_H__*/
