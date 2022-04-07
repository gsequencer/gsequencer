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

#define AGS_CONNECTION_EDITOR_DEFAULT_VERSION "4.0.0"
#define AGS_CONNECTION_EDITOR_DEFAULT_BUILD_ID "Mon Feb 28 04:39:43 UTC 2022"

typedef struct _AgsConnectionEditor AgsConnectionEditor;
typedef struct _AgsConnectionEditorClass AgsConnectionEditorClass;

struct _AgsConnectionEditor
{
  GtkGrid grid;

  guint connectable_flags;
  
  gchar *version;
  gchar *build_id;

  AgsMachine *machine;

  GtkNotebook *notebook;

  AgsConnectionEditorListing *output_listing_editor;
  AgsConnectionEditorCollection *output_collection_editor;

  AgsConnectionEditorListing *input_listing_editor;
  AgsConnectionEditorCollection *input_collection_editor;
};

struct _AgsConnectionEditorClass
{
  GtkGridClass grid;

  void (*set_machine)(AgsConnectionEditor *connection_editor,
		      AgsMachine *machine);
};

GType ags_connection_editor_get_type(void);

void ags_connection_editor_set_machine(AgsConnectionEditor *connection_editor,
				       AgsMachine *machine);

AgsConnectionEditor* ags_connection_editor_new(AgsMachine *machine);

G_END_DECLS

#endif /*__AGS_CONNECTION_EDITOR_H__*/
