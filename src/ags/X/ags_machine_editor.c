/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_machine_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_link_collection_editor.h>

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
void ags_machine_editor_destroy(GtkObject *object);
void ags_machine_editor_show(GtkWidget *widget);

void ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor,
					 AgsMachine *machine);

/**
 * SECTION:ags_machine_editor
 * @short_description: pack pad editors.
 * @title: AgsMachineEditor
 * @section_id:
 * @include: ags/X/ags_machine_editor.h
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
  static GType ags_type_machine_editor = 0;

  if(!ags_type_machine_editor){
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
						     "AgsMachineEditor\0", &ags_machine_editor_info,
						     0);

    g_type_add_interface_static(ags_type_machine_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_machine_editor);
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
   * Since: 0.3
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine editor is assigned with\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* AgsMachineEditorClass */
  machine_editor->set_machine = ags_machine_editor_real_set_machine;

  /* signals */
  /**
   * AgsMachine::add-default-recalls:
   * @machine_editor: the #AgsMachineEditor
   * @machine: the #AgsMachine to set
   *
   * The ::set-machine notify about modified machine.
   */
  machine_editor_signals[SET_MACHINE] =
    g_signal_new("set_machine\0",
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

  gtk_window_set_title((GtkWindow *) machine_editor, g_strdup("properties\0"));

  machine_editor->flags = 0;

  machine_editor->version = AGS_MACHINE_EDITOR_DEFAULT_VERSION;
  machine_editor->build_id = AGS_MACHINE_EDITOR_DEFAULT_BUILD_ID;

  machine_editor->parent = NULL;
  machine_editor->machine = NULL;

  machine_editor->notebook =
    notebook = (GtkNotebook *) gtk_notebook_new();
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.vbox,
		     (GtkWidget*) notebook,
		     TRUE, TRUE,
		     0);

  /* AgsOutput */
  machine_editor->output_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("output\0")));

  /* AgsInput */
  machine_editor->input_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("input\0")));


  /* AgsOutput link editor */
  machine_editor->output_link_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("link output\0")));

  /* AgsInput link editor */
  machine_editor->input_link_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook,
			   (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("link input\0")));

  /* resize editor */
  machine_editor->resize_editor_scrolled_window =
    scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_notebook_append_page(notebook, (GtkWidget *) scrolled_window,
			   (GtkWidget *) gtk_label_new(g_strdup("resize channels\0")));

  /* GtkButton's in GtkDialog->action_area  */
  machine_editor->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->add, FALSE, FALSE, 0);

  machine_editor->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->remove, FALSE, FALSE, 0);

  machine_editor->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->apply, FALSE, FALSE, 0);

  machine_editor->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->ok, FALSE, FALSE, 0);

  machine_editor->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) machine_editor->dialog.action_area, (GtkWidget *) machine_editor->cancel, FALSE, FALSE, 0);
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

  /* GtkObject */
  g_signal_connect((GObject *) machine_editor, "destroy\0",
		   G_CALLBACK(ags_machine_editor_destroy_callback), (gpointer) machine_editor);

  /* GtkWidget */
  g_signal_connect((GObject *) machine_editor, "show\0",
		   G_CALLBACK(ags_machine_editor_show_callback), (gpointer) machine_editor);

  /* GtkNotebook */
  g_signal_connect((GtkNotebook *) machine_editor->notebook, "switch-page\0",
		   G_CALLBACK(ags_machine_editor_switch_page_callback), (gpointer) machine_editor);

  /* AgsMachineEditor tabs */
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->output_editor));
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->input_editor));

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->output_link_editor));
  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->input_link_editor));

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor->resize_editor));

  /* AgsMachineEditor buttons */
  g_signal_connect((GObject *) machine_editor->add, "clicked\0",
		   G_CALLBACK(ags_machine_editor_add_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->remove, "clicked\0",
		   G_CALLBACK(ags_machine_editor_remove_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->apply, "clicked\0",
		   G_CALLBACK(ags_machine_editor_apply_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->ok, "clicked\0",
		   G_CALLBACK(ags_machine_editor_ok_callback), (gpointer) machine_editor);

  g_signal_connect((GObject *) machine_editor->cancel, "clicked\0",
		   G_CALLBACK(ags_machine_editor_cancel_callback), (gpointer) machine_editor);
}

void
ags_machine_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
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

void
ags_machine_editor_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_machine_editor_show(GtkWidget *widget)
{
  /* empty */
}

/**
 * ags_machine_editor_add_children:
 *
 * Add all child editors.
 *
 * Since: 0.3
 */
void
ags_machine_editor_add_children(AgsMachineEditor *machine_editor)
{
  GParameter *output_link_editor_child_parameter;
  GParameter *input_link_editor_child_parameter;

  /* output */
  output_link_editor_child_parameter = g_new0(GParameter, 1);

  output_link_editor_child_parameter[0].name = "channel_type";

  g_value_init(&(output_link_editor_child_parameter[0].value), G_TYPE_GTYPE);
  g_value_set_gtype(&(output_link_editor_child_parameter[0].value), AGS_TYPE_OUTPUT);

  /* input */
  input_link_editor_child_parameter = g_new0(GParameter, 1);

  input_link_editor_child_parameter[0].name = "channel_type";

  g_value_init(&(input_link_editor_child_parameter[0].value), G_TYPE_GTYPE);
  g_value_set_gtype(&(input_link_editor_child_parameter[0].value), AGS_TYPE_INPUT);


  /* AgsOutput */
  machine_editor->output_editor = ags_listing_editor_new(AGS_TYPE_OUTPUT);
  gtk_scrolled_window_add_with_viewport(machine_editor->output_scrolled_window,
					(GtkWidget *) machine_editor->output_editor);

  /* AgsInput */
  machine_editor->input_editor = ags_listing_editor_new(AGS_TYPE_INPUT);
  gtk_scrolled_window_add_with_viewport(machine_editor->input_scrolled_window,
					(GtkWidget *) machine_editor->input_editor);

  /* AgsOutput link editor */
  machine_editor->output_link_editor = ags_property_collection_editor_new(AGS_TYPE_LINK_COLLECTION_EDITOR,
									  1,
									  output_link_editor_child_parameter);
  gtk_scrolled_window_add_with_viewport(machine_editor->output_link_editor_scrolled_window,
					(GtkWidget *) machine_editor->output_link_editor);

  /* AgsInput link editor */
  machine_editor->input_link_editor = ags_property_collection_editor_new(AGS_TYPE_LINK_COLLECTION_EDITOR,
									 1,
									 input_link_editor_child_parameter);
  gtk_scrolled_window_add_with_viewport(machine_editor->input_link_editor_scrolled_window,
					(GtkWidget *) machine_editor->input_link_editor);

  /* resize editor */
  machine_editor->resize_editor = ags_resize_editor_new(AGS_TYPE_OUTPUT);
  gtk_scrolled_window_add_with_viewport(machine_editor->resize_editor_scrolled_window,
					(GtkWidget *) machine_editor->resize_editor);
}

void
ags_machine_editor_real_set_machine(AgsMachineEditor *machine_editor, AgsMachine *machine)
{
  AgsWindow *window;
  
  if(machine_editor->machine != NULL){
    gtk_widget_destroy(GTK_WIDGET(machine_editor->output_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->input_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->output_link_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->input_link_editor));
    gtk_widget_destroy(GTK_WIDGET(machine_editor->resize_editor));

    g_object_unref(machine_editor->machine);
  }

  if(machine != NULL){
    window = (AgsWindow *) gtk_widget_get_toplevel(machine);
    
    g_object_ref(machine);
  }

  machine_editor->machine = machine;
  machine_editor->parent = window;
  
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
 * Since: 0.3
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
 * Since: 0.3
 */
AgsMachineEditor*
ags_machine_editor_new(AgsMachine *machine)
{
  AgsMachineEditor *machine_editor;

  machine_editor = (AgsMachineEditor *) g_object_new(AGS_TYPE_MACHINE_EDITOR,
						     "machine\0", machine,
						     NULL);

  return(machine_editor);
}
