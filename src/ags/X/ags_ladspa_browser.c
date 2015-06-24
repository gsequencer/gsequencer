/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_ladspa_browser.h>
#include <ags/X/ags_ladspa_browser_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

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
  static GType ags_type_ladspa_browser = 0;

  if(!ags_type_ladspa_browser){
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

    ags_type_ladspa_browser = g_type_register_static(GTK_TYPE_DIALOG,
						     "AgsLadspaBrowser\0", &ags_ladspa_browser_info,
						     0);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ladspa_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_ladspa_browser);
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
  GtkVBox *vbox;
  GtkTable *table;
  GtkComboBoxText *combo_box;
  GtkLabel *label;
  GList *list;

  gchar **filenames, **filenames_start;

  ladspa_browser->parent = NULL;

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) gtk_dialog_get_content_area((GtkDialog *) ladspa_browser),
		    GTK_WIDGET(vbox));
  
  /* plugin */
  ladspa_browser->plugin = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(ladspa_browser->plugin),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new("filename: \0");
  gtk_box_pack_start(GTK_BOX(ladspa_browser->plugin),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(ladspa_browser->plugin),
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  ladspa_browser->path = NULL;

  ags_ladspa_manager_load_default_directory();
  filenames =
    filenames_start = ags_ladspa_manager_get_filenames();

  while(*filenames != NULL){
    gtk_combo_box_text_append_text(combo_box,
				   *filenames);

    filenames++;
  }

  free(filenames_start);

  label = (GtkLabel *) gtk_label_new("effect: \0");
  gtk_box_pack_start(GTK_BOX(ladspa_browser->plugin),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(ladspa_browser->plugin),
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  /* description */
  ladspa_browser->description = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(ladspa_browser->description),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Label: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Maker: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Copyright: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Ports: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
  
  table = (GtkTable *) gtk_table_new(256, 2,
				     FALSE);
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* action area */
  gtk_dialog_add_buttons((GtkDialog *) ladspa_browser,
			 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			 NULL);

  list = gtk_container_get_children((GtkContainer *) gtk_dialog_get_action_area((GtkDialog *) ladspa_browser));
  ladspa_browser->ok = GTK_BUTTON(list->data);
  ladspa_browser->cancel = GTK_BUTTON(list->next->data);
  g_list_free(list);
}

void
ags_ladspa_browser_connect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;
  GList *list, *list_start;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));
  list = list->next;

  g_signal_connect_after(G_OBJECT(list->data), "changed\0",
			 G_CALLBACK(ags_ladspa_browser_plugin_filename_callback), ladspa_browser);

  list = list->next->next;
  g_signal_connect_after(G_OBJECT(list->data), "changed\0",
			 G_CALLBACK(ags_ladspa_browser_plugin_effect_callback), ladspa_browser);

  g_list_free(list_start);

  /* AgsLadspaBrowser buttons */
  g_signal_connect((GObject *) ladspa_browser->ok, "clicked\0",
		   G_CALLBACK(ags_ladspa_browser_ok_callback), (gpointer) ladspa_browser);

  g_signal_connect((GObject *) ladspa_browser->cancel, "clicked\0",
		   G_CALLBACK(ags_ladspa_browser_cancel_callback), (gpointer) ladspa_browser);
}

void
ags_ladspa_browser_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_ladspa_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = AGS_LADSPA_BROWSER(applicable);

  /* empty */
}

void
ags_ladspa_browser_apply(AgsApplicable *applicable)
{
  AgsLadspaBrowser *ladspa_browser;
  GtkComboBoxText *filename, *effect;

  ladspa_browser = AGS_LADSPA_BROWSER(applicable);

  //TODO:JK: implement me
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
 *
 * Retrieve selected ladspa plugin filename.
 *
 * Returns: the active ladspa filename
 *
 * Since: 0.4
 */
gchar*
ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *filename;
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));
  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  return(gtk_combo_box_text_get_active_text(filename));
}

/**
 * ags_ladspa_browser_get_plugin_effect:
 *
 * Retrieve selected ladspa effect.
 *
 * Returns: the active ladspa effect
 *
 * Since: 0.4
 */
gchar*
ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser)
{
  GtkComboBoxText *effect;
  GList *list;
  gchar *str;
  
  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));
  effect = GTK_COMBO_BOX_TEXT(list->next->next->next->data);
  g_list_free(list);
  
  str = gtk_combo_box_text_get_active_text(effect);
  str = &(str[6]);
  
  return(str);
}

GtkWidget*
ags_ladspa_browser_combo_box_boolean_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "toggle button\0");
  gtk_combo_box_text_append_text(combo_box,
				 "check button\0");

  gtk_combo_box_set_active((GtkComboBox *) combo_box,
			   1);

  return((GtkWidget *) combo_box);
}

/**
 * ags_ladspa_browser_combo_box_float_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 0.4
 */
GtkWidget*
ags_ladspa_browser_combo_box_float_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "spin button\0");
  gtk_combo_box_text_append_text(combo_box,
				 "dial\0");
  gtk_combo_box_text_append_text(combo_box,
				 "vertical scale\0");

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
 * Creates an #AgsLadspaBrowser
 *
 * Returns: a new #AgsLadspaBrowser
 *
 * Since: 0.4
 */
AgsLadspaBrowser*
ags_ladspa_browser_new(GtkWidget *parent)
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = (AgsLadspaBrowser *) g_object_new(AGS_TYPE_LADSPA_BROWSER,
						     NULL);

  ladspa_browser->parent = parent;

  return(ladspa_browser);
}
