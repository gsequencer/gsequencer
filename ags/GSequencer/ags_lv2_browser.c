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

#include <ags/GSequencer/ags_lv2_browser.h>
#include <ags/GSequencer/ags_lv2_browser_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>

#include <ags/i18n.h>

void ags_lv2_browser_class_init(AgsLv2BrowserClass *lv2_browser);
void ags_lv2_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_lv2_browser_init(AgsLv2Browser *lv2_browser);

void ags_lv2_browser_connect(AgsConnectable *connectable);
void ags_lv2_browser_disconnect(AgsConnectable *connectable);

void ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_lv2_browser_apply(AgsApplicable *applicable);
void ags_lv2_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_lv2_browser
 * @short_description: A composite to select lv2 uri.
 * @title: AgsLv2Browser
 * @section_id:
 * @include: ags/X/ags_lv2_browser.h
 *
 * #AgsLv2Browser is a composite widget to select lv2 plugin and the desired
 * uri.
 */

GType
ags_lv2_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_browser = 0;

    static const GTypeInfo ags_lv2_browser_info = {
      sizeof (AgsLv2BrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Browser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_browser = g_type_register_static(GTK_TYPE_BOX,
						  "AgsLv2Browser", &ags_lv2_browser_info,
						  0);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_browser_class_init(AgsLv2BrowserClass *lv2_browser)
{
  /* empty */
}

void
ags_lv2_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_lv2_browser_connect;
  connectable->disconnect = ags_lv2_browser_disconnect;
}

void
ags_lv2_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_lv2_browser_set_update;
  applicable->apply = ags_lv2_browser_apply;
  applicable->reset = ags_lv2_browser_reset;
}

