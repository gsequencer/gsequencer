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

#include <ags/X/editor/ags_port_selection.h>
#include <ags/X/editor/ags_port_selection_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>

#include <ags/X/ags_port_editor.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

void ags_port_selection_class_init(AgsPortSelectionClass *port_selection);
void ags_port_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_port_selection_init(AgsPortSelection *port_selection);
void ags_port_selection_connect(AgsConnectable *connectable);
void ags_port_selection_disconnect(AgsConnectable *connectable);

void ags_port_selection_real_add_port(AgsPortSelection *port_selection,
				      AgsPort *port);

/**
 * SECTION:ags_port_selection
 * @short_description: edit tool
 * @title: AgsPortSelection
 * @section_id:
 * @include: ags/X/editor/ags_note_edit.h
 *
 * The #AgsPortSelection lets you choose edit tool.
 */

GType
ags_port_selection_get_type(void)
{
  static GType ags_type_port_selection = 0;

  if (!ags_type_port_selection){
    static const GTypeInfo ags_port_selection_info = {
      sizeof (AgsPortSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_port_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPortSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_port_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_port_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_port_selection = g_type_register_static(GTK_TYPE_SELECTION,
							 "AgsPortSelection\0", &ags_port_selection_info,
							 0);
    
    g_type_add_interface_static(ags_type_port_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_port_selection);
}

void
ags_port_selection_class_init(AgsPortSelectionClass *port_selection)
{
}

void
ags_port_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_port_selection_connect;
  connectable->disconnect = ags_port_selection_disconnect;
}

void
ags_port_selection_init(AgsPortSelection *port_selection)
{
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkLabel *label;

  port_selection->position = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					      "image\0", gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
										  GTK_ICON_SIZE_LARGE_SELECTION),
					      "relief\0", GTK_RELIEF_NONE,
					      NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    (GtkWidget *) port_selection->position,
			    "position cursor\0",
			    NULL);

  port_selection->edit = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					  "image\0", gtk_image_new_from_stock(GTK_STOCK_EDIT,
									      GTK_ICON_SIZE_LARGE_SELECTION),
					  "relief\0", GTK_RELIEF_NONE,
					  NULL);
  port_selection->selected_edit_mode = port_selection->edit;
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    (GtkWidget *) port_selection->edit,
			    "edit port\0",
			    NULL);

  port_selection->clear = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					   "image\0", gtk_image_new_from_stock(GTK_STOCK_CLEAR,
									       GTK_ICON_SIZE_LARGE_SELECTION),
					   "relief\0", GTK_RELIEF_NONE,
					   NULL);
  
  port_selection->select = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					    "image\0", gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
										GTK_ICON_SIZE_LARGE_SELECTION),
					    "relief\0", GTK_RELIEF_NONE,
					    NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    (GtkWidget *) port_selection->select,
			    "select port\0",
			    NULL);

  port_selection->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					     "image\0", gtk_image_new_from_stock(GTK_STOCK_COPY,
										 GTK_ICON_SIZE_LARGE_SELECTION),
					     "relief\0", GTK_RELIEF_NONE,
					     NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    port_selection->copy,
			    "copy port\0",
			    NULL);

  port_selection->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						       "image\0", gtk_image_new_from_stock(GTK_STOCK_CUT,
											   GTK_ICON_SIZE_LARGE_SELECTION),
						       "relief\0", GTK_RELIEF_NONE,
						       NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    port_selection->cut,
			    "cut port\0",
			    NULL);

  port_selection->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							 "image\0", gtk_image_new_from_stock(GTK_STOCK_PASTE,
											     GTK_ICON_SIZE_LARGE_SELECTION),
							 "relief\0", GTK_RELIEF_NONE,
							 NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    port_selection->paste,
			    "paste port\0",
			    NULL);

  /*  */
  label = gtk_label_new("zoom\0");
  gtk_container_add(GTK_CONTAINER(port_selection),
		    label);

  port_selection->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(port_selection->zoom, 4);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    (GtkWidget *) port_selection->zoom,
			    NULL,
			    NULL);

  /*  */
  port_selection->ports = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
					   "menu\0", NULL,
					   "stock-id\0", GTK_STOCK_EXECUTE,
					   NULL);
  gtk_selection_append_widget((GtkSelection *) port_selection,
			    (GtkWidget *) port_selection->ports,
			    NULL,
			    NULL);
}

void
ags_port_selection_connect(AgsConnectable *connectable)
{
  AgsPortEditor *port_editor;
  AgsPortSelection *port_selection;

  port_selection = AGS_PORT_SELECTION(connectable);
  port_editor = gtk_widget_get_ancestor(port_selection,
					      AGS_TYPE_PORT_EDITOR);

  /*  */
  g_signal_connect_after(G_OBJECT(port_editor), "machine-changed\0",
			 G_CALLBACK(ags_port_selection_machine_changed_callback), port_selection);
}

void
ags_port_selection_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_port_selection_load_ports(AgsPortSelection *selection)
{
  AgsPortEditor *port_editor;
  AgsMachine *machine;
  AgsAudio *audio;
  GtkMenu *menu;
  GtkMenuItem *item;
  AgsChannel *channel;
  GList *port;

  auto GList* ags_port_selection_list_ports(GType type);

  GList* ags_port_selection_list_ports(GType type){
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
			      AGS_PORT_SELECTION_DATA_CHANNEL,
			      channel);
	    g_object_set_data(G_OBJECT(item),
			      AGS_PORT_SELECTION_DATA_PORT,
			      port->data);
	    gtk_menu_shell_append(GTK_MENU_SHELL(menu),
				  GTK_WIDGET(item));
	    g_signal_connect(item, "toggled\0",
			     G_CALLBACK(ags_port_selection_ports_toggled_callback), selection); 

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

  port_editor = gtk_widget_get_ancestor(selection,
					      AGS_TYPE_PORT_EDITOR);

  if(port_editor->selected_machine == NULL){
    return;
  }

  machine = port_editor->selected_machine;
  audio = machine->audio;

  port = NULL;

  /* retrieve port specifiers */
  menu = gtk_menu_new();

  /* read output ports of line member */
  if(port_editor->selected_machine->output != NULL){
    port = ags_port_selection_list_ports(AGS_TYPE_OUTPUT);
    g_list_free(port);
  }

  /* read input ports of line member */
  if(port_editor->selected_machine->input != NULL){
    port = ags_port_selection_list_ports(AGS_TYPE_INPUT);
    g_list_free(port);
  }
    
  g_object_set(selection->ports,
	       "menu\0", menu,
	       NULL);
  gtk_widget_show_all(menu);
}

void
ags_port_selection_add_port(AgsPortSelection *port_selection,
				 AgsPort *port)
{
  //TODO:JK: implement me
}

/**
 * ags_port_selection_new:
 *
 * Create a new #AgsPortSelection.
 *
 * Since: 0.4
 */
AgsPortSelection*
ags_port_selection_new()
{
  AgsPortSelection *port_selection;

  port_selection = (AgsPortSelection *) g_object_new(AGS_TYPE_PORT_SELECTION,
						     NULL);

  return(port_selection);
}
