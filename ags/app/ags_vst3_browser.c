/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/app/ags_vst3_browser.h>
#include <ags/app/ags_vst3_browser_callbacks.h>

#include <ags/libags-vst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_vst3_browser_class_init(AgsVst3BrowserClass *vst3_browser);
void ags_vst3_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_vst3_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_vst3_browser_init(AgsVst3Browser *vst3_browser);

void ags_vst3_browser_connect(AgsConnectable *connectable);
void ags_vst3_browser_disconnect(AgsConnectable *connectable);

void ags_vst3_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_vst3_browser_apply(AgsApplicable *applicable);
void ags_vst3_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_vst3_browser
 * @short_description: A composite to select vst3 uri.
 * @title: AgsVst3Browser
 * @section_id:
 * @include: ags/app/ags_vst3_browser.h
 *
 * #AgsVst3Browser is a composite widget to select vst3 plugin and the desired
 * uri.
 */

GType
ags_vst3_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_browser = 0;

    static const GTypeInfo ags_vst3_browser_info = {
      sizeof (AgsVst3BrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vst3_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Browser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_vst3_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_vst3_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_vst3_browser = g_type_register_static(GTK_TYPE_BOX,
						   "AgsVst3Browser", &ags_vst3_browser_info,
						   0);

    g_type_add_interface_static(ags_type_vst3_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_vst3_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_browser_class_init(AgsVst3BrowserClass *vst3_browser)
{
  /* empty */
}

void
ags_vst3_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_vst3_browser_connect;
  connectable->disconnect = ags_vst3_browser_disconnect;
}

void
ags_vst3_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_vst3_browser_set_update;
  applicable->apply = ags_vst3_browser_apply;
  applicable->reset = ags_vst3_browser_reset;
}

void
ags_vst3_browser_init(AgsVst3Browser *vst3_browser)
{
  GtkLabel *label;

  AgsVst3Manager *vst3_manager;

  GList *start_list, *list;

  gchar *str;
  gchar **filenames, **filenames_start;

  guint length;
  guint i;

  GRecMutex *vst3_manager_mutex;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(vst3_browser),
				 GTK_ORIENTATION_VERTICAL);

  vst3_browser->flags = 0;
  
  /* plugin */
  vst3_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);
  gtk_box_append((GtkBox *) vst3_browser,
		 (GtkWidget *) vst3_browser->plugin);

  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) label);

  vst3_browser->filename = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) vst3_browser->filename);

  vst3_browser->path = NULL;

  vst3_manager = ags_vst3_manager_get_instance();

  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  filenames =
    filenames_start = ags_vst3_manager_get_filenames(ags_vst3_manager_get_instance());

  if(filenames_start != NULL){
    list = NULL;
    
    while(filenames[0] != NULL){
      list = g_list_prepend(list,
			    filenames[0]);
     
      filenames++;
    }

    list =
      start_list = g_list_sort(list,
			       g_strcmp0);

    while(list != NULL){
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(vst3_browser->filename),
				     list->data);
 
      list = list->next;
    }
 
    g_list_free(start_list);

    g_strfreev(filenames_start);
  }
  
  label = (GtkLabel *) gtk_label_new(i18n("effect: "));
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) label);

  vst3_browser->effect = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_append(vst3_browser->plugin,
		 (GtkWidget *) vst3_browser->effect);

  /* description */
  vst3_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     0);
  gtk_box_append((GtkBox *) vst3_browser,
		 (GtkWidget *) vst3_browser->description);

  //TODO:JK: implement me
  
  str = g_strdup_printf("%s: ",
			i18n("Ports"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_append(vst3_browser->description,
		 (GtkWidget *) label);

  g_free(str);
  
  vst3_browser->port_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_append(vst3_browser->description,
		 (GtkWidget *) vst3_browser->port_grid);
}

void
ags_vst3_browser_connect(AgsConnectable *connectable)
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = AGS_VST3_BROWSER(connectable);

  if((AGS_VST3_BROWSER_CONNECTED & (vst3_browser->flags)) != 0){
    return;
  }

  vst3_browser->flags |= AGS_VST3_BROWSER_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(vst3_browser->filename), "changed",
			 G_CALLBACK(ags_vst3_browser_plugin_filename_callback), vst3_browser);

  g_signal_connect_after(G_OBJECT(vst3_browser->effect), "changed",
			 G_CALLBACK(ags_vst3_browser_plugin_uri_callback), vst3_browser);
}

