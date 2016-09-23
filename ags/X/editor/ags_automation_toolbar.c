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

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags/lib/ags_string_util.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>

#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

#include <ags/config.h>

void ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar);
void ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_connect(AgsConnectable *connectable);
void ags_automation_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_toolbar
 * @short_description: edit tool
 * @title: AgsAutomationToolbar
 * @section_id:
 * @include: ags/X/editor/ags_note_edit.h
 *
 * The #AgsAutomationToolbar lets you choose edit tool.
 */

GType
ags_automation_toolbar_get_type(void)
{
  static GType ags_type_automation_toolbar = 0;

  if (!ags_type_automation_toolbar){
    static const GTypeInfo ags_automation_toolbar_info = {
      sizeof (AgsAutomationToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
							 "AgsAutomationToolbar\0", &ags_automation_toolbar_info,
							 0);
    
    g_type_add_interface_static(ags_type_automation_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_automation_toolbar);
}

void
ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar)
{
}

void
ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_toolbar_connect;
  connectable->disconnect = ags_automation_toolbar_disconnect;
}

void
ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar)
{
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  
  automation_toolbar->position = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					      "image\0", gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
										  GTK_ICON_SIZE_LARGE_TOOLBAR),
					      "relief\0", GTK_RELIEF_NONE,
					      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->position,
			    "position cursor\0",
			    NULL);

  automation_toolbar->edit = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					  "image\0", gtk_image_new_from_stock(GTK_STOCK_EDIT,
									      GTK_ICON_SIZE_LARGE_TOOLBAR),
					  "relief\0", GTK_RELIEF_NONE,
					  NULL);
  automation_toolbar->selected_edit_mode = automation_toolbar->edit;
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->edit,
			    "edit automation\0",
			    NULL);

  automation_toolbar->clear = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					   "image\0", gtk_image_new_from_stock(GTK_STOCK_CLEAR,
									       GTK_ICON_SIZE_LARGE_TOOLBAR),
					   "relief\0", GTK_RELIEF_NONE,
					   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->clear,
			    "select automation\0",
			    NULL);
  
  automation_toolbar->select = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					    "image\0", gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
										GTK_ICON_SIZE_LARGE_TOOLBAR),
					    "relief\0", GTK_RELIEF_NONE,
					    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->select,
			    "select automation\0",
			    NULL);

  automation_toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					     "image\0", gtk_image_new_from_stock(GTK_STOCK_COPY,
										 GTK_ICON_SIZE_LARGE_TOOLBAR),
					     "relief\0", GTK_RELIEF_NONE,
					     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->copy,
			    "copy automation\0",
			    NULL);

  automation_toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						       "image\0", gtk_image_new_from_stock(GTK_STOCK_CUT,
											   GTK_ICON_SIZE_LARGE_TOOLBAR),
						       "relief\0", GTK_RELIEF_NONE,
						       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    automation_toolbar->cut,
			    (GtkWidget *) "cut automation\0",
			    NULL);

  automation_toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							 "image\0", gtk_image_new_from_stock(GTK_STOCK_PASTE,
											     GTK_ICON_SIZE_LARGE_TOOLBAR),
							 "relief\0", GTK_RELIEF_NONE,
							 NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->paste,
			    "paste automation\0",
			    NULL);

  /*  */
  automation_toolbar->zoom_history = 4;
  
  label = (GtkLabel *) gtk_label_new("zoom\0");
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    (GtkWidget *) label);

  automation_toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(GTK_COMBO_BOX(automation_toolbar->zoom),
			   2);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->zoom,
			    NULL,
			    NULL);

  /*  */
  label = (GtkLabel *) gtk_label_new("port\0");
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    (GtkWidget *) label);

  automation_toolbar->port = (GtkComboBox *) gtk_combo_box_new();

  cell_renderer = gtk_cell_renderer_toggle_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(automation_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(automation_toolbar->port), cell_renderer,
				 "active\0", 0,
				 NULL);
  gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer),
					   TRUE);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(automation_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(automation_toolbar->port), cell_renderer,
				 "text\0", 1,
				 NULL);

  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->port,
			    NULL,
			    NULL);
}

