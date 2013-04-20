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

void ags_menu_bar_class_init(AgsMenuBarClass *menu_bar);
void ags_menu_bar_init(AgsMenuBar *menu_bar);
void ags_menu_bar_connect(AgsMenuBar *menu_bar);
void ags_menu_bar_destroy(GtkObject *object);
void ags_menu_bar_show(GtkWidget *widget);

GType
ags_menu_bar_get_type(void)
{
  static GType menu_bar_type = 0;

  if(!menu_bar_type){
    static const GtkTypeInfo menu_bar_info = {
      "AgsMenuBar\0",
      sizeof(AgsMenuBar), /* base_init */
      sizeof(AgsMenuBarClass), /* base_finalize */
      (GtkClassInitFunc) ags_menu_bar_class_init,
      (GtkObjectInitFunc) ags_menu_bar_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    menu_bar_type = gtk_type_unique (GTK_TYPE_MENU_BAR, &menu_bar_info);
  }

  return(menu_bar_type);
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

  menu_bar->add = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu((GtkMenuItem*) item, (GtkWidget*) menu_bar->add);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("Panel\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("Mixer\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("Drum\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("Matrix\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("Synth\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_with_label(g_strdup("FPlayer\0"));
  gtk_menu_shell_append((GtkMenuShell*) menu_bar->add, (GtkWidget*) item);

  item = (GtkImageMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
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
ags_menu_bar_connect(AgsMenuBar *menu_bar)
{
  GList *list0, *list1, *list2;

  /* File */
  g_signal_connect((GObject *) menu_bar, "destroy\0",
		   G_CALLBACK(ags_menu_bar_destroy_callback), (gpointer) menu_bar);

  g_signal_connect((GObject *) menu_bar, "show\0",
		   G_CALLBACK(ags_menu_bar_show_callback), (gpointer) menu_bar);

  list0 = GTK_MENU_SHELL(menu_bar)->children;

  list1 = gtk_container_get_children ((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_open_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_save_callback), (gpointer) menu_bar);
  list1 = list1->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_save_as_callback), (gpointer) menu_bar);
  list1 = list1->next->next;

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_quit_callback), (gpointer) menu_bar);

  /* Edit */
  list0 = list0->next;
  list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_add_callback), (gpointer) menu_bar);
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

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_remove_callback), (gpointer) menu_bar);

  /* Help */
  list0 = list0->next;
  list1 = gtk_container_get_children((GtkContainer *) gtk_menu_item_get_submenu((GtkMenuItem *) list0->data));

  g_signal_connect (G_OBJECT (list1->data), "activate\0",
                    G_CALLBACK (ags_menu_bar_about_callback), (gpointer) menu_bar);
}

void
ags_menu_bar_destroy(GtkObject *object)
{
}

void
ags_menu_bar_show(GtkWidget *widget)
{
  gtk_widget_show_all(widget);
}

AgsMenuBar*
ags_menu_bar_new()
{
  AgsMenuBar *menu_bar;

  menu_bar = (AgsMenuBar *) g_object_new(AGS_TYPE_MENU_BAR, NULL);

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

  return(menu);
}
