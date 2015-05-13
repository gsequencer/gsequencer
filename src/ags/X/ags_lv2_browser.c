/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_lv2_browser.h>
#include <ags/X/ags_lv2_browser_callbacks.h>

#include <ags/main.h>

#include <ags/object/ags_connectable.h>

#include <ags/plugin/ags_lv2_manager.h>

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

#include <lv2.h>

void ags_lv2_browser_class_init(AgsLv2BrowserClass *lv2_browser);
void ags_lv2_browser_init(AgsLv2Browser *lv2_browser);
void ags_lv2_browser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_browser_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_lv2_browser_connect(AgsConnectable *connectable);
void ags_lv2_browser_disconnect(AgsConnectable *connectable);
void ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update);
void ags_lv2_browser_apply(AgsApplicable *applicable);
void ags_lv2_browser_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_lv2_browser
 * @short_description: A composite to select lv2 effect.
 * @title: AgsLv2Browser
 * @section_id:
 * @include: ags/X/ags_lv2_browser.h
 *
 * #AgsLv2Browser is a composite widget to select lv2 plugin and the desired
 * effect.
 */

GType
ags_lv2_browser_get_type(void)
{
  static GType ags_type_lv2_browser = 0;

  if(!ags_type_lv2_browser){
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

    ags_type_lv2_browser = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsLv2Browser\0", &ags_lv2_browser_info,
						  0);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_lv2_browser,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_lv2_browser);
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
  //TODO:JK: implement me
}

void
ags_lv2_browser_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_lv2_browser_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_browser_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = AGS_LV2_BROWSER(applicable);

  /* empty */
}

void
ags_lv2_browser_apply(AgsApplicable *applicable)
{
  AgsLv2Browser *lv2_browser;
  GtkComboBoxText *filename, *effect;

  lv2_browser = AGS_LV2_BROWSER(applicable);

  //TODO:JK: implement me
}

void
ags_lv2_browser_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_lv2_browser_get_plugin_filename:
 *
 * Retrieve selected lv2 plugin filename.
 *
 * Returns: the active lv2 filename
 *
 * Since: 0.4.2
 */
gchar*
ags_lv2_browser_get_plugin_filename(AgsLv2Browser *lv2_browser)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_lv2_browser_get_plugin_effect:
 *
 * Retrieve selected lv2 effect.
 *
 * Returns: the active lv2 effect
 *
 * Since: 0.4.2
 */
gchar*
ags_lv2_browser_get_plugin_effect(AgsLv2Browser *lv2_browser)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_lv2_browser_combo_box_controls_new:
 *
 * Creates a #GtkComboBox containing suitable widgets as controls.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 0.4.2
 */
GtkWidget*
ags_lv2_browser_combo_box_controls_new()
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
 * Creates an #AgsLv2Browser
 *
 * Returns: a new #AgsLv2Browser
 *
 * Since: 0.4.2
 */
AgsLv2Browser*
ags_lv2_browser_new()
{
  AgsLv2Browser *lv2_browser;

  lv2_browser = (AgsLv2Browser *) g_object_new(AGS_TYPE_LV2_BROWSER,
					       NULL);

  return(lv2_browser);
}
