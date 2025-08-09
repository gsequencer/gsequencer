/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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
#include <ags/app/ags_gsequencer_application_context.h>
#include <ags/app/ags_gsequencer_application.h>
#include <ags/app/ags_gsequencer_resource.h>

#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_automation_edit_box.h>
#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit.h>

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

gboolean ags_window_tick_callback(GtkWidget *widget,
				  GdkFrameClock *frame_clock,
				  gpointer user_data);

gboolean ags_window_is_connected(AgsConnectable *connectable);
void ags_window_connect(AgsConnectable *connectable);
void ags_window_disconnect(AgsConnectable *connectable);

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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_window);
  }

  return(g_define_type_id__static);
}

void
ags_window_class_init(AgsWindowClass *window)
{
  GObjectClass *gobject;

  ags_window_parent_class = g_type_class_peek_parent(window);

  /* GObjectClass */
  gobject = (GObjectClass *) window;

  gobject->set_property = ags_window_set_property;
  gobject->get_property = ags_window_get_property;

  gobject->finalize = ags_window_finalize;
}

void
ags_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_window_is_connected;  
  connectable->connect = ags_window_connect;
  connectable->disconnect = ags_window_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_window_init(AgsWindow *window)
{
  GtkLabel *label;
  GtkMenuButton *menu_button;
  GtkBox *vbox;
  GtkScrolledWindow *scrolled_window;

  GtkBuilder *builder;
  GtkSettings *settings;

  GApplication *gsequencer_app;
  GMenu *menu;

  AgsApplicationContext *application_context;

  gchar *window_title;

  GError *error;

  application_context = ags_application_context_get_instance();

  settings = gtk_settings_get_default();
  
  window->flags = 0;
  window->connectable_flags = 0;

  g_signal_connect(application_context, "setup-completed",
		   G_CALLBACK(ags_window_setup_completed_callback), window);

  ags_ui_provider_set_window(AGS_UI_PROVIDER(application_context),
			     (GtkWidget *) window);

  gsequencer_app = (GApplication *) ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context));
  gsequencer_app = G_APPLICATION(gsequencer_app);

  g_object_set(G_OBJECT(window),
  	       "icon-name", "gsequencer",
  	       NULL);

  /* window header bar */
  window->no_config = FALSE;

#if defined(AGS_OSXAPI)
  window->shows_menu_bar = FALSE;
#else  
  window->shows_menu_bar = TRUE;
  
  g_object_get(settings,
	       "gtk-shell-shows-menubar", &(window->shows_menu_bar),
	       NULL);
#endif
  
  window->queued_filename = NULL;

  window->loaded_filename = g_strdup("unnamed");

  window_title = g_strdup_printf("GSequencer - %s", window->loaded_filename);
  
  gtk_window_set_title((GtkWindow *) window,
		       window_title);

  g_free(window_title);

  window->header_bar = NULL;

  window->app_button = NULL;
  window->add_button = NULL;
  window->edit_button = NULL;

#if defined(AGS_OSXAPI)
  {
    AgsGSequencerApplication *gsequencer_app;

    gsequencer_app = (AgsGSequencerApplication *) AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->app;

    if(gsequencer_app != NULL){
      gtk_application_set_menubar((GtkApplication *) gsequencer_app,
				  G_MENU_MODEL(gsequencer_app->menubar));
    };

    window->header_bar = (GtkHeaderBar *) gtk_header_bar_new();

    gtk_header_bar_set_decoration_layout(window->header_bar,
					 "menu:minimize,maximize,close");
    gtk_header_bar_set_use_native_controls(window->header_bar,
					   TRUE);

    window_title = g_strdup_printf("GSequencer\n<small>%s</small>",
				   window->loaded_filename);

    label = (GtkLabel *) gtk_label_new(window_title);
    gtk_label_set_use_markup(label,
			     TRUE);
    gtk_header_bar_set_title_widget(window->header_bar,
				    (GtkWidget *) label);

    g_free(window_title);

    /* add menu button */
    window->add_button = (GtkMenuButton *) gtk_menu_button_new();

    gtk_menu_button_set_icon_name(window->add_button,
				  "list-add-symbolic");

    gtk_header_bar_pack_end(window->header_bar,
			    (GtkWidget *) window->add_button);  

    /* app edit buttton */
    window->edit_button = (GtkMenuButton *) gtk_menu_button_new();

    gtk_menu_button_set_icon_name(window->edit_button,
				  "text-editor");

    gtk_header_bar_pack_end(window->header_bar,
			    (GtkWidget *) window->edit_button);
  
    builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_edit_menu.ui");

    menu = (GMenu *) gtk_builder_get_object(builder,
					    "ags-edit-menu");

    if(menu != NULL){
      gtk_menu_button_set_menu_model(window->edit_button,
				     G_MENU_MODEL(menu));
    }
  }
