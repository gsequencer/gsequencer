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

#ifndef __AGS_LV2_BROWSER_H__
#define __AGS_LV2_BROWSER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_port_editor.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_BROWSER                (ags_lv2_browser_get_type())
#define AGS_LV2_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LV2_BROWSER, AgsLv2Browser))
#define AGS_LV2_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LV2_BROWSER, AgsLv2BrowserClass))
#define AGS_IS_LV2_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_BROWSER))
#define AGS_IS_LV2_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_BROWSER))
#define AGS_LV2_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_BROWSER, AgsLv2BrowserClass))

#define AGS_LV2_BROWSER_FILENAME_HEIGHT_REQUEST (256)
#define AGS_LV2_BROWSER_FILENAME_WIDTH_REQUEST (500)

#define AGS_LV2_BROWSER_EFFECT_HEIGHT_REQUEST (256)
#define AGS_LV2_BROWSER_EFFECT_WIDTH_REQUEST (500)

typedef struct _AgsLv2Browser AgsLv2Browser;
typedef struct _AgsLv2BrowserClass AgsLv2BrowserClass;

struct _AgsLv2Browser
{
  GtkBox box;

  guint connectable_flags;
  
  gchar *path;

  GtkBox *plugin;

  GtkTreeView *filename_tree_view;
  GtkTreeView *effect_tree_view;

  GtkBox *description;

  GtkLabel *lv2_name;  
  GtkLabel *lv2_homepage;  
  GtkLabel *lv2_mbox;  

  GList *port_editor;
  
  GtkGrid *port_editor_grid;

  GtkWidget *preview;
};

struct _AgsLv2BrowserClass
{
  GtkBoxClass box;
};

GType ags_lv2_browser_get_type(void);

gchar* ags_lv2_browser_get_plugin_filename(AgsLv2Browser *lv2_browser);
gchar* ags_lv2_browser_get_plugin_effect(AgsLv2Browser *lv2_browser);

GList* ags_lv2_browser_get_port_editor(AgsLv2Browser *lv2_browser);
void ags_lv2_browser_add_port_editor(AgsLv2Browser *lv2_browser,
				     AgsPortEditor *port_editor,
				     guint x, guint y,
				     guint width, guint height);
void ags_lv2_browser_remove_port_editor(AgsLv2Browser *lv2_browser,
					AgsPortEditor *port_editor);

void ags_lv2_browser_clear(AgsLv2Browser *lv2_browser);

GtkWidget* ags_lv2_browser_preview_new();

AgsLv2Browser* ags_lv2_browser_new();

G_END_DECLS

#endif /*__AGS_LV2_BROWSER_H__*/
