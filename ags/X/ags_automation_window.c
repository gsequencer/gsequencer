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

#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_window_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_notation.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_automation_window_class_init(AgsAutomationWindowClass *automation_window);
void ags_automation_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_window_init(AgsAutomationWindow *automation_window);
void ags_automation_window_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_automation_window_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_automation_window_finalize(GObject *gobject);
void ags_automation_window_connect(AgsConnectable *connectable);
void ags_automation_window_disconnect(AgsConnectable *connectable);
gboolean ags_automation_window_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_automation_window
 * @short_description: The automation window
 * @title: AgsAutomationWindow
 * @section_id:
 * @include: ags/X/ags_automation_window.h
 *
 * #AgsAutomationWindow lets you automation to audio files. Currently only
 * WAV supported.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_automation_window_parent_class = NULL;

GType
ags_automation_window_get_type()
{
  static GType ags_type_automation_window = 0;

  if(!ags_type_automation_window){
    static const GTypeInfo ags_automation_window_info = {
      sizeof (AgsAutomationWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_window = g_type_register_static(GTK_TYPE_WINDOW,
							"AgsAutomationWindow", &ags_automation_window_info,
							0);
    
    g_type_add_interface_static(ags_type_automation_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_automation_window);
}

void
ags_automation_window_class_init(AgsAutomationWindowClass *automation_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_automation_window_parent_class = g_type_class_peek_parent(automation_window);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_window;

  gobject->set_property = ags_automation_window_set_property;
  gobject->get_property = ags_automation_window_get_property;

  gobject->finalize = ags_automation_window_finalize;

  /**
   * AgsAutomationWindow:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) automation_window;

  widget->delete_event = ags_automation_window_delete_event;
}

void
ags_automation_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_window_connect;
  connectable->disconnect = ags_automation_window_disconnect;
}

void
ags_automation_window_init(AgsAutomationWindow *automation_window)
{
  g_object_set(G_OBJECT(automation_window),
	       "title", "edit automation",
	       NULL);

  automation_window->flags = 0;

  automation_window->soundcard = NULL;
  
  automation_window->automation_editor = ags_automation_editor_new();
  gtk_container_add((GtkContainer *) automation_window,
		    (GtkWidget *) automation_window->automation_editor);		    
}

void
ags_automation_window_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationWindow *automation_window;

  automation_window = AGS_AUTOMATION_WINDOW(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(automation_window->soundcard == soundcard){
	return;
      }

      if(automation_window->soundcard != NULL){
	g_object_unref(automation_window->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      automation_window->soundcard = soundcard;

      g_object_set(G_OBJECT(automation_window->automation_editor),
		   "soundcard", soundcard,
		   NULL);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_window_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAutomationWindow *automation_window;

  automation_window = AGS_AUTOMATION_WINDOW(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    g_value_set_object(value, automation_window->soundcard);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_window_connect(AgsConnectable *connectable)
{
  AgsAutomationWindow *automation_window;

  automation_window = AGS_AUTOMATION_WINDOW(connectable);

  if((AGS_AUTOMATION_WINDOW_CONNECTED & (automation_window->flags)) != 0){
    return;
  }

  automation_window->flags |= AGS_AUTOMATION_WINDOW_CONNECTED;
  
  if(automation_window->automation_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(automation_window->automation_editor));
  }
}

void
ags_automation_window_disconnect(AgsConnectable *connectable)
{
  AgsAutomationWindow *automation_window;

  automation_window = AGS_AUTOMATION_WINDOW(connectable);

  if((AGS_AUTOMATION_WINDOW_CONNECTED & (automation_window->flags)) == 0){
    return;
  }

  automation_window->flags &= (~AGS_AUTOMATION_WINDOW_CONNECTED);

  if(automation_window->automation_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(automation_window->automation_editor));
  }
}

void
ags_automation_window_finalize(GObject *gobject)
{
  AgsAutomationWindow *automation_window;

  automation_window = (AgsAutomationWindow *) gobject;

  if(automation_window->soundcard != NULL){
    g_object_unref(G_OBJECT(automation_window->soundcard));
  }
  
  G_OBJECT_CLASS(ags_automation_window_parent_class)->finalize(gobject);
}

gboolean
ags_automation_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

/**
 * ags_automation_window_new:
 * @parent_window: the parent #AgsWindow
 * 
 * Instantiate #AgsAutomationWindow
 * 
 * Returns: the new #AgsAutomationWindow instance
 * 
 * Since: 1.0.0
 */
AgsAutomationWindow*
ags_automation_window_new(GtkWidget *parent_window)
{
  AgsAutomationWindow *automation_window;

  automation_window = (AgsAutomationWindow *) g_object_new(AGS_TYPE_AUTOMATION_WINDOW,
							   NULL);
  automation_window->parent_window = parent_window;
  
  return(automation_window);
}
