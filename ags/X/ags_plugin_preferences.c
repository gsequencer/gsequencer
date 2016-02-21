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

#include <ags/X/ags_plugin_preferences.h>
#include <ags/X/ags_plugin_preferences_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_plugin_preferences_class_init(AgsPluginPreferencesClass *plugin_preferences);
void ags_plugin_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_plugin_preferences_init(AgsPluginPreferences *plugin_preferences);
void ags_plugin_preferences_connect(AgsConnectable *connectable);
void ags_plugin_preferences_disconnect(AgsConnectable *connectable);
static void ags_plugin_preferences_finalize(GObject *gobject);
void ags_plugin_preferences_show(GtkWidget *widget);

void ags_plugin_preferences_reset(AgsPluginPreferences *plugin_preferences);
void* ags_plugin_preferences_refresh(void *ptr);

/**
 * SECTION:ags_plugin_preferences
 * @short_description: A composite widget to do plugin related preferences
 * @title: AgsPluginPreferences
 * @section_id: 
 * @include: ags/X/ags_plugin_preferences.h
 *
 * #AgsPluginPreferences enables you to make plugin related preferences.
 */

static gpointer ags_plugin_preferences_parent_class = NULL;

GType
ags_plugin_preferences_get_type(void)
{
  static GType ags_type_plugin_preferences = 0;

  if(!ags_type_plugin_preferences){
    static const GTypeInfo ags_plugin_preferences_info = {
      sizeof (AgsPluginPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_plugin_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPluginPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_plugin_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_plugin_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_plugin_preferences = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsPluginPreferences\0", &ags_plugin_preferences_info,
							 0);
    
    g_type_add_interface_static(ags_type_plugin_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_plugin_preferences);
}

void
ags_plugin_preferences_class_init(AgsPluginPreferencesClass *plugin_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_plugin_preferences_parent_class = g_type_class_peek_parent(plugin_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) plugin_preferences;

  gobject->finalize = ags_plugin_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) plugin_preferences;

  widget->show = ags_plugin_preferences_show;
}

void
ags_plugin_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_plugin_preferences_connect;
  connectable->disconnect = ags_plugin_preferences_disconnect;
}

void
ags_plugin_preferences_init(AgsPluginPreferences *plugin_preferences)
{
  //TODO:JK: implement me
}

void
ags_plugin_preferences_connect(AgsConnectable *connectable)
{
  AgsPluginPreferences *plugin_preferences;

  plugin_preferences = AGS_PLUGIN_PREFERENCES(connectable);
}

void
ags_plugin_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_plugin_preferences_finalize(GObject *gobject)
{
}

void
ags_plugin_preferences_show(GtkWidget *widget)
{
  AgsPluginPreferences *plugin_preferences;
  pthread_t thread;

  plugin_preferences = AGS_PLUGIN_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_plugin_preferences_parent_class)->show(widget);
}

AgsLadspaPluginPreferences*
ags_ladspa_plugin_preferences_alloc(gchar *ladspa_path)
{
  AgsLadspaPluginPreferences *ladspa_plugin_preferences;
  GtkListStore *list_store;
  GtkTreePath *path;
  GtkTreeIter iter;
  gchar **filenames, **filenames_start;
    
  static const gchar *default_ladspa_path = "/usr/lib/ladspa\0";

  ladspa_plugin_preferences = (AgsLadspaPluginPreferences *) malloc(sizeof(AgsLadspaPluginPreferences));

  if(ladspa_path == NULL){
    gtk_entry_set_text(ladspa_plugin_preferences->ladspa_path,
		       default_ladspa_path);
  }else{
    gtk_entry_set_text(ladspa_plugin_preferences->ladspa_path,
		       ladspa_path);
  }

  ladspa_plugin_preferences->plugin_file = (GtkCellView *) gtk_cell_view_new();

  list_store = gtk_list_store_new(1,
				  G_TYPE_STRING);
  filenames =
    filenames_start = ags_ladspa_manager_get_filenames();
  
  while(*filenames != NULL){
    // Add a new row to the model
    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set(list_store, &iter,
		       0, *filenames,
		       -1);
    
    filenames++;
  }

  free(filenames_start);
  gtk_cell_view_set_model(ladspa_plugin_preferences->plugin_file,
			  GTK_TREE_MODEL(list_store));

  return(ladspa_plugin_preferences);
}

/**
 * ags_plugin_preferences_new:
 *
 * Creates an #AgsPluginPreferences
 *
 * Returns: a new #AgsPluginPreferences
 *
 * Since: 0.4
 */
AgsPluginPreferences*
ags_plugin_preferences_new()
{
  AgsPluginPreferences *plugin_preferences;

  plugin_preferences = (AgsPluginPreferences *) g_object_new(AGS_TYPE_PLUGIN_PREFERENCES,
							     NULL);
  
  return(plugin_preferences);
}