void
ags_automation_toolbar_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = AGS_AUTOMATION_TOOLBAR(connectable);
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  /*  */
  g_signal_connect_after(G_OBJECT(automation_editor), "machine-changed\0",
			 G_CALLBACK(ags_automation_toolbar_machine_changed_callback), automation_toolbar);

  /* tool */
  g_signal_connect_after((GObject *) automation_toolbar->position, "toggled\0",
			 G_CALLBACK(ags_automation_toolbar_position_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->edit, "toggled\0",
			 G_CALLBACK(ags_automation_toolbar_edit_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->clear, "toggled\0",
			 G_CALLBACK(ags_automation_toolbar_clear_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->select, "toggled\0",
			 G_CALLBACK(ags_automation_toolbar_select_callback), (gpointer) automation_toolbar);

  /* edit */
  g_signal_connect((GObject *) automation_toolbar->copy, "clicked\0",
		   G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback), (gpointer) automation_toolbar);

  g_signal_connect((GObject *) automation_toolbar->cut, "clicked\0",
		   G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback), (gpointer) automation_toolbar);

  g_signal_connect((GObject *) automation_toolbar->paste, "clicked\0",
		   G_CALLBACK(ags_automation_toolbar_paste_callback), (gpointer) automation_toolbar);

  /* zoom */
  g_signal_connect_after((GObject *) automation_toolbar->zoom, "changed\0",
			 G_CALLBACK(ags_automation_toolbar_zoom_callback), (gpointer) automation_toolbar);

  /* port */
  g_signal_connect(automation_toolbar->port, "changed\0",
		   G_CALLBACK(ags_automation_toolbar_port_changed_callback), automation_toolbar);
}

void
ags_automation_toolbar_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_toolbar_load_port:
 * @automation_toolbar: an #AgsAutomationToolbar
 *
 * Fill in port field with available ports.
 *
 * Since: 0.4.3
 */
