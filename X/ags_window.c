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

#include <ags/X/ags_window.h>
#include <ags/X/ags_window_callbacks.h>

#include <stdlib.h>

void ags_window_class_init(AgsWindowClass *window);
void ags_window_init(AgsWindow *window);
void ags_window_destroy(GtkObject *object);
void ags_window_show(GtkWidget *widget);
void ags_window_connect(AgsWindow *window);

GType
ags_window_get_type(void)
{
  static GType window_type = 0;

  if(!window_type){
    static const GtkTypeInfo window_info = {
      "AgsWindow\0",
      sizeof(AgsWindow), /* base_init */
      sizeof(AgsWindowClass), /* base_finalize */
      (GtkClassInitFunc) ags_window_class_init,
      (GtkObjectInitFunc) ags_window_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };
    window_type = gtk_type_unique (GTK_TYPE_WINDOW, &window_info);
  }

  return(window_type);
}

void
ags_window_class_init(AgsWindowClass *window)
{
}

void
ags_window_init(AgsWindow *window)
{
  GtkVBox *vbox;
  GtkWidget *scrolled_window;

  window->devout = ags_devout_new();

  window->name = g_strdup("unnamed\0");

  gtk_window_set_title((GtkWindow *) window, g_strconcat("ags - \0", window->name, NULL));

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) window, (GtkWidget*) vbox);

  window->menu_bar = ags_menu_bar_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) window->menu_bar,
		     FALSE, FALSE, 0);

  window->paned = (GtkVPaned *) gtk_vpaned_new();
  gtk_box_pack_start((GtkBox*) vbox, (GtkWidget*) window->paned, TRUE, TRUE, 0);

  scrolled_window = (GtkWidget *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_add1((GtkPaned *) window->paned,
		 scrolled_window);

  window->machines = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport((GtkScrolledWindow *) scrolled_window,
					(GtkWidget *) window->machines);
  window->editor = ags_editor_new();
  gtk_paned_add2((GtkPaned *) window->paned,
		 (GtkWidget *) window->editor);

  window->navigation = ags_navigation_new();
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) window->navigation,
		     FALSE, FALSE, 0);

  window->selected = NULL;
  window->counter = (AgsMachineCounter *) malloc(sizeof(AgsMachineCounter));
  window->counter->everything = 0;
  window->counter->panel = 0;
  window->counter->mixer = 0;
  window->counter->drum = 0;
  window->counter->matrix = 0;
  window->counter->synth = 0;
  window->counter->ffplayer = 0;
}

void
ags_window_connect(AgsWindow *window)
{
  g_signal_connect(G_OBJECT(window), "delete_event\0",
		   G_CALLBACK(ags_window_delete_event_callback), (gpointer) window);

  g_signal_connect((GObject *) window, "destroy\0",
		   G_CALLBACK(ags_window_destroy_callback), (gpointer) window);

  g_signal_connect((GObject *) window, "show\0",
		   G_CALLBACK(ags_window_show_callback), (gpointer) window);

  ags_menu_bar_connect(window->menu_bar);
  ags_editor_connect(window->editor);
  ags_navigation_connect(window->navigation);
}

void
ags_window_destroy(GtkObject *object)
{
  AgsWindow *window;
  GList *list0, *list1;

  window = (AgsWindow *) object;

  g_object_unref(G_OBJECT(window->devout));

  free(window->counter);
  free(window->name);
}

void
ags_window_show(GtkWidget *widget)
{
  AgsWindow *window;

  window = (AgsWindow *) widget;

  gtk_widget_show((GtkWidget *) window->menu_bar);
}

AgsWindow*
ags_window_new()
{
  AgsWindow *window;

  window = (AgsWindow *) g_object_new(AGS_TYPE_WINDOW, NULL);

  return(window);
}
