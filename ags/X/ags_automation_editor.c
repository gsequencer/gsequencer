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

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

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

  automation_editor->notebook = gtk_notebook_new();
  gtk_paned_pack2((GtkPaned *) paned,
		  (GtkWidget *) automation_editor->notebook,
		  TRUE, FALSE);
  
  /* audio */
  automation_editor->audio_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   automation_editor->audio_table,
			   gtk_label_new("audio\0"));
  
  /* output */
  automation_editor->output_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
			   automation_editor->output_table,
			   gtk_label_new("output\0"));

  /* input */
  automation_editor->input_table = (GtkTable *) gtk_table_new(4, 3, FALSE);
  gtk_notebook_append_page(automation_editor->notebook,
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

/**
 * ags_automation_editor_reset_port:
 * @automation_toolbar: an #AgsAutomationToolbar
 * @machine: the #AgsMachine
 * @channel_type: G_TYPE_NONE, AGS_TYPE_OUTPUT or AGS_TYPE_INPUT
 * @remove_specifier: the specifiers to remove
 *
 * Reset port if removed automation.
 *
 * Since: 0.7.38
 */
void
ags_automation_editor_reset_port(AgsAutomationEditor *automation_editor,
				 AgsMachine *machine,
				 GType channel_type,
				 gchar **remove_specifier)
{
  AgsAutomationEdit *automation_edit;
  
  AgsMachine *selected_machine;

  AgsMutexManager *mutex_manager;

  GList *editor_child;
  
  gchar **specifier, **new_automation_port, **unique_specifier;

  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  selected_machine = automation_editor->selected_machine;

  editor_child = automation_editor->automation_editor_child;

  while(editor_child != NULL){
    if(AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->machine == machine){
      break;
    }
    
    editor_child = editor_child->next;
  }

  specifier = machine->automation_port;

  pthread_mutex_lock(audio_mutex);
  
  unique_specifier = ags_automation_get_specifier_unique(machine->audio->automation);
  
  pthread_mutex_unlock(audio_mutex);

  new_automation_port = NULL;
  i = 0;
  
  if(machine->automation_port != NULL){
    for(; *specifier != NULL; specifier++){      
      /* create specifier array */
      if(g_strv_contains(remove_specifier,
			 *specifier)){
	if(editor_child != NULL){
	  AgsScale *scale;
	  GList *scale_area;
	  GList *automation_area;

	  /* remove audio port */
	  if(channel_type == G_TYPE_NONE){
	    automation_edit = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->audio_automation_edit;
	    scale = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->audio_scale;

	    scale_area = ags_scale_area_find_specifier(scale->scale_area,
						       *specifier);
    
	    if(scale_area != NULL){
	      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
								   *specifier);
	    
	      ags_scale_remove_area(scale,
				    scale_area->data);
	      gtk_widget_queue_draw(scale);

	      ags_automation_edit_remove_area(automation_edit,
					      automation_area->data);
	    
	      if(machine == selected_machine){
		gtk_widget_queue_draw(automation_edit->drawing_area);
	      }
	    }
	  }
	  
	  /* remove output port */
	  if(channel_type == AGS_TYPE_OUTPUT){
	    automation_edit = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->output_automation_edit;
	    scale = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->output_scale;

	    scale_area = ags_scale_area_find_specifier(scale->scale_area,
						       *specifier);
    
	    if(scale_area != NULL){
	      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
								   *specifier);
	    
	      ags_scale_remove_area(scale,
				    scale_area->data);
	      gtk_widget_queue_draw(scale);

	      ags_automation_edit_remove_area(automation_edit,
					      automation_area->data);
	    
	      if(machine == selected_machine){
		gtk_widget_queue_draw(automation_edit->drawing_area);
	      }
	    }
	  }
	  
	  /* remove input port */
	  if(channel_type == AGS_TYPE_INPUT){
	    automation_edit = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->input_automation_edit;
	    scale = AGS_AUTOMATION_EDITOR_CHILD(editor_child->data)->input_scale;

	    scale_area = ags_scale_area_find_specifier(scale->scale_area,
						       *specifier);
    
	    if(scale_area != NULL){
	      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
								   *specifier);
	    
	      ags_scale_remove_area(scale,
				    scale_area->data);
	      gtk_widget_queue_draw(scale);

	      ags_automation_edit_remove_area(automation_edit,
					      automation_area->data);
	    
	      if(machine == selected_machine){
		gtk_widget_queue_draw(automation_edit->drawing_area);
	      }
	    }
	  }
	}
      }else{
	if(g_strv_contains(unique_specifier,
			   *specifier)){
	  if(new_automation_port == NULL){
	    new_automation_port = (gchar **) malloc(2 * sizeof(gchar *));
	  }else{
	    new_automation_port = (gchar **) realloc(new_automation_port,
						     (i + 2) * sizeof(gchar *));
	  }

	  new_automation_port[i] = *specifier;
	  i++;
	}
      }
    }
    
    if(new_automation_port != NULL){
      new_automation_port[i] = NULL;
    }
    
    machine->automation_port = new_automation_port;
  }

  if(machine == selected_machine){
    ags_automation_toolbar_load_port(automation_editor->automation_toolbar);
  }
}

