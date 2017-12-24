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

#include <ags/X/ags_window.h>
#include <ags/X/ags_window_callbacks.h>

#include <ags/libags.h>

#include <ags/X/ags_xorg_application_context.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_syncsynth.h>
#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>

#include <ags/X/file/ags_simple_file.h>

#include <ags/X/thread/ags_gui_thread.h>

#ifdef AGS_WITH_QUARTZ
#include <gtkmacintegration-gtk2/gtkosxapplication.h>
#endif

#include <stdlib.h>

#include <ags/i18n.h>

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

/**
 * SECTION:ags_window
 * @short_description: The window object.
 * @title: AgsWindow
 * @section_id:
 * @include: ags/X/ags_window.h
 *
 * #AgsWindow is a composite toplevel widget. It contains the
 * menubar, the machine rack and the notation editor.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_window_parent_class = NULL;
GHashTable *ags_window_load_file = NULL;

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
					     "AgsWindow", &ags_window_info,
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
  /**
   * AgsWindow:soundcard:
   *
   * The assigned main AgsSoundcard.
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

  /**
   * AgsWindow:application-context:
   *
   * The assigned application context.
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
  widget = (GtkWidgetClass *) window;

  widget->show = ags_window_show;
  widget->delete_event = ags_window_delete_event;
}

void
ags_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_window_connect;
  connectable->disconnect = ags_window_disconnect;
}

void
ags_window_init(AgsWindow *window)
{
  GtkVBox *vbox;
  GtkWidget *scrolled_window;

  gchar *str;
  
  GError *error;

  window->flags = 0;

  error = NULL;

  str = g_strdup_printf("%s%s", DESTDIR, "/gsequencer/icons/jumper.png");
  g_object_set(G_OBJECT(window),
  	       "icon", gdk_pixbuf_new_from_file(str, &error),
  	       NULL);
  g_free(str);

  window->application_context = NULL;
  window->application_mutex = NULL;
  
  window->soundcard = NULL;

  /* window name and title */
  window->filename = NULL;

  window->name = g_strdup("unnamed");

  str = g_strconcat("GSequencer - ",
		    window->name,
		    NULL);
  gtk_window_set_title((GtkWindow *) window,
		       str);

  g_free(str);

  /* vbox */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) window, (GtkWidget*) vbox);

  /* menubar */
  window->context_menu = ags_context_menu_new();
  gtk_widget_set_events(GTK_WIDGET(window), 
			(GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK));
  gtk_widget_show_all(window->context_menu);

  g_signal_connect((GObject *) window, "button-press-event",
		   G_CALLBACK(ags_window_button_press_event), (gpointer) window);
  
  window->menu_bar = ags_menu_bar_new();
  gtk_box_pack_start((GtkBox *) vbox,
  		     (GtkWidget *) window->menu_bar,
  		     FALSE, FALSE, 0);

  /* vpaned and scrolled window */
  window->paned = (GtkVPaned *) gtk_vpaned_new();
  gtk_box_pack_start((GtkBox*) vbox, (GtkWidget*) window->paned, TRUE, TRUE, 0);

  scrolled_window = (GtkWidget *) gtk_scrolled_window_new(NULL, NULL);
  gtk_paned_pack1((GtkPaned *) window->paned,
		  scrolled_window,
		  TRUE, TRUE);

  /* machines rack */
  window->machines = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport((GtkScrolledWindow *) scrolled_window,
					(GtkWidget *) window->machines);

  window->machine_counter = ags_window_standard_machine_counter_alloc();
  window->selected = NULL;

  /* editor */
  window->notation_editor = g_object_new(AGS_TYPE_NOTATION_EDITOR,
					 "homogeneous", FALSE,
					 "spacing", 0,
					 NULL);
  gtk_paned_pack2((GtkPaned *) window->paned,
		  (GtkWidget *) window->notation_editor,
		  TRUE, TRUE);

  /* navigation */
  window->navigation = g_object_new(AGS_TYPE_NAVIGATION,
				    "homogeneous", FALSE,
				    "spacing", 0,
				    NULL);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) window->navigation,
		     FALSE, FALSE, 0);

  /* windows and dialogs */
  window->dialog = NULL;

  window->automation_window = ags_automation_window_new((GtkWidget *) window);

  window->export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW,
							   "main-window", window,
							   NULL);

  window->midi_import_wizard = NULL;
  window->midi_export_wizard = NULL;
  window->midi_file_chooser = NULL;
  
  window->preferences = NULL;
  window->history_browser = NULL;

  /* load file */
  if(ags_window_load_file == NULL){
    ags_window_load_file = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						 NULL,
						 NULL);
  }

  g_hash_table_insert(ags_window_load_file,
		      window, ags_window_load_file_timeout);

  g_timeout_add(1000, (GSourceFunc) ags_window_load_file_timeout, (gpointer) window);
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
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(window->soundcard == soundcard){
	return;
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      window->soundcard = soundcard;

      g_object_set(G_OBJECT(window->automation_window),
		   "soundcard", soundcard,
		   NULL);

      g_object_set(G_OBJECT(window->notation_editor),
		   "soundcard", soundcard,
		   NULL);

      g_object_set(G_OBJECT(window->navigation),
		   "soundcard", soundcard,
		   NULL);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) window->application_context == application_context){
	return;
      }

      if(window->application_context != NULL){
	window->application_mutex = NULL;
	g_object_unref(window->application_context);
      }

      if(application_context != NULL){
	window->application_mutex = application_context->mutex;
	
	g_object_ref(application_context);
      }

      window->application_context = (GObject *) application_context;

      g_object_set(G_OBJECT(window->export_window),
		   "application-context", application_context,
		   NULL);
      ags_export_window_reload_soundcard_editor(window->export_window);
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
  case PROP_SOUNDCARD:
    g_value_set_object(value, window->soundcard);
    break;
  case PROP_APPLICATION_CONTEXT:
    g_value_set_object(value, window->application_context);
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

  GList *list, *list_start;

  window = AGS_WINDOW(connectable);

  if((AGS_WINDOW_CONNECTED & (window->flags)) != 0){
    return;
  }

  window->flags |= AGS_WINDOW_CONNECTED;
  
  g_signal_connect(G_OBJECT(window), "delete_event",
		   G_CALLBACK(ags_window_delete_event_callback), NULL);

  ags_connectable_connect(AGS_CONNECTABLE(window->context_menu));
  ags_connectable_connect(AGS_CONNECTABLE(window->menu_bar));

  list_start = 
    list = gtk_container_get_children((GtkContainer *) window->machines);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_connectable_connect(AGS_CONNECTABLE(window->notation_editor));
  ags_connectable_connect(AGS_CONNECTABLE(window->navigation));

  ags_connectable_connect(AGS_CONNECTABLE(window->automation_window));

  ags_connectable_connect(AGS_CONNECTABLE(window->export_window));
}

