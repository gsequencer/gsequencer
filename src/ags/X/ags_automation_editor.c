/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/editor/ags_automation_editor.h>

#include <ags-lib/object/ags_connectable.h>

void ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor);
void ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_editor_init(AgsAutomationEditor *automation_editor);
void ags_automation_editor_connect(AgsConnectable *connectable);
void ags_automation_editor_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_editor
 * @short_description: machine radio buttons
 * @title: AgsAutomationEditor
 * @section_id:
 * @include: ags/X/editor/ags_automation_editor.h
 *
 * The #AgsAutomationEditor enables you make choice of an #AgsMachine.
 */

static gpointer ags_automation_editor_parent_class = NULL;

GType
ags_automation_editor_get_type(void)
{
  static GType ags_type_automation_editor = 0;

  if(!ags_type_automation_editor){
    static const GTypeInfo ags_automation_editor_info = {
      sizeof (AgsAutomationEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_editor = g_type_register_static(GTK_TYPE_DIALOG,
							"AgsAutomationEditor\0", &ags_automation_editor_info,
							0);
    
    g_type_add_interface_static(ags_type_automation_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_editor);
}

void
ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_editor_connect;
  connectable->disconnect = ags_automation_editor_disconnect;
}

void
ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor)
{
  GObjectClass *gobject;

  ags_automation_editor_parent_class = g_type_class_peek_parent(automation_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_editor;
}

void
ags_automation_editor_init(AgsAutomationEditor *automation_editor)
{
  g_object_set(G_OBJECT(automation_editor),
	       "title\0", g_strdup("edit automation\0"),
	       NULL);
}

void
ags_automation_editor_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_editor_new:
 *
 * Create a new #AgsAutomationEditor.
 *
 * Since: 0.4
 */
AgsAutomationEditor*
ags_automation_editor_new(AgsWindow *window)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) g_object_new(AGS_TYPE_AUTOMATION_EDITOR,
							   NULL);
  automation_editor->window = window;

  return(automation_editor);
}
