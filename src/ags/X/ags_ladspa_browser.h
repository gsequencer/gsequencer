/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_LADSPA_BROWSER_H__
#define __AGS_LADSPA_BROWSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_LADSPA_BROWSER                (ags_ladspa_browser_get_type())
#define AGS_LADSPA_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowser))
#define AGS_LADSPA_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowserClass))
#define AGS_IS_LADSPA_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LADSPA_BROWSER))
#define AGS_IS_LADSPA_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LADSPA_BROWSER))
#define AGS_LADSPA_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowserClass))

typedef struct _AgsLadspaBrowser AgsLadspaBrowser;
typedef struct _AgsLadspaBrowserClass AgsLadspaBrowserClass;

struct _AgsLadspaBrowser
{
  GtkVBox vbox;

  gchar *path;

  GtkHBox *plugin;
  GtkVBox *description;

  GtkWidget *preview;
};

struct _AgsLadspaBrowserClass
{
  GtkVBoxClass vbox;
};

GType ags_ladspa_browser_get_type(void);

gchar* ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser);
gchar* ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser);

GtkWidget* ags_ladspa_browser_combo_box_controls_new();
GtkWidget* ags_ladspa_browser_preview_new();

AgsLadspaBrowser* ags_ladspa_browser_new();

#endif /*__AGS_LADSPA_BROWSER_H__*/
