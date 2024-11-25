/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_download_window.h>
#include <ags/app/ags_download_window_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include "config.h"

#if defined(AGS_WITH_VTE)
#include <pty.h>
#include <vte/vte.h>
#endif

#include <stdlib.h>
#include <ags/i18n.h>

void ags_download_window_class_init(AgsDownloadWindowClass *download_window);
void ags_download_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_download_window_init(AgsDownloadWindow *download_window);
void ags_download_window_finalize(GObject *gobject);

gboolean ags_download_window_is_connected(AgsConnectable *connectable);
void ags_download_window_connect(AgsConnectable *connectable);
void ags_download_window_disconnect(AgsConnectable *connectable);

void ags_download_window_activate_button_callback(GtkButton *activate_button,
						  AgsDownloadWindow *download_window);

gboolean ags_download_window_key_pressed_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsDownloadWindow *download_window);
void ags_download_window_key_released_callback(GtkEventControllerKey *event_controller,
					       guint keyval,
					       guint keycode,
					       GdkModifierType state,
					       AgsDownloadWindow *download_window);
gboolean ags_download_window_modifiers_callback(GtkEventControllerKey *event_controller,
						GdkModifierType keyval,
						AgsDownloadWindow *download_window);

/**
 * SECTION:ags_download_window
 * @short_description: The download dialog
 * @title: AgsDownloadWindow
 * @section_id:
 * @include: ags/app/ags_download_window.h
 *
 * #AgsDownloadWindow lets you download to audio files. This widget
 * can contain mulitple #AgsDownloadSoundcard widgets.
 */

static gpointer ags_download_window_parent_class = NULL;

GType
ags_download_window_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_download_window = 0;

    static const GTypeInfo ags_download_window_info = {
      sizeof (AgsDownloadWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_download_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDownloadWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_download_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_download_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_download_window = g_type_register_static(GTK_TYPE_WINDOW,
						      "AgsDownloadWindow", &ags_download_window_info,
						      0);
    
    g_type_add_interface_static(ags_type_download_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_download_window);
  }

  return g_define_type_id__volatile;
}

void
ags_download_window_class_init(AgsDownloadWindowClass *download_window)
{
  GObjectClass *gobject;

  ags_download_window_parent_class = g_type_class_peek_parent(download_window);

  /* GObjectClass */
  gobject = (GObjectClass *) download_window;

  gobject->finalize = ags_download_window_finalize;
}

void
ags_download_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_download_window_is_connected;
  connectable->connect = ags_download_window_connect;
  connectable->disconnect = ags_download_window_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_download_window_init(AgsDownloadWindow *download_window)
{
  GtkBox *vbox;

#if defined(AGS_WITH_VTE)
  VteTerminal *terminal;
  VtePty *pty;
#else
  gpointer terminal;
  gpointer pty;
#endif
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  download_window->flags = 0;
  download_window->connectable_flags = 0;

  ags_ui_provider_set_download_window(AGS_UI_PROVIDER(application_context),
				      (GtkWidget *) download_window);
  
  g_object_set(download_window,
	       "title", i18n("download audio samples"),
	       NULL);

  gtk_window_set_default_size((GtkWindow *) download_window,
			      800, 600);

  gtk_window_set_hide_on_close((GtkWindow *) download_window,
			       TRUE);

  /*  */  
  g_signal_connect_after(download_window, "close-request",
			 G_CALLBACK(ags_download_window_close_request_callback), NULL);

  event_controller = gtk_event_controller_key_new();

  gtk_widget_add_controller((GtkWidget *) download_window,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_download_window_key_pressed_callback), download_window);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_download_window_key_released_callback), download_window);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_download_window_modifiers_callback), download_window);
  
  /* pack */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) download_window,
		       (GtkWidget *) vbox);

  /*  */
  download_window->download_script = g_strdup(AGS_DOWNLOAD_WINDOW_DEFAULT_DOWNLOAD_SCRIPT);