#else
  if(!window->shows_menu_bar){
    window->header_bar = (GtkHeaderBar *) gtk_header_bar_new();

    gtk_header_bar_set_decoration_layout(window->header_bar,
					 "menu:minimize,maximize,close");

    window_title = g_strdup_printf("GSequencer\n<small>%s</small>",
				   window->loaded_filename);

    label = (GtkLabel *) gtk_label_new(window_title);
    gtk_label_set_use_markup(label,
			     TRUE);
    gtk_header_bar_set_title_widget(window->header_bar,
				    (GtkWidget *) label);

    g_free(window_title);
  
    /* app menu buttton */
    window->app_button = (GtkMenuButton *) gtk_menu_button_new();
    g_object_set(window->app_button,
		 "direction", GTK_ARROW_NONE,
		 NULL);
    gtk_header_bar_pack_end(window->header_bar,
			    (GtkWidget *) window->app_button);
  
    builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_primary_menu.ui");

    menu = (GMenu *) gtk_builder_get_object(builder,
					    "ags-primary-menu");

    if(menu != NULL){
      gtk_menu_button_set_menu_model(window->app_button,
				     G_MENU_MODEL(menu));
    }
    
    /* add menu button */
    window->add_button = (GtkMenuButton *) gtk_menu_button_new();

    gtk_menu_button_set_icon_name(window->add_button,
				  "list-add-symbolic");

    gtk_header_bar_pack_end(window->header_bar,
			    (GtkWidget *) window->add_button);  

    /* app edit buttton */
    window->edit_button = (GtkMenuButton *) gtk_menu_button_new();

    gtk_menu_button_set_icon_name(window->edit_button,
				  "text-editor");

    gtk_header_bar_pack_end(window->header_bar,
			    (GtkWidget *) window->edit_button);
  
    builder = gtk_builder_new_from_resource("/org/nongnu/gsequencer/ags/app/ui/ags_edit_menu.ui");

    menu = (GMenu *) gtk_builder_get_object(builder,
					    "ags-edit-menu");

    if(menu != NULL){
      gtk_menu_button_set_menu_model(window->edit_button,
				     G_MENU_MODEL(menu));
    }
  }
#endif
  
  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				6);
  gtk_window_set_child((GtkWindow *) window,
		       (GtkWidget *) vbox);
  
  /* menubar */
  if(window->shows_menu_bar){
    AgsGSequencerApplication *gsequencer_app;

    gsequencer_app = (AgsGSequencerApplication *) AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->app;
    
    window->menu_bar = (GtkPopoverMenuBar *) gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(gsequencer_app->menubar));
    gtk_box_append(vbox,
		   (GtkWidget *) window->menu_bar);

    if(gsequencer_app != NULL){
      gtk_application_set_menubar((GtkApplication *) gsequencer_app,
				  G_MENU_MODEL(gsequencer_app->menubar));
    };
	
    gtk_application_window_set_show_menubar((GtkApplicationWindow *) window,
					    TRUE);
  }
  
  /* vpaned and scrolled window */
  window->paned = (GtkPaned *) gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_append((GtkBox*) vbox,
		 (GtkWidget*) window->paned);
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_paned_set_start_child(window->paned,
			    (GtkWidget *) scrolled_window);

  /* machines rack */
  window->machine = NULL;
  
  window->machine_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					       0);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) window->machine_box);

  window->selected = NULL;

  /* composite editor */
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  
  gtk_paned_set_end_child(window->paned,
			  (GtkWidget *) scrolled_window);

  window->composite_editor = ags_composite_editor_new();
  ags_ui_provider_set_composite_editor(AGS_UI_PROVIDER(application_context),
				       (GtkWidget *) window->composite_editor);
  g_object_set(window->composite_editor,
	       "homogeneous", FALSE,
	       "spacing", 0,
	       NULL);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) window->composite_editor);

  /* navigation */
  window->navigation = g_object_new(AGS_TYPE_NAVIGATION,
				    "homogeneous", FALSE,
				    "spacing", 0,
				    NULL);

  ags_ui_provider_set_navigation(AGS_UI_PROVIDER(application_context),
				 (GtkWidget *) window->navigation);
  
  gtk_box_append(vbox,
		 (GtkWidget *) window->navigation);

  /* load file */
  if(ags_window_load_file == NULL){
    ags_window_load_file = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						 NULL,
						 NULL);
  }

  g_hash_table_insert(ags_window_load_file,
		      window, ags_window_load_file_timeout);

  g_timeout_add(1000 / 4, (GSourceFunc) ags_window_load_file_timeout, (gpointer) window);

  window->animation_time = 0;
  
  gtk_widget_add_tick_callback((GtkWidget *) window,
			       (GtkTickCallback) ags_window_tick_callback,
			       NULL,
			       NULL);
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

