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

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_automation_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/X/ags_window.h>

void ags_automation_editor_class_init(AgsAutomationEditorClass *automation_editor);
void ags_automation_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_editor_init(AgsAutomationEditor *automation_editor);
void ags_automation_editor_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_automation_editor_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_automation_editor_connect(AgsConnectable *connectable);
void ags_automation_editor_disconnect(AgsConnectable *connectable);
void ags_automation_editor_finalize(GObject *gobject);

void ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_automation_editor_parent_class = NULL;
static guint automation_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_automation_editor
 * @short_description: A composite widget to edit automation
 * @title: AgsAutomationEditor
 * @section_id:
 * @include: ags/X/ags_automation_editor.h
 *
 * #AgsAutomationEditor is a composite widget to edit automation. You may select machines
 * or change editor tool to do automation.
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

    ags_type_automation_editor = g_type_register_static(GTK_TYPE_VBOX,
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

  gobject->set_property = ags_automation_editor_set_property;
  gobject->get_property = ags_automation_editor_get_property;

  gobject->finalize = ags_automation_editor_finalize;
  
  /* properties */
  /**
   * AgsAutomationEditor:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "assigned soundcard\0",
				   "The soundcard it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

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
  GtkHPaned *paned;
  GtkScrolledWindow *scrolled_window;
  GtkNotebook *notebook;
  
  g_signal_connect_after((GObject *) automation_editor, "parent-set\0",
			 G_CALLBACK(ags_automation_editor_parent_set_callback), automation_editor);

  automation_editor->flags = 0;

  automation_editor->version = AGS_AUTOMATION_EDITOR_DEFAULT_VERSION;
  automation_editor->build_id = AGS_AUTOMATION_EDITOR_DEFAULT_BUILD_ID;

  automation_editor->soundcard = NULL;

  automation_editor->automation_toolbar = ags_automation_toolbar_new();
  gtk_box_pack_start((GtkBox *) automation_editor,
		     (GtkWidget *) automation_editor->automation_toolbar,
		     FALSE, FALSE, 0);

  paned = (GtkHPaned *) gtk_hpaned_new();
  gtk_box_pack_start((GtkBox *) automation_editor,
		     (GtkWidget *) paned,
		     TRUE, TRUE, 0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) paned, (GtkWidget *) scrolled_window, FALSE, TRUE);
  //  gtk_widget_set_size_request((GtkWidget *) scrolled_window, 180, -1);

  automation_editor->machine_selector = g_object_new(AGS_TYPE_MACHINE_SELECTOR,
						     "homogeneous\0", FALSE,
						     "spacing\0", 0,
						     NULL);
  automation_editor->machine_selector->flags |= (AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING |
						 AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO |
						 AGS_MACHINE_SELECTOR_AUTOMATION);
  gtk_label_set_label(automation_editor->machine_selector->label,
		      "automation\0");
  
  automation_editor->machine_selector->popup = ags_machine_selector_popup_new(automation_editor->machine_selector);
  g_object_set(automation_editor->machine_selector->menu_button,
	       "menu\0", automation_editor->machine_selector->popup,
	       NULL);
  
  gtk_scrolled_window_add_with_viewport(scrolled_window, (GtkWidget *) automation_editor->machine_selector);

  automation_editor->selected_machine = NULL;

  automation_editor->automation_editor_child = NULL;

  notebook = gtk_notebook_new();
  gtk_paned_pack2((GtkPaned *) paned,
		  (GtkWidget *) notebook,
		  TRUE, FALSE);
  
  /* audio */
  automation_editor->audio_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(notebook,
			   automation_editor->audio_table,
			   gtk_label_new("audio\0"));
  
  /* output */
  automation_editor->output_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(notebook,
			   automation_editor->output_table,
			   gtk_label_new("output\0"));

  /* input */
  automation_editor->input_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(notebook,
			   automation_editor->input_table,
			   gtk_label_new("input\0"));

  /* currenty selected widgets */
  automation_editor->current_audio_scale = NULL;
  automation_editor->current_audio_automation_edit = NULL;

  automation_editor->current_output_notebook = NULL;  
  automation_editor->current_output_scale = NULL;
  automation_editor->current_output_automation_edit = NULL;

  automation_editor->current_input_notebook = NULL;  
  automation_editor->current_input_scale = NULL;
  automation_editor->current_input_automation_edit = NULL;

  /* offset */
  automation_editor->tact_counter = 0;
  automation_editor->current_tact = 0.0;
}

