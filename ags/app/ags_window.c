/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_window.h>
#include <ags/app/ags_window_callbacks.h>

#include "config.h"

#include <ags/app/ags_ui_provider.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>
#include <ags/app/machine/ags_audiorec.h>
#include <ags/app/machine/ags_desk.h>
#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_live_dssi_bridge.h>
#include <ags/app/machine/ags_live_lv2_bridge.h>

#if defined(AGS_WITH_VST3)
#include <ags/app/machine/ags_vst3_bridge.h>
#include <ags/app/machine/ags_live_vst3_bridge.h>
#endif

#include <ags/app/file/ags_simple_file.h>

#include <stdlib.h>

#define _GNU_SOURCE
#include <locale.h>

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
 * @include: ags/app/ags_window.h
 *
 * #AgsWindow is a composite toplevel widget. It contains the
 * menubar, the machine rack and the notation editor.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_window_parent_class = NULL;
GHashTable *ags_window_load_file = NULL;

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_utf8_locale;
#endif

static gboolean locale_initialized = FALSE;

GType
ags_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_window = 0;

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

    ags_type_window = g_type_register_static(GTK_TYPE_APPLICATION_WINDOW,
					     "AgsWindow", &ags_window_info,
					     0);
    
    g_type_add_interface_static(ags_type_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_window);
  }

  return g_define_type_id__volatile;
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
  GtkMenuButton *menu_button;
  GtkBox *vbox;
  GtkViewport *viewport;
  GtkWidget *scrolled_window;

  GtkBuilder *builder;
  GtkSettings *settings;

  GApplication *gsequencer_app;
  GMenu *menu;

  AgsApplicationContext *application_context;

  gchar *window_title;
  gchar *app_icon;

  gboolean use_composite_editor;
  
  GError *error;

  application_context = ags_application_context_get_instance();

  settings = gtk_settings_get_default();
  
  window->flags = 0;

  g_signal_connect(application_context, "setup-completed",
		   G_CALLBACK(ags_window_setup_completed_callback), window);

#if defined(AGS_WINDOW_APP_ICON)
  app_icon = g_strdup(AGS_WINDOW_APP_ICON);
#else
  if((app_icon = getenv("AGS_WINDOW_APP_ICON")) != NULL){
    app_icon = g_strdup(app_icon);
  }else{
    app_icon = g_strdup_printf("%s%s",
			       AGS_DATA_DIR,
			       "/icons/hicolor/128x128/apps/gsequencer.png");
  }
#endif

  gsequencer_app = ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context));
  
  error = NULL;  
  g_object_set(G_OBJECT(window),
  	       "icon", gdk_pixbuf_new_from_file(app_icon, &error),
  	       NULL);
  g_free(app_icon);

  if(error != NULL){
    g_error_free(error);
  }

  /* window header bar */
  window->no_config = FALSE;

  window->shows_menu_bar = TRUE;
  
  g_object_get(settings,
	       "gtk-shell-shows-menubar", &(window->shows_menu_bar),
	       NULL);
  
  window->filename = NULL;

  window->name = g_strdup("unnamed");

  window_title = g_strdup_printf("GSequencer - %s", window->name);
  gtk_window_set_title(window,
		       window_title);

  g_free(window_title);
  
  window->header_bar = gtk_header_bar_new();

  if(!window->shows_menu_bar){
    gtk_window_set_titlebar((GtkWindow *) window,
			    (GtkWidget *) window->header_bar);
  }

  gtk_header_bar_set_decoration_layout(window->header_bar,
				       "menu:minimize,maximize,close");
  gtk_header_bar_set_title(window->header_bar,
			   "GSequencer");
  gtk_header_bar_set_subtitle(window->header_bar,
			      window->name);
  
  /* app menu buttton */
  window->app_button = gtk_menu_button_new();
  g_object_set(window->app_button,
	       "direction", GTK_ARROW_NONE,
	       NULL);
  gtk_header_bar_pack_end(window->header_bar,
			  window->app_button);
  
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_primary_menu.ui");

  menu = gtk_builder_get_object(builder,
				"ags-primary-menu");
  gtk_menu_button_set_menu_model(window->app_button,
				 menu);

  /* add menu button */
  window->add_button = gtk_menu_button_new();
  gtk_container_add(window->add_button,
		    gtk_image_new_from_icon_name("list-add-symbolic",
						 GTK_ICON_SIZE_BUTTON));
  gtk_header_bar_pack_end(window->header_bar,
			  window->add_button);  

  /* app edit buttton */
  window->edit_button = gtk_menu_button_new();
  gtk_container_add(window->edit_button,
		    gtk_image_new_from_icon_name("document-edit-symbolic",
						 GTK_ICON_SIZE_BUTTON));
  gtk_header_bar_pack_end(window->header_bar,
			  window->edit_button);
  
  builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_edit_menu.ui");

  menu = gtk_builder_get_object(builder,
				"ags-edit-menu");
  gtk_menu_button_set_menu_model(window->edit_button,
				 menu);

  /* vbox */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) window, (GtkWidget*) vbox);
  
  /* menubar */
  window->context_menu = ags_context_menu_new();
  gtk_widget_set_events(GTK_WIDGET(window), 
			(GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK));
  gtk_widget_show_all(GTK_WIDGET(window->context_menu));

  g_signal_connect((GObject *) window, "button-press-event",
		   G_CALLBACK(ags_window_button_press_event), (gpointer) window);
  
  window->menu_bar = ags_menu_bar_new();
  gtk_box_pack_start((GtkBox *) vbox,
  		     (GtkWidget *) window->menu_bar,
  		     FALSE, FALSE, 0);

  //FIXME:JK: the header bar is actually not working properly on Xorg
