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

#include <ags/X/ags_history_browser.h>
#include <ags/X/ags_history_browser_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_history_browser_class_init(AgsHistoryBrowserClass *history_browser);
void ags_history_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_history_browser_init(AgsHistoryBrowser *history_browser);
void ags_history_browser_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_history_browser_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_history_browser_finalize(GObject *gobject);
void ags_history_browser_connect(AgsConnectable *connectable);
void ags_history_browser_disconnect(AgsConnectable *connectable);
void ags_history_browser_show(GtkWidget *widget);
gboolean ags_history_browser_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_history_browser
 * @short_description: The history dialog
 * @title: AgsHistoryBrowser
 * @section_id:
 * @include: ags/X/ags_history_browser.h
 *
 * #AgsHistoryBrowser lets edit history.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_history_browser_parent_class = NULL;

GType
ags_history_browser_get_type()
{
  static GType ags_type_history_browser = 0;

  if(!ags_type_history_browser){
    static const GTypeInfo ags_history_browser_info = {
      sizeof (AgsHistoryBrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_history_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsHistoryBrowser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_history_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_history_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_history_browser = g_type_register_static(GTK_TYPE_WINDOW,
						      "AgsHistoryBrowser", &ags_history_browser_info,
						      0);
    
    g_type_add_interface_static(ags_type_history_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_history_browser);
}

void
ags_history_browser_class_init(AgsHistoryBrowserClass *history_browser)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_history_browser_parent_class = g_type_class_peek_parent(history_browser);

  /* GObjectClass */
  gobject = (GObjectClass *) history_browser;

  gobject->set_property = ags_history_browser_set_property;
  gobject->get_property = ags_history_browser_get_property;

  gobject->finalize = ags_history_browser_finalize;

  /* properties */
  /**
   * AgsHistoryBrowser:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("assigned application context"),
				   i18n_pspec("The AgsApplicationContext it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);


  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) history_browser;

  widget->show = ags_history_browser_show;
  widget->delete_event = ags_history_browser_delete_event;
}

void
ags_history_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_history_browser_connect;
  connectable->disconnect = ags_history_browser_disconnect;
}

void
ags_history_browser_init(AgsHistoryBrowser *history_browser)
{
  history_browser->flags = 0;
}

void
ags_history_browser_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsHistoryBrowser *history_browser;

  history_browser = AGS_HISTORY_BROWSER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) history_browser->application_context == application_context)
	return;

      if(history_browser->application_context != NULL){
	g_object_unref(history_browser->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      history_browser->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_history_browser_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsHistoryBrowser *history_browser;

  history_browser = AGS_HISTORY_BROWSER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    g_value_set_object(value, history_browser->application_context);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_history_browser_connect(AgsConnectable *connectable)
{
  AgsHistoryBrowser *history_browser;

  history_browser = AGS_HISTORY_BROWSER(connectable);
}

void
ags_history_browser_disconnect(AgsConnectable *connectable)
{
  //TODO:JK:
  /* implement me */
}

void
ags_history_browser_finalize(GObject *gobject)
{
  AgsHistoryBrowser *history_browser;

  history_browser = (AgsHistoryBrowser *) gobject;

  G_OBJECT_CLASS(ags_history_browser_parent_class)->finalize(gobject);
}

void
ags_history_browser_show(GtkWidget *widget)
{
  AgsHistoryBrowser *history_browser;

  GTK_WIDGET_CLASS(ags_history_browser_parent_class)->show(widget);

  history_browser = (AgsHistoryBrowser *) widget;
}

gboolean
ags_history_browser_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_history_browser_parent_class)->delete_event(widget, event);

  return(TRUE);
}

AgsHistoryBrowser*
ags_history_browser_new()
{
  AgsHistoryBrowser *history_browser;

  history_browser = (AgsHistoryBrowser *) g_object_new(AGS_TYPE_HISTORY_BROWSER,
						       NULL);

  return(history_browser);
}
