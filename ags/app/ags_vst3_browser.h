/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_VST3_BROWSER_H__
#define __AGS_VST3_BROWSER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_VST3_BROWSER                (ags_vst3_browser_get_type())
#define AGS_VST3_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_VST3_BROWSER, AgsVst3Browser))
#define AGS_VST3_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_VST3_BROWSER, AgsVst3BrowserClass))
#define AGS_IS_VST3_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VST3_BROWSER))
#define AGS_IS_VST3_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VST3_BROWSER))
#define AGS_VST3_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VST3_BROWSER, AgsVst3BrowserClass))

typedef struct _AgsVst3Browser AgsVst3Browser;
typedef struct _AgsVst3BrowserClass AgsVst3BrowserClass;

typedef enum{
  AGS_VST3_BROWSER_CONNECTED        = 1,
}AgsVst3BrowserFlags;

struct _AgsVst3Browser
{
  GtkBox box;

  guint flags;
  
  gchar *path;

  GtkBox *plugin;

  GtkComboBox *filename;
  GtkComboBox *effect;

  GtkBox *description;

  GtkGrid *port_grid;

  GtkWidget *preview;
};

struct _AgsVst3BrowserClass
{
  GtkBoxClass box;
};

GType ags_vst3_browser_get_type(void);

gchar* ags_vst3_browser_get_plugin_filename(AgsVst3Browser *vst3_browser);
gchar* ags_vst3_browser_get_plugin_effect(AgsVst3Browser *vst3_browser);

GtkWidget* ags_vst3_browser_combo_box_output_boolean_controls_new();
GtkWidget* ags_vst3_browser_combo_box_output_controls_new();

GtkWidget* ags_vst3_browser_combo_box_boolean_controls_new();
GtkWidget* ags_vst3_browser_combo_box_controls_new();

GtkWidget* ags_vst3_browser_preview_new();

AgsVst3Browser* ags_vst3_browser_new();

G_END_DECLS

#endif /*__AGS_VST3_BROWSER_H__*/