void
ags_automation_editor_real_machine_changed(AgsAutomationEditor *automation_editor, AgsMachine *machine)
{
  AgsMachine *machine_old;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  GList *child;

  guint output_lines, input_lines;
  guint pads;
  guint i;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
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

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio properties */
  pthread_mutex_lock(audio_mutex);

  output_lines = machine->audio->output_lines;
  input_lines = machine->audio->input_lines;
  
  pthread_mutex_unlock(audio_mutex);
  
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
    automation_editor_child->output_notebook->prefix = g_strdup("line\0");
    g_object_ref(automation_editor_child->output_notebook);
    gtk_table_attach(automation_editor->output_table, (GtkWidget *) automation_editor_child->output_notebook,
		     0, 3, y, y + 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    for(i = 0; i < output_lines; i++){
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
    automation_editor_child->input_notebook->prefix = g_strdup("line\0");
    g_object_ref(automation_editor_child->input_notebook);
    gtk_table_attach(automation_editor->input_table, (GtkWidget *) automation_editor_child->input_notebook,
		     0, 3, y, y + 1,
		     GTK_FILL|GTK_EXPAND, GTK_FILL,
		     0, 0);

    for(i = 0; machine != NULL && i < input_lines; i++){
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
  AgsNotebook *notebook;  
  AgsAutomationEdit *current_edit_widget;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  GList *automation;
  
  GType channel_type;

  guint current_page;
  guint line;
  gboolean is_audio, is_output, is_input;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  current_page = gtk_notebook_get_current_page(automation_editor->notebook);

  switch(current_page){
  case 0:
    {
      notebook = NULL;
      current_edit_widget = automation_editor->current_audio_automation_edit;

      channel_type = G_TYPE_NONE;

      is_audio = TRUE;
    }
    break;
  case 1:
    {
      notebook = automation_editor->current_output_notebook;
      current_edit_widget = automation_editor->current_output_automation_edit;

      channel_type = AGS_TYPE_OUTPUT;
      
      is_output = TRUE;
    }
    break;
  case 2:
    {
      notebook = automation_editor->current_input_notebook;
      current_edit_widget = automation_editor->current_input_automation_edit;

      channel_type = AGS_TYPE_INPUT;
      
      is_input = TRUE;
    }
    break;
  }

  /*  */
  if(automation_editor->selected_machine != NULL &&
     current_edit_widget != NULL &&
     current_edit_widget->automation_area != NULL){
    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) automation_editor->selected_machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get automation area */
    list =
      list_start = g_list_reverse(g_list_copy(current_edit_widget->automation_area));

    /* select all */
    pthread_mutex_lock(audio_mutex);
    
    if(channel_type == G_TYPE_NONE){
      automation = automation_editor->selected_machine->audio->automation;

      while(list != NULL){
	while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									     AGS_AUTOMATION_AREA(list->data)->control_name,
									     channel_type,
									     0)) != NULL){
	  ags_automation_add_all_to_selection(automation->data);
	  
	  automation = automation->next;
	}

	list = list->next;
      }
    }else{
      while(list != NULL){
	line = 0;
	
	while((line = ags_notebook_next_active_tab(notebook,
						   line)) != -1){
	  automation = automation_editor->selected_machine->audio->automation;

	  while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									       AGS_AUTOMATION_AREA(list->data)->control_name,
									       channel_type,
									       line)) != NULL){
	    ags_automation_add_all_to_selection(automation->data);
	  
	    automation = automation->next;
	  }
	  
	  line++;
	}

	list = list->next;
      }
    }

    pthread_mutex_unlock(audio_mutex);
    
    gtk_widget_queue_draw(current_edit_widget);
    
    g_list_free(list_start);
  }
}