gboolean
ags_window_is_connected(AgsConnectable *connectable)
{
  AgsWindow *window;
  
  gboolean is_connected;
  
  window = AGS_WINDOW(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (window->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_window_connect(AgsConnectable *connectable)
{
  AgsWindow *window;

  GList *start_list, *list;

  window = AGS_WINDOW(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  window->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(window, "close-request",
		   G_CALLBACK(ags_window_close_request_callback), NULL);
  
  list =
    start_list = ags_window_get_machine(window);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);

  ags_connectable_connect(AGS_CONNECTABLE(window->composite_editor));
  
  ags_connectable_connect(AGS_CONNECTABLE(window->navigation));
}

void
ags_window_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;

  GList *start_list, *list;

  window = AGS_WINDOW(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  window->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(window,
		      "any_signal::close-request",
		      G_CALLBACK(ags_window_close_request_callback),
		      NULL,
		      NULL);
  
  list =
    start_list = ags_window_get_machine(window);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);

  ags_connectable_disconnect(AGS_CONNECTABLE(window->composite_editor));
  
  ags_connectable_disconnect(AGS_CONNECTABLE(window->navigation));
}

void
ags_window_finalize(GObject *gobject)
{
  AgsWindow *window;

  window = (AgsWindow *) gobject;

  /* remove message monitor */
  g_hash_table_remove(ags_window_load_file,
		      window);

  if(window->loaded_filename != NULL){
    g_free(window->loaded_filename);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_window_parent_class)->finalize(gobject);
}


gboolean
ags_window_tick_callback(GtkWidget *widget,
			 GdkFrameClock *frame_clock,
			 gpointer user_data)
{
  AgsWindow *window;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;

  gchar *str;
  
  gint64 update_ui_timeout;
  gint64 current_time;

  window = (AgsWindow *) widget;
  
  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();

  update_ui_timeout = AGS_UI_PROVIDER_UPDATE_UI_TIMEOUT * G_TIME_SPAN_SECOND;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "update-ui-timeout");

  if(str != NULL){
    update_ui_timeout = g_ascii_strtod(str, NULL) * G_TIME_SPAN_SECOND;
    
    g_free(str);
  }
  
  current_time = g_get_monotonic_time();
  
  if(window->animation_time == 0 ||
     current_time - window->animation_time >= update_ui_timeout){
    window->animation_time = current_time;
    
    ags_ui_provider_update_ui(AGS_UI_PROVIDER(application_context));
  }
  
  return(G_SOURCE_CONTINUE);
}

/**
 * ags_window_get_machine:
 * @window: the #AgsWindow
 * 
 * Get bulk member of @window.
 * 
 * Returns: the #GList-struct containing #AgsMachine
 *
 * Since: 4.0.0
 */
GList*
ags_window_get_machine(AgsWindow *window)
{
  g_return_val_if_fail(AGS_IS_WINDOW(window), NULL);

  return(g_list_reverse(g_list_copy(window->machine)));
}

/**
 * ags_window_add_machine:
 * @window: the #AgsWindow
 * @machine: the #AgsMachine
 * 
 * Add @machine to @window.
 * 
 * Since: 4.0.0
 */
void
ags_window_add_machine(AgsWindow *window,
		       AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_WINDOW(window));
  g_return_if_fail(AGS_IS_MACHINE(machine));

  if(g_list_find(window->machine, machine) == NULL){
    window->machine = g_list_prepend(window->machine,
				     machine);
    
    gtk_box_append(window->machine_box,
		   (GtkWidget *) machine);
  }
}

/**
 * ags_window_remove_machine:
 * @window: the #AgsWindow
 * @machine: the #AgsMachine
 * 
 * Remove @machine from @window.
 * 
 * Since: 4.0.0
 */
