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

#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_connection_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_output_collection_editor.h>
#include <ags/X/ags_output_listing_editor.h>

void ags_connection_editor_class_init(AgsConnectionEditorClass *connection_editor);
void ags_connection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_connection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_connection_editor_init(AgsConnectionEditor *connection_editor);
void ags_connection_editor_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_connection_editor_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_connection_editor_connect(AgsConnectable *connectable);
void ags_connection_editor_disconnect(AgsConnectable *connectable);
void ags_connection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_connection_editor_apply(AgsApplicable *applicable);
void ags_connection_editor_reset(AgsApplicable *applicable);

void ags_connection_editor_real_set_machine(AgsConnectionEditor *connection_editor,
					    AgsMachine *machine);

/**
 * SECTION:ags_connection_editor
 * @short_description: pack pad editors.
 * @title: AgsConnectionEditor
 * @section_id:
 * @include: ags/X/ags_connection_editor.h
 *
 * #AgsConnectionEditor is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editors.
 */

enum{
  SET_MACHINE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static guint connection_editor_signals[LAST_SIGNAL];

GType
ags_connection_editor_get_type(void)
{
  static GType ags_type_connection_editor = 0;

  if(!ags_type_connection_editor){
    static const GTypeInfo ags_connection_editor_info = {
      sizeof (AgsConnectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_connection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_connection_editor = g_type_register_static(GTK_TYPE_DIALOG,
							"AgsConnectionEditor", &ags_connection_editor_info,
							0);

    g_type_add_interface_static(ags_type_connection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_connection_editor);
}

void
ags_connection_editor_class_init(AgsConnectionEditorClass *connection_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) connection_editor;

  gobject->set_property = ags_connection_editor_set_property;
  gobject->get_property = ags_connection_editor_get_property;

  /* properties */
  /**
   * AgsConnection:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_object("machine",
				   "assigned machine",
				   "The machine which this machine editor is assigned with",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* AgsConnectionEditorClass */
  connection_editor->set_machine = ags_connection_editor_real_set_machine;

  /* signals */
  /**
   * AgsConnection::set-machine:
   * @connection_editor: the #AgsConnectionEditor
   * @machine: the #AgsMachine to set
   *
   * The ::set-machine notify about modified machine.
   */
  connection_editor_signals[SET_MACHINE] =
    g_signal_new("set-machine",
		 G_TYPE_FROM_CLASS (connection_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConnectionEditorClass, set_machine),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_connection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_connection_editor_connect;
  connectable->disconnect = ags_connection_editor_disconnect;
}

void
ags_connection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_connection_editor_set_update;
  applicable->apply = ags_connection_editor_apply;
  applicable->reset = ags_connection_editor_reset;
}

void
ags_connection_editor_init(AgsConnectionEditor *connection_editor)
{
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;

  gtk_window_set_title((GtkWindow *) connection_editor, g_strdup("connections"));

  connection_editor->flags = 0;

  connection_editor->version = AGS_CONNECTION_EDITOR_DEFAULT_VERSION;
  connection_editor->build_id = AGS_CONNECTION_EDITOR_DEFAULT_BUILD_ID;

  connection_editor->machine = NULL;

  connection_editor->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_box_pack_start((GtkBox *) connection_editor->dialog.vbox, (GtkWidget*) notebook, TRUE, TRUE, 0);

  /* output listing editor */
  connection_editor->output_listing_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("output")));

  /* audio connection editor */
  connection_editor->output_connection_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("connect output")));

  /* GtkButton's in GtkDialog->action_area  */
  connection_editor->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) connection_editor->dialog.action_area, (GtkWidget *) connection_editor->apply, FALSE, FALSE, 0);

  connection_editor->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) connection_editor->dialog.action_area, (GtkWidget *) connection_editor->ok, FALSE, FALSE, 0);

  connection_editor->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) connection_editor->dialog.action_area, (GtkWidget *) connection_editor->cancel, FALSE, FALSE, 0);
}

