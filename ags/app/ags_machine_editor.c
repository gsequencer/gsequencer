/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_callbacks.h>

#include <ags/app/ags_listing_editor.h>
#include <ags/app/ags_property_collection_editor.h>
#include <ags/app/ags_link_collection_editor.h>
#include <ags/app/ags_resize_editor.h>

#include <ags/i18n.h>

void ags_machine_editor_class_init(AgsMachineEditorClass *machine_editor);
void ags_machine_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_init(AgsMachineEditor *machine_editor);
void ags_machine_editor_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_machine_editor_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);

void ags_machine_editor_connect(AgsConnectable *connectable);
void ags_machine_editor_disconnect(AgsConnectable *connectable);

void ags_machine_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_apply(AgsApplicable *applicable);
void ags_machine_editor_reset(AgsApplicable *applicable);

void ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor,
					 AgsMachine *machine);

/**
 * SECTION:ags_machine_editor
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditor
 * @section_id:
 * @include: ags/app/ags_machine_editor.h
 *
 * #AgsMachineEditor is a composite widget to edit all aspects of #AgsAudio.
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

static guint machine_editor_signals[LAST_SIGNAL];

GType
ags_machine_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor = 0;

    static const GTypeInfo ags_machine_editor_info = {
      sizeof (AgsMachineEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor = g_type_register_static(GTK_TYPE_DIALOG,
						     "AgsMachineEditor", &ags_machine_editor_info,
						     0);

    g_type_add_interface_static(ags_type_machine_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_class_init(AgsMachineEditorClass *machine_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor;

  gobject->set_property = ags_machine_editor_set_property;
  gobject->get_property = ags_machine_editor_get_property;

  /* properties */
  /**
   * AgsMachineEditor:machine:
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

  /* AgsMachineEditorClass */
  machine_editor->set_machine = ags_machine_editor_real_set_machine;

  /* signals */
  /**
   * AgsMachineEditor::set-machine:
   * @machine_editor: the #AgsMachineEditor
   * @machine: the #AgsMachine to set
   *
   * The ::set-machine notify about modified machine.
   * 
   * Since: 3.0.0
   */
  machine_editor_signals[SET_MACHINE] =
    g_signal_new("set-machine",
		 G_TYPE_FROM_CLASS (machine_editor),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsMachineEditorClass, set_machine),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_machine_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_connect;
  connectable->disconnect = ags_machine_editor_disconnect;
}

void
ags_machine_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_set_update;
  applicable->apply = ags_machine_editor_apply;
  applicable->reset = ags_machine_editor_reset;
}

void
ags_machine_editor_init(AgsMachineEditor *machine_editor)
{
  GtkNotebook *notebook;
  GtkScrolledWindow *scrolled_window;

  gtk_window_set_title((GtkWindow *) machine_editor,
		       i18n("properties"));

  machine_editor->flags = 0;

  machine_editor->version = AGS_MACHINE_EDITOR_DEFAULT_VERSION;
  machine_editor->build_id = AGS_MACHINE_EDITOR_DEFAULT_BUILD_ID;

  machine_editor->machine = NULL;

  machine_editor->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) machine_editor),
		     (GtkWidget *) notebook,
		     TRUE, TRUE,
		     0);

  /* AgsOutput */
  machine_editor->output_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("output")));

  /* AgsInput */
  machine_editor->input_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("input")));


  /* AgsOutput link editor */
  machine_editor->output_link_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("link output")));

  /* AgsInput link editor */
  machine_editor->input_link_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("link input")));

  /* resize editor */
  machine_editor->resize_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(i18n("resize channels")));

  /* GtkButton's in GtkDialog->action_area  */
  machine_editor->apply = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Apply"));
  gtk_dialog_add_action_widget((GtkDialog *) machine_editor,
			       (GtkWidget *) machine_editor->apply,
			       GTK_RESPONSE_NONE);

  machine_editor->ok = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_OK"));
  gtk_dialog_add_action_widget((GtkDialog *) machine_editor,
			       (GtkWidget *) machine_editor->ok,
			       GTK_RESPONSE_NONE);

  machine_editor->cancel = (GtkButton *) gtk_button_new_with_mnemonic("_Cancel");
  gtk_dialog_add_action_widget((GtkDialog *) machine_editor,
			       (GtkWidget *) machine_editor->cancel,
			       GTK_RESPONSE_NONE);
}

