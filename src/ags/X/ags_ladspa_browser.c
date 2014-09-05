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
  GtkComboBoxText *combo_box;
  GtkLabel *label;
  GDir *dir;

  static const gchar *default_path = "/usr/lib/ladspa\0";
  gchar *filename;

  GError *error;

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) gtk_dialog_get_content_area(ladspa_browser),
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

  ladspa_browser->path = g_strdup(default_path);

  //TODO:JK: read environment variable

  error = NULL;
  dir = g_dir_open(ladspa_browser->path,
		   0,
		   &error);

  //TODO:JK: perform error detection

  while((filename = g_dir_read_name(dir)) != NULL){
    if(g_str_has_suffix(filename,
			".so\0")){
      gtk_combo_box_text_append_text(combo_box,
				     filename);
    }
  }


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

  label = (GtkLabel *) gtk_label_new("NULL\0");
  gtk_box_pack_start(GTK_BOX(ladspa_browser->description),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
}

void
ags_ladspa_browser_connect(AgsConnectable *connectable)
{
  AgsLadspaBrowser *ladspa_browser;
  GList *list;

  ladspa_browser = AGS_LADSPA_BROWSER(connectable);

  list = gtk_container_get_children(GTK_CONTAINER(ladspa_browser->plugin));
  list = list->next;

  g_signal_connect(G_OBJECT(list->data), "changed\0",
		   G_CALLBACK(ags_ladspa_browser_plugin_filename_callback), ladspa_browser);

  list = list->next->next;

  g_signal_connect(G_OBJECT(list->data), "changed\0",
		   G_CALLBACK(ags_ladspa_browser_plugin_effect_callback), ladspa_browser);
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
  //TODO:JK: implement me
}

void
ags_ladspa_browser_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_ladspa_browser_set_plugin_filename(AgsLadspaBrowser *ladspa_browser,
				       gchar *filename)
{
  //TODO:JK: implement me
}

gchar*
ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser)
{
  //TODO:JK: implement me
}

void
ags_ladspa_browser_set_plugin_effect(AgsLadspaBrowser *ladspa_browser,
				     gchar *effect)
{
  //TODO:JK: implement me
}

gchar*
ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser)
{
  //TODO:JK: implement me
}


void
ags_ladspa_browser_set_description(AgsLadspaBrowser *ladspa_browser,
				   gchar *description)
{
  //TODO:JK: implement me
}

gchar*
ags_ladspa_browser_get_description(AgsLadspaBrowser *ladspa_browser)
{
  //TODO:JK: implement me
}

GtkWidget*
ags_ladspa_browser_preview_new()
{
  GtkWidget *preview;

  preview = NULL;

  //TODO:JK: implement me

  return(preview);
}

AgsLadspaBrowser*
ags_ladspa_browser_new()
{
  AgsLadspaBrowser *ladspa_browser;

  ladspa_browser = (AgsLadspaBrowser *) g_object_new(AGS_TYPE_LADSPA_BROWSER,
						     NULL);

  return(ladspa_browser);
}