void
ags_lv2_browser_init(AgsLv2Browser *lv2_browser)
{
  GtkLabel *label;

  AgsLv2Manager *lv2_manager;

  gchar *str;
  gchar **filename_strv;

  guint length;
  guint i;

  GRecMutex *lv2_manager_mutex;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(lv2_browser),
				 GTK_ORIENTATION_VERTICAL);

  lv2_browser->flags = 0;
  
  /* plugin */
  lv2_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					       0);
  gtk_box_pack_start((GtkBox *) lv2_browser,
		     (GtkWidget *) lv2_browser->plugin,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_pack_start(lv2_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  lv2_browser->filename = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_pack_start(lv2_browser->plugin,
		     (GtkWidget *) lv2_browser->filename,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  lv2_browser->path = NULL;

  lv2_manager = ags_lv2_manager_get_instance();

  /* get lv2 manager mutex */
  lv2_manager_mutex = AGS_LV2_MANAGER_GET_OBJ_MUTEX(lv2_manager);

  /* get plugin */
  filename_strv = NULL;

  g_rec_mutex_lock(lv2_manager_mutex);

  length = 0;

  if(lv2_manager->quick_scan_plugin_filename != NULL){
    length = g_strv_length(lv2_manager->quick_scan_plugin_filename);
  }
  
  for(i = 0; i < length; i++){
    gchar *filename, *effect;
    
    /* get filename and effect */
    filename = lv2_manager->quick_scan_plugin_filename[i];
    effect = lv2_manager->quick_scan_plugin_effect[i];
    
    /* create item */
    if(filename != NULL &&
       effect != NULL &&
       (filename_strv == NULL ||
	!g_strv_contains(filename_strv,
			 filename))){
      guint length;
      
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(lv2_browser->filename),
				     filename);

      if(filename_strv == NULL){
	length = 0;
	
	filename_strv = (gchar **) g_malloc(2 * sizeof(gchar *));
      }else{
	length = g_strv_length(filename_strv);

	filename_strv = (gchar **) g_realloc(filename_strv,
					     (length + 2) * sizeof(gchar *));
      }

      filename_strv[length] = filename;
      filename_strv[length + 1] = NULL;
    }
  }

  g_rec_mutex_unlock(lv2_manager_mutex);

  g_free(filename_strv);
  
  label = (GtkLabel *) gtk_label_new(i18n("effect: "));
  gtk_box_pack_start(lv2_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  lv2_browser->effect = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_pack_start(lv2_browser->plugin,
		     (GtkWidget *) lv2_browser->effect,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  /* description */
  lv2_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						    0);
  gtk_box_pack_start((GtkBox *) lv2_browser,
		     (GtkWidget *) lv2_browser->description,
		     FALSE, FALSE,
		     0);

  str = g_strdup_printf("%s: ",
			i18n("Name"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_pack_start(lv2_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  g_free(str);
  
  str = g_strdup_printf("%s: ",
			i18n("Homepage"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_pack_start(lv2_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  g_free(str);

  str = g_strdup_printf("%s: ",
			i18n("M-Box"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_pack_start(lv2_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  g_free(str);

  str = g_strdup_printf("%s: ",
			i18n("Ports"));
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", str,
				    NULL);
  gtk_box_pack_start(lv2_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  g_free(str);
  
  lv2_browser->port_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(lv2_browser->description,
		     (GtkWidget *) lv2_browser->port_grid,
		     FALSE, FALSE,
		     0);
}

void
ags_lv2_browser_connect(AgsConnectable *connectable)
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = AGS_LV2_BROWSER(connectable);

  if((AGS_LV2_BROWSER_CONNECTED & (lv2_browser->flags)) != 0){
    return;
  }

  lv2_browser->flags |= AGS_LV2_BROWSER_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(lv2_browser->filename), "changed",
			 G_CALLBACK(ags_lv2_browser_plugin_filename_callback), lv2_browser);

  g_signal_connect_after(G_OBJECT(lv2_browser->effect), "changed",
			 G_CALLBACK(ags_lv2_browser_plugin_uri_callback), lv2_browser);
}

void
ags_lv2_browser_disconnect(AgsConnectable *connectable)
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = AGS_LV2_BROWSER(connectable);

  if((AGS_LV2_BROWSER_CONNECTED & (lv2_browser->flags)) == 0){
    return;
  }

  lv2_browser->flags &= (~AGS_LV2_BROWSER_CONNECTED);
  
  g_object_disconnect(G_OBJECT(lv2_browser->filename),
		      "any_signal::changed",
		      G_CALLBACK(ags_lv2_browser_plugin_filename_callback),
		      lv2_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(lv2_browser->effect),
		      "any_signal::changed",
		      G_CALLBACK(ags_lv2_browser_plugin_uri_callback),
		      lv2_browser,
		      NULL);
}

void
ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_lv2_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_lv2_browser_reset(AgsApplicable *applicable)
{
  AgsLv2Browser *lv2_browser;
  GtkComboBoxText *filename;
  GList *list;

  lv2_browser = AGS_LV2_BROWSER(applicable);

  list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  gtk_combo_box_set_active((GtkComboBox *) filename,
			   0);
}

/**
 * ags_lv2_browser_get_plugin_filename:
 * @lv2_browser: the #AgsLv2Browser
 *
 * Retrieve selected lv2 plugin filename.
 *
 * Returns: the active lv2 filename
 *
 * Since: 3.0.0
 */
gchar*
ags_lv2_browser_get_plugin_filename(AgsLv2Browser *lv2_browser)
{
  GtkComboBoxText *filename;

  GList *start_list, *list;

  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);

  g_list_free(start_list);

  return(gtk_combo_box_text_get_active_text(filename));
}

/**
 * ags_lv2_browser_get_plugin_uri:
 * @lv2_browser: the #AgsLv2Browser
 *
 * Retrieve selected lv2 uri.
 *
 * Returns: the active lv2 uri
 *
 * Since: 3.0.0
 */
gchar*
ags_lv2_browser_get_plugin_effect(AgsLv2Browser *lv2_browser)
{
  GtkComboBoxText *effect;

  GList *start_list, *list;

  gchar *effect_name;

  /* retrieve filename and effect */
  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(lv2_browser->plugin));

  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);

  effect_name = gtk_combo_box_text_get_active_text(effect);
  
  g_list_free(start_list);

  return(effect_name);
}

/**
 * ags_lv2_browser_combo_box_output_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_lv2_browser_combo_box_output_boolean_controls_new()
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
 * ags_lv2_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_lv2_browser_combo_box_output_controls_new()
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
 * ags_lv2_browser_combo_box_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_lv2_browser_combo_box_boolean_controls_new()
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
 * ags_lv2_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_lv2_browser_combo_box_controls_new()
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
ags_lv2_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_lv2_browser_new:
 *
 * Create a new instance of #AgsLv2Browser
 *
 * Returns: the new #AgsLv2Browser
 *
 * Since: 3.0.0
 */
AgsLv2Browser*
ags_lv2_browser_new()
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = (AgsLv2Browser *) g_object_new(AGS_TYPE_LV2_BROWSER,
					       "homogeneous", FALSE,
					       "spacing", 0,
					       NULL);

  return(lv2_browser);
}