void
ags_machine_editor_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_MACHINE:
  {
    AgsMachine *machine;

    machine = (AgsMachine *) g_value_get_object(value);

    ags_machine_editor_set_machine(machine_editor, machine);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    g_value_set_object(value, machine_editor->machine);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_connect(AgsConnectable *connectable)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(connectable);

  if((AGS_MACHINE_EDITOR_CONNECTED & (machine_editor->flags)) != 0){
    return;
  }

  machine_editor->flags |= AGS_MACHINE_EDITOR_CONNECTED;
  
  /* AgsMachineEditor tabs */
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->output_editor));
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->input_editor));

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->output_link_editor));
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->input_link_editor));

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->resize_editor));

  /* AgsMachineEditor buttons */
  g_signal_connect((GObject *) machine_editor->apply, "clicked",
		   G_CALLBACK(ags_machine_editor_apply_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->ok, "clicked",
		   G_CALLBACK(ags_machine_editor_ok_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->cancel, "clicked",
		   G_CALLBACK(ags_machine_editor_cancel_callback), (gpointer) machine_editor);
}

void
ags_machine_editor_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(connectable);

  if((AGS_MACHINE_EDITOR_CONNECTED & (machine_editor->flags)) == 0){
    return;
  }
  
  machine_editor->flags &= (~AGS_MACHINE_EDITOR_CONNECTED);
  
  /* AgsMachineEditor tabs */
  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor->output_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor->input_editor));

  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor->output_link_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor->input_link_editor));

  ags_connectable_disconnect(AGS_CONNECTABLE(machine_editor->resize_editor));
}

void
ags_machine_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(machine_editor->output_editor), update);
  ags_applicable_set_update(AGS_APPLICABLE(machine_editor->input_editor), update);

  ags_applicable_set_update(AGS_APPLICABLE(machine_editor->output_link_editor), update);
  ags_applicable_set_update(AGS_APPLICABLE(machine_editor->input_link_editor), update);

  ags_applicable_set_update(AGS_APPLICABLE(machine_editor->resize_editor), update);
}

void
ags_machine_editor_apply(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(applicable);

  ags_applicable_apply(AGS_APPLICABLE(machine_editor->output_editor));
  ags_applicable_apply(AGS_APPLICABLE(machine_editor->input_editor));

  ags_applicable_apply(AGS_APPLICABLE(machine_editor->output_link_editor));
  ags_applicable_apply(AGS_APPLICABLE(machine_editor->input_link_editor));

  ags_applicable_apply(AGS_APPLICABLE(machine_editor->resize_editor));
}

void
ags_machine_editor_reset(AgsApplicable *applicable)
{
  AgsMachineEditor *machine_editor;

  machine_editor = AGS_MACHINE_EDITOR(applicable);

  ags_applicable_reset(AGS_APPLICABLE(machine_editor->output_editor));
  ags_applicable_reset(AGS_APPLICABLE(machine_editor->input_editor));

  ags_applicable_reset(AGS_APPLICABLE(machine_editor->output_link_editor));
  ags_applicable_reset(AGS_APPLICABLE(machine_editor->input_link_editor));

  ags_applicable_reset(AGS_APPLICABLE(machine_editor->resize_editor));
}

/**
 * ags_machine_editor_add_children:
 * @machine_editor: the #AgsMachineEditor
 *
 * Add all child editors.
 *
 * Since: 3.0.0
 */
