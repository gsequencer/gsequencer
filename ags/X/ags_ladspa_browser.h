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

#ifndef __AGS_LADSPA_BROWSER_H__
#define __AGS_LADSPA_BROWSER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_LADSPA_BROWSER                (ags_ladspa_browser_get_type())
#define AGS_LADSPA_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowser))
#define AGS_LADSPA_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowserClass))
#define AGS_IS_LADSPA_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LADSPA_BROWSER))
#define AGS_IS_LADSPA_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LADSPA_BROWSER))
#define AGS_LADSPA_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LADSPA_BROWSER, AgsLadspaBrowserClass))

typedef struct _AgsLadspaBrowser AgsLadspaBrowser;
typedef struct _AgsLadspaBrowserClass AgsLadspaBrowserClass;

typedef enum{
  AGS_LADSPA_BROWSER_CONNECTED   = 1,
}AgsLadspaBrowserFlags;

struct _AgsLadspaBrowser
{
  GtkVBox vbox;

  guint flags;
  
  gchar *path;

  GtkHBox *plugin;
  
  GtkComboBox *filename;
  GtkComboBox *effect;
  
  GtkVBox *description;

  GtkLabel *label;
  GtkLabel *maker;
  GtkLabel *copyright;

  GtkTable *port_table;

  GtkWidget *preview;
};

struct _AgsLadspaBrowserClass
{
  GtkVBoxClass vbox;
};

GType ags_ladspa_browser_get_type(void);

gchar* ags_ladspa_browser_get_plugin_filename(AgsLadspaBrowser *ladspa_browser);
gchar* ags_ladspa_browser_get_plugin_effect(AgsLadspaBrowser *ladspa_browser);

GtkWidget* ags_ladspa_browser_combo_box_output_boolean_controls_new();
GtkWidget* ags_ladspa_browser_combo_box_output_controls_new();

GtkWidget* ags_ladspa_browser_combo_box_boolean_controls_new();
GtkWidget* ags_ladspa_browser_combo_box_controls_new();

GtkWidget* ags_ladspa_browser_preview_new();

AgsLadspaBrowser* ags_ladspa_browser_new();

G_END_DECLS

#endif /*__AGS_LADSPA_BROWSER_H__*/
