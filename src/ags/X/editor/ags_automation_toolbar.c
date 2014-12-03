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

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

void ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar);
void ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_connect(AgsConnectable *connectable);
void ags_automation_toolbar_disconnect(AgsConnectable *connectable);
void ags_automation_toolbar_show(GtkWidget *widget);

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
			    automation_toolbar->copy,
			    "copy automation\0",
			    NULL);

  automation_toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						       "image\0", gtk_image_new_from_stock(GTK_STOCK_CUT,
											   GTK_ICON_SIZE_LARGE_TOOLBAR),
						       "relief\0", GTK_RELIEF_NONE,
						       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    automation_toolbar->cut,
			    "cut automation\0",
			    NULL);

  automation_toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							 "image\0", gtk_image_new_from_stock(GTK_STOCK_PASTE,
											     GTK_ICON_SIZE_LARGE_TOOLBAR),
							 "relief\0", GTK_RELIEF_NONE,
							 NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    automation_toolbar->paste,
			    "paste automation\0",
			    NULL);

  /*  */
  label = gtk_label_new("zoom\0");
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    label);

  automation_toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(automation_toolbar->zoom, 4);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->zoom,
			    NULL,
			    NULL);

  /*  */
  automation_toolbar->ports = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
					   "menu\0", NULL,
					   "stock-id\0", GTK_STOCK_EXECUTE,
					   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) automation_toolbar,
			    (GtkWidget *) automation_toolbar->ports,
			    NULL,
			    NULL);
}

void
ags_automation_toolbar_connect(AgsConnectable *connectable)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = AGS_AUTOMATION_TOOLBAR(connectable);
  automation_editor = gtk_widget_get_ancestor(automation_toolbar,
					      AGS_TYPE_AUTOMATION_EDITOR);

  /*  */
  g_signal_connect_after(G_OBJECT(automation_editor), "machine-changed\0",
			 G_CALLBACK(ags_automation_toolbar_machine_changed_callback), automation_toolbar);
}

void
ags_automation_toolbar_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_automation_toolbar_load_ports(AgsAutomationToolbar *toolbar)
{
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  AgsAudio *audio;
  GtkMenu *menu;
  GtkMenuItem *item;
  AgsChannel *channel;
  GList *port;

  auto GList* ags_automation_toolbar_list_ports(GType type);

  GList* ags_automation_toolbar_list_ports(GType type){
    GList *pad, *pad_start;
    GList *line, *line_start;
    GList *line_member, *line_member_start;
    GList *port;

    pad_start =
      pad = gtk_container_get_children(((type == AGS_TYPE_OUTPUT) ? machine->output: machine->input));
    port = NULL;
  
    while(pad != NULL){
      line_start =
	line = gtk_container_get_children(AGS_PAD(pad->data)->expander_set);
      
      while(line != NULL){
	channel = AGS_LINE(line->data)->channel;

	line_member_start = 
	  line_member = gtk_container_get_children(AGS_LINE(line->data)->expander->table);

	while(line_member != NULL){
	  if(AGS_IS_LINE_MEMBER(line_member->data)){
	    /* fill list */
	    port = g_list_prepend(port,
				  AGS_LINE_MEMBER(line_member->data)->port);

	    /* add menu item */
	    item = gtk_check_menu_item_new_with_label(g_strdup_printf("%s: %s\0",
								      AGS_PORT(port->data)->plugin_name,
								      AGS_PORT(port->data)->specifier));
	    g_object_set_data(G_OBJECT(item),
			      AGS_AUTOMATION_TOOLBAR_DATA_CHANNEL,
			      channel);
	    g_object_set_data(G_OBJECT(item),
			      AGS_AUTOMATION_TOOLBAR_DATA_PORT,
			      port->data);
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu),
				  GTK_WIDGET(item));
	    g_signal_connect(item, "toggled\0",
			     G_CALLBACK(ags_automation_toolbar_ports_toggled_callback), toolbar); 

	  }

	  line_member = line_member->next;
	}
	
	g_list_free(line_member_start);

	line = line->next;
      }

      g_list_free(line_start);

      pad = pad->next;
    }

    g_list_free(pad_start);

    port = g_list_reverse(port);

    return(port);
  }

  automation_editor = gtk_widget_get_ancestor(toolbar,
					      AGS_TYPE_AUTOMATION_EDITOR);

  if(automation_editor->selected_machine == NULL){
    return;
  }

  machine = automation_editor->selected_machine;
  audio = machine->audio;

  port = NULL;

  /* retrieve port specifiers */
  menu = gtk_menu_new();

  /* read output ports of line member */
  if(automation_editor->selected_machine->output != NULL){
    port = ags_automation_toolbar_list_ports(AGS_TYPE_OUTPUT);
    g_list_free(port);
  }

  /* read input ports of line member */
  if(automation_editor->selected_machine->input != NULL){
    port = ags_automation_toolbar_list_ports(AGS_TYPE_INPUT);
    g_list_free(port);
  }
    
  g_object_set(toolbar->ports,
	       "menu\0", menu,
	       NULL);
  gtk_widget_show_all(menu);
}

/**
 * ags_automation_toolbar_new:
 *
 * Create a new #AgsAutomationToolbar.
 *
 * Since: 0.4
 */
AgsAutomationToolbar*
ags_automation_toolbar_new()
{
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = (AgsAutomationToolbar *) g_object_new(AGS_TYPE_AUTOMATION_TOOLBAR, NULL);

  return(automation_toolbar);
}