void
ags_connection_editor_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      ags_connection_editor_set_machine(connection_editor, machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    g_value_set_object(value, connection_editor->machine);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_editor_connect(AgsConnectable *connectable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(connectable);

  /* GtkNotebook */
  g_signal_connect((GtkNotebook *) connection_editor->notebook, "switch-page",
		   G_CALLBACK(ags_connection_editor_switch_page_callback), (gpointer) connection_editor);

  /* AgsConnectionEditor tabs */
  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->output_listing_editor));
  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->output_connection_editor));

  /* AgsConnectionEditor buttons */
  g_signal_connect((GObject *) connection_editor->apply, "clicked",
		   G_CALLBACK(ags_connection_editor_apply_callback), (gpointer) connection_editor);

  g_signal_connect((GObject *) connection_editor->ok, "clicked",
		   G_CALLBACK(ags_connection_editor_ok_callback), (gpointer) connection_editor);

  g_signal_connect((GObject *) connection_editor->cancel, "clicked",
		   G_CALLBACK(ags_connection_editor_cancel_callback), (gpointer) connection_editor);
}

void
ags_connection_editor_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(connectable);

  /* AgsConnectionEditor tabs */
  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->output_listing_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->output_connection_editor));
}

void
ags_connection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->output_listing_editor), update);
  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->output_connection_editor), update);
}

void
ags_connection_editor_apply(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);

  ags_applicable_apply(AGS_APPLICABLE(connection_editor->output_listing_editor));
  ags_applicable_apply(AGS_APPLICABLE(connection_editor->output_connection_editor));
}

void
ags_connection_editor_reset(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);

  ags_applicable_reset(AGS_APPLICABLE(connection_editor->output_listing_editor));
  ags_applicable_reset(AGS_APPLICABLE(connection_editor->output_connection_editor));
}

/**
 * ags_connection_editor_add_children:
 * @connection_editor: the #AgsConnectionEditor
 *
 * Add all child editors.
 *
 * Since: 0.7.65
 */
void
ags_connection_editor_add_children(AgsConnectionEditor *connection_editor)
{
  GParameter *output_connection_editor_child_parameter;
  
  /* output */
  output_connection_editor_child_parameter = g_new0(GParameter, 1);

  output_connection_editor_child_parameter[0].name = "channel_type";

  g_value_init(&(output_connection_editor_child_parameter[0].value), G_TYPE_GTYPE);
  g_value_set_gtype(&(output_connection_editor_child_parameter[0].value), AGS_TYPE_OUTPUT);

  /* AgsOutput listing editor */
  connection_editor->output_listing_editor = ags_output_listing_editor_new(AGS_TYPE_OUTPUT);
  gtk_scrolled_window_add_with_viewport(connection_editor->output_listing_editor_scrolled_window,
					(GtkWidget *) connection_editor->output_listing_editor);

  /* AgsOutput connection editor */
  connection_editor->output_connection_editor = ags_property_collection_editor_new(AGS_TYPE_OUTPUT_COLLECTION_EDITOR,
										   1,
										   output_connection_editor_child_parameter);
  gtk_scrolled_window_add_with_viewport(connection_editor->output_connection_editor_scrolled_window,
					(GtkWidget *) connection_editor->output_connection_editor);
}

void
ags_connection_editor_real_set_machine(AgsConnectionEditor *connection_editor, AgsMachine *machine)
{
  if(connection_editor->machine != NULL){
    gtk_widget_destroy(GTK_WIDGET(connection_editor->output_listing_editor));
    gtk_widget_destroy(GTK_WIDGET(connection_editor->output_connection_editor));
  }
  
  connection_editor->machine = machine;

  if(machine != NULL){
    ags_connection_editor_add_children(connection_editor);
  }
}

/**
 * ags_connection_editor_set_machine:
 * @connection_editor: an #AgsConnectionEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine gets modified.
 *
 * Since: 0.7.65
 */
void
ags_connection_editor_set_machine(AgsConnectionEditor *connection_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR(connection_editor));

  g_object_ref((GObject *) connection_editor);
  g_signal_emit(G_OBJECT(connection_editor),
		connection_editor_signals[SET_MACHINE], 0,
		machine);
  g_object_unref((GObject *) connection_editor);
}

/**
 * ags_connection_editor_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsConnectionEditor
 *
 * Returns: a new #AgsConnectionEditor
 *
 * Since: 0.7.65
 */
AgsConnectionEditor*
ags_connection_editor_new(AgsMachine *machine)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = (AgsConnectionEditor *) g_object_new(AGS_TYPE_CONNECTION_EDITOR,
							   "machine", machine,
							   NULL);

  return(connection_editor);
}
