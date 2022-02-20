/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/app/ags_sheet_window.h>
#include <ags/app/ags_sheet_window_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_sheet_window_class_init(AgsSheetWindowClass *sheet_window);
void ags_sheet_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_window_init(AgsSheetWindow *sheet_window);
void ags_sheet_window_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_sheet_window_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_sheet_window_finalize(GObject *gobject);
void ags_sheet_window_connect(AgsConnectable *connectable);
void ags_sheet_window_disconnect(AgsConnectable *connectable);
gboolean ags_sheet_window_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_sheet_window
 * @short_description: The sheet window
 * @title: AgsSheetWindow
 * @section_id:
 * @include: ags/app/ags_sheet_window.h
 *
 * #AgsSheetWindow lets you edit audio files.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_sheet_window_parent_class = NULL;

GType
ags_sheet_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_window = 0;

    static const GTypeInfo ags_sheet_window_info = {
      sizeof (AgsSheetWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_window = g_type_register_static(GTK_TYPE_WINDOW,
						   "AgsSheetWindow", &ags_sheet_window_info,
						   0);
    
    g_type_add_interface_static(ags_type_sheet_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_window);
  }

  return g_define_type_id__volatile;
}

void
ags_sheet_window_class_init(AgsSheetWindowClass *sheet_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_sheet_window_parent_class = g_type_class_peek_parent(sheet_window);

  /* GObjectClass */
  gobject = (GObjectClass *) sheet_window;

  gobject->set_property = ags_sheet_window_set_property;
  gobject->get_property = ags_sheet_window_get_property;

  gobject->finalize = ags_sheet_window_finalize;

  /**
   * AgsSheetWindow:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 3.0.0
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
  widget = (GtkWidgetClass *) sheet_window;

  widget->delete_event = ags_sheet_window_delete_event;
}

void
ags_sheet_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_window_connect;
  connectable->disconnect = ags_sheet_window_disconnect;
}

void
ags_sheet_window_init(AgsSheetWindow *sheet_window)
{
  g_object_set(G_OBJECT(sheet_window),
	       "title", "edit sheet",
	       NULL);

  sheet_window->flags = 0;

  sheet_window->soundcard = NULL;
  
  sheet_window->sheet_editor = ags_sheet_editor_new();
  gtk_container_add((GtkContainer *) sheet_window,
		    (GtkWidget *) sheet_window->sheet_editor);		    
}

void
ags_sheet_window_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsSheetWindow *sheet_window;

  sheet_window = AGS_SHEET_WINDOW(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
  {
    GObject *soundcard;

    soundcard = g_value_get_object(value);

    if(sheet_window->soundcard == soundcard){
      return;
    }

    if(sheet_window->soundcard != NULL){
      g_object_unref(sheet_window->soundcard);
    }
      
    if(soundcard != NULL){
      g_object_ref(soundcard);
    }
      
    sheet_window->soundcard = soundcard;

    g_object_set(G_OBJECT(sheet_window->sheet_editor),
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
ags_sheet_window_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsSheetWindow *sheet_window;

  sheet_window = AGS_SHEET_WINDOW(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    g_value_set_object(value, sheet_window->soundcard);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sheet_window_connect(AgsConnectable *connectable)
{
  AgsSheetWindow *sheet_window;

  sheet_window = AGS_SHEET_WINDOW(connectable);

  if((AGS_SHEET_WINDOW_CONNECTED & (sheet_window->flags)) != 0){
    return;
  }

  sheet_window->flags |= AGS_SHEET_WINDOW_CONNECTED;
  
  if(sheet_window->sheet_editor != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(sheet_window->sheet_editor));
  }
}

void
ags_sheet_window_disconnect(AgsConnectable *connectable)
{
  AgsSheetWindow *sheet_window;

  sheet_window = AGS_SHEET_WINDOW(connectable);

  if((AGS_SHEET_WINDOW_CONNECTED & (sheet_window->flags)) == 0){
    return;
  }

  sheet_window->flags &= (~AGS_SHEET_WINDOW_CONNECTED);

  if(sheet_window->sheet_editor != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(sheet_window->sheet_editor));
  }
}

void
ags_sheet_window_finalize(GObject *gobject)
{
  AgsSheetWindow *sheet_window;

  sheet_window = (AgsSheetWindow *) gobject;

  if(sheet_window->soundcard != NULL){
    g_object_unref(G_OBJECT(sheet_window->soundcard));
  }
  
  G_OBJECT_CLASS(ags_sheet_window_parent_class)->finalize(gobject);
}

gboolean
ags_sheet_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

/**
 * ags_sheet_window_new:
 * @parent_window: the parent #AgsWindow
 * 
 * Instantiate #AgsSheetWindow
 * 
 * Returns: the new #AgsSheetWindow instance
 * 
 * Since: 3.0.0
 */
AgsSheetWindow*
ags_sheet_window_new(GtkWidget *parent_window)
{
  AgsSheetWindow *sheet_window;

  sheet_window = (AgsSheetWindow *) g_object_new(AGS_TYPE_SHEET_WINDOW,
						 NULL);
  sheet_window->parent_window = parent_window;
  
  return(sheet_window);
}
