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

#include <ags/app/ags_dssi_browser.h>
#include <ags/app/ags_dssi_browser_callbacks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <dssi.h>

#include <ags/i18n.h>

void ags_dssi_browser_class_init(AgsDssiBrowserClass *dssi_browser);
void ags_dssi_browser_init(AgsDssiBrowser *dssi_browser);
void ags_dssi_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_dssi_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_dssi_browser_connect(AgsConnectable *connectable);
void ags_dssi_browser_disconnect(AgsConnectable *connectable);
void ags_dssi_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_dssi_browser_apply(AgsApplicable *applicable);
void ags_dssi_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_dssi_browser
 * @short_description: A composite to select dssi effect.
 * @title: AgsDssiBrowser
 * @section_id:
 * @include: ags/app/ags_dssi_browser.h
 *
 * #AgsDssiBrowser is a composite widget to select dssi plugin and the desired
 * effect.
 */

GType
ags_dssi_browser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_dssi_browser = 0;

    static const GTypeInfo ags_dssi_browser_info = {
      sizeof (AgsDssiBrowserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dssi_browser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDssiBrowser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dssi_browser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_dssi_browser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_dssi_browser_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_dssi_browser = g_type_register_static(GTK_TYPE_BOX,
						   "AgsDssiBrowser", &ags_dssi_browser_info,
						   0);

    g_type_add_interface_static(ags_type_dssi_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_dssi_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_dssi_browser);
  }

  return g_define_type_id__volatile;
}

void
ags_dssi_browser_class_init(AgsDssiBrowserClass *dssi_browser)
{
  /* empty */
}

void
ags_dssi_browser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_dssi_browser_connect;
  connectable->disconnect = ags_dssi_browser_disconnect;
}

void
ags_dssi_browser_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_dssi_browser_set_update;
  applicable->apply = ags_dssi_browser_apply;
  applicable->reset = ags_dssi_browser_reset;
}

void
ags_dssi_browser_init(AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *combo_box;
  GtkLabel *label;

  AgsDssiManager *dssi_manager;
  
  gchar **filenames, **filenames_start;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(dssi_browser),
				 GTK_ORIENTATION_VERTICAL);

  dssi_browser->flags = 0;
  
  dssi_manager = ags_dssi_manager_get_instance();
  
  /* plugin */
  dssi_browser->plugin = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						0);
  gtk_box_pack_start((GtkBox *) dssi_browser,
		     (GtkWidget *) dssi_browser->plugin,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("filename: "));
  gtk_box_pack_start((GtkBox *) dssi_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  dssi_browser->filename = (GtkComboBox *) gtk_combo_box_text_new();
  combo_box = (GtkComboBoxText *) dssi_browser->filename;
  gtk_box_pack_start((GtkBox *) dssi_browser->plugin,
		     (GtkWidget *) combo_box,
		     FALSE, FALSE,
		     0);

  dssi_browser->path = NULL;

  ags_dssi_manager_load_default_directory(dssi_manager);
  
  filenames =
    filenames_start = ags_dssi_manager_get_filenames(dssi_manager);

  while(*filenames != NULL){
    gtk_combo_box_text_append_text(combo_box,
				   *filenames);

    filenames++;
  }

  g_strfreev(filenames_start);

  label = (GtkLabel *) gtk_label_new(i18n("effect: "));
  gtk_box_pack_start((GtkBox *) dssi_browser->plugin,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  dssi_browser->effect = (GtkComboBox *) gtk_combo_box_text_new();
  combo_box = (GtkComboBoxText *) dssi_browser->effect;
  gtk_box_pack_start((GtkBox *) dssi_browser->plugin,
		     (GtkWidget *) combo_box,
		     FALSE, FALSE,
		     0);

  /* description */
  dssi_browser->description = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						     0);
  gtk_box_pack_start((GtkBox *) dssi_browser,
		     (GtkWidget *) dssi_browser->description,
		     FALSE, FALSE,
		     0);

  dssi_browser->label =
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Label: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) dssi_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  dssi_browser->maker = 
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Maker: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) dssi_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  dssi_browser->copyright = 
    label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				      "xalign", 0.0,
				      "label", i18n("Copyright: "),
				      NULL);
  gtk_box_pack_start((GtkBox *) dssi_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign", 0.0,
				    "label", i18n("Ports: "),
				    NULL);
  gtk_box_pack_start((GtkBox *) dssi_browser->description,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);
  
  dssi_browser->port_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) dssi_browser->description,
		     (GtkWidget *) dssi_browser->port_grid,
		     FALSE, FALSE,
		     0);

  dssi_browser->preview = NULL;
}