void
ags_automation_editor_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      AgsSoundcard *soundcard;

      soundcard = (AgsSoundcard *) g_value_get_object(value);

      if(automation_editor->soundcard == soundcard){
	return;
      }

      if(automation_editor->soundcard != NULL){
	g_object_unref(automation_editor->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      automation_editor->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_editor_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    g_value_set_object(value, automation_editor->soundcard);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_editor_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(connectable);

  if((AGS_AUTOMATION_EDITOR_CONNECTED & (automation_editor->flags)) != 0){
    return;
  }

  automation_editor->flags |= AGS_AUTOMATION_EDITOR_CONNECTED;
  
  /*  */
  g_signal_connect(automation_editor->soundcard, "tic\0",
		   ags_automation_editor_tic_callback, automation_editor);

  
  g_signal_connect((GObject *) automation_editor->machine_selector, "changed\0",
		   G_CALLBACK(ags_automation_editor_machine_changed_callback), (gpointer) automation_editor);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->automation_toolbar));
  ags_connectable_connect(AGS_CONNECTABLE(automation_editor->machine_selector));
}

void
ags_automation_editor_disconnect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(connectable);

  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->automation_toolbar)); 
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_editor->machine_selector));
}

void
ags_automation_editor_finalize(GObject *gobject)
{
  AgsAutomationEditor *automation_editor;

  automation_editor = AGS_AUTOMATION_EDITOR(gobject);
  
  G_OBJECT_CLASS(ags_automation_editor_parent_class)->finalize(gobject);
}

AgsAutomationEditorChild*
ags_automation_editor_child_alloc(AgsMachine *machine,
				  AgsScale *audio_scale, AgsAutomationEdit *audio_automation_edit,
				  AgsNotebook *output_notebook, AgsScale *output_scale, AgsAutomationEdit *output_automation_edit,
				  AgsNotebook *input_notebook, AgsScale *input_scale, AgsAutomationEdit *input_automation_edit)
{
  AgsAutomationEditorChild *automation_editor_child;

  automation_editor_child = (AgsAutomationEditorChild *) malloc(sizeof(AgsAutomationEditorChild));

  automation_editor_child->machine = machine;

  automation_editor_child->audio_scale = audio_scale;
  automation_editor_child->audio_automation_edit = audio_automation_edit;

  automation_editor_child->output_notebook = output_notebook;
  automation_editor_child->output_scale = output_scale;
  automation_editor_child->output_automation_edit = output_automation_edit;

  automation_editor_child->input_notebook = input_notebook;
  automation_editor_child->input_scale = input_scale;
  automation_editor_child->input_automation_edit = input_automation_edit;
  
  return(automation_editor_child);
}

