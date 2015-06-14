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

#include <ags/X/editor/ags_machine_selection.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/machine/ags_ffplayer.h>

void ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection);
void ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_selection_init(AgsMachineSelection *machine_selection);
void ags_machine_selection_connect(AgsConnectable *connectable);
void ags_machine_selection_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_machine_selection
 * @short_description: machine radio buttons
 * @title: AgsMachineSelection
 * @section_id:
 * @include: ags/X/editor/ags_machine_selection.h
 *
 * The #AgsMachineSelection enables you make choice of an #AgsMachine.
 */

static gpointer ags_machine_selection_parent_class = NULL;

GType
ags_machine_selection_get_type(void)
{
  static GType ags_type_machine_selection = 0;

  if(!ags_type_machine_selection){
    static const GTypeInfo ags_machine_selection_info = {
      sizeof (AgsMachineSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_selection = g_type_register_static(GTK_TYPE_DIALOG,
							"AgsMachineSelection\0", &ags_machine_selection_info,
							0);
    
    g_type_add_interface_static(ags_type_machine_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine_selection);
}

void
ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_selection_connect;
  connectable->disconnect = ags_machine_selection_disconnect;
}

void
ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection)
{
  GObjectClass *gobject;

  ags_machine_selection_parent_class = g_type_class_peek_parent(machine_selection);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_selection;
}

void
ags_machine_selection_init(AgsMachineSelection *machine_selection)
{
  g_object_set(G_OBJECT(machine_selection),
	       "title\0", g_strdup("select machines\0"),
	       NULL);

  gtk_dialog_add_buttons(GTK_DIALOG(machine_selection),
			 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			 NULL);
}

void
ags_machine_selection_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_selection_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_machine_selection_run:
 * @machine_selection: the #AgsMachineSelection
 *
 * Run the dialog.
 *
 * Returns: the #AgsMachine
 */
AgsMachine*
ags_machine_selection_run(AgsMachineSelection *machine_selection)
{
  AgsMachine *machine;
  GtkVBox *vbox;
  GtkContainer *content_area;
  GtkRadioButton *group;
  GList *list, *list_start, *index, *index_start;
  gint response;

  machine_selection->machine =
    list = gtk_container_get_children(GTK_CONTAINER(machine_selection->window->machines));
  machine = NULL;

  content_area = (GtkContainer *) gtk_dialog_get_content_area(GTK_DIALOG(machine_selection));

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add(content_area,
		    (GtkWidget *) vbox);

  group = NULL;

  while(list != NULL){
    GtkRadioButton *radio_button;

    if(AGS_IS_FFPLAYER(list->data)){
      radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label_from_widget(group,
										    g_strdup_printf("%s: %s\0",  G_OBJECT_TYPE_NAME(list->data), AGS_MACHINE(list->data)->name));
      gtk_box_pack_start(GTK_BOX(vbox),
			 GTK_WIDGET(radio_button),
			 FALSE, FALSE,
			 0);
      
      if(group == NULL){
	group = radio_button;
      }
    }
      
    list = list->next;
  }

  gtk_widget_show_all((GtkWidget *) vbox);

  //FIXME:JK: don't do this
  response = gtk_dialog_run(GTK_DIALOG(machine_selection));

  if(response == GTK_RESPONSE_ACCEPT){
    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(machine_selection->window->machines));
    index_start = 
      index = gtk_container_get_children((GtkContainer *) vbox);

    while(index != NULL){
      if(AGS_IS_FFPLAYER(list->data)){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(index->data))){
	  machine = AGS_MACHINE(list->data);
	  
	  break;
	}

	index = index->next;
      }
      
      list = list->next;
    }

    g_list_free(list_start);
    g_list_free(index_start);
  }

  gtk_widget_destroy((GtkWidget *) machine_selection);

  return(machine);
}

/**
 * ags_machine_selection_new:
 *
 * Create a new #AgsMachineSelection.
 *
 * Since: 0.4
 */
AgsMachineSelection*
ags_machine_selection_new(AgsWindow *window)
{
  AgsMachineSelection *machine_selection;

  machine_selection = (AgsMachineSelection *) g_object_new(AGS_TYPE_MACHINE_SELECTION,
							   NULL);
  machine_selection->window = window;

  return(machine_selection);
}
