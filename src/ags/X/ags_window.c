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

#include <ags-lib/object/ags_connectable.h>

#include <stdlib.h>

void ags_window_class_init(AgsWindowClass *window);
void ags_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_window_init(AgsWindow *window);
void ags_window_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_window_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_window_finalize(GObject *gobject);
void ags_window_connect(AgsConnectable *connectable);
void ags_window_disconnect(AgsConnectable *connectable);
void ags_window_show(GtkWidget *widget);
gboolean ags_window_delete_event(GtkWidget *widget, GdkEventAny *event);

enum{
  PROP_0,
  PROP_DEVOUT,
};

static gpointer ags_window_parent_class = NULL;

GType
ags_window_get_type()
{
  static GType ags_type_window = 0;

  if(!ags_type_window){
    static const GTypeInfo ags_window_info = {
      sizeof (AgsWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_window = g_type_register_static(GTK_TYPE_WINDOW,
					     "AgsWindow\0", &ags_window_info,
					     0);
    
    g_type_add_interface_static(ags_type_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_window);
}

void
ags_window_class_init(AgsWindowClass *window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_window_parent_class = g_type_class_peek_parent(window);

  /* GObjectClass */
  gobject = (GObjectClass *) window;

  gobject->set_property = ags_window_set_property;
  gobject->get_property = ags_window_get_property;

  gobject->finalize = ags_window_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) window;

  widget->show = ags_window_show;
  widget->delete_event = ags_window_delete_event;
}

void
ags_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_window_connect;
  connectable->disconnect = ags_window_disconnect;
}

void
ags_window_init(AgsWindow *window)
{
  GtkVBox *vbox;
  GtkWidget *scrolled_window;
  GError *error;

  window->flags = 0;

  error = NULL;

  g_object_set(G_OBJECT(window),
	       "icon\0", gdk_pixbuf_new_from_file("./doc/images/jumper.png\0", &error),
	       NULL);

  window->main = NULL;
  window->devout = NULL;

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

  window->preferences = NULL;

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
ags_window_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsWindow *window;

  window = AGS_WINDOW(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = g_value_get_object(value);

      if(window->devout == devout)
	return;

      if(devout != NULL)
	g_object_ref(devout);

      window->devout = devout;

      g_object_set(G_OBJECT(window->editor),
		   "devout\0", devout,
		   NULL);

      g_object_set(G_OBJECT(window->navigation),
		   "devout\0", devout,
		   NULL);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_window_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsWindow *window;

  window = AGS_WINDOW(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, window->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_window_connect(AgsConnectable *connectable)
{
  AgsWindow *window;

  window = AGS_WINDOW(connectable);

  g_signal_connect(G_OBJECT(window), "delete_event\0",
		   G_CALLBACK(ags_window_delete_event_callback), NULL);

  ags_connectable_connect(AGS_CONNECTABLE(window->menu_bar));
  ags_connectable_connect(AGS_CONNECTABLE(window->editor));
  ags_connectable_connect(AGS_CONNECTABLE(window->navigation));
}

void
ags_window_disconnect(AgsConnectable *connectable)
{
  //TODO:JK:
  /* implement me */
}

void
ags_window_finalize(GObject *gobject)
{
  AgsWindow *window;

  window = (AgsWindow *) gobject;

  g_object_unref(G_OBJECT(window->devout));

  free(window->counter);
  free(window->name);

  G_OBJECT_CLASS(ags_window_parent_class)->finalize(gobject);
}

void
ags_window_show(GtkWidget *widget)
{
  AgsWindow *window;

  GTK_WIDGET_CLASS(ags_window_parent_class)->show(widget);

  window = (AgsWindow *) widget;

  gtk_widget_show((GtkWidget *) window->menu_bar);
}

gboolean
ags_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_destroy(widget);

  GTK_WIDGET_CLASS(ags_window_parent_class)->delete_event(widget, event);

  return(FALSE);
}

AgsWindow*
ags_window_new(GObject *main)
{
  AgsWindow *window;

  window = (AgsWindow *) g_object_new(AGS_TYPE_WINDOW, NULL);

  window->main = main;

  return(window);
}
