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

#ifndef __AGS_LV2_BROWSER_H__
#define __AGS_LV2_BROWSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_LV2_BROWSER                (ags_lv2_browser_get_type())
#define AGS_LV2_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LV2_BROWSER, AgsLv2Browser))
#define AGS_LV2_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LV2_BROWSER, AgsLv2BrowserClass))
#define AGS_IS_LV2_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_BROWSER))
#define AGS_IS_LV2_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_BROWSER))
#define AGS_LV2_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_BROWSER, AgsLv2BrowserClass))

typedef struct _AgsLv2Browser AgsLv2Browser;
typedef struct _AgsLv2BrowserClass AgsLv2BrowserClass;

struct _AgsLv2Browser
{
  GtkVBox vbox;

  gchar *path;

  GtkHBox *plugin;
  GtkVBox *description;

  GtkWidget *preview;
};

struct _AgsLv2BrowserClass
{
  GtkVBoxClass vbox;
};

GType ags_lv2_browser_get_type(void);

gchar* ags_lv2_browser_get_plugin_filename(AgsLv2Browser *lv2_browser);
gchar* ags_lv2_browser_get_plugin_effect(AgsLv2Browser *lv2_browser);

GtkWidget* ags_lv2_browser_combo_box_output_boolean_controls_new();
GtkWidget* ags_lv2_browser_combo_box_output_controls_new();

GtkWidget* ags_lv2_browser_combo_box_boolean_controls_new();
GtkWidget* ags_lv2_browser_combo_box_controls_new();

GtkWidget* ags_lv2_browser_preview_new();

AgsLv2Browser* ags_lv2_browser_new();

#endif /*__AGS_LV2_BROWSER_H__*/
