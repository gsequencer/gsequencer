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

#include <ags/X/ags_dssi_browser.h>
#include <ags/X/ags_dssi_browser_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/plugin/ags_dssi_manager.h>

#include <ags/object/ags_applicable.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 
#include <ags/thread/ags_task_thread.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <dssi.h>

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
 * @include: ags/X/ags_dssi_browser.h
 *
 * #AgsDssiBrowser is a composite widget to select dssi plugin and the desired
 * effect.
 */

GType
ags_dssi_browser_get_type(void)
{
  static GType ags_type_dssi_browser = 0;

  if(!ags_type_dssi_browser){
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

    ags_type_dssi_browser = g_type_register_static(GTK_TYPE_VBOX,
						   "AgsDssiBrowser\0", &ags_dssi_browser_info,
						   0);

    g_type_add_interface_static(ags_type_dssi_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_dssi_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_dssi_browser);
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
  GtkTable *table;
  GtkComboBoxText *combo_box;
  GtkLabel *label;

  GList *list;
  gchar **filenames, **filenames_start;
  
  /* plugin */
  dssi_browser->plugin = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(dssi_browser),
		     GTK_WIDGET(dssi_browser->plugin),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new("filename: \0");
  gtk_box_pack_start(GTK_BOX(dssi_browser->plugin),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(dssi_browser->plugin),
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  dssi_browser->path = NULL;

  ags_dssi_manager_load_default_directory();
  filenames =
    filenames_start = ags_dssi_manager_get_filenames();

  while(*filenames != NULL){
    gtk_combo_box_text_append_text(combo_box,
				   *filenames);

    filenames++;
  }

  //  free(filenames_start);

  label = (GtkLabel *) gtk_label_new("effect: \0");
  gtk_box_pack_start(GTK_BOX(dssi_browser->plugin),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(dssi_browser->plugin),
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  /* description */
  dssi_browser->description = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(dssi_browser),
		     GTK_WIDGET(dssi_browser->description),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Label: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(dssi_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Maker: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(dssi_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Copyright: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(dssi_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "xalign\0", 0.0,
				    "label\0", "Ports: \0",
				    NULL);
  gtk_box_pack_start(GTK_BOX(dssi_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
  
  table = gtk_table_new(256, 2,
			FALSE);
  gtk_box_pack_start(GTK_BOX(dssi_browser->description),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);
}

void
ags_dssi_browser_connect(AgsConnectable *connectable)
{
  AgsDssiBrowser *dssi_browser;
  GList *list, *list_start;

  dssi_browser = AGS_DSSI_BROWSER(connectable);

  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));
  list = list->next;

  g_signal_connect_after(G_OBJECT(list->data), "changed\0",
			 G_CALLBACK(ags_dssi_browser_plugin_filename_callback), dssi_browser);

  list = list->next->next;
  g_signal_connect_after(G_OBJECT(list->data), "changed\0",
			 G_CALLBACK(ags_dssi_browser_plugin_effect_callback), dssi_browser);

  g_list_free(list_start);
}

void
ags_dssi_browser_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_dssi_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsDssiBrowser *dssi_browser;

  dssi_browser = AGS_DSSI_BROWSER(applicable);

  /* empty */
}

void
ags_dssi_browser_apply(AgsApplicable *applicable)
{
  AgsDssiBrowser *dssi_browser;
  GtkComboBoxText *filename, *effect;

  dssi_browser = AGS_DSSI_BROWSER(applicable);

  //TODO:JK: implement me
}

void
ags_dssi_browser_reset(AgsApplicable *applicable)
{
  AgsDssiBrowser *dssi_browser;
  GtkComboBoxText *filename;
  GList *list;

  dssi_browser = AGS_DSSI_BROWSER(applicable);

  list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  g_list_free(list);

  gtk_combo_box_set_active(filename,
			   0);
}

/**
 * ags_dssi_browser_get_plugin_filename:
 *
 * Retrieve selected dssi plugin filename.
 *
 * Returns: the active dssi filename
 *
 * Since: 0.4.2
 */
gchar*
ags_dssi_browser_get_plugin_filename(AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *filename;
  GList *list;

  list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));
  filename = GTK_COMBO_BOX_TEXT(list->next->data);
  g_list_free(list);

  return(gtk_combo_box_text_get_active_text(filename));
}

/**
 * ags_dssi_browser_get_plugin_effect:
 *
 * Retrieve selected dssi effect.
 *
 * Returns: the active dssi effect
 *
 * Since: 0.4.2
 */
gchar*
ags_dssi_browser_get_plugin_effect(AgsDssiBrowser *dssi_browser)
{
  GtkComboBoxText *filename, *effect;
  AgsDssiPlugin *dssi_plugin;
  GList *list, *list_start;
  gchar *effect_name;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  unsigned long index;

  /* retrieve filename and effect */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->plugin));

  filename = GTK_COMBO_BOX(list->next->data);
  effect = GTK_COMBO_BOX(list->next->next->next->data);

  g_list_free(list_start);

  /* update description */
  list_start = 
    list = gtk_container_get_children(GTK_CONTAINER(dssi_browser->description));

  ags_dssi_manager_load_file(gtk_combo_box_text_get_active_text(filename));
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(gtk_combo_box_text_get_active_text(filename));
  
  plugin_so = dssi_plugin->plugin_so;

  effect_name = NULL;
  
  index = (unsigned long) gtk_combo_box_get_active(effect);
  
  if(index != -1 &&
     plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      plugin_descriptor = dssi_descriptor(index);

      effect_name = plugin_descriptor->LADSPA_Plugin->Name;
    }
  }

  return(effect_name);
}

/**
 * ags_dssi_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 0.4.2
 */
GtkWidget*
ags_dssi_browser_combo_box_controls_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(combo_box,
				 "spin button\0");
  gtk_combo_box_text_append_text(combo_box,
				 "dial\0");
  gtk_combo_box_text_append_text(combo_box,
				 "vertical scale\0");
  gtk_combo_box_text_append_text(combo_box,
				 "horizontal scale\0");

  gtk_combo_box_set_active(combo_box,
			   1);

  return(combo_box);
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
 * Creates an #AgsDssiBrowser
 *
 * Returns: a new #AgsDssiBrowser
 *
 * Since: 0.4.2
 */
AgsDssiBrowser*
ags_dssi_browser_new()
{
  AgsDssiBrowser *dssi_browser;

  dssi_browser = (AgsDssiBrowser *) g_object_new(AGS_TYPE_DSSI_BROWSER,
						 "homogeneous\0", FALSE,
						 "spacing\0", 0,
						 NULL);

  return(dssi_browser);
}
