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

#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_menu_bar_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

void ags_menu_bar_class_init(AgsMenuBarClass *menu_bar);
void ags_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_menu_bar_init(AgsMenuBar *menu_bar);
void ags_menu_bar_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_menu_bar_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_menu_bar_connect(AgsConnectable *connectable);
void ags_menu_bar_disconnect(AgsConnectable *connectable);
void ags_menu_bar_destroy(GtkObject *object);
void ags_menu_bar_show(GtkWidget *widget);

/**
 * SECTION:ags_menu_bar
 * @short_description: the menu bar.
 * @title: AgsMenuBar
 * @section_id:
 * @include: ags/X/ags_menu_bar.h
 *
 * #AgsMenuBar is a composite widget to be used as application's menu bar.
 */

GType
ags_menu_bar_get_type(void)
{
  static GType ags_type_menu_bar = 0;

  if(!ags_type_menu_bar){
    static const GTypeInfo ags_menu_bar_info = {
      sizeof (AgsMenuBarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_menu_bar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMenuBar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_menu_bar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_menu_bar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_menu_bar = g_type_register_static(GTK_TYPE_MENU_BAR,
					       "AgsMenuBar\0", &ags_menu_bar_info,
					       0);
    
    g_type_add_interface_static(ags_type_menu_bar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_menu_bar);
}

void
ags_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_menu_bar_connect;
  connectable->disconnect = ags_menu_bar_disconnect;
}

void
ags_menu_bar_class_init(AgsMenuBarClass *menu_bar)
{
}

void
ags_menu_bar_init(AgsMenuBar *menu_bar)
{
  GtkImageMenuItem *item;

  /* File */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_FILE, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->file = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->file);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("export\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->file, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_EDIT, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  /* Edit */
  menu_bar->edit = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->edit);

  //  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_UNDO, NULL);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  //  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_REDO, NULL);
  //  gtk_menu_shell_append((GtkMenuShell*) menu, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_ADD, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* add */
  menu_bar->add = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->add);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Panel\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Mixer\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Drum\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Matrix\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Synth\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("FPlayer\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("bridge\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  /* bridge */
  menu_bar->bridge = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->bridge);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Replicator\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->bridge, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Midi Input\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->bridge, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Midi Output\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->bridge, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("LADSPA\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->bridge, (GtkWidget*) item);

  /* LADSPA */
  menu_bar->ladspa = ags_ladspa_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->ladspa);
  
  /* bridge */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("lv2\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->bridge, (GtkWidget*) item);
  
  /* Lv2 */
  menu_bar->lv2 = ags_lv2_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->lv2);
  
  /* edit */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label("Automation Editor\0");
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit,
			(GtkWidget*) gtk_separator_menu_item_new());

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->edit, (GtkWidget*) item);

  /* Help */
  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar, (GtkWidget*) item);

  menu_bar->help = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->help);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->help, (GtkWidget*) item);
}

void
ags_menu_bar_connect(AgsConnectable *connectable)
{
  AgsMenuBar *menu_bar;
  GList *list0, *list1, *list2, *list3, *list4;
  GList *list1_start, *list2_start, *list3_start, *list4_start;

  menu_bar = AGS_MENU_BAR(connectable);

  /* File */
  g_signal_connect((GObject *) menu_bar, "destroy\0",
		   G_CALLBACK(ags_menu_bar_destroy_callback), (gpointer) menu_bar);

  g_signal_connect((GObject *) menu_bar, "show\0",
		   G_CALLBACK(ags_menu_bar_show_callback), (gpointer) menu_bar);

  list0 = GTK_MENU_SHELL(menu_bar)->children;

  list1_start = 
    list1 = gtk_container_get_children ((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_open_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_save_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_save_as_callback), menu_bar);
  list1 = list1->next->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_export_callback), menu_bar);
  list1 = list1->next->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_quit_callback), (gpointer) menu_bar);

  g_list_free(list1_start);

  /* Edit */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_callback), (gpointer) menu_bar);

  /* add */
  list2_start = 
    list2 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list1->data));
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_panel_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_mixer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_drum_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_matrix_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_synth_callback), (gpointer) menu_bar);
  list2 = list2->next;

  g_signal_connect (G_OBJECT (list2->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_ffplayer_callback), (gpointer) menu_bar);
  list2 = list2->next;

  /* bridge */
  list3_start = 
    list3 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list2->data));
  list2 = list2->next;

  //TODO:JK: replicator bridge
  list3 = list3->next;

  //TODO:JK: midi input
  list3 = list3->next;

  //TODO:JK: midi output
  list3 = list3->next;

  /* ladspa */
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;

  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate\0",
		     G_CALLBACK(ags_menu_bar_add_ladspa_bridge_callback), (gpointer) menu_bar);
    
    list4 = list4->next;
  }

  g_list_free(list4_start);

  /* lv2 */
  list4_start = 
    list4 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list3->data));
  list3 = list3->next;

  while(list4 != NULL){
    g_signal_connect(G_OBJECT(list4->data), "activate\0",
		     G_CALLBACK(ags_menu_bar_add_lv2_bridge_callback), (gpointer) menu_bar);
    
    list4 = list4->next;
  }

  g_list_free(list4_start);

  
  g_list_free(list3_start);
  g_list_free(list2_start);

  /* edit */
  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_remove_callback), (gpointer) menu_bar);
  list1 = list1->next;
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_automation_editor_callback), (gpointer) menu_bar);
  list1 = list1->next;
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_preferences_callback), (gpointer) menu_bar);

  g_list_free(list1_start);

  /* Help */
  list0 = list0->next;
  list1_start = 
    list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_about_callback), (gpointer) menu_bar);

  g_list_free(list1_start);
}