void
ags_automation_editor_paste(AgsAutomationEditor *automation_editor)
{
  AgsNotebook *notebook;  
  AgsAutomationEdit *current_edit_widget;

  AgsMutexManager *mutex_manager;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;
  
  GType channel_type;

  gchar *buffer;

  guint current_page;
  guint line;
  guint position_x;
  gint first_x, last_x;
  gboolean is_audio, is_output, is_input;
  gboolean paste_from_position;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  auto gint ags_automation_editor_paste_read_automation();

  gint ags_automation_editor_paste_read_automation(){
    xmlXPathContext *xpathCtxt;
    xmlXPathObject *xpathObj;
    xmlNodeSet *nodeset;

    GList *automation;

    xmlChar *control_name;
    
    guint first_x;
    guint i, size;
    guint line;
    
    xpathCtxt = xmlXPathNewContext(clipboard);
    xpathObj = xmlXPathEvalExpression("/audio/automation\0", xpathCtxt);

    first_x = -1;
    
    if(xpathObj != NULL){
      guint current_x;

      if(channel_type == G_TYPE_NONE){
	nodeset = xpathObj->nodesetval;
	
	size = (nodeset != NULL) ? nodeset->nodeNr: 0;

	for(i = 0; i < size; i++){
	  control_name = xmlGetProp(nodeset->nodeTab[i],
				    "control-name");

	  automation = automation_editor->selected_machine->audio->automation;
	  automation = ags_automation_find_specifier_with_type_and_line(automation,
									control_name,
									channel_type,
									0);
	  
	  if(paste_from_position){
	    xmlNode *child;
	    
	    guint x_boundary;

	    ags_automation_insert_from_clipboard(automation->data,
						 nodeset->nodeTab[i],
						 TRUE, position_x,
						 FALSE, 0.0);
	    
	    /* get boundaries */
	    child = nodeset->nodeTab[i]->children;
	    current_x = 0;
	  
	    while(child != NULL){
	      if(child->type == XML_ELEMENT_NODE){
		if(!xmlStrncmp(child->name,
			       "acceleration\0",
			       5)){
		  guint tmp;

		  tmp = g_ascii_strtoull(xmlGetProp(child,
						    "x\0"),
					 NULL,
					 10);

		  if(tmp > current_x){
		    current_x = tmp;
		  }
		}
	      }

	      child = child->next;
	    }

	    x_boundary = g_ascii_strtoull(xmlGetProp(nodeset->nodeTab[i],
						     "x-boundary\0"),
					  NULL,
					  10);

	    if(first_x == -1 || x_boundary < first_x){
	      first_x = x_boundary;
	    }
	  
	    if(position_x > x_boundary){
	      current_x += (position_x - x_boundary);
	    }else{
	      current_x -= (x_boundary - position_x);
	    }
	  
	    if(current_x > last_x){
	      last_x = current_x;
	    }
	  }else{
	    xmlNode *child;
	    
	    ags_automation_insert_from_clipboard(automation->data,
						 nodeset->nodeTab[i],
						 FALSE, 0.0,
						 FALSE, 0.0);


	    child = nodeset->nodeTab[i]->children;
	    current_x = 0;
	  
	    while(child != NULL){
	      if(child->type == XML_ELEMENT_NODE){
		if(!xmlStrncmp(child->name,
			       "acceleration\0",
			       5)){
		  guint tmp;

		  tmp = g_ascii_strtoull(xmlGetProp(child,
						    "x\0"),
					 NULL,
					 10);

		  if(tmp > current_x){
		    current_x = tmp;
		  }
		}
	      }

	      child = child->next;
	    }

	    if(current_x > last_x){
	      last_x = current_x;
	    }
	  }
	}
      }else{
	xmlChar *str;

	nodeset = xpathObj->nodesetval;
	
	size = (nodeset != NULL) ? nodeset->nodeNr: 0;
 
	for(i = 0; i < size; i++){
	  control_name = xmlGetProp(nodeset->nodeTab[i],
				    "control-name");

	  /* get line */
	  line = 0;
	  str  = xmlGetProp(nodeset->nodeTab[i],
			    "line");

	  if(str != NULL){
	    line = g_ascii_strtoull(str,
				    NULL,
				    10);
	  }
	  
	  /*  */
	  automation = automation_editor->selected_machine->audio->automation;

	  while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									       control_name,
									       channel_type,
									       line)) != NULL){
	  
	    if(paste_from_position){
	      xmlNode *child;
	    
	      guint x_boundary;

	      ags_automation_insert_from_clipboard(automation->data,
						   nodeset->nodeTab[i],
						   TRUE, position_x,
						   FALSE, 0.0);
	    
	      /* get boundaries */
	      child = nodeset->nodeTab[i]->children;
	      current_x = 0;
	  
	      while(child != NULL){
		if(child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(child->name,
				 "acceleration\0",
				 5)){
		    guint tmp;

		    tmp = g_ascii_strtoull(xmlGetProp(child,
						      "x\0"),
					   NULL,
					   10);

		    if(tmp > current_x){
		      current_x = tmp;
		    }
		  }
		}

		child = child->next;
	      }

	      x_boundary = g_ascii_strtoull(xmlGetProp(nodeset->nodeTab[i],
						       "x-boundary\0"),
					    NULL,
					    10);

	      if(first_x == -1 || x_boundary < first_x){
		first_x = x_boundary;
	      }
	  
	      if(position_x > x_boundary){
		current_x += (position_x - x_boundary);
	      }else{
		current_x -= (x_boundary - position_x);
	      }
	  
	      if(current_x > last_x){
		last_x = current_x;
	      }
	    }else{
	      xmlNode *child;
	    
	      ags_automation_insert_from_clipboard(automation->data,
						   nodeset->nodeTab[i],
						   FALSE, 0.0,
						   FALSE, 0.0);


	      child = nodeset->nodeTab[i]->children;
	      current_x = 0;
	  
	      while(child != NULL){
		if(child->type == XML_ELEMENT_NODE){
		  if(!xmlStrncmp(child->name,
				 "acceleration\0",
				 5)){
		    guint tmp;

		    tmp = g_ascii_strtoull(xmlGetProp(child,
						      "x\0"),
					   NULL,
					   10);

		    if(tmp > current_x){
		      current_x = tmp;
		    }
		  }
		}

		child = child->next;
	      }

	      if(current_x > last_x){
		last_x = current_x;
	      }
	    }

	    automation = automation->next;
	  }
	}
      }

      xmlXPathFreeObject(xpathObj);
    }

    return(first_x);
  }
  
  current_page = gtk_notebook_get_current_page(automation_editor->notebook);

  switch(current_page){
  case 0:
    {
      notebook = NULL;
      current_edit_widget = automation_editor->current_audio_automation_edit;

      channel_type = G_TYPE_NONE;

      is_audio = TRUE;
    }
    break;
  case 1:
    {
      notebook = automation_editor->current_output_notebook;
      current_edit_widget = automation_editor->current_output_automation_edit;

      channel_type = AGS_TYPE_OUTPUT;
      
      is_output = TRUE;
    }
    break;
  case 2:
    {
      notebook = automation_editor->current_input_notebook;
      current_edit_widget = automation_editor->current_input_automation_edit;

      channel_type = AGS_TYPE_INPUT;
      
      is_input = TRUE;
    }
    break;
  }

  /* get clipboard */
  buffer = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    
  if(buffer == NULL){
    return;
  }

  /*  */
  if(automation_editor->selected_machine != NULL &&
     current_edit_widget != NULL &&
     current_edit_widget->automation_area != NULL){
    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) automation_editor->selected_machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* edit mode */
    if(automation_editor->automation_toolbar->selected_edit_mode == automation_editor->automation_toolbar->position){
      last_x = 0;
      paste_from_position = TRUE;

      position_x = current_edit_widget->edit_x;
    }else{
      paste_from_position = FALSE;
    }
    
    /* get xml tree */
    clipboard = xmlReadMemory(buffer, strlen(buffer),
			      NULL, "UTF-8\0",
			      0);
    audio_node = xmlDocGetRootElement(clipboard);

    /* read automation */
    first_x = -1;

    if(first_x == -1){
      first_x = 0;
    }

    pthread_mutex_lock(audio_mutex);
    
    while(audio_node != NULL){
      if(audio_node->type == XML_ELEMENT_NODE && !xmlStrncmp("audio\0", audio_node->name, 6)){

	automation_node = audio_node->children;
	
	first_x = ags_automation_editor_paste_read_automation();
	
	break;
      }
      
      audio_node = audio_node->next;
    }    

    pthread_mutex_unlock(audio_mutex);

    /* reset cursor */
    if(paste_from_position){
      double tact_factor, zoom_factor;
      double tact;

      zoom_factor = 1.0 / 4.0;

      tact_factor = exp2(8.0 - (double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom));
      tact = exp2((double) gtk_combo_box_get_active(automation_editor->automation_toolbar->zoom) - 2.0);

      current_edit_widget->edit_x = (AGS_AUTOMATION_EDIT_DEFAULT_WIDTH * ceil((double) last_x / AGS_AUTOMATION_EDIT_DEFAULT_WIDTH)) / tact;
    }

    xmlFreeDoc(clipboard); 
    
    /* redraw */
    gtk_widget_queue_draw(current_edit_widget);
  }
}

