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

#ifndef __AGS_CONNECTION_EDITOR_BULK_H__
#define __AGS_CONNECTION_EDITOR_BULK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONNECTION_EDITOR_BULK                (ags_connection_editor_bulk_get_type())
#define AGS_CONNECTION_EDITOR_BULK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTION_EDITOR_BULK, AgsConnectionEditorBulk))
#define AGS_CONNECTION_EDITOR_BULK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CONNECTION_EDITOR_BULK, AgsConnectionEditorBulkClass))
#define AGS_IS_CONNECTION_EDITOR_BULK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CONNECTION_EDITOR_BULK))
#define AGS_IS_CONNECTION_EDITOR_BULK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CONNECTION_EDITOR_BULK))
#define AGS_CONNECTION_EDITOR_BULK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CONNECTION_EDITOR_BULK, AgsConnectionEditorBulkClass))

typedef struct _AgsConnectionEditorBulk AgsConnectionEditorBulk;
typedef struct _AgsConnectionEditorBulkClass AgsConnectionEditorBulkClass;

struct _AgsConnectionEditorBulk
{
  GtkBox box;

  guint connectable_flags;

  GtkGrid *output_grid;

  GtkComboBox *output_soundcard;

  GtkSpinButton *output_first_line;
  GtkSpinButton *output_first_soundcard_line;

  GtkSpinButton *output_count;
  
  GtkButton *output_remove_bulk;

  GtkGrid *input_grid;

  GtkComboBox *input_soundcard;

  GtkSpinButton *input_first_line;
  GtkSpinButton *input_first_soundcard_line;

  GtkSpinButton *input_count;
  
  GtkButton *input_remove_bulk;
};

struct _AgsConnectionEditorBulkClass
{
  GtkBoxClass box;
};

GType ags_connection_editor_bulk_get_type(void);

xmlNode* ags_connection_editor_bulk_to_xml_node(AgsConnectionEditorBulk *connection_editor_bulk);
void ags_connection_editor_bulk_from_xml_node(AgsConnectionEditorBulk *connection_editor_bulk,
					      xmlNode *node);

AgsConnectionEditorBulk* ags_connection_editor_bulk_new();

G_END_DECLS

#endif /*__AGS_CONNECTION_EDITOR_BULK_H__*/