#if 0
  if(window->shows_menu_bar){
    gtk_widget_show_all((GtkWidget *) window->menu_bar);
  }
#else
  gtk_widget_show_all((GtkWidget *) window->menu_bar);
#endif
  
  /* vpaned and scrolled window */
  window->paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_start((GtkBox*) vbox,
		     (GtkWidget*) window->paned,
		     TRUE, TRUE,
		     0);

  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack1((GtkPaned *) window->paned,
		  (GtkWidget *) viewport,
		  TRUE, TRUE);
  
  scrolled_window = (GtkWidget *) gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add((GtkContainer *) viewport,
		    (GtkWidget *) scrolled_window);

  /* machines rack */
  window->machines = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					    0);
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) window->machines);

  window->machine_counter = ags_window_standard_machine_counter_alloc();
  window->selected = NULL;

  /* composite editor */
  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  viewport = (GtkViewport *) gtk_viewport_new(NULL,
					      NULL);
  g_object_set(viewport,
	       "shadow-type", GTK_SHADOW_NONE,
	       NULL);
  gtk_paned_pack2((GtkPaned *) window->paned,
		  (GtkWidget *) viewport,
		  TRUE, TRUE);

  window->composite_editor = ags_composite_editor_new();
  ags_ui_provider_set_composite_editor(AGS_UI_PROVIDER(application_context),
				       window->composite_editor);
  g_object_set(window->composite_editor,
	       "homogeneous", FALSE,
	       "spacing", 0,
	       NULL);
  gtk_container_add((GtkContainer *) viewport,
		    (GtkWidget *) window->composite_editor);

  /* editor */
  if(!use_composite_editor){
    viewport = (GtkViewport *) gtk_viewport_new(NULL,
						NULL);
    g_object_set(viewport,
		 "shadow-type", GTK_SHADOW_NONE,
		 NULL);
    gtk_paned_pack2((GtkPaned *) window->paned,
		    (GtkWidget *) viewport,
		    TRUE, TRUE);

    window->notation_editor = g_object_new(AGS_TYPE_NOTATION_EDITOR,
					   "homogeneous", FALSE,
					   "spacing", 0,
					   NULL);
    gtk_container_add((GtkContainer *) viewport,
		      (GtkWidget *) window->notation_editor);
  }else{
    window->notation_editor = NULL;
  }
  
    /* navigation */
  window->navigation = g_object_new(AGS_TYPE_NAVIGATION,
				    "homogeneous", FALSE,
				    "spacing", 0,
				    NULL);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) window->navigation,
		     FALSE, FALSE,
		     0);

  /* windows and dialogs */
  window->dialog = NULL;

  if(!use_composite_editor){
    window->automation_window = ags_automation_window_new((GtkWidget *) window);

    window->wave_window = ags_wave_window_new((GtkWidget *) window);
  }else{
    window->automation_window = NULL;
    window->wave_window = NULL;
  }
  
  window->export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW,
							   NULL);
  ags_export_window_add_export_soundcard(window->export_window);
  
  ags_ui_provider_set_export_window(AGS_UI_PROVIDER(application_context),
				    window->export_window);
  
  window->midi_import_wizard = NULL;
  window->midi_export_wizard = NULL;
  window->midi_file_chooser = NULL;
  
  window->preferences = NULL;
  
  /* load file */
  if(ags_window_load_file == NULL){
    ags_window_load_file = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						 NULL,
						 NULL);
  }

  g_hash_table_insert(ags_window_load_file,
		      window, ags_window_load_file_timeout);

  g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_window_load_file_timeout, (gpointer) window);
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
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_window_connect(AgsConnectable *connectable)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  gboolean use_composite_editor;

  window = AGS_WINDOW(connectable);

  if((AGS_WINDOW_CONNECTED & (window->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

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

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  if(use_composite_editor){
    ags_connectable_connect(AGS_CONNECTABLE(window->composite_editor));
  }
  
  if(!use_composite_editor){
    ags_connectable_connect(AGS_CONNECTABLE(window->notation_editor));
  }
  
  ags_connectable_connect(AGS_CONNECTABLE(window->navigation));

  if(!use_composite_editor){
    ags_connectable_connect(AGS_CONNECTABLE(window->automation_window));
    
    ags_connectable_connect(AGS_CONNECTABLE(window->wave_window));
  }
  
  ags_connectable_connect(AGS_CONNECTABLE(window->export_window));
}

void
ags_window_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  gboolean use_composite_editor;

  window = AGS_WINDOW(connectable);

  if((AGS_WINDOW_CONNECTED & (window->flags)) == 0){
    return;
  }

  application_context = ags_application_context_get_instance();

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

  use_composite_editor = ags_ui_provider_use_composite_editor(AGS_UI_PROVIDER(application_context));

  if(use_composite_editor){
    ags_connectable_disconnect(AGS_CONNECTABLE(window->composite_editor));
  }
  
  if(!use_composite_editor){
    ags_connectable_disconnect(AGS_CONNECTABLE(window->notation_editor));
  }
  
  ags_connectable_disconnect(AGS_CONNECTABLE(window->navigation));

  if(!use_composite_editor){
    ags_connectable_disconnect(AGS_CONNECTABLE(window->automation_window));
    
    ags_connectable_disconnect(AGS_CONNECTABLE(window->wave_window));
  }
  
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

  window = (AgsWindow *) widget;

  GTK_WIDGET_CLASS(ags_window_parent_class)->show(widget);
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
 * Since: 3.0.0
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
							     AGS_TYPE_SPECTROMETER, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_EQUALIZER10, 0));
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
							     AGS_TYPE_FM_SYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_SYNCSYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_FM_SYNCSYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_HYBRID_SYNTH, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_HYBRID_FM_SYNTH, 0));

