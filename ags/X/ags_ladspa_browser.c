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

#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_ladspa_browser_class_init(AgsLadspaBrowserClass *ladspa_browser);
void ags_ladspa_browser_init(AgsLadspaBrowser *ladspa_browser);
void ags_ladspa_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ladspa_browser_applicable_interface_init(AgsApplicableInterface *applicable);

void ags_ladspa_browser_connect(AgsConnectable *connectable);
void ags_ladspa_browser_disconnect(AgsConnectable *connectable);

void ags_ladspa_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_ladspa_browser_apply(AgsApplicable *applicable);
void ags_ladspa_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_ladspa_browser
 * @short_description: A composite to select ladspa effect.
 * @title: AgsLadspaBrowser
 * @section_id:
 * @include: ags/X/ags_ladspa_browser.h
 *
 * #AgsLadspaBrowser is a composite widget to select ladspa plugin and the desired
 * effect.
 */

GType
ags_ladspa_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ladspa_browser = 0;

    static const GTypeInfo ags_ladspa_browser_info = {
      sizeof (AgsLadspaBrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaBrowser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ladspa_browser = g_type_register_static(GTK_TYPE_BOX,
						     "AgsLadspaBrowser", &ags_ladspa_browser_info,
						     0);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ladspa_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_ladspa_browser_class_init(AgsLadspaBrowserClass *ladspa_browser)
{
  /* empty */
}

void
ags_ladspa_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ladspa_browser_connect;
  connectable->disconnect = ags_ladspa_browser_disconnect;
}

void
ags_ladspa_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_ladspa_browser_set_update;
  applicable->apply = ags_ladspa_browser_apply;
  applicable->reset = ags_ladspa_browser_reset;
}

void
ags_ladspa_browser_init(AgsLadspaBrowser *ladspa_browser)
{
  GtkLabel *label;

  GList *start_list, *list;
  gchar **filenames, **filenames_start;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(ladspa_browser),
				 GTK_ORIENTATION_VERTICAL);
  
  /* plugin */
  ladspa_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						  0);
  gtk_box_pack_start((GtkBox *) ladspa_browser,
		     (GtkWidget *) ladspa_browser->plugin,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_pack_start(ladspa_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  ladspa_browser->filename = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_pack_start(ladspa_browser->plugin,
		     (GtkWidget *) ladspa_browser->filename,
		     FALSE, FALSE,
		     0);

  ladspa_browser->path = NULL;

  filenames =
    filenames_start = ags_ladspa_manager_get_filenames(ags_ladspa_manager_get_instance());

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
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ladspa_browser->filename),
				     list->data);
 
      list = list->next;
    }
 
    g_list_free(start_list);

    g_strfreev(filenames_start);
  }
    
  label = (GtkLabel *) gtk_label_new(i18n("effect: "));
  gtk_box_pack_start(ladspa_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  ladspa_browser->effect = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_pack_start(ladspa_browser->plugin,
		     (GtkWidget *) ladspa_browser->effect,
		     FALSE, FALSE,
		     0);

  /* description */
  ladspa_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						       0);
  gtk_box_pack_start((GtkBox *) ladspa_browser,
		     (GtkWidget *) ladspa_browser->description,
		     FALSE, FALSE,
		     0);

  ladspa_browser->label =
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Label: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) ladspa_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  ladspa_browser->maker = 
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Maker: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) ladspa_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  ladspa_browser->copyright = 
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Copyright: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) ladspa_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", i18n("Ports: "),
				    NULL);
  gtk_box_pack_start((GtkBox *) ladspa_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  ladspa_browser->port_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) ladspa_browser->description,
		     (GtkWidget *) ladspa_browser->port_grid,
		     FALSE, FALSE,
		     0);

  ladspa_browser->preview = NULL;
}

void
ags_ladspa_browser_connect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  if((AGS_LADSPA_BROWSER_CONNECTED & (ladspa_browser->flags)) != 0){
    return;
  }

  ladspa_browser->flags |= AGS_LADSPA_BROWSER_CONNECTED;

  g_signal_connect_after(G_OBJECT(ladspa_browser->filename), "changed",
			 G_CALLBACK(ags_ladspa_browser_plugin_filename_callback), ladspa_browser);

  g_signal_connect_after(G_OBJECT(ladspa_browser->effect), "changed",
			 G_CALLBACK(ags_ladspa_browser_plugin_effect_callback), ladspa_browser);
}

void
ags_ladspa_browser_disconnect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  if((AGS_LADSPA_BROWSER_CONNECTED & (ladspa_browser->flags)) == 0){
    return;
  }

  ladspa_browser->flags &= (~AGS_LADSPA_BROWSER_CONNECTED);

  g_object_disconnect(G_OBJECT(ladspa_browser->filename),
		      "any_signal::changed",
		      G_CALLBACK(ags_ladspa_browser_plugin_filename_callback),
		      ladspa_browser,
		      NULL);

  g_object_disconnect(G_OBJECT(ladspa_browser->effect),
		      "any_signal::changed",
		      G_CALLBACK(ags_ladspa_browser_plugin_effect_callback),
		      ladspa_browser,
		      NULL);
}

void
ags_ladspa_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_ladspa_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_ladspa_browser_reset(AgsApplicable *applicable)
{
  AgsLadspaBrowser *ladspa_browser;
  GtkComboBoxText *filename;
  GList *list;

  ladspa_browser = AGS_LADSPA_BROWSER(applicable);

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  gtk_combo_box_set_active((GtkComboBox *) filename,
			   0);
}

/**
 * ags_ladspa_browser_get_plugin_filename:
 * @ladspa_browser: the #AgsLadspaBrowser
 *
 * Retrieve selected ladspa plugin filename.
 *
 * Returns: the active ladspa filename
 *
 * Since: 3.0.0
 */
gchar*
ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser)
{
  if(!AGS_IS_LADSPA_BROWSER(ladspa_browser)){
    return(NULL);
  }
  
  return(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ladspa_browser->filename)));
}

/**
 * ags_ladspa_browser_get_plugin_effect:
 * @ladspa_browser: the #AgsLadspaBrowser
 *
 * Retrieve selected ladspa effect.
 *
 * Returns: the active ladspa effect
 *
 * Since: 3.0.0
 */
gchar*
ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser)
{
  if(!AGS_IS_LADSPA_BROWSER(ladspa_browser)){
    return(NULL);
  }

  return(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ladspa_browser->effect)));
}

/**
 * ags_ladspa_browser_combo_box_output_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_ladspa_browser_combo_box_output_boolean_controls_new()
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
 * ags_ladspa_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_ladspa_browser_combo_box_output_controls_new()
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
 * ags_ladspa_browser_combo_box_boolean_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_ladspa_browser_combo_box_boolean_controls_new()
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
 * ags_ladspa_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_ladspa_browser_combo_box_controls_new()
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
ags_ladspa_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_ladspa_browser_new:
 *
 * Create a new instance of #AgsLadspaBrowser
 *
 * Returns: the new #AgsLadspaBrowser
 *
 * Since: 3.0.0
 */
AgsLadspaBrowser*
ags_ladspa_browser_new()
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = (AgsLadspaBrowser *) g_object_new(AGS_TYPE_LADSPA_BROWSER,
						     "homogeneous", FALSE,
						     "spacing", 0,
						     NULL);

  return(ladspa_browser);
}