void
ags_vst3_browser_disconnect(AgsConnectable *connectable)
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = AGS_VST3_BROWSER(connectable);

  if((AGS_VST3_BROWSER_CONNECTED & (vst3_browser->flags)) == 0){
    return;
  }

  vst3_browser->flags &= (~AGS_VST3_BROWSER_CONNECTED);
  
  g_object_disconnect(G_OBJECT(vst3_browser->filename),
		      "any_signal::changed",
		      G_CALLBACK(ags_vst3_browser_plugin_filename_callback),
		      vst3_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(vst3_browser->effect),
		      "any_signal::changed",
		      G_CALLBACK(ags_vst3_browser_plugin_uri_callback),
		      vst3_browser,
		      NULL);
}

void
ags_vst3_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_vst3_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_vst3_browser_reset(AgsApplicable *applicable)
{
  AgsVst3Browser *vst3_browser;
  GtkComboBoxText *filename;
  GList *list;

  vst3_browser = AGS_VST3_BROWSER(applicable);

  list = gtk_container_get_children(GTK_CONTAINER(vst3_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  gtk_combo_box_set_active((GtkComboBox *) filename,
			   0);
}

/**
 * ags_vst3_browser_get_plugin_filename:
 * @vst3_browser: the #AgsVst3Browser
 *
 * Retrieve selected vst3 plugin filename.
 *
 * Returns: the active vst3 filename
 *
 * Since: 3.10.12
 */
gchar*
ags_vst3_browser_get_plugin_filename(AgsVst3Browser *vst3_browser)
{
  GtkComboBoxText *filename;

  GList *start_list, *list;

  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(vst3_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);

  g_list_free(start_list);

  return(gtk_combo_box_text_get_active_text(filename));
}

/**
 * ags_vst3_browser_get_plugin_uri:
 * @vst3_browser: the #AgsVst3Browser
 *
 * Retrieve selected vst3 uri.
 *
 * Returns: the active vst3 uri
 *
 * Since: 3.10.12
 */
gchar*
ags_vst3_browser_get_plugin_effect(AgsVst3Browser *vst3_browser)
{
  GtkComboBoxText *effect;

  GList *start_list, *list;

  gchar *effect_name;

  /* retrieve filename and effect */
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(vst3_browser->plugin));

  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  effect_name = gtk_combo_box_text_get_active_text(effect);
  
  g_list_free(start_list);

  return(effect_name);
}

/**
 * ags_vst3_browser_combo_box_output_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.10.12
 */
GtkWidget*
ags_vst3_browser_combo_box_output_boolean_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "led");

  gtk_combo_box_set_active((GtkComboBox *) combo_box,
			   1);

  return((GtkWidget *) combo_box);
}

/**
 * ags_vst3_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.10.12
 */
GtkWidget*
ags_vst3_browser_combo_box_output_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "vertical indicator");
  gtk_combo_box_text_append_text(combo_box,
				 "horizontal indicator");

  gtk_combo_box_set_active((GtkComboBox *) combo_box,
			   1);

  return((GtkWidget *) combo_box);
}

/**
 * ags_vst3_browser_combo_box_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.10.12
 */
GtkWidget*
ags_vst3_browser_combo_box_boolean_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "check-button");
  gtk_combo_box_text_append_text(combo_box,
				 "toggle button");

  gtk_combo_box_set_active((GtkComboBox *) combo_box,
			   1);

  return((GtkWidget *) combo_box);
}

/**
 * ags_vst3_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.10.12
 */
GtkWidget*
ags_vst3_browser_combo_box_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "spin button");
  gtk_combo_box_text_append_text(combo_box,
				 "dial");
  gtk_combo_box_text_append_text(combo_box,
				 "vertical scale");
  gtk_combo_box_text_append_text(combo_box,
				 "horizontal scale");

  gtk_combo_box_set_active((GtkComboBox *) combo_box,
			   1);

  return((GtkWidget *) combo_box);
}

GtkWidget*
ags_vst3_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_vst3_browser_new:
 *
 * Create a new instance of #AgsVst3Browser
 *
 * Returns: the new #AgsVst3Browser
 *
 * Since: 3.10.12
 */
AgsVst3Browser*
ags_vst3_browser_new()
{
  AgsVst3Browser *vst3_browser;

  vst3_browser = (AgsVst3Browser *) g_object_new(AGS_TYPE_VST3_BROWSER,
						 "homogeneous", FALSE,
						 "spacing", 0,
						 NULL);

  return(vst3_browser);
}
