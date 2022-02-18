/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/ags_wave_window.h>
#include <ags/app/ags_wave_window_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_wave_window_class_init(AgsWaveWindowClass *wave_window);
void ags_wave_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_window_init(AgsWaveWindow *wave_window);
void ags_wave_window_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_wave_window_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_wave_window_finalize(GObject *gobject);
void ags_wave_window_connect(AgsConnectable *connectable);
void ags_wave_window_disconnect(AgsConnectable *connectable);
gboolean ags_wave_window_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_wave_window
 * @short_description: The wave window
 * @title: AgsWaveWindow
 * @section_id:
 * @include: ags/X/ags_wave_window.h
 *
 * #AgsWaveWindow lets you edit audio files.
 */

enum{
  PROP_0,
};

static gpointer ags_wave_window_parent_class = NULL;

GType
ags_wave_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_window = 0;

    static const GTypeInfo ags_wave_window_info = {
      sizeof (AgsWaveWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_window = g_type_register_static(GTK_TYPE_WINDOW,
						  "AgsWaveWindow", &ags_wave_window_info,
						  0);
    
    g_type_add_interface_static(ags_type_wave_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_window);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_window_class_init(AgsWaveWindowClass *wave_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_wave_window_parent_class = g_type_class_peek_parent(wave_window);

  /* GObjectClass */
  gobject = (GObjectClass *) wave_window;

  gobject->set_property = ags_wave_window_set_property;
  gobject->get_property = ags_wave_window_get_property;

  gobject->finalize = ags_wave_window_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) wave_window;

  widget->delete_event = ags_wave_window_delete_event;
}

void
ags_wave_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_window_connect;
  connectable->disconnect = ags_wave_window_disconnect;
}

void
ags_wave_window_init(AgsWaveWindow *wave_window)
{
  g_object_set(G_OBJECT(wave_window),
	       "title", i18n("edit wave"),
	       NULL);

  wave_window->flags = 0;

  wave_window->wave_editor = ags_wave_editor_new();
  gtk_container_add((GtkContainer *) wave_window,
		    (GtkWidget *) wave_window->wave_editor);		    
}

void
ags_wave_window_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveWindow *wave_window;

  wave_window = AGS_WAVE_WINDOW(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_window_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveWindow *wave_window;

  wave_window = AGS_WAVE_WINDOW(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_window_connect(AgsConnectable *connectable)
{
  AgsWaveWindow *wave_window;

  wave_window = AGS_WAVE_WINDOW(connectable);

  if((AGS_WAVE_WINDOW_CONNECTED & (wave_window->flags)) != 0){
    return;
  }

  wave_window->flags |= AGS_WAVE_WINDOW_CONNECTED;
  
  if(wave_window->wave_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(wave_window->wave_editor));
  }
}

void
ags_wave_window_disconnect(AgsConnectable *connectable)
{
  AgsWaveWindow *wave_window;

  wave_window = AGS_WAVE_WINDOW(connectable);

  if((AGS_WAVE_WINDOW_CONNECTED & (wave_window->flags)) == 0){
    return;
  }

  wave_window->flags &= (~AGS_WAVE_WINDOW_CONNECTED);

  if(wave_window->wave_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(wave_window->wave_editor));
  }
}

void
ags_wave_window_finalize(GObject *gobject)
{
  AgsWaveWindow *wave_window;

  wave_window = (AgsWaveWindow *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_wave_window_parent_class)->finalize(gobject);
}

gboolean
ags_wave_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

/**
 * ags_wave_window_new:
 * @parent_window: the parent #AgsWindow
 * 
 * Instantiate #AgsWaveWindow
 * 
 * Returns: the new #AgsWaveWindow instance
 * 
 * Since: 3.0.0
 */
AgsWaveWindow*
ags_wave_window_new(GtkWidget *parent_window)
{
  AgsWaveWindow *wave_window;

  wave_window = (AgsWaveWindow *) g_object_new(AGS_TYPE_WAVE_WINDOW,
					       NULL);
  wave_window->parent_window = parent_window;
  
  return(wave_window);
}