void
ags_window_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;

  GList *list, *list_start;

  window = AGS_WINDOW(connectable);

  if((AGS_WINDOW_CONNECTED & (window->flags)) == 0){
    return;
  }

  window->flags &= (~AGS_WINDOW_CONNECTED);
  
  g_object_disconnect(window,
		      "any_signal::delete_event",
		      G_CALLBACK(ags_window_delete_event_callback),
		      NULL,
		      NULL);

  ags_connectable_disconnect(AGS_CONNECTABLE(window->context_menu));
  ags_connectable_disconnect(AGS_CONNECTABLE(window->menu_bar));

  list_start = 
    list = gtk_container_get_children((GtkContainer *) window->machines);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
  
  ags_connectable_disconnect(AGS_CONNECTABLE(window->notation_editor));
  ags_connectable_disconnect(AGS_CONNECTABLE(window->navigation));

  ags_connectable_disconnect(AGS_CONNECTABLE(window->automation_window));

  ags_connectable_disconnect(AGS_CONNECTABLE(window->export_window));
}

void
ags_window_finalize(GObject *gobject)
{
  AgsWindow *window;

  window = (AgsWindow *) gobject;

  /* remove message monitor */
  g_hash_table_remove(ags_window_load_file,
		      window);

  g_object_unref(G_OBJECT(window->soundcard));
  g_object_unref(G_OBJECT(window->export_window));

  if(window->name != NULL){
    free(window->name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_window_parent_class)->finalize(gobject);
}

void
ags_window_show(GtkWidget *widget)
{
  AgsWindow *window;

#ifdef AGS_WITH_QUARTZ
  GtkosxApplication *app;

  GList *list;
#endif

  window = (AgsWindow *) widget;

  GTK_WIDGET_CLASS(ags_window_parent_class)->show(widget);

#ifndef AGS_WITH_QUARTZ
  gtk_widget_show((GtkWidget *) window->menu_bar);
#else
  app = gtkosx_application_get();

  gtk_widget_hide((GtkWidget *) window->menu_bar);
  gtkosx_application_set_menu_bar(app,
				  window->menu_bar);

  list = gtk_container_get_children(window->menu_bar);

  while(list != NULL){
    gtk_widget_show(list->data);

    list = list->next;
  }

  gtkosx_application_sync_menubar(app);
  gtkosx_application_ready(app);
#endif
}

gboolean
ags_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_destroy(widget);

  GTK_WIDGET_CLASS(ags_window_parent_class)->delete_event(widget, event);

  return(FALSE);
}

/**
 * ags_window_standard_machine_counter_alloc:
 *
 * Keep track of count of machines. Allocates a #GList of well
 * known machines.
 * 
 * Returns: a new #GList containing #AgsMachineCounter for know machines
 * 
 * Since: 1.0.0
 */
GList*
ags_window_standard_machine_counter_alloc()
{
  GList *machine_counter = NULL;

  machine_counter = NULL;

  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_PANEL, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_MIXER, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_DRUM, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_MATRIX, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_SYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_SYNCSYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_FFPLAYER, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LADSPA_BRIDGE, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_DSSI_BRIDGE, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LV2_BRIDGE, 0));
  
  return(machine_counter);
}

