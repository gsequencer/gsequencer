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

#ifndef __AGS_MACHINE_EDITOR_BULK_H__
#define __AGS_MACHINE_EDITOR_BULK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_EDITOR_BULK                (ags_machine_editor_bulk_get_type())
#define AGS_MACHINE_EDITOR_BULK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_EDITOR_BULK, AgsMachineEditorBulk))
#define AGS_MACHINE_EDITOR_BULK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_EDITOR_BULK, AgsMachineEditorBulkClass))
#define AGS_IS_MACHINE_EDITOR_BULK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_EDITOR_BULK))
#define AGS_IS_MACHINE_EDITOR_BULK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_EDITOR_BULK))
#define AGS_MACHINE_EDITOR_BULK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_EDITOR_BULK, AgsMachineEditorBulkClass))

typedef struct _AgsMachineEditorBulk AgsMachineEditorBulk;
typedef struct _AgsMachineEditorBulkClass AgsMachineEditorBulkClass;

struct _AgsMachineEditorBulk
{
  GtkBox box;

  guint connectable_flags;

  GtkWidget *parent_machine_editor_collection;
  
  GtkComboBox *link;
  
  GtkSpinButton *first_line;
  GtkSpinButton *first_link_line;

  GtkSpinButton *count;
  
  GtkButton *remove_bulk;
};

struct _AgsMachineEditorBulkClass
{
  GtkBoxClass box;
};

GType ags_machine_editor_bulk_get_type(void);

xmlNode* ags_machine_editor_bulk_to_xml_node(AgsMachineEditorBulk *machine_editor_bulk);
void ags_machine_editor_bulk_from_xml_node(AgsMachineEditorBulk *machine_editor_bulk,
					   xmlNode *node);

AgsMachineEditorBulk* ags_machine_editor_bulk_new();

G_END_DECLS

#endif /*__AGS_MACHINE_EDITOR_BULK_H__*/