#ifdef AGS_WITH_LIBINSTPATCH
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_FFPLAYER, 0));

  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_SF2_SYNTH, 0));
#endif

  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_PITCH_SAMPLER, 0));

  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_SFZ_SYNTH, 0));
  
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_AUDIOREC, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_DESK, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LADSPA_BRIDGE, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_DSSI_BRIDGE, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LV2_BRIDGE, 0));

#if defined(AGS_WITH_VST3)
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_VST3_BRIDGE, 0));
#endif
  
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LIVE_DSSI_BRIDGE, 0));
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LIVE_LV2_BRIDGE, 0));

#if defined(AGS_WITH_VST3)
  machine_counter = g_list_prepend(machine_counter,
				   ags_machine_counter_alloc(AGS_RECALL_DEFAULT_VERSION, AGS_RECALL_DEFAULT_BUILD_ID,
							     AGS_TYPE_LIVE_VST3_BRIDGE, 0));
#endif
  
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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

/**
 * ags_window_load_add_menu_ladspa:
 * @window: the #AgsWindow
 * @menu: the #GMenu
 * 
 * Load add menu LADSPA.
 * 
 * Since: 3.18.0
 */
void
ags_window_load_add_menu_ladspa(AgsWindow *window,
				GMenu *menu)
{
  GMenu *ladspa_menu;
  GMenuItem *ladspa_item;
  GMenuItem *item;

  AgsLadspaManager *ladspa_manager;

  GList *start_list, *list;

  GRecMutex *ladspa_manager_mutex;

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* ladspa sub-menu */
  ladspa_menu = g_menu_new();
  ladspa_item = g_menu_item_new("LADSPA",
				NULL);
  
  /* get plugin */
  g_rec_mutex_lock(ladspa_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(ladspa_manager->ladspa_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(ladspa_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    GVariantBuilder *builder;

    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);

    item = g_menu_item_new(effect,
			   "app.add_ladspa_bridge");

    builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
    g_variant_builder_add(builder, "s", filename);
    g_variant_builder_add(builder, "s", effect);

    g_menu_item_set_attribute_value(item,
				    "target",
				    g_variant_new("as", builder));
    
    g_menu_append_item(ladspa_menu,
		       item);

    g_variant_builder_unref(builder);
    
    /* iterate */
    list = list->next;
  }
  
  g_menu_item_set_submenu(ladspa_item,
			  G_MENU_MODEL(ladspa_menu));  

  g_menu_append_item(menu,
		     ladspa_item);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_window_load_add_menu_dssi:
 * @window: the #AgsWindow
 * @menu: the #GMenu
 * 
 * Load add menu DSSI.
 * 
 * Since: 3.18.0
 */
void
ags_window_load_add_menu_dssi(AgsWindow *window,
			      GMenu *menu)
{
  GMenu *dssi_menu;
  GMenuItem *dssi_item;
  GMenuItem *item;

  AgsDssiManager *dssi_manager;

  GList *start_list, *list;

  GRecMutex *dssi_manager_mutex;

  dssi_manager = ags_dssi_manager_get_instance();

  /* get dssi manager mutex */
  dssi_manager_mutex = AGS_DSSI_MANAGER_GET_OBJ_MUTEX(dssi_manager);

  /* dssi sub-menu */
  dssi_menu = g_menu_new();
  dssi_item = g_menu_item_new("DSSI",
			      NULL);

  /* get plugin */
  g_rec_mutex_lock(dssi_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(dssi_manager->dssi_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(dssi_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    GVariantBuilder *builder;

    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
    
    item = g_menu_item_new(effect,
			   "app.add_dssi_bridge");

    builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
    g_variant_builder_add(builder, "s", filename);
    g_variant_builder_add(builder, "s", effect);

    g_menu_item_set_attribute_value(item,
				    "target",
				    g_variant_new("as", builder));
    
    g_menu_append_item(dssi_menu,
		       item);

    g_variant_builder_unref(builder);
    
    /* iterate */
    list = list->next;
  }

  g_menu_item_set_submenu(dssi_item,
			  G_MENU_MODEL(dssi_menu));  

  g_menu_append_item(menu,
		     dssi_item);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_window_load_add_menu_lv2:
 * @window: the #AgsWindow
 * @menu: the #GMenu
 * 
 * Load add menu LV2.
 * 
 * Since: 3.18.0
 */
void
ags_window_load_add_menu_lv2(AgsWindow *window,
			     GMenu *menu)
{
  GMenu *lv2_menu;
  GMenuItem *lv2_item;
  GMenuItem *item;

  AgsLv2Manager *lv2_manager;

  guint length;
  guint i;

  GRecMutex *lv2_manager_mutex;

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* lv2 sub-menu */
  lv2_menu = g_menu_new();
  lv2_item = g_menu_item_new("LV2",
			      NULL);

  /* get plugin */
  g_rec_mutex_lock(lv2_manager_mutex);
  
  if(lv2_manager->quick_scan_plugin_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_plugin_filename);

    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = lv2_manager->quick_scan_plugin_filename[i];
      effect = lv2_manager->quick_scan_plugin_effect[i];
      
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	GVariantBuilder *builder;
    
	item = g_menu_item_new(effect,
			       "app.add_lv2_bridge");

	builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
	g_variant_builder_add(builder, "s", filename);
	g_variant_builder_add(builder, "s", effect);

	g_menu_item_set_attribute_value(item,
					"target",
					g_variant_new("as", builder));
    
	g_menu_append_item(lv2_menu,
			   item);

	g_variant_builder_unref(builder);
      }
    }
  }
  
  if(lv2_manager->quick_scan_instrument_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_instrument_filename);
  
    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = lv2_manager->quick_scan_instrument_filename[i];
      effect = lv2_manager->quick_scan_instrument_effect[i];
    
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	GVariantBuilder *builder;
    
	item = g_menu_item_new(effect,
			       "app.add_lv2_bridge");

	builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
	g_variant_builder_add(builder, "s", filename);
	g_variant_builder_add(builder, "s", effect);

	g_menu_item_set_attribute_value(item,
					"target",
					g_variant_new("as", builder));
    
	g_menu_append_item(lv2_menu,
			   item);

	g_variant_builder_unref(builder);
      }
    }
  }

  g_rec_mutex_unlock(lv2_manager_mutex);
  
  g_menu_item_set_submenu(lv2_item,
			  G_MENU_MODEL(lv2_menu));  

  g_menu_append_item(menu,
		     lv2_item);
}