void
ags_window_remove_machine(AgsWindow *window,
			  AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_WINDOW(window));
  g_return_if_fail(AGS_IS_MACHINE(machine));

  if(g_list_find(window->machine, machine) != NULL){
    window->machine = g_list_remove(window->machine,
				    machine);
    
    gtk_box_remove(window->machine_box,
		   (GtkWidget *) machine);
  }
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
  ladspa_item = g_menu_item_new("LADSPA",
				NULL);
  
  ladspa_menu = g_menu_new();  

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

    filename = NULL;
    effect = NULL;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);

    item = g_menu_item_new(effect,
			   "app.add_ladspa_bridge");

    builder = g_variant_builder_new(g_variant_type_new("as"));
    
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
  dssi_item = g_menu_item_new("DSSI",
			      NULL);

  dssi_menu = g_menu_new();

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

    filename = NULL;
    effect = NULL;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
    
    item = g_menu_item_new(effect,
			   "app.add_dssi_bridge");

    builder = g_variant_builder_new(g_variant_type_new("as"));
    
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
  lv2_item = g_menu_item_new("LV2",
			      NULL);  

  lv2_menu = g_menu_new();

  /* get plugin */
  g_rec_mutex_lock(lv2_manager_mutex);
  
  if(lv2_manager->quick_scan_plugin_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_plugin_filename);

    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = g_strdup(lv2_manager->quick_scan_plugin_filename[i]);
      effect = g_strdup(lv2_manager->quick_scan_plugin_effect[i]);
      
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	GVariantBuilder *builder;
    
	item = g_menu_item_new(effect,
			       "app.add_lv2_bridge");

	builder = g_variant_builder_new(g_variant_type_new("as"));
    
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

	builder = g_variant_builder_new(g_variant_type_new("as"));
    
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
  vst3_item = g_menu_item_new("VST3",
			      NULL);
  
  vst3_menu = g_menu_new();

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

    filename = NULL;
    effect = NULL;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);

    item = g_menu_item_new(effect,
			   "app.add_vst3_bridge");

    builder = g_variant_builder_new(g_variant_type_new("as"));
    
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

/**
 * ags_window_load_add_menu_audio_unit:
 * @window: the #AgsWindow
 * @menu: the #GMenu
 * 
 * Load add menu Audio Unit.
 * 
 * Since: 8.1.2
 */
