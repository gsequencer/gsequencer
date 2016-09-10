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

#ifndef __AGS_CONNECTION_EDITOR_H__
#define __AGS_CONNECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_property_collection_editor.h>

#define AGS_TYPE_CONNECTION_EDITOR                (ags_connection_editor_get_type())
#define AGS_CONNECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditor))
#define AGS_CONNECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditorClass))
#define AGS_IS_CONNECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_EDITOR))
#define AGS_IS_CONNECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_EDITOR))
#define AGS_CONNECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditorClass))

#define AGS_CONNECTION_EDITOR_DEFAULT_VERSION "0.7.65\0"
#define AGS_CONNECTION_EDITOR_DEFAULT_BUILD_ID "Sat Sep 10 07:49:08 CEST 2016\0"

typedef struct _AgsConnectionEditor AgsConnectionEditor;
typedef struct _AgsConnectionEditorClass AgsConnectionEditorClass;

typedef enum{
  AGS_CONNECTION_EDITOR_CLOSING                    = 1,
}AgsConnectionEditorFlags;

struct _AgsConnectionEditor
{
  GtkDialog dialog;

  guint flags;

  gchar *version;
  gchar *build_id;

  AgsMachine *machine;

  GtkNotebook *notebook;

  GtkScrolledWindow *output_connection_editor_scrolled_window;
  AgsPropertyCollectionEditor *output_connection_editor;

  GtkButton *apply;
  GtkButton *ok;
  GtkButton *cancel;
};

struct _AgsConnectionEditorClass
{
  GtkDialogClass dialog;

  void (*set_machine)(AgsConnectionEditor *connection_editor,
		      AgsMachine *machine);
};

GType ags_connection_editor_get_type(void);

AgsConnectionEditor* ags_connection_editor_new(AgsMachine *machine);

#endif /*__AGS_CONNECTION_EDITOR_H__*/