void
ags_dssi_browser_connect(AgsConnectable *connectable)
{
  AgsDssiBrowser *dssi_browser;

  dssi_browser = AGS_DSSI_BROWSER(connectable);

  if((AGS_DSSI_BROWSER_CONNECTED & (dssi_browser->flags)) != 0){
    return;
  }

  dssi_browser->flags |= AGS_DSSI_BROWSER_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(dssi_browser->filename), "changed",
			 G_CALLBACK(ags_dssi_browser_plugin_filename_callback), dssi_browser);

  g_signal_connect_after(G_OBJECT(dssi_browser->effect), "changed",
			 G_CALLBACK(ags_dssi_browser_plugin_effect_callback), dssi_browser);
}

void
ags_dssi_browser_disconnect(AgsConnectable *connectable)
{
  AgsDssiBrowser *dssi_browser;

  dssi_browser = AGS_DSSI_BROWSER(connectable);

  if((AGS_DSSI_BROWSER_CONNECTED & (dssi_browser->flags)) == 0){
    return;
  }

  dssi_browser->flags &= (~AGS_DSSI_BROWSER_CONNECTED);

  g_object_disconnect(G_OBJECT(dssi_browser->filename),
		      "any_signal::changed",
		      G_CALLBACK(ags_dssi_browser_plugin_filename_callback),
		      dssi_browser,
		      NULL);
  
  g_object_disconnect(G_OBJECT(dssi_browser->effect),
		      "any_signal::changed",
		      G_CALLBACK(ags_dssi_browser_plugin_effect_callback),
		      dssi_browser,
		      NULL);
}

void
ags_dssi_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_dssi_browser_apply(AgsApplicable *applicable)
{
  /* empty */
}

void
ags_dssi_browser_reset(AgsApplicable *applicable)
{
  AgsDssiBrowser *dssi_browser;
  GtkComboBoxText *filename;

  GList *list;

  dssi_browser = AGS_DSSI_BROWSER(applicable);

  list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));

  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  gtk_combo_box_set_active((GtkComboBox *) filename,
			   0);
}

/**
 * ags_dssi_browser_get_plugin_filename:
 * @dssi_browser: the #AgsDssiBrowser
 *
 * Retrieve selected dssi plugin filename.
 *
 * Returns: the active dssi filename
 *
 * Since: 3.0.0
 */
gchar*
ags_dssi_browser_get_plugin_filename(AgsDssiBrowser *dssi_browser)
{
  if(!AGS_IS_DSSI_BROWSER(dssi_browser)){
    return(NULL);
  }
  
  return(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(dssi_browser->filename)));
}

/**
 * ags_dssi_browser_get_plugin_effect:
 * @dssi_browser: the #AgsDssiBrowser
 *
 * Retrieve selected dssi effect.
 *
 * Returns: the active dssi effect
 *
 * Since: 3.0.0
 */
gchar*
ags_dssi_browser_get_plugin_effect(AgsDssiBrowser *dssi_browser)
{
  if(!AGS_IS_DSSI_BROWSER(dssi_browser)){
    return(NULL);
  }

  return(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(dssi_browser->effect)));
}

/**
 * ags_dssi_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkWidget*
ags_dssi_browser_combo_box_controls_new()
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
ags_dssi_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

/**
 * ags_dssi_browser_new:
 *
 * Create a new instance of #AgsDssiBrowser
 *
 * Returns: the new #AgsDssiBrowser
 *
 * Since: 3.0.0
 */
AgsDssiBrowser*
ags_dssi_browser_new()
{
  AgsDssiBrowser *dssi_browser;

  dssi_browser = (AgsDssiBrowser *) g_object_new(AGS_TYPE_DSSI_BROWSER,
						 "homogeneous", FALSE,
						 "spacing", 0,
						 NULL);

  return(dssi_browser);
}
