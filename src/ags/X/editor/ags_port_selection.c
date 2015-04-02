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

#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>
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
 * The #AgsPortSelection lets you add ports to automation.
 */

enum{
  ADD_PORT,
  LAST_SIGNAL,
};

static guint port_selection_signals[LAST_SIGNAL];

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

    ags_type_port_selection = g_type_register_static(GTK_TYPE_MENU_TOOL_BUTTON,
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
  /* AgsPortSelectionClass */
  port_selection->add_port = ags_port_selection_real_add_port;

  port_selection_signals[ADD_PORT] =
    g_signal_new("add-port\0",
                 G_TYPE_FROM_CLASS(port_selection),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPortSelectionClass, add_port),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
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
  g_object_set(G_OBJECT(port_selection),
	       "menu\0", NULL,
	       "stock-id\0", GTK_STOCK_EXECUTE,
	       NULL);
}

void
ags_port_selection_connect(AgsConnectable *connectable)
{
  AgsPortSelection *port_selection;

  port_selection = AGS_PORT_SELECTION(connectable);

  //TODO:JK: implement me
}

void
ags_port_selection_disconnect(AgsConnectable *connectable)
{
  AgsPortSelection *port_selection;

  port_selection = AGS_PORT_SELECTION(connectable);

  //TODO:JK: implement me
}

/**
 * ags_port_selection_load_ports:
 * @selection: 
 *
 * 
 */
void
ags_port_selection_load_ports(AgsPortSelection *selection)
{
  AgsAutomationEditor *automation_editor;
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
	    item = gtk_check_menu_item_new_with_label(g_strdup_printf("[%d] %s: %s\0",
								      channel->line,
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

  automation_editor = gtk_widget_get_ancestor(selection,
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
    port = ags_port_selection_list_ports(AGS_TYPE_OUTPUT);
    g_list_free(port);
  }

  /* read input ports of line member */
  if(automation_editor->selected_machine->input != NULL){
    port = ags_port_selection_list_ports(AGS_TYPE_INPUT);
    g_list_free(port);
  }
    
  g_object_set(selection,
	       "menu\0", menu,
	       NULL);
  gtk_widget_show_all(menu);
}

/**
 * ags_port_selection_enable_ports:
 * @port_selection: 
 * @ports:
 *
 * 
 */
void
ags_port_selection_enable_ports(AgsPortSelection *port_selection,
				GList *ports)
{
  GtkMenu *menu;
  AgsPort *current;
  GList *list, *list_start;

  g_object_get(port_selection,
	       "menu\0", &menu,
	       NULL);

  /*  */
  list_start = 
    list = gtk_container_get_children(menu);

  while(list != NULL &&
	ports != NULL){
    current = g_object_get_data(list->data,
				AGS_PORT_SELECTION_DATA_PORT);

    if(current == ports->data){
      gtk_check_menu_item_set_active(list->data,
				     TRUE);

      ports = ports->next;
    }

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_port_selection_real_add_port(AgsPortSelection *port_selection,
				 AgsPort *port)
{
  AgsAutomationEditor *automation_editor;
  AgsAutomationEdit *automation_edit;
  AgsAutomation *automation;
  GtkDrawingArea *drawing_area;

  automation_editor = gtk_widget_get_ancestor(port_selection,
					      AGS_TYPE_AUTOMATION_EDITOR);
  automation_edit = automation_editor->automation_edit;

  /* add automation */
  automation = g_object_new(AGS_TYPE_AUTOMATION,
			    "port\0", port,
			    NULL);
  ags_audio_add_automation(automation_editor->selected_machine->audio,
			   automation);

  /* add automation area */
  drawing_area = ags_automation_edit_add_drawing_area(automation_edit,
						      automation);
}

/**
 * ags_port_selection_add_port:
 * @port_selection:
 * @port: 
 *
 * 
 */
void
ags_port_selection_add_port(AgsPortSelection *port_selection,
			    AgsPort *port)
{
  g_return_if_fail(AGS_IS_PORT_SELECTION(port_selection));

  g_object_ref((GObject *) port_selection);
  g_signal_emit(G_OBJECT(port_selection),
		port_selection_signals[ADD_PORT], 0,
		port);
  g_object_unref((GObject *) port_selection);
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