void
ags_window_load_add_menu_audio_unit(AgsWindow *window,
				    GMenu *menu)
{
#if defined(AGS_WITH_AUDIO_UNIT_PLUGINS)
  GMenu *audio_unit_menu;
  GMenuItem *audio_unit_item;
  GMenuItem *item;

  AgsAudioUnitManager *audio_unit_manager;

  GList *start_list, *list;

  GRecMutex *audio_unit_manager_mutex;

  audio_unit_manager = ags_audio_unit_manager_get_instance();

  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* audio_unit sub-menu */
  audio_unit_item = g_menu_item_new("Audio Unit",
				    NULL);
  
  audio_unit_menu = g_menu_new();

  /* get plugin */
  g_rec_mutex_lock(audio_unit_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(audio_unit_manager->audio_unit_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(audio_unit_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    GVariantBuilder *builder;

    gchar *filename, *effect;

    filename = NULL;
    effect = NULL;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);

    item = g_menu_item_new(effect,
			   "app.add_audio_unit_bridge");

    builder = g_variant_builder_new(g_variant_type_new("as"));
    
    //    g_variant_builder_add(builder, "s", filename);
    g_variant_builder_add(builder, "s", effect);

    g_menu_item_set_attribute_value(item,
				    "target",
				    g_variant_new("as", builder));
    
    g_menu_append_item(audio_unit_menu,
		       item);

    g_variant_builder_unref(builder);
    
    /* iterate */
    list = list->next;
  }
  
  g_menu_item_set_submenu(audio_unit_item,
			  G_MENU_MODEL(audio_unit_menu));  

  g_menu_append_item(menu,
		     audio_unit_item);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
#endif
}

void
ags_window_load_add_menu_live_dssi(AgsWindow *window,
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
  dssi_item = g_menu_item_new("live DSSI",
			      NULL);

  dssi_menu = g_menu_new();

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

    filename = NULL;
    effect = NULL;
    
    /* get filename and effect */
    g_object_get(list->data,
		 "filename", &filename,
		 "effect", &effect,
		 NULL);
    
    item = g_menu_item_new(effect,
			   "app.add_live_dssi_bridge");

    builder = g_variant_builder_new(g_variant_type_new("as"));
    
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

void
ags_window_load_add_menu_live_lv2(AgsWindow *window,
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
  lv2_item = g_menu_item_new("live LV2",
			      NULL);

  lv2_menu = g_menu_new();

  /* get plugin */
  g_rec_mutex_lock(lv2_manager_mutex);
    
  if(lv2_manager->quick_scan_instrument_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_instrument_filename);
  
    for(i = 0; i < length; i++){
      gchar *filename, *effect;
    
      /* get filename and effect */
      filename = g_strdup(lv2_manager->quick_scan_instrument_filename[i]);
      effect = g_strdup(lv2_manager->quick_scan_instrument_effect[i]);
    
      /* create item */
      if(filename != NULL &&
	 effect != NULL){
	GVariantBuilder *builder;
    
	item = g_menu_item_new(effect,
			       "app.add_live_lv2_bridge");

	builder = g_variant_builder_new(g_variant_type_new("as"));
    
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

void
ags_window_load_add_menu_live_vst3(AgsWindow *window,
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
  vst3_item = g_menu_item_new("live VST3",
			      NULL);
  
  vst3_menu = g_menu_new();

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

    filename = NULL;
    effect = NULL;
    
    if(ags_base_plugin_test_flags(list->data, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      /* get filename and effect */
      g_object_get(list->data,
		   "filename", &filename,
		   "effect", &effect,
		   NULL);

      item = g_menu_item_new(effect,
			     "app.add_live_vst3_bridge");

      builder = g_variant_builder_new(g_variant_type_new("as"));
    
      g_variant_builder_add(builder, "s", filename);
      g_variant_builder_add(builder, "s", effect);

      g_menu_item_set_attribute_value(item,
				      "target",
				      g_variant_new("as", builder));
    
      g_menu_append_item(vst3_menu,
			 item);

      g_variant_builder_unref(builder);
    }
    
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
ags_window_load_add_menu_live_audio_unit(AgsWindow *window,
					 GMenu *menu)
{
#if defined(AGS_WITH_AUDIO_UNIT_PLUGINS)
  GMenu *audio_unit_menu;
  GMenuItem *audio_unit_item;
  GMenuItem *item;

  AgsAudioUnitManager *audio_unit_manager;

  GList *start_list, *list;

  GRecMutex *audio_unit_manager_mutex;

  audio_unit_manager = ags_audio_unit_manager_get_instance();

  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* audio_unit sub-menu */
  audio_unit_item = g_menu_item_new("live Audio Unit",
				    NULL);
  
  audio_unit_menu = g_menu_new();

  /* get plugin */
  g_rec_mutex_lock(audio_unit_manager_mutex);
  
  list =
    start_list = g_list_copy_deep(audio_unit_manager->audio_unit_plugin,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(audio_unit_manager_mutex);
  
  start_list = ags_base_plugin_sort(start_list);
  g_list_free(list);
 
  list = start_list;

  while(list != NULL){
    GVariantBuilder *builder;

    gchar *filename, *effect;

    filename = NULL;
    effect = NULL;
    
    if(ags_base_plugin_test_flags(list->data, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      /* get filename and effect */
      g_object_get(list->data,
		   "filename", &filename,
		   "effect", &effect,
		   NULL);

      item = g_menu_item_new(effect,
			     "app.add_live_audio_unit_bridge");

      builder = g_variant_builder_new(g_variant_type_new("as"));
    
      //      g_variant_builder_add(builder, "s", filename);
      g_variant_builder_add(builder, "s", effect);

      g_menu_item_set_attribute_value(item,
				      "target",
				      g_variant_new("as", builder));
    
      g_menu_append_item(audio_unit_menu,
			 item);

      g_variant_builder_unref(builder);
    }
    
    /* iterate */
    list = list->next;
  }

  g_menu_item_set_submenu(audio_unit_item,
			  G_MENU_MODEL(audio_unit_menu));  

  g_menu_append_item(menu,
		     audio_unit_item);

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
#endif
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
  gtk_widget_show((GtkWidget *) dialog);
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
    if(window->queued_filename != NULL){
      AgsSimpleFile *simple_file;
      
      GError *error;

      if(ags_ui_provider_get_show_animation(AGS_UI_PROVIDER(application_context))){
	return(TRUE);
      }
      
      simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
						   "filename", window->queued_filename,
						   "no-config", window->no_config,
						   NULL);
      
      error = NULL;
      ags_simple_file_open(simple_file,
			   &error);

      if(error != NULL){
	g_error_free(error);
      }

      ags_simple_file_read(simple_file);
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
