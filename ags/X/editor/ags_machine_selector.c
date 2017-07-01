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

#include <ags/X/editor/ags_machine_selector.h>
#include <ags/X/editor/ags_machine_selector_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_notation.h>

#include <ags/X/ags_editor.h>
#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_machine_radio_button.h>

#include <ags/i18n.h>

void ags_machine_selector_class_init(AgsMachineSelectorClass *machine_selector);
void ags_machine_selector_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_selector_init(AgsMachineSelector *machine_selector);
void ags_machine_selector_connect(AgsConnectable *connectable);
void ags_machine_selector_disconnect(AgsConnectable *connectable);
void ags_machine_selector_show(GtkWidget *widget);

/**
 * SECTION:ags_machine_selector
 * @short_description: machine radio buttons
 * @title: AgsMachineSelector
 * @section_id:
 * @include: ags/X/editor/ags_machine_selector.h
 *
 * The #AgsMachineSelector enables you make choice of an #AgsMachine.
 */

enum{
  CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_machine_selector_parent_class = NULL;
static guint machine_selector_signals[LAST_SIGNAL];

GType
ags_machine_selector_get_type(void)
{
  static GType ags_type_machine_selector = 0;

  if(!ags_type_machine_selector){
    static const GTypeInfo ags_machine_selector_info = {
      sizeof (AgsMachineSelectorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_selector_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineSelector),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_selector_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_selector_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_selector = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsMachineSelector", &ags_machine_selector_info,
						       0);
    
    g_type_add_interface_static(ags_type_machine_selector,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine_selector);
}

void
ags_machine_selector_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_selector_connect;
  connectable->disconnect = ags_machine_selector_disconnect;
}

void
ags_machine_selector_class_init(AgsMachineSelectorClass *machine_selector)
{
  ags_machine_selector_parent_class = g_type_class_peek_parent(machine_selector);

  /* AgsMachineSelectorClass */
  machine_selector->changed = NULL;

  /**
   * AgsMachineSelector::changed:
   * @machine_selector: the #AgsMachineSelector
   * @machine: the changed #AgsMachine
   *
   * The ::changed signal notifies changed #AgsMachine.
   *
   * Since: 0.4.2
   */
  machine_selector_signals[CHANGED] =
    g_signal_new("changed",
                 G_TYPE_FROM_CLASS(machine_selector),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineSelectorClass, changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_machine_selector_init(AgsMachineSelector *machine_selector)
{
  GtkHBox *hbox;

  machine_selector->flags = 0;
  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(machine_selector),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  machine_selector->label = (GtkLabel *) gtk_label_new(NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(machine_selector->label),
		     FALSE, FALSE,
		     0);

  machine_selector->current = NULL;
  machine_selector->popup = NULL;

  machine_selector->menu_button = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
					       "stock-id", GTK_STOCK_EXECUTE,
					       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(machine_selector->menu_button),
		     FALSE, FALSE,
		     0);

  machine_selector->machine_selection = NULL;
}

void
ags_machine_selector_connect(AgsConnectable *connectable)
{
  AgsMachineSelector *machine_selector;

  GList *list, *list_start;
  
  machine_selector = AGS_MACHINE_SELECTOR(connectable);

  if((AGS_MACHINE_SELECTOR_CONNECTED & (machine_selector->flags)) != 0){
    return;
  }

  machine_selector->flags |= AGS_MACHINE_SELECTOR_CONNECTED;
  
  /* machine radio button */
  list =
    list_start = gtk_container_get_children((GtkContainer *) machine_selector);
  
  list = list->next;
  
  while(list != NULL){
    g_signal_connect_after(G_OBJECT(list->data), "clicked",
			   G_CALLBACK(ags_machine_selector_radio_changed), machine_selector);

    list = list->next;
  }
  
  g_list_free(list_start);
}

void
ags_machine_selector_disconnect(AgsConnectable *connectable)
{
  AgsMachineSelector *machine_selector;

  GList *list, *list_start;
  
  machine_selector = AGS_MACHINE_SELECTOR(connectable);

  if((AGS_MACHINE_SELECTOR_CONNECTED & (machine_selector->flags)) == 0){
    return;
  }

  machine_selector->flags &= (~AGS_MACHINE_SELECTOR_CONNECTED);

  /* machine radio button */
  list =
    list_start = gtk_container_get_children((GtkContainer *) machine_selector);
  
  list = list->next;
  
  while(list != NULL){
    g_object_disconnect(G_OBJECT(list->data),
			"clicked",
			G_CALLBACK(ags_machine_selector_radio_changed),
			machine_selector,
			NULL);

    list = list->next;
  }
  
  g_list_free(list_start);
}

void
ags_machine_selector_add_index(AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button, *group;
  GList *list, *list_start;

  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  if(list == NULL){
    group = NULL;
  }else{
    group = AGS_MACHINE_RADIO_BUTTON(list->data);
  }

  g_list_free(list_start);

  machine_radio_button = (AgsMachineRadioButton *) g_object_new(AGS_TYPE_MACHINE_RADIO_BUTTON,
								"group", group,
								NULL);
  gtk_box_pack_start(GTK_BOX(machine_selector),
		     GTK_WIDGET(machine_radio_button),
		     FALSE, FALSE,
		     0);
  g_signal_connect_after(G_OBJECT(machine_radio_button), "clicked",
			 G_CALLBACK(ags_machine_selector_radio_changed), machine_selector);
  gtk_widget_show_all((GtkWidget *) machine_radio_button);
}

void
ags_machine_selector_remove_index(AgsMachineSelector *machine_selector,
				  guint nth)
{
  AgsEditor *editor;
  AgsMachineRadioButton *machine_radio_button;
  
  GList *list, *list_start;

  /* get machine radio button */
  machine_radio_button = NULL;

  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  list = g_list_nth(list,
		    nth);
  
  if(list != NULL){
    machine_radio_button = list->data;
  }
  
  g_list_free(list_start);

  if(machine_radio_button == NULL){
    return;
  }
  
  /*  */
  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_selector,
						 AGS_TYPE_EDITOR);

  if(editor != NULL){  
    /* destroy edit widgets */
    if(machine_radio_button->machine != NULL){
      AgsEditorChild *editor_child;

      editor_child = NULL;
      
      list = editor->editor_child;

      while(list != NULL){
	if(AGS_EDITOR_CHILD(list->data)->machine == machine_radio_button->machine){
	  editor_child = AGS_EDITOR_CHILD(list->data);
	  
	  gtk_widget_destroy((GtkWidget *) editor_child->notebook);
	  gtk_widget_destroy((GtkWidget *) editor_child->meter);
	  gtk_widget_destroy((GtkWidget *) editor_child->edit_widget);

	  editor->current_notebook = NULL;
	  editor->current_meter = NULL;
	  editor->current_edit_widget = NULL;
	  
	  break;
	}
    
	list = list->next;
      }

      if(editor_child != NULL){
	editor->editor_child = g_list_remove(editor->editor_child,
					     editor_child);
	free(editor_child);
      }
    }
  }else{
    AgsAutomationEditor *automation_editor;
    
    automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_selector,
									AGS_TYPE_AUTOMATION_EDITOR);

    if(automation_editor != NULL){
      /* destroy edit widgets */
      if(machine_radio_button->machine != NULL){
	AgsAutomationEditorChild *automation_editor_child;

	automation_editor_child = NULL;
	
	list = automation_editor->automation_editor_child;

	while(list != NULL){
	  if(AGS_AUTOMATION_EDITOR_CHILD(list->data)->machine == machine_radio_button->machine){
	    automation_editor_child = AGS_AUTOMATION_EDITOR_CHILD(list->data);
	    
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->audio_scale);
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->audio_automation_edit);
	    
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->output_scale);
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->output_notebook);
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->output_automation_edit);