void
ags_menu_bar_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_menu_bar_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_menu_bar_show(GtkWidget *widget)
{
  gtk_widget_show_all(widget);
}

/**
 * ags_menu_bar_new:
 *
 * Creates an #AgsMenuBar
 *
 * Returns: a new #AgsMenuBar
 *
 * Since: 0.3
 */
AgsMenuBar*
ags_menu_bar_new()
{
  AgsMenuBar *menu_bar;

  menu_bar = (AgsMenuBar *) g_object_new(AGS_TYPE_MENU_BAR,
					 NULL);

  return(menu_bar);
}

GtkMenu*
ags_zoom_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;

  menu = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("16:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("8:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("4:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("2:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:2\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:4\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:8\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:16\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  return(menu);
}

GtkMenu*
ags_tact_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;

  menu = (GtkMenu *) gtk_menu_new();

  //TODO:JK: uncomment me if tact implemented
  /*
  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("16/1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("8/1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("4/1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("2/1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1:1\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);


  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1/2\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1/4\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1/8\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new();
  label = (GtkLabel *) gtk_label_new(g_strdup("1/16\0"));
  gtk_container_add((GtkContainer *) item, (GtkWidget *) label);
  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);
  */

  return(menu);
}

/**
 * ags_ladspa_menu_new:
 *
 * Creates an #GtkMenu to select LADSPA plugin.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 0.4.3
 */
GtkMenu*
ags_ladspa_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;
  
  gchar **filename, **filename_start;
  
  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;

  menu = (GtkMenu *) gtk_menu_new();

  /* retrieve LADSPA filenames */
  ags_ladspa_manager_load_default_directory();
  filename =
    filename_start = ags_ladspa_manager_get_filenames();

  while(*filename != NULL){
    ags_ladspa_manager_load_file(*filename);
    ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(*filename);
  
    plugin_so = ladspa_plugin->plugin_so;

    if(plugin_so){
      ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							     "ladspa_descriptor\0");

      if(dlerror() == NULL && ladspa_descriptor){
	unsigned long index;

	/* We've successfully found a ladspa_descriptor function. Now load name and uuid member. */
	for(index = 0; (plugin_descriptor = ladspa_descriptor(index)) != NULL; index++){
	  item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup_printf("%s:%lu\0",
									      plugin_descriptor->Name,
									      plugin_descriptor->UniqueID));
	  g_object_set_data_full(item,
				 AGS_MENU_BAR_LADSPA_FILENAME_KEY,
				 g_strdup(*filename),
				 (GDestroyNotify) g_free);
	  g_object_set_data_full(item,
				 AGS_MENU_BAR_LADSPA_EFFECT_KEY,
				 g_strdup(plugin_descriptor->Name),
				 (GDestroyNotify) g_free);

	  gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);
	}
      }
    }

    filename++;
  }

  //  g_strfreev(filename_start);
  
  return(menu);
}

/**
 * ags_lv2_menu_new:
 *
 * Creates an #GtkMenu to select lv2 plugin.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 0.4.3
 */
GtkMenu*
ags_lv2_menu_new()
{
  GtkMenu *menu;
  GtkMenuItem *item;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GList *list;
  
  gchar *plugin_name;
  gchar **filename, **filename_start;
  
  menu = (GtkMenu *) gtk_menu_new();

  /* retrieve LV2 filenames */
  ags_lv2_manager_load_default_directory();
  filename =
    filename_start = ags_lv2_manager_get_filenames();

  while(*filename != NULL){
    g_message(*filename);
    lv2_plugin = ags_lv2_manager_find_lv2_plugin(*filename);

    list = ags_turtle_find_xpath(lv2_plugin->turtle,
				 "//rdf-triple/rdf-verb[@do=\"doap:name\"]//rdf-value[1]\0");

    /* We've successfully found a lv2_descriptor function. Now load name and uuid member. */
    while(list != NULL){
      /* instantiate and load turtle */      
      item = (GtkMenuItem *) gtk_menu_item_new_with_label(g_strdup(xmlGetProp(list->data,
									      "value\0")));
      g_object_set_data_full(item,
			     AGS_MENU_BAR_LV2_FILENAME_KEY,
			     g_strdup(*filename),
			     (GDestroyNotify) g_free);
      g_object_set_data_full(item,
			     AGS_MENU_BAR_LV2_URI_KEY,
			     g_strdup(xmlGetProp(((xmlNode *) list->data)->parent->parent->parent,
						 "subject\0")),
			     (GDestroyNotify) g_free);
      
      gtk_menu_shell_append((GtkMenuShell *) menu, (GtkWidget *) item);

      list = list->next;
    }

    filename++;
  }

  //  g_strfreev(filename_start);
  
  return(menu);
}

/**
 * ags_zoom_combo_box_new:
 *
 * Creates an #GtkComboBox to select zoom.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 0.4
 */
GtkComboBox*
ags_zoom_combo_box_new()
{
  GtkComboBoxText *combo_box;

  combo_box = gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "16:1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "8:1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "4:1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "2:1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1:1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1:2\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1:4\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1:8\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1:16\0");

  return(combo_box);
}

/**
 * ags_zoom_combo_box_new:
 *
 * Creates an #GtkComboBox to select tact
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 0.4
 */
GtkComboBox*
ags_tact_combo_box_new()
{
  GtkComboBoxText *combo_box;

  combo_box = gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "16/1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "8/1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "4/1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "2/1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1/1\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1/2\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1/4\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1/8\0");
  gtk_combo_box_text_append_text(combo_box,
				 "1/16\0");

  return(combo_box);
}