/**
 * ags_window_load_add_menu_vst3:
 * @window: the #AgsWindow
 * @menu: the #GMenu
 * 
 * Load add menu VST3.
 * 
 * Since: 3.18.0
 */
void
ags_window_load_add_menu_vst3(AgsWindow *window,
			      GMenu *menu)
{
#if defined(AGS_WITH_VST3)
  GMenu *vst3_menu;
  GMenuItem *vst3_item;
  GMenuItem *item;

  AgsVst3Manager *vst3_manager;

  GList *start_list, *list;

  GRecMutex *vst3_manager_mutex;

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* vst3 sub-menu */
  vst3_menu = g_menu_new();
  vst3_item = g_menu_item_new("VST3",
			      NULL);
  
  /* get plugin */
  g_rec_mutex_lock(vst3_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(vst3_manager->vst3_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(vst3_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    GVariantBuilder *builder;

    gchar *filename, *effect;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);

    item = g_menu_item_new(effect,
			   "app.add_vst3_bridge");

    builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
    
    g_variant_builder_add(builder, "s", filename);
    g_variant_builder_add(builder, "s", effect);

    g_menu_item_set_attribute_value(item,
				    "target",
				    g_variant_new("as", builder));
    
    g_menu_append_item(vst3_menu,
		       item);

    g_variant_builder_unref(builder);
    
    /* iterate */
    list = list->next;
  }
  
  g_menu_item_set_submenu(vst3_item,
			  G_MENU_MODEL(vst3_menu));  

  g_menu_append_item(menu,
		     vst3_item);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
#endif
}

void
ags_window_load_add_menu_live_dssi(AgsWindow *window,
				   GMenu *menu)
{
  //TODO:JK: implement me
}

void
ags_window_load_add_menu_live_lv2(AgsWindow *window,
				  GMenu *menu)
{
  //TODO:JK: implement me
}

void
ags_window_load_add_menu_live_vst3(AgsWindow *window,
				   GMenu *menu)
{
  //TODO:JK: implement me
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
 * Since: 3.0.0
 */
gboolean
ags_window_load_file_timeout(AgsWindow *window)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  if((AGS_WINDOW_TERMINATING & (window->flags)) != 0){
    ags_application_context_quit(application_context);

    return(FALSE);
  }

  if(g_hash_table_lookup(ags_window_load_file,
			 window) != NULL){
    if(window->filename != NULL){
      AgsSimpleFile *simple_file;

      gchar *window_title;
      
      GError *error;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
      locale_t current;
#endif

      if(ags_ui_provider_get_show_animation(AGS_UI_PROVIDER(application_context))){
	return(TRUE);
      }

      g_mutex_lock(&locale_mutex);

      if(!locale_initialized){
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
	locale_env = getenv("LC_ALL");
#else
	c_utf8_locale = newlocale(LC_ALL_MASK, "C.UTF-8", (locale_t) 0);
#endif
    
	locale_initialized = TRUE;
      }

      g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, "C.UTF-8");
#else
      current = uselocale(c_utf8_locale);
#endif
      
      simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						   "filename", window->filename,
						   "no-config", window->no_config,
						   NULL);
      
      error = NULL;
      ags_simple_file_open(simple_file,
			   &error);

      if(error != NULL){
	g_error_free(error);
      }

      ags_simple_file_read(simple_file);
      ags_simple_file_close(simple_file);
      
      /* set name */
      window->name = g_strdup(window->filename);

      window_title = g_strdup_printf("GSequencer - %s", window->name);
      gtk_window_set_title(window,
			   window_title);

      g_free(window_title);
      
      gtk_header_bar_set_subtitle(window->header_bar,
				  window->name);
      
      window->filename = NULL;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, locale_env);
#else
      uselocale(current);
#endif
    }

    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_window_new:
 *
 * Creates an #AgsWindow
 *
 * Returns: a new #AgsWindow
 *
 * Since: 3.0.0
 */
AgsWindow*
ags_window_new()
{
  AgsWindow *window;

  window = (AgsWindow *) g_object_new(AGS_TYPE_WINDOW,
				      NULL);

  return(window);
}