	    gtk_widget_destroy((GtkWidget *) automation_editor_child->input_scale);
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->input_notebook);
	    gtk_widget_destroy((GtkWidget *) automation_editor_child->input_automation_edit);

	    automation_editor->current_audio_scale = NULL;
	    automation_editor->current_audio_automation_edit = NULL;

	    automation_editor->current_output_notebook = NULL;
	    automation_editor->current_output_scale = NULL;
	    automation_editor->current_output_automation_edit = NULL;

	    automation_editor->current_input_notebook = NULL;
	    automation_editor->current_input_scale = NULL;
	    automation_editor->current_input_automation_edit = NULL;

	    break;
	  }
    
	  list = list->next;
	}

	if(automation_editor_child != NULL){
	  automation_editor->automation_editor_child = g_list_remove(automation_editor->automation_editor_child,
								     automation_editor_child);
	  free(automation_editor_child);
	}
      }
    }
  }
  
  /**/
  gtk_widget_destroy(GTK_WIDGET(machine_radio_button));
}

void
ags_machine_selector_link_index(AgsMachineSelector *machine_selector,
				AgsMachine *machine)
{
  AgsMachineRadioButton *machine_radio_button, *existing_radio_button;

  GList *list, *list_start;

  /* retrieve selected */    
  machine_radio_button = NULL;
  existing_radio_button = NULL;
    
  list_start =
    list = gtk_container_get_children(GTK_CONTAINER(machine_selector));
  list = list->next;

  while(list != NULL){
    if(GTK_IS_TOGGLE_BUTTON(list->data)){
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
	machine_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }

      if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
	existing_radio_button = AGS_MACHINE_RADIO_BUTTON(list->data);
      }
    }
      
    list = list->next;
  }

  g_list_free(list_start);


  /* decide if link to editor or change index */
  if(existing_radio_button != NULL){
    gtk_button_clicked((GtkButton *) existing_radio_button);
  }else{
    g_object_set(G_OBJECT(machine_radio_button),
		 "machine", machine,
		 NULL);
  }
}