void
ags_automation_editor_copy(AgsAutomationEditor *automation_editor)
{
  AgsNotebook *notebook;  
  AgsAutomationEdit *current_edit_widget;

  AgsMutexManager *mutex_manager;

  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;

  GList *list, *list_start;
  GList *automation;

  GType channel_type;

  xmlChar *buffer;

  guint current_page;
  gint line;
  int size;
  gboolean is_audio, is_output, is_input;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  current_page = gtk_notebook_get_current_page(automation_editor->notebook);

  switch(current_page){
  case 0:
    {
      notebook = NULL;
      current_edit_widget = automation_editor->current_audio_automation_edit;

      channel_type = G_TYPE_NONE;

      is_audio = TRUE;
    }
    break;
  case 1:
    {
      notebook = automation_editor->current_output_notebook;
      current_edit_widget = automation_editor->current_output_automation_edit;

      channel_type = AGS_TYPE_OUTPUT;
      
      is_output = TRUE;
    }
    break;
  case 2:
    {
      notebook = automation_editor->current_input_notebook;
      current_edit_widget = automation_editor->current_input_automation_edit;

      channel_type = AGS_TYPE_INPUT;
      
      is_input = TRUE;
    }
    break;
  }

  /*  */
  if(automation_editor->selected_machine != NULL &&
     current_edit_widget != NULL &&
     current_edit_widget->automation_area != NULL){    
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio\0");
    xmlDocSetRootElement(clipboard, audio_node);

    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) automation_editor->selected_machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* find automation to copy */
    list =
      list_start = g_list_reverse(g_list_copy(current_edit_widget->automation_area));

    pthread_mutex_lock(audio_mutex);

    if(channel_type == G_TYPE_NONE){
      automation = automation_editor->selected_machine->audio->automation;

      while(list != NULL){
	while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									     AGS_AUTOMATION_AREA(list->data)->control_name,
									     channel_type,
									     0)) != NULL){
	  automation_node = ags_automation_copy_selection(automation->data);
	  ags_automation_merge_clipboard(audio_node,
					 automation_node);
	  xmlFreeNode(automation_node);
	  
	  automation = automation->next;
	}

	list = list->next;
      }
    }else{
      while(list != NULL){
	line = 0;
	
	while((line = ags_notebook_next_active_tab(notebook,
						   line)) != -1){
	  automation = automation_editor->selected_machine->audio->automation;

	  while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									       AGS_AUTOMATION_AREA(list->data)->control_name,
									       channel_type,
									       line)) != NULL){
	    automation_node = ags_automation_copy_selection(automation->data);
	    ags_automation_merge_clipboard(audio_node,
					   automation_node);
	    xmlFreeNode(automation_node);
	  
	    automation = automation->next;
	  }
	  
	  line++;
	}

	list = list->next;
      }
    }

    pthread_mutex_unlock(audio_mutex);
    
    g_list_free(list_start);
    
    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8\0", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);
  }
}

