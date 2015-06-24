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

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_notation.h>

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
						       "AgsMachineSelector\0", &ags_machine_selector_info,
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
  GObjectClass *gobject;

  ags_machine_selector_parent_class = g_type_class_peek_parent(machine_selector);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_selector;

  /* AgsMachineSelectorClass */
  machine_selector->changed = NULL;

  machine_selector_signals[CHANGED] =
    g_signal_new("changed\0",
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
  GtkLabel *label;
  GtkMenuToolButton *menu_button;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(machine_selector),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new("Notation\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  machine_selector->current = NULL;
  machine_selector->popup = ags_machine_selector_popup_new(machine_selector);

  menu_button = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
			     "stock-id\0", GTK_STOCK_EXECUTE,
			     "menu\0", machine_selector->popup,
			     NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(menu_button),
		     FALSE, FALSE,
		     0);
}

void
ags_machine_selector_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_machine_selector_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_machine_selector_changed:
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
 *
 * Create a new #AgsMachineSelectorPopup.
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
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("add tab\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("remove tab\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);
  */
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("add index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("remove index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("link index\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_check_menu_item_new_with_label(g_strdup("reverse mapping\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("shift piano\0"));
  gtk_menu_shell_append((GtkMenuShell*) popup, (GtkWidget*) item);

  keys = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(item,
			    keys);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("A\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("A#\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("H\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("C\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("C#\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("D\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("D#\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("E\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("F\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("F#\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("G\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup("G#\0"));
  gtk_menu_shell_append((GtkMenuShell*) keys, (GtkWidget*) item);

  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) popup);

  /*
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_machine_selector_popup_add_tab_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_machine_selector_popup_remove_tab_callback), (gpointer) machine_selector);

  list = list->next;
  */
  
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_machine_selector_popup_add_index_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_machine_selector_popup_remove_index_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate\0",
		   G_CALLBACK(ags_machine_selector_popup_link_index_callback), (gpointer) machine_selector);

  list = list->next;
  g_signal_connect_after(G_OBJECT(list->data), "activate\0",
			 G_CALLBACK(ags_machine_selector_popup_reverse_mapping_callback), (gpointer) machine_selector);

  g_list_free(list_start);

  /* keys */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) keys);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "activate\0",
		     G_CALLBACK(ags_machine_selector_popup_shift_piano_callback), (gpointer) machine_selector);

    list = list->next;
  }

  g_list_free(list_start);

  /* show */
  gtk_widget_show_all((GtkWidget *) keys);
  gtk_widget_show_all((GtkWidget *) popup);

  return(popup);
}