void
ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{
  AgsMachine *machine_old;

  GList *list, *list_start;
  GList *child;
  
  guint pads;
  guint i;
  
  if(automation_editor->selected_machine == machine){
    return;
  }

  machine_old = automation_editor->selected_machine;
  automation_editor->selected_machine = machine;
  
  child = automation_editor->automation_editor_child;

  while(child != NULL){
    if(AGS_AUTOMATION_EDITOR_CHILD(child->data)->machine == machine){
      break;
    }
    
    child = child->next;
  }

  /* hide */
  if(automation_editor->current_audio_automation_edit != NULL){
    gtk_widget_hide(automation_editor->current_audio_scale);
    gtk_widget_hide(automation_editor->current_audio_automation_edit);
  }

  if(automation_editor->current_output_automation_edit != NULL){
    gtk_widget_hide(automation_editor->current_output_notebook);
    gtk_widget_hide(automation_editor->current_output_scale);
    gtk_widget_hide(automation_editor->current_output_automation_edit);
  }

  if(automation_editor->current_input_automation_edit != NULL){
    gtk_widget_hide(automation_editor->current_input_notebook);
    gtk_widget_hide(automation_editor->current_input_scale);
    gtk_widget_hide(automation_editor->current_input_automation_edit);
  }

  /* reset */
  automation_editor->current_audio_scale = NULL;
  automation_editor->current_audio_automation_edit = NULL;

  automation_editor->current_output_notebook = NULL;
  automation_editor->current_output_scale = NULL;
  automation_editor->current_output_automation_edit = NULL;

  automation_editor->current_input_notebook = NULL;
  automation_editor->current_input_scale = NULL;
  automation_editor->current_input_automation_edit = NULL;

  if(machine == NULL){
    return;
  }

  /* instantiate automation edit */
  if(child == NULL){
    AgsAutomationEditorChild *automation_editor_child;
    guint y;

    y = 2 * g_list_length(automation_editor->automation_editor_child);
    
    automation_editor_child = ags_automation_editor_child_alloc(machine,
								NULL, NULL,
								NULL, NULL, NULL,
								NULL, NULL, NULL);
    automation_editor->automation_editor_child = g_list_prepend(automation_editor->automation_editor_child,
								automation_editor_child);

    /* audio */
    automation_editor_child->audio_scale = 
      automation_editor->current_audio_scale = ags_scale_new();
    g_object_ref(automation_editor_child->audio_scale);
    gtk_table_attach(automation_editor->audio_table, (GtkWidget *) automation_editor_child->audio_scale,
		     0, 1, y + 1, y + 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->audio_scale));
    gtk_widget_show_all(automation_editor_child->audio_scale);

    automation_editor_child->audio_automation_edit = 
      automation_editor->current_audio_automation_edit = ags_automation_edit_new();
    g_object_ref(automation_editor_child->audio_automation_edit);
    gtk_table_attach(automation_editor->audio_table, (GtkWidget *) automation_editor_child->audio_automation_edit,
		     1, 2, y + 1, y + 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);

    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->audio_automation_edit));
    gtk_widget_show_all(automation_editor_child->audio_automation_edit);

    /* output */
    automation_editor_child->output_notebook = 
      automation_editor->current_output_notebook = g_object_new(AGS_TYPE_NOTEBOOK,
								"homogeneous\0", FALSE,
								"spacing\0", 0,
								NULL);
    g_object_ref(automation_editor_child->output_notebook);
    gtk_table_attach(automation_editor->output_table, (GtkWidget *) automation_editor_child->output_notebook,
		     0, 3, y, y + 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    for(i = 0; i < machine->audio->output_lines; i++){
      ags_notebook_insert_tab(automation_editor_child->output_notebook,
			      i);
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(automation_editor_child->output_notebook->tabs->data)->toggle,
				   TRUE);
    }

    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->output_notebook));
    gtk_widget_show_all(automation_editor_child->output_notebook);

    automation_editor_child->output_scale = 
      automation_editor->current_output_scale = ags_scale_new();
    g_object_ref(automation_editor_child->output_scale);
    gtk_table_attach(automation_editor->output_table, (GtkWidget *) automation_editor_child->output_scale,
		     0, 1, y + 1, y + 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->output_scale));
    gtk_widget_show_all(automation_editor_child->output_scale);

    automation_editor_child->output_automation_edit = 
      automation_editor->current_output_automation_edit = ags_automation_edit_new();
    g_object_ref(automation_editor_child->output_automation_edit);
    gtk_table_attach(automation_editor->output_table, (GtkWidget *) automation_editor_child->output_automation_edit,
		     1, 2, y + 1, y + 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);

    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->output_automation_edit));
    gtk_widget_show_all(automation_editor_child->output_automation_edit);

    /* input */
    automation_editor_child->input_notebook = 
      automation_editor->current_input_notebook = g_object_new(AGS_TYPE_NOTEBOOK,
							       "homogeneous\0", FALSE,
							       "spacing\0", 0,
							       NULL);
    g_object_ref(automation_editor_child->input_notebook);
    gtk_table_attach(automation_editor->input_table, (GtkWidget *) automation_editor_child->input_notebook,
		     0, 3, y, y + 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    for(i = 0; machine != NULL && i < machine->audio->input_lines; i++){
      ags_notebook_insert_tab(automation_editor_child->input_notebook,
			      i);
      gtk_toggle_button_set_active(AGS_NOTEBOOK_TAB(automation_editor_child->input_notebook->tabs->data)->toggle,
				   TRUE);
    }

    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->input_notebook));
    gtk_widget_show_all(automation_editor_child->input_notebook);

    automation_editor_child->input_scale = 
      automation_editor->current_input_scale = ags_scale_new();
    g_object_ref(automation_editor_child->input_scale);
    gtk_table_attach(automation_editor->input_table, (GtkWidget *) automation_editor_child->input_scale,
		     0, 1, y + 1, y + 2,
		     GTK_FILL, GTK_FILL,
		     0, 0);
    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->input_scale));
    gtk_widget_show_all(automation_editor_child->input_scale);

    automation_editor_child->input_automation_edit = 
      automation_editor->current_input_automation_edit = ags_automation_edit_new();
    g_object_ref(automation_editor_child->input_automation_edit);
    gtk_table_attach(automation_editor->input_table, (GtkWidget *) automation_editor_child->input_automation_edit,
		     1, 2, y + 1, y + 2,
		     GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		     0, 0);

    ags_connectable_connect(AGS_CONNECTABLE(automation_editor_child->input_automation_edit));
    gtk_widget_show_all(automation_editor_child->input_automation_edit);
  }else{
    AgsAutomationEditorChild *automation_editor_child;

    automation_editor_child = child->data;

    /* reset */
    automation_editor->current_audio_scale = automation_editor_child->audio_scale;
    automation_editor->current_audio_automation_edit = automation_editor_child->audio_automation_edit;

    automation_editor->current_output_notebook = automation_editor_child->output_notebook;
    automation_editor->current_output_scale = automation_editor_child->output_scale;
    automation_editor->current_output_automation_edit = automation_editor_child->output_automation_edit;

    automation_editor->current_input_notebook = automation_editor_child->input_notebook;
    automation_editor->current_input_scale = automation_editor_child->input_scale;
    automation_editor->current_input_automation_edit = automation_editor_child->input_automation_edit;

    /* show */
    gtk_widget_show_all(automation_editor_child->audio_scale);
    gtk_widget_show_all(automation_editor_child->audio_automation_edit);

    gtk_widget_show_all(automation_editor_child->output_notebook);
    gtk_widget_show_all(automation_editor_child->output_scale);
    gtk_widget_show_all(automation_editor_child->output_automation_edit);

    gtk_widget_show_all(automation_editor_child->input_notebook);
    gtk_widget_show_all(automation_editor_child->input_scale);
    gtk_widget_show_all(automation_editor_child->input_automation_edit);
  }
}

/**
 * ags_automation_editor_machine_changed:
 * @automation_editor: an #AgsAutomationEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of automation editor.
 *
 * Since: 0.4.2
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

void
ags_automation_editor_select_all(AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_paste(AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_copy(AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_cut(AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

void
ags_automation_editor_invert(AgsAutomationEditor *automation_editor)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_editor_new:
 *
 * Create a new #AgsAutomationEditor.
 *
 * Since: 0.4.2
 */
AgsAutomationEditor*
ags_automation_editor_new()
{
  AgsAutomationEditor *automation_editor;

  automation_editor = (AgsAutomationEditor *) g_object_new(AGS_TYPE_AUTOMATION_EDITOR,
							   NULL);

  return(automation_editor);
}
