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

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_automation_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor);
void ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_editor_init(AgsAutomationEditor *automation_editor);
void ags_automation_editor_connect(AgsConnectable *connectable);
void ags_automation_editor_disconnect(AgsConnectable *connectable);
void ags_automation_editor_finalize(GObject *gobject);

void ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_automation_editor_parent_class = NULL;
static guint automation_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_automation_editor
 * @short_description: machine radio buttons
 * @title: AgsAutomationEditor
 * @section_id:
 * @include: ags/X/editor/ags_automation_editor.h
 *
 * The #AgsAutomationEditor enables you make choice of an #AgsMachine.
 */

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
  GParamSpec *param_spec;

  ags_automation_editor_parent_class = g_type_class_peek_parent(automation_editor);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_editor;

  gobject->finalize = ags_automation_editor_finalize;

  /* AgsEditorClass */
  automation_editor->machine_changed = ags_automation_editor_real_machine_changed;

  /* signals */
  /**
   * AgsEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   */
  automation_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed\0",
                 G_TYPE_FROM_CLASS(automation_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAutomationEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_automation_editor_init(AgsAutomationEditor *automation_editor)
{
  GtkTable *table;

  g_object_set(G_OBJECT(automation_editor),
	       "title\0", "edit automation\0",
	       NULL);

  table = gtk_table_new(2,
			2,
			FALSE);
  gtk_box_pack_start(gtk_dialog_get_content_area(automation_editor),
		     GTK_WIDGET(table),
		     TRUE, TRUE,
		     0);

  automation_editor->automation_toolbar = ags_automation_toolbar_new();
  gtk_table_attach(table,
		   GTK_WIDGET(automation_editor->automation_toolbar),
		   0, 2,
		   0, 1,
		   GTK_FILL, 0,
		   0, 0);
		   
  automation_editor->machine_selector = ags_machine_selector_new();
  gtk_table_attach(table,
		   GTK_WIDGET(automation_editor->machine_selector),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  automation_editor->automation_edit = ags_automation_edit_new();
  gtk_table_attach(table,
		   GTK_WIDGET(automation_editor->automation_edit),
		   1, 2,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
}

void
ags_automation_editor_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(connectable);

  /*  */
  g_signal_connect_after(automation_editor, "delete-event\0",
			 G_CALLBACK(ags_automation_editor_delete_event_callback), NULL);

  g_signal_connect((GObject *) automation_editor->machine_selector, "changed\0",
		   G_CALLBACK(ags_automation_editor_machine_changed_callback), (gpointer) automation_editor);

  /* */
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->automation_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->automation_edit));
}

void
ags_automation_editor_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_automation_editor_parent_class)->finalize(gobject);
}

void
ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{
  automation_editor->selected_machine = machine;

  //TODO:JK: implement me
}

/**
 * ags_automation_editor_machine_changed:
 * @automation_editor: an #AgsAutomationEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of automation editor.
 *
 * Since: 0.4
 */
void
ags_automation_editor_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDITOR(automation_editor));

  g_object_ref((GObject *) automation_editor);
  g_signal_emit((GObject *) automation_editor,
		automation_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) automation_editor);
}

/**
 * ags_automation_editor_new:
 *
 * Create a new #AgsAutomationEditor.
 *
 * Since: 0.4
 */
AgsAutomationEditor*
ags_automation_editor_new(GObject *window)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) g_object_new(AGS_TYPE_AUTOMATION_EDITOR,
							   NULL);
  automation_editor->window = window;

  return(automation_editor);
}