void
ags_machine_editor_add_children(AgsMachineEditor *machine_editor)
{
  gchar **output_link_editor_child_strv;
  gchar **input_link_editor_child_strv;

  GValue *output_link_editor_child_value;
  GValue *input_link_editor_child_value;

  /* output */
  output_link_editor_child_strv = (gchar **) g_malloc(2 * sizeof(gchar *));

  output_link_editor_child_strv[0] = g_strdup("channel-type");
  output_link_editor_child_strv[1] = NULL;

  output_link_editor_child_value = g_new0(GValue,
					  1);

  g_value_init(output_link_editor_child_value, G_TYPE_GTYPE);
  g_value_set_gtype(output_link_editor_child_value,
		    AGS_TYPE_OUTPUT);

  /* input */
  input_link_editor_child_strv = (gchar **) g_malloc(2 * sizeof(gchar *));

  input_link_editor_child_strv[0] = g_strdup("channel-type");
  input_link_editor_child_strv[1] = NULL;
  
  input_link_editor_child_value = g_new0(GValue,
					 1);

  g_value_init(input_link_editor_child_value, G_TYPE_GTYPE);
  g_value_set_gtype(input_link_editor_child_value,
		    AGS_TYPE_INPUT);

  /* AgsOutput */
  machine_editor->output_editor = ags_listing_editor_new(AGS_TYPE_OUTPUT);
  gtk_container_add((GtkContainer *) machine_editor->output_scrolled_window,
		    (GtkWidget *) machine_editor->output_editor);

  ags_listing_editor_add_children(machine_editor->output_editor,
				  machine_editor->machine->audio, 0,
				  FALSE);

  /* AgsInput */
  machine_editor->input_editor = ags_listing_editor_new(AGS_TYPE_INPUT);
  gtk_container_add((GtkContainer *) machine_editor->input_scrolled_window,
		    (GtkWidget *) machine_editor->input_editor);

  ags_listing_editor_add_children(machine_editor->input_editor,
				  machine_editor->machine->audio, 0,
				  FALSE);

  /* AgsOutput link editor */
  machine_editor->output_link_editor = ags_property_collection_editor_new(AGS_TYPE_LINK_COLLECTION_EDITOR,
									  output_link_editor_child_strv,
									  output_link_editor_child_value);
  gtk_container_add((GtkContainer *) machine_editor->output_link_editor_scrolled_window,
		    (GtkWidget *) machine_editor->output_link_editor);

  /* AgsInput link editor */
  machine_editor->input_link_editor = ags_property_collection_editor_new(AGS_TYPE_LINK_COLLECTION_EDITOR,
									 input_link_editor_child_strv,
									 input_link_editor_child_value);
  gtk_container_add((GtkContainer *) machine_editor->input_link_editor_scrolled_window,
		    (GtkWidget *) machine_editor->input_link_editor);

  /* resize editor */
  machine_editor->resize_editor = ags_resize_editor_new();
  gtk_container_add((GtkContainer *) machine_editor->resize_editor_scrolled_window,
		    (GtkWidget *) machine_editor->resize_editor);
}

void
ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor, AgsMachine *machine)
{
  if(machine_editor->machine != NULL){
    gtk_widget_destroy(GTK_WIDGET(machine_editor->output_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->input_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->output_link_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->input_link_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->resize_editor));
  }
  
  machine_editor->machine = machine;

  if(machine != NULL){
    ags_machine_editor_add_children(machine_editor);
  }
}

/**
 * ags_machine_editor_set_machine:
 * @machine_editor: an #AgsMachineEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine gets modified.
 *
 * Since: 3.0.0
 */
void
ags_machine_editor_set_machine(AgsMachineEditor *machine_editor, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR(machine_editor));

  g_object_ref((GObject *) machine_editor);
  g_signal_emit(G_OBJECT(machine_editor),
		machine_editor_signals[SET_MACHINE], 0,
		machine);
  g_object_unref((GObject *) machine_editor);
}

/**
 * ags_machine_editor_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsMachineEditor
 *
 * Returns: a new #AgsMachineEditor
 *
 * Since: 3.0.0
 */
AgsMachineEditor*
ags_machine_editor_new(AgsMachine *machine)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) g_object_new(AGS_TYPE_MACHINE_EDITOR,
						     "machine", machine,
						     NULL);

  return(machine_editor);
}
