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

#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_connection_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

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
 * @short_description: Edit audio connections
 * @title: AgsConnectionEditor
 * @section_id:
 * @include: ags/app/ags_connection_editor.h
 *
 * #AgsConnectionEditor is a composite widget intended to assign output or input
 * soundcards. It consists of multiple child editors.
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor = 0;

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

    ags_type_connection_editor = g_type_register_static(GTK_TYPE_GRID,
							"AgsConnectionEditor", &ags_connection_editor_info,
							0);

    g_type_add_interface_static(ags_type_connection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_connection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine editor is assigned with"),
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
   * 
   * Since: 3.0.0
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

  gtk_grid_set_column_spacing(connection_editor,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(connection_editor,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  connection_editor->flags = 0;
  connection_editor->connectable_flags = 0;

  connection_editor->version = AGS_CONNECTION_EDITOR_DEFAULT_VERSION;
  connection_editor->build_id = AGS_CONNECTION_EDITOR_DEFAULT_BUILD_ID;

  connection_editor->machine = NULL;

  notebook = 
    connection_editor->notebook = (GtkNotebook *) gtk_notebook_new();

  gtk_widget_set_hexpand(connection_editor->notebook,
			 TRUE);
  gtk_widget_set_vexpand(connection_editor->notebook,
			 TRUE);

  gtk_widget_set_halign(connection_editor->notebook,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(connection_editor->notebook,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) connection_editor,
		  (GtkWidget *) notebook,
		  0, 0,
		  1, 1);

  /* AgsOutput */
  scrolled_window =
    connection_editor->output_listing_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand(scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_halign(scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(scrolled_window,
			GTK_ALIGN_FILL);

  connection_editor->output_editor_listing = ags_connection_editor_listing_new(AGS_TYPE_OUTPUT);  

  gtk_widget_set_halign(connection_editor->output_editor_listing,
			GTK_ALIGN_START);
  gtk_widget_set_valign(connection_editor->output_editor_listing,
			GTK_ALIGN_START);

  gtk_scrolled_window_set_child(scrolled_window,
				connection_editor->output_editor_listing);
  
  /* AgsOutput bulk editor */
  scrolled_window =
    connection_editor->output_collection_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand(scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_halign(scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(scrolled_window,
			GTK_ALIGN_FILL);

  connection_editor->output_editor_collection = ags_connection_editor_collection_new(AGS_TYPE_OUTPUT);  

  gtk_scrolled_window_set_child(scrolled_window,
				connection_editor->output_editor_collection);

  /* AgsInput */
  scrolled_window =
    connection_editor->input_listing_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand(scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_halign(scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(scrolled_window,
			GTK_ALIGN_FILL);

  connection_editor->input_editor_listing = ags_connection_editor_listing_new(AGS_TYPE_INPUT);  

  gtk_widget_set_halign(connection_editor->input_editor_listing,
			GTK_ALIGN_START);
  gtk_widget_set_valign(connection_editor->input_editor_listing,
			GTK_ALIGN_START);

  gtk_scrolled_window_set_child(scrolled_window,
				connection_editor->input_editor_listing);

  /* AgsInput bulk editor */
  scrolled_window =
    connection_editor->input_collection_scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_hexpand(scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand(scrolled_window,
			 TRUE);

  gtk_widget_set_halign(scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(scrolled_window,
			GTK_ALIGN_FILL);

  connection_editor->input_editor_collection = ags_connection_editor_collection_new(AGS_TYPE_INPUT);  

  gtk_widget_set_halign(connection_editor->output_editor_listing,
			GTK_ALIGN_START);
  gtk_widget_set_valign(connection_editor->output_editor_listing,
			GTK_ALIGN_START);

  gtk_scrolled_window_set_child(scrolled_window,
				connection_editor->input_editor_collection);
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
    {
      g_value_set_object(value, connection_editor->machine);
    }
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

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor->connectable_flags)) != 0){
    return;
  }

  connection_editor->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->output_editor_listing));
  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->output_editor_collection));

  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->input_editor_listing));
  ags_connectable_connect(AGS_CONNECTABLE(connection_editor->input_editor_collection));
}

void
ags_connection_editor_disconnect(AgsConnectable *connectable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (connection_editor->connectable_flags)) == 0){
    return;
  }
  
  connection_editor->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->output_editor_listing));
  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->output_editor_collection));

  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->input_editor_listing));
  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor->input_editor_collection));
}

void
ags_connection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->output_editor_listing), update);
  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->output_editor_collection), update);

  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->input_editor_listing), update);
  ags_applicable_set_update(AGS_APPLICABLE(connection_editor->input_editor_collection), update);
}

void
ags_connection_editor_apply(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);

  ags_applicable_apply(AGS_APPLICABLE(connection_editor->output_editor_listing));
  ags_applicable_apply(AGS_APPLICABLE(connection_editor->output_editor_collection));

  ags_applicable_apply(AGS_APPLICABLE(connection_editor->input_editor_listing));
  ags_applicable_apply(AGS_APPLICABLE(connection_editor->input_editor_collection));
}

void
ags_connection_editor_reset(AgsApplicable *applicable)
{
  AgsConnectionEditor *connection_editor;

  connection_editor = AGS_CONNECTION_EDITOR(applicable);
  
  ags_applicable_reset(AGS_APPLICABLE(connection_editor->output_editor_listing));
  ags_applicable_reset(AGS_APPLICABLE(connection_editor->output_editor_collection));

  ags_applicable_reset(AGS_APPLICABLE(connection_editor->input_editor_listing));
  ags_applicable_reset(AGS_APPLICABLE(connection_editor->input_editor_collection));
}

void
ags_connection_editor_real_set_machine(AgsConnectionEditor *connection_editor, AgsMachine *machine)
{
  connection_editor->machine = machine;

  if((AGS_CONNECTION_EDITOR_SHOW_OUTPUT & (connection_editor->flags)) != 0){
    gtk_notebook_append_page(connection_editor->notebook,
			     (GtkWidget *) connection_editor->output_listing_scrolled_window,
			     (GtkWidget *) gtk_label_new(i18n("output")));

    gtk_notebook_append_page(connection_editor->notebook,
			     (GtkWidget *) connection_editor->output_collection_scrolled_window,
			     (GtkWidget *) gtk_label_new(i18n("bulk output")));
  }
  
  if((AGS_CONNECTION_EDITOR_SHOW_INPUT & (connection_editor->flags)) != 0){
    gtk_notebook_append_page(connection_editor->notebook,
			     (GtkWidget *) connection_editor->input_listing_scrolled_window,
			     (GtkWidget *) gtk_label_new(i18n("input")));

    gtk_notebook_append_page(connection_editor->notebook,
			     (GtkWidget *) connection_editor->output_collection_scrolled_window,
			     (GtkWidget *) gtk_label_new(i18n("bulk input")));
  }
}

/**
 * ags_connection_editor_set_machine:
 * @connection_editor: an #AgsConnectionEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine gets modified.
 *
 * Since: 3.0.0
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
 * @machine: the assigned #AgsMachine.
 *
 * Create a new instance of #AgsConnectionEditor
 *
 * Returns: the new #AgsConnectionEditor
 *
 * Since: 3.0.0
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