/**
 * ags_machine_selector_changed:
 * @machine_selector: the #AgsMachineSelector
 * @machine: the selected #AgsMachine
 *
 * Emitted as #AgsMachineSelector modified.
 *
 * Since: 0.4
 */
void
ags_machine_selector_changed(AgsMachineSelector *machine_selector, AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_MACHINE_SELECTOR(machine_selector));

  g_object_ref((GObject *) machine_selector);
  g_signal_emit((GObject *) machine_selector,
		machine_selector_signals[CHANGED], 0,
		machine);
  g_object_unref((GObject *) machine_selector);
}

/**
 * ags_machine_selector_new:
 *
 * Create a new #AgsMachineSelector.
 *
 * Returns: a new #AgsMachineSelector
 *
 * Since: 0.4
 */
AgsMachineSelector*
ags_machine_selector_new()
{
  AgsMachineSelector *machine_selector;

  machine_selector = (AgsMachineSelector *) g_object_new(AGS_TYPE_MACHINE_SELECTOR,
					   NULL);

  return(machine_selector);
}

/**
 * ags_machine_selector_popup_new:
 * @machine_selector: the #AgsMachineSelector
 *
 * Create a new #AgsMachineSelectorPopup.
 *
 * Returns: a new #GtkMenu suitable for #AgsMachineSelector
 *
 * Since: 0.4
 */
GtkMenu*
ags_machine_selector_popup_new(AgsMachineSelector *machine_selector)
{
  GtkMenu *popup, *keys;
  GtkMenuItem *item;
  GList *list, *list_start;

  popup = (GtkMenu *) gtk_menu_new();
  g_object_set_data((GObject *) popup, g_type_name(AGS_TYPE_MACHINE_SELECTOR), machine_selector);

  /*
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("add tab"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("remove tab"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);
  */
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("add index"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("remove index"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("link index"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) != 0){
    item = (GtkMenuItem *) gtk_check_menu_item_new_with_label(i18n("reverse mapping"));
    gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);
  }

  if((AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (machine_selector->flags)) != 0){
    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("shift piano"));
    gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

    keys = (GtkMenu *) gtk_menu_new();
    gtk_menu_item_set_submenu(item,
			      (GtkWidget *) keys);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("A");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("A#");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("H");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("C");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("C#");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("D");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("D#");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("E");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("F");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("F#");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("G");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

    item = (GtkMenuItem *) gtk_menu_item_new_with_label("G#");
    gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);
  }
  
  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) popup);

  /*
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_machine_selector_popup_add_tab_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_machine_selector_popup_remove_tab_callback), (gpointer) machine_selector);

  list = list->next;
  */
  
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_machine_selector_popup_add_index_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_machine_selector_popup_remove_index_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_machine_selector_popup_link_index_callback), (gpointer) machine_selector);

  if((AGS_MACHINE_SELECTOR_SHOW_REVERSE_MAPPING & (machine_selector->flags)) != 0){
    list = list->next;
    g_signal_connect_after(G_OBJECT(list->data), "activate",
			   G_CALLBACK(ags_machine_selector_popup_reverse_mapping_callback), (gpointer) machine_selector);
  }

  g_list_free(list_start);

  /* keys */
  if((AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (machine_selector->flags)) != 0){
    list_start = 
      list = gtk_container_get_children((GtkContainer *) keys);

    while(list != NULL){
      g_signal_connect(G_OBJECT(list->data), "activate",
		       G_CALLBACK(ags_machine_selector_popup_shift_piano_callback), (gpointer) machine_selector);

      list = list->next;
    }

    g_list_free(list_start);
  }
  
  /* show */
  if((AGS_MACHINE_SELECTOR_SHOW_SHIFT_PIANO & (machine_selector->flags)) != 0){
    gtk_widget_show_all((GtkWidget *) keys);
  }
  
  gtk_widget_show_all((GtkWidget *) popup);

  return(popup);
}