/**
 * ags_window_find_machine_counter:
 * @window: the #AgsWindow
 * @machine_type: the machine type
 *
 * Keep track of count of machines. Lookup window's counter.
 * 
 * Returns: an #AgsMachineCounter
 * 
 * Since: 1.0.0
 */
AgsMachineCounter*
ags_window_find_machine_counter(AgsWindow *window,
				GType machine_type)
{
  GList *list;

  list = window->machine_counter;

  while(list != NULL){
    if(AGS_MACHINE_COUNTER(list->data)->machine_type == machine_type){
      return(AGS_MACHINE_COUNTER(list->data));
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_window_increment_machine_counter:
 * @window: the #AgsWindow
 * @machine_type: the machine type
 *
 * Keep track of count of machines. Increment window's counter.
 * 
 * Since: 1.0.0
 */
void
ags_window_increment_machine_counter(AgsWindow *window,
				     GType machine_type)
{
  AgsMachineCounter *machine_counter;

  machine_counter = ags_window_find_machine_counter(window,
						    machine_type);

  if(machine_counter != NULL){
    machine_counter->counter++;
  }
}

/**
 * ags_window_decrement_machine_counter:
 * @window: the #AgsWindow
 * @machine_type: the machine type
 *
 * Keep track of count of machines. Decrement window's counter.
 * 
 * Since: 1.0.0
 */
void
ags_window_decrement_machine_counter(AgsWindow *window,
				     GType machine_type)
{
  AgsMachineCounter *machine_counter;

  machine_counter = ags_window_find_machine_counter(window,
						    machine_type);

  if(machine_counter != NULL){
    machine_counter->counter--;
  }
}

/**
 * ags_machine_counter_alloc:
 * @version: the machine's version
 * @build_id: the machine's build id
 * @machine_type: the machine type
 * @initial_value: initialize counter
 *
 * Keep track of count of machines.
 * 
 * Returns: an #AgsMachineCounter
 * 
 * Since: 1.0.0
 */
AgsMachineCounter*
ags_machine_counter_alloc(gchar *version, gchar *build_id,
			  GType machine_type, guint initial_value)
{
  AgsMachineCounter *machine_counter;

  machine_counter = (AgsMachineCounter *) malloc(sizeof(AgsMachineCounter));

  machine_counter->version = version;
  machine_counter->build_id = build_id;

  machine_counter->machine_type = machine_type;
  machine_counter->filename = NULL;
  machine_counter->effect = NULL;
  machine_counter->counter = initial_value;

  return(machine_counter);
}

void
ags_window_show_error(AgsWindow *window,
		      gchar *message)
{
  GtkDialog *dialog;

  dialog = (GtkDialog *) gtk_message_dialog_new((GtkWindow *) window,
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_OK,
						"%s", message);
  gtk_widget_show_all((GtkWidget *) dialog);
}

/**
 * ags_window_load_file_timeout:
 * @window: the #AgsWindow
 * 
 * Load file.
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 * 
 * Since: 1.2.0
 */
gboolean
ags_window_load_file_timeout(AgsWindow *window)
{
  AgsGuiThread *gui_thread;

  gui_thread = NULL;
  
  if(window->application_context != NULL){
    gui_thread = AGS_XORG_APPLICATION_CONTEXT(window->application_context)->gui_thread;
  }
  
  if(gui_thread != NULL &&
     gui_thread->gtk_thread == NULL){
      gui_thread->gtk_thread = g_thread_self();
  }

  if((AGS_WINDOW_TERMINATING & (window->flags)) != 0){
    ags_application_context_quit(window->application_context);

    return(FALSE);
  }

  if(g_hash_table_lookup(ags_window_load_file,
			 window) != NULL){
    if(window->filename != NULL){
      AgsSimpleFile *simple_file;
      
      gchar *str;

      GError *error;

      simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						   "application-context", window->application_context,
						   "filename", window->filename,
						   NULL);
      
      error = NULL;
      ags_simple_file_open(simple_file,
			   &error);

      ags_simple_file_read(simple_file);
      ags_simple_file_close(simple_file);
      
      /* set name */
      window->name = g_strdup(window->filename);

      str = g_strconcat("GSequencer - ",
			window->name,
			NULL);
      
      gtk_window_set_title((GtkWindow *) window,
			   str);
      
      window->filename = NULL;
    }

    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_window_new:
 * @application_context: the application object.
 *
 * Creates an #AgsWindow
 *
 * Returns: a new #AgsWindow
 *
 * Since: 1.0.0
 */
AgsWindow*
ags_window_new(GObject *application_context)
{
  AgsWindow *window;

  window = (AgsWindow *) g_object_new(AGS_TYPE_WINDOW,
				      "application-context", application_context,
				      NULL);

  return(window);
}