#if defined(AGS_WITH_VTE)
  terminal = (VteTerminal *) vte_terminal_new();
#else
  terminal = NULL;
#endif
  
  download_window->terminal = (GtkWidget *) terminal;
  
#if defined(AGS_WITH_VTE)
   /* infinite scrollback */
  vte_terminal_set_scrollback_lines(VTE_TERMINAL (terminal),
				    -1);
  
  gtk_widget_set_valign((GtkWidget *) terminal,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) terminal,
			GTK_ALIGN_FILL);
  
  gtk_widget_set_vexpand((GtkWidget *) terminal,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) terminal,
			 TRUE);
  
  gtk_box_append(vbox,
		 (GtkWidget *) terminal);
#endif

  /* button */
  download_window->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("download"));
  gtk_widget_set_halign((GtkWidget *) download_window->activate_button,
			GTK_ALIGN_END);
  gtk_box_append(vbox,
		 (GtkWidget *) download_window->activate_button);

  g_signal_connect(download_window->activate_button, "clicked",
		   G_CALLBACK(ags_download_window_activate_button_callback), download_window);
}

gboolean
ags_download_window_is_connected(AgsConnectable *connectable)
{
  AgsDownloadWindow *download_window;
  
  gboolean is_connected;
  
  download_window = AGS_DOWNLOAD_WINDOW(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (download_window->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_download_window_connect(AgsConnectable *connectable)
{
  AgsDownloadWindow *download_window;

  download_window = AGS_DOWNLOAD_WINDOW(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  download_window->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_download_window_disconnect(AgsConnectable *connectable)
{
  AgsDownloadWindow *download_window;

  download_window = AGS_DOWNLOAD_WINDOW(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  download_window->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_download_window_finalize(GObject *gobject)
{
  AgsDownloadWindow *download_window;

  download_window = (AgsDownloadWindow *) gobject;
  
  G_OBJECT_CLASS(ags_download_window_parent_class)->finalize(gobject);
}

void
ags_download_window_activate_button_callback(GtkButton *activate_button,
					     AgsDownloadWindow *download_window)
{
  char* argv[2] = {
    AGS_DOWNLOAD_WINDOW_DEFAULT_DOWNLOAD_SCRIPT,
    NULL
  };

  GError *error;

#if defined(AGS_WITH_VTE)
  vte_terminal_spawn_async(VTE_TERMINAL(download_window->terminal),
			   VTE_PTY_NO_HELPER,
			   NULL,
			   argv,
			   NULL,
			   G_SPAWN_SEARCH_PATH,
			   NULL,
			   NULL,
			   NULL,
			   2000,
			   NULL,
			   NULL,
			   NULL);
#endif
}

gboolean
ags_download_window_key_pressed_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsDownloadWindow *download_window)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_download_window_key_released_callback(GtkEventControllerKey *event_controller,
					  guint keyval,
					  guint keycode,
					  GdkModifierType state,
					  AgsDownloadWindow *download_window)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_window_close((GtkWindow *) download_window);	
      }
      break;
    }
  }
}

gboolean
ags_download_window_modifiers_callback(GtkEventControllerKey *event_controller,
				       GdkModifierType keyval,
				       AgsDownloadWindow *download_window)
{
  return(FALSE);
}

/**
 * ags_download_window_new:
 * @transient_for: the transient for #GtkWindow
 * 
 * Create a new instance of #AgsDownloadWindow
 * 
 * Returns: the #AgsDownloadWindow
 * 
 * Since: 7.3.0
 */
AgsDownloadWindow*
ags_download_window_new(GtkWindow *transient_for)
{
  AgsDownloadWindow *download_window;

  download_window = (AgsDownloadWindow *) g_object_new(AGS_TYPE_DOWNLOAD_WINDOW,
						       "transient-for", transient_for,
						       NULL);

  return(download_window);
}