void
ags_automation_editor_cut(AgsAutomationEditor *automation_editor)
{
  AgsNotebook *notebook;  
  AgsAutomationEdit *current_edit_widget;

  AgsMutexManager *mutex_manager;

  xmlDoc *clipboard;
  xmlNode *audio_node, *automation_node;

  GList *list, *list_start;
  GList *automation;

  GType channel_type;

  xmlChar *buffer;

  guint current_page;
  guint line;
  int size;
  gboolean is_audio, is_output, is_input;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  current_page = gtk_notebook_get_current_page(automation_editor->notebook);

  switch(current_page){
  case 0:
    {
      notebook = NULL;
      current_edit_widget = automation_editor->current_audio_automation_edit;

      channel_type = G_TYPE_NONE;

      is_audio = TRUE;
    }
    break;
  case 1:
    {
      notebook = automation_editor->current_output_notebook;
      current_edit_widget = automation_editor->current_output_automation_edit;

      channel_type = AGS_TYPE_OUTPUT;
      
      is_output = TRUE;
    }
    break;
  case 2:
    {
      notebook = automation_editor->current_input_notebook;
      current_edit_widget = automation_editor->current_input_automation_edit;

      channel_type = AGS_TYPE_INPUT;
      
      is_input = TRUE;
    }
    break;
  }

  /*  */
  if(automation_editor->selected_machine != NULL &&
     current_edit_widget != NULL &&
     current_edit_widget->automation_area != NULL){    
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio\0");
    xmlDocSetRootElement(clipboard, audio_node);

    /* get mutex manager and application mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* get audio mutex */
    pthread_mutex_lock(application_mutex);

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) automation_editor->selected_machine->audio);
  
    pthread_mutex_unlock(application_mutex);

    /* find automation to cut */
    list =
      list_start = g_list_reverse(g_list_copy(current_edit_widget->automation_area));

    pthread_mutex_lock(audio_mutex);
    
    if(channel_type == G_TYPE_NONE){
      automation = automation_editor->selected_machine->audio->automation;

      while(list != NULL){
	while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									     AGS_AUTOMATION_AREA(list->data)->control_name,
									     channel_type,
									     0)) != NULL){
	  automation_node = ags_automation_cut_selection(automation->data);
	  ags_automation_merge_clipboard(audio_node,
					 automation_node);
	  xmlFreeNode(automation_node);
	  
	  automation = automation->next;
	}

	list = list->next;
      }
    }else{
      while(list != NULL){
	line = 0;
	
	while((line = ags_notebook_next_active_tab(notebook,
						   line)) != -1){
	  automation = automation_editor->selected_machine->audio->automation;

	  while((automation = ags_automation_find_specifier_with_type_and_line(automation,
									       AGS_AUTOMATION_AREA(list->data)->control_name,
									       channel_type,
									       line)) != NULL){
	    automation_node = ags_automation_cut_selection(automation->data);
	    ags_automation_merge_clipboard(audio_node,
					   automation_node);
	    xmlFreeNode(automation_node);
	  
	    automation = automation->next;
	  }
	  
	  line++;
	}

	list = list->next;
      }
    }

    pthread_mutex_unlock(audio_mutex);

    g_list_free(list_start);

    /* write to clipboard */
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8\0", TRUE);
    gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),
			   buffer, size);
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));

    xmlFreeDoc(clipboard);

    /* redraw */
    gtk_widget_queue_draw(current_edit_widget);
  }
}

void
ags_automation_editor_invert(AgsAutomationEditor *automation_editor)
{
  g_message("ags_automation_editor_invert() - not implemented\0");
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
