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

#include <ags/X/ags_export_window.h>
#include <ags/X/ags_export_window_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <stdlib.h>

void ags_export_window_class_init(AgsExportWindowClass *export_window);
void ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_window_init(AgsExportWindow *export_window);
void ags_export_window_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_export_window_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_export_window_finalize(GObject *gobject);
void ags_export_window_connect(AgsConnectable *connectable);
void ags_export_window_disconnect(AgsConnectable *connectable);
void ags_export_window_show(GtkWidget *widget);
gboolean ags_export_window_delete_event(GtkWidget *widget, GdkEventAny *event);

enum{
  PROP_0,
  PROP_DEVOUT,
  PROP_MAIN,
};

static gpointer ags_export_window_parent_class = NULL;

GType
ags_export_window_get_type()
{
  static GType ags_type_export_window = 0;

  if(!ags_type_export_window){
    static const GTypeInfo ags_export_window_info = {
      sizeof (AgsExportWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_window = g_type_register_static(GTK_TYPE_WINDOW,
						    "AgsExportWindow\0", &ags_export_window_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_export_window);
}

void
ags_export_window_class_init(AgsExportWindowClass *export_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_export_window_parent_class = g_type_class_peek_parent(export_window);

  /* GObjectClass */
  gobject = (GObjectClass *) export_window;

  gobject->set_property = ags_export_window_set_property;
  gobject->get_property = ags_export_window_get_property;

  gobject->finalize = ags_export_window_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  param_spec = g_param_spec_object("ags-main\0",
				   "assigned ags_main\0",
				   "The AgsMain it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN,
				  param_spec);


  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) export_window;

  widget->show = ags_export_window_show;
  widget->delete_event = ags_export_window_delete_event;
}

void
ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_export_window_connect;
  connectable->disconnect = ags_export_window_disconnect;
}

void
ags_export_window_init(AgsExportWindow *export_window)
{
  export_window->flags = 0;
}

void
ags_export_window_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = g_value_get_object(value);

      if(export_window->devout == devout)
	return;

      if(devout != NULL)
	g_object_ref(devout);

      export_window->devout = devout;
    }
    break;
  case PROP_MAIN:
    {
      AgsMain *ags_main;

      ags_main = g_value_get_object(value);

      if(export_window->ags_main == ags_main)
	return;

      if(export_window->ags_main != NULL){
	g_object_unref(export_window->ags_main);
      }

      if(ags_main != NULL){
	g_object_ref(ags_main);
      }

      export_window->ags_main = ags_main;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_window_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, export_window->devout);
    break;
  case PROP_MAIN:
    g_value_set_object(value, export_window->ags_main);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_window_connect(AgsConnectable *connectable)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(connectable);
}

void
ags_export_window_disconnect(AgsConnectable *connectable)
{
  //TODO:JK:
  /* implement me */
}

void
ags_export_window_finalize(GObject *gobject)
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) gobject;

  G_OBJECT_CLASS(ags_export_window_parent_class)->finalize(gobject);
}

void
ags_export_window_show(GtkWidget *widget)
{
  AgsExportWindow *export_window;

  GTK_WIDGET_CLASS(ags_export_window_parent_class)->show(widget);

  export_window = (AgsExportWindow *) widget;
}

gboolean
ags_export_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_destroy(widget);

  GTK_WIDGET_CLASS(ags_export_window_parent_class)->delete_event(widget, event);

  return(FALSE);
}

AgsExportWindow*
ags_export_window_new()
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW, NULL);

  return(export_window);
}