void
ags_automation_toolbar_load_port(AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  
  GtkListStore *list_store;
  GtkTreeIter iter;

  AgsMutexManager *mutex_manager;

  gchar **specifier;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  machine = automation_editor->selected_machine;

  if(machine == NULL){
    gtk_combo_box_set_model(automation_toolbar->port,
			    NULL);
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

  /*  */
  list_store = gtk_list_store_new(2,
				  G_TYPE_BOOLEAN,
				  G_TYPE_STRING);
  gtk_combo_box_set_model(automation_toolbar->port,
			  GTK_TREE_MODEL(list_store));

  pthread_mutex_lock(audio_mutex);
  
  specifier = ags_automation_get_specifier_unique(machine->audio->automation);

  pthread_mutex_unlock(audio_mutex);

  for(; *specifier != NULL; specifier++){
    gtk_list_store_append(list_store, &iter);

#ifdef HAVE_GLIB_2_44
    gtk_list_store_set(list_store, &iter,
		       0, ((machine->automation_port != NULL &&
			    g_strv_contains(machine->automation_port, *specifier)) ? : TRUE, FALSE),
		       1, g_strdup(*specifier),
		       -1);
#else
    gtk_list_store_set(list_store, &iter,
		       0, ((machine->automation_port != NULL &&
			    ags_strv_contains(machine->automation_port, *specifier)) ? : TRUE, FALSE),
		       1, g_strdup(*specifier),
		       -1);
#endif
  }
}

/**
 * ags_automation_toolbar_apply_port:
 * @automation_toolbar: an #AgsAutomationToolbar
 * @control_name: the specifier as string
 *
 * Applies all port to appropriate #AgsMachine.
 *
 * Since: 0.4.3
 */
void
ags_automation_toolbar_apply_port(AgsAutomationToolbar *automation_toolbar,
				  gchar *control_name)
{
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsMutexManager *mutex_manager;

  gchar **specifier, *current;
  guint length;
  gboolean is_active;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  machine = automation_editor->selected_machine;

  model = gtk_combo_box_get_model(automation_toolbar->port);

  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* create specifier array */
  specifier = NULL;
  length = 0;
  
  if(gtk_tree_model_get_iter_first(model,
				   &iter)){
    do{
      gtk_tree_model_get(model,
			 &iter,
			 0, &is_active,
			 -1);

      if(is_active){
	if(length == 0){
	  specifier = (gchar **) malloc(2 * sizeof(gchar *));
	}else{
	  specifier = (gchar **) realloc(specifier,
					 (length + 2) * sizeof(gchar *));
	}
      
	gtk_tree_model_get(model,
			   &iter,
			   1, &current,
			   -1);
	specifier[length] = current;

	length++;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));

    if(specifier != NULL){
      specifier[length] = NULL;
    }
  }

  if(machine->automation_port != NULL){
    free(machine->automation_port);
  }

  /* apply */
  machine->automation_port = specifier;
  
#ifdef HAVE_GLIB_2_44
  if(specifier != NULL &&
     g_strv_contains(specifier,
		     control_name)){
#else
  if(specifier != NULL &&
     ags_strv_contains(specifier,
		       control_name)){
#endif
    AgsScaleArea *scale_area;
    AgsAutomationArea *automation_area;

    AgsAudio *audio;
    AgsAutomation *automation;

    GList *list;

    gboolean found_audio, found_output, found_input;
    
    audio = machine->audio;
    
    pthread_mutex_lock(audio_mutex);
    
    list = audio->automation;
    
    /* add port */
    found_audio = FALSE;
    found_output = FALSE;
    found_input = FALSE;
    
    while((list = ags_automation_find_specifier(list,
						control_name)) != NULL){
      AGS_AUTOMATION(list->data)->flags &= (~AGS_AUTOMATION_BYPASS);

      if(AGS_AUTOMATION(list->data)->channel_type == G_TYPE_NONE &&
	 !found_audio){
	scale_area = ags_scale_area_new((GtkDrawingArea *) automation_editor->current_audio_scale,
					control_name,
					AGS_AUTOMATION(list->data)->lower,
					AGS_AUTOMATION(list->data)->upper,
					AGS_AUTOMATION(list->data)->steps);
	ags_scale_add_area(automation_editor->current_audio_scale,
			   scale_area);
	gtk_widget_queue_draw((GtkWidget *) automation_editor->current_audio_scale);
	
	automation_area = ags_automation_area_new(AGS_AUTOMATION_EDIT(automation_editor->current_audio_automation_edit)->drawing_area,
						  audio,
						  G_TYPE_NONE,
						  control_name);
	ags_automation_edit_add_area((AgsAutomationEdit *) automation_editor->current_audio_automation_edit,
				     automation_area);
	gtk_widget_queue_draw((GtkWidget *) AGS_AUTOMATION_EDIT(automation_editor->current_audio_automation_edit)->drawing_area);

	found_audio = TRUE;
      }

      if(AGS_AUTOMATION(list->data)->channel_type == AGS_TYPE_OUTPUT &&
	 !found_output){
	scale_area = ags_scale_area_new((GtkDrawingArea *) automation_editor->current_output_scale,
					control_name,
					AGS_AUTOMATION(list->data)->lower,
					AGS_AUTOMATION(list->data)->upper,
					AGS_AUTOMATION(list->data)->steps);
	ags_scale_add_area(automation_editor->current_output_scale,
			   scale_area);
	gtk_widget_queue_draw((GtkWidget *) automation_editor->current_output_scale);
	
	automation_area = ags_automation_area_new(AGS_AUTOMATION_EDIT(automation_editor->current_output_automation_edit)->drawing_area,
						  audio,
						  AGS_TYPE_OUTPUT,
						  control_name);
	ags_automation_edit_add_area((AgsAutomationEdit *) automation_editor->current_output_automation_edit,
				     automation_area);
	gtk_widget_queue_draw((GtkWidget *) AGS_AUTOMATION_EDIT(automation_editor->current_output_automation_edit)->drawing_area);
	
	found_output = TRUE;
      }

      if(AGS_AUTOMATION(list->data)->channel_type == AGS_TYPE_INPUT &&
	 !found_input){
	scale_area = ags_scale_area_new((GtkDrawingArea *) automation_editor->current_input_scale,
					control_name,
					AGS_AUTOMATION(list->data)->lower,
					AGS_AUTOMATION(list->data)->upper,
					AGS_AUTOMATION(list->data)->steps);
	ags_scale_add_area(automation_editor->current_input_scale,
			   scale_area);
	gtk_widget_queue_draw((GtkWidget *) automation_editor->current_input_scale);
	
	automation_area = ags_automation_area_new(AGS_AUTOMATION_EDIT(automation_editor->current_input_automation_edit)->drawing_area,
						  audio,
						  AGS_TYPE_INPUT,
						  control_name);
	ags_automation_edit_add_area((AgsAutomationEdit *) automation_editor->current_input_automation_edit,
				     automation_area);
	gtk_widget_queue_draw((GtkWidget *) AGS_AUTOMATION_EDIT(automation_editor->current_input_automation_edit)->drawing_area);
	
	found_input = TRUE;
      }
      
      list = list->next;
    }

    pthread_mutex_unlock(audio_mutex);
  }else{
    AgsAutomationEdit *automation_edit;
    AgsScale *scale;
    
    AgsAudio *audio;

    GList *scale_area;
    GList *automation_area;

    GList *list;

    gboolean found_audio, found_output, found_input;
    
    audio = machine->audio;
    list = audio->automation;
    
    /* add port */
    found_audio = FALSE;
    found_output = FALSE;
    found_input = FALSE;

    /* set bypass */
    while((list = ags_automation_find_specifier(list,
						control_name)) != NULL){
      AGS_AUTOMATION(list->data)->flags |= AGS_AUTOMATION_BYPASS;
      
      list = list->next;
    }
    
    /* remove audio port */
    automation_edit = automation_editor->current_audio_automation_edit;
    scale = automation_editor->current_audio_scale;

    scale_area = ags_scale_area_find_specifier(scale->scale_area,
					       control_name);
    
    if(scale_area != NULL){
      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
							   control_name);

      ags_scale_remove_area(scale,
			    scale_area->data);
      gtk_widget_queue_draw((GtkWidget *) scale);

      ags_automation_edit_remove_area(automation_edit,
				      automation_area->data);
      gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
    }
    
    /* remove output port */
    automation_edit = automation_editor->current_output_automation_edit;
    scale = automation_editor->current_output_scale;
    
    scale_area = ags_scale_area_find_specifier(scale->scale_area,
					       control_name);

    if(scale_area != NULL){
      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
							   control_name);

      ags_scale_remove_area(scale,
			    scale_area->data);
      gtk_widget_queue_draw((GtkWidget *) scale);

      ags_automation_edit_remove_area(automation_edit,
				      automation_area->data);
      gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
    }

    /* remove input port */
    automation_edit = automation_editor->current_input_automation_edit;
    scale = automation_editor->current_input_scale;
    
    scale_area = ags_scale_area_find_specifier(scale->scale_area,
					       control_name);

    if(scale_area != NULL){
      automation_area = ags_automation_area_find_specifier(automation_edit->automation_area,
							   control_name);

      ags_scale_remove_area(scale,
			    scale_area->data);
      gtk_widget_queue_draw((GtkWidget *) scale);

      ags_automation_edit_remove_area(automation_edit,
				      automation_area->data);
      gtk_widget_queue_draw((GtkWidget *) automation_edit->drawing_area);
    }
  }
}

/**
 * ags_automation_toolbar_new:
 *
 * Create a new #AgsAutomationToolbar.
 *
 * Returns: a new #AgsAutomationToolbar
 *
 * Since: 0.4.3
 */
AgsAutomationToolbar*
ags_automation_toolbar_new()
{
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = (AgsAutomationToolbar *) g_object_new(AGS_TYPE_AUTOMATION_TOOLBAR, NULL);

  return(automation_toolbar);
}
