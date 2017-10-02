/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_playback_window.h>
#include <ags/X/ags_playback_window_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_playback_window_class_init(AgsPlaybackWindowClass *playback_window);
void ags_playback_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playback_window_init(AgsPlaybackWindow *playback_window);
void ags_playback_window_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_playback_window_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_playback_window_finalize(GObject *gobject);
void ags_playback_window_connect(AgsConnectable *connectable);
void ags_playback_window_disconnect(AgsConnectable *connectable);
void ags_playback_window_show(GtkWidget *widget);
gboolean ags_playback_window_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_playback_window
 * @short_description: The playback dialog
 * @title: AgsPlaybackWindow
 * @section_id:
 * @include: ags/X/ags_playback_window.h
 *
 * #AgsPlaybackWindow lets you playback to audio files.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

static gpointer ags_playback_window_parent_class = NULL;

GType
ags_playback_window_get_type()
{
  static GType ags_type_playback_window = 0;

  if(!ags_type_playback_window){
    static const GTypeInfo ags_playback_window_info = {
      sizeof (AgsPlaybackWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlaybackWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playback_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playback_window = g_type_register_static(GTK_TYPE_WINDOW,
						      "AgsPlaybackWindow", &ags_playback_window_info,
						      0);
    
    g_type_add_interface_static(ags_type_playback_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_playback_window);
}

void
ags_playback_window_class_init(AgsPlaybackWindowClass *playback_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_playback_window_parent_class = g_type_class_peek_parent(playback_window);

  /* GObjectClass */
  gobject = (GObjectClass *) playback_window;

  gobject->set_property = ags_playback_window_set_property;
  gobject->get_property = ags_playback_window_get_property;

  gobject->finalize = ags_playback_window_finalize;

  /* properties */
  /**
   * AgsPlaybackWindow:application-context:
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

  /**
   * AgsPlaybackWindow:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("main-window",
				   i18n_pspec("assigned main window"),
				   i18n_pspec("The assigned main window"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_WINDOW,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) playback_window;

  widget->show = ags_playback_window_show;
  widget->delete_event = ags_playback_window_delete_event;
}

void
ags_playback_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_playback_window_connect;
  connectable->disconnect = ags_playback_window_disconnect;
}

void
ags_playback_window_init(AgsPlaybackWindow *playback_window)
{  
  playback_window->flags = 0;

  g_object_set(playback_window,
	       "title", "MIDI playback",
	       NULL);

  playback_window->application_context = NULL;

  playback_window->main_window = NULL;
}

void
ags_playback_window_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlaybackWindow *playback_window;

  playback_window = AGS_PLAYBACK_WINDOW(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) playback_window->application_context == application_context){
	return;
      }
      
      if(playback_window->application_context != NULL){
	g_object_unref(playback_window->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      playback_window->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) playback_window->main_window == main_window){
	return;
      }

      if(playback_window->main_window != NULL){
	g_object_unref(playback_window->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      playback_window->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_window_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlaybackWindow *playback_window;

  playback_window = AGS_PLAYBACK_WINDOW(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, playback_window->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, playback_window->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_window_connect(AgsConnectable *connectable)
{
  AgsPlaybackWindow *playback_window;

  playback_window = AGS_PLAYBACK_WINDOW(connectable);

  if((AGS_PLAYBACK_WINDOW_CONNECTED & (playback_window->flags)) != 0){
    return;
  }

  playback_window->flags |= AGS_PLAYBACK_WINDOW_CONNECTED;
}

void
ags_playback_window_disconnect(AgsConnectable *connectable)
{
  AgsPlaybackWindow *playback_window;

  playback_window = AGS_PLAYBACK_WINDOW(connectable);

  if((AGS_PLAYBACK_WINDOW_CONNECTED & (playback_window->flags)) == 0){
    return;
  }

  playback_window->flags &= (~AGS_PLAYBACK_WINDOW_CONNECTED);
}

void
ags_playback_window_finalize(GObject *gobject)
{
  AgsPlaybackWindow *playback_window;

  playback_window = (AgsPlaybackWindow *) gobject;

  if(playback_window->application_context != NULL){
    g_object_unref(playback_window->application_context);
  }
  
  G_OBJECT_CLASS(ags_playback_window_parent_class)->finalize(gobject);
}

void
ags_playback_window_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_playback_window_parent_class)->show(widget);
}

gboolean
ags_playback_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_playback_window_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_playback_window_new:
 * 
 * Instantiate an #AgsPlaybackWindow
 * 
 * Returns: an #AgsPlaybackWindow
 * 
 * Since: 1.0.0
 */
AgsPlaybackWindow*
ags_playback_window_new()
{
  AgsPlaybackWindow *playback_window;

  playback_window = (AgsPlaybackWindow *) g_object_new(AGS_TYPE_PLAYBACK_WINDOW, NULL);

  return(playback_window);
}
