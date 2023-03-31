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

#ifndef __AGS_CONNECTION_EDITOR_H__
#define __AGS_CONNECTION_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor_listing.h>
#include <ags/app/ags_connection_editor_collection.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONNECTION_EDITOR                (ags_connection_editor_get_type())
#define AGS_CONNECTION_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditor))
#define AGS_CONNECTION_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditorClass))
#define AGS_IS_CONNECTION_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_EDITOR))
#define AGS_IS_CONNECTION_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_EDITOR))
#define AGS_CONNECTION_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONNECTION_EDITOR, AgsConnectionEditorClass))

#define AGS_CONNECTION_EDITOR_DEFAULT_VERSION "4.5.0"
#define AGS_CONNECTION_EDITOR_DEFAULT_BUILD_ID "Tue Mar 28 06:49:41 UTC 2023"

typedef struct _AgsConnectionEditor AgsConnectionEditor;
typedef struct _AgsConnectionEditorClass AgsConnectionEditorClass;

typedef enum{
  AGS_CONNECTION_EDITOR_SHOW_OUTPUT             = 1,
  AGS_CONNECTION_EDITOR_SHOW_INPUT              = 1 <<  1,
  AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT   = 1 <<  2,
  AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT    = 1 <<  3,
}AgsConnectionEditorFlags;

struct _AgsConnectionEditor
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
  
  gchar *version;
  gchar *build_id;

  AgsMachine *machine;

  GtkNotebook *notebook;

  GtkScrolledWindow *output_listing_scrolled_window;
  AgsConnectionEditorListing *output_editor_listing;

  GtkScrolledWindow *output_collection_scrolled_window;
  AgsConnectionEditorCollection *output_editor_collection;

  GtkScrolledWindow *input_listing_scrolled_window;  
  AgsConnectionEditorListing *input_editor_listing;
  
  GtkScrolledWindow *input_collection_scrolled_window;
  AgsConnectionEditorCollection *input_editor_collection;
};

struct _AgsConnectionEditorClass
{
  GtkGridClass grid;

  void (*set_machine)(AgsConnectionEditor *connection_editor,
		      AgsMachine *machine);
};

GType ags_connection_editor_get_type(void);

gboolean ags_connection_editor_test_flags(AgsConnectionEditor *connection_editor,
					  guint flags);
void ags_connection_editor_set_flags(AgsConnectionEditor *connection_editor,
				     guint flags);
void ags_connection_editor_unset_flags(AgsConnectionEditor *connection_editor,
				       guint flags);

void ags_connection_editor_set_machine(AgsConnectionEditor *connection_editor,
				       AgsMachine *machine);

AgsConnectionEditor* ags_connection_editor_new(AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_CONNECTION_EDITOR_H__*/
