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

#ifndef __AGS_DSSI_BROWSER_H__
#define __AGS_DSSI_BROWSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_DSSI_BROWSER                (ags_dssi_browser_get_type())
#define AGS_DSSI_BROWSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_DSSI_BROWSER, AgsDssiBrowser))
#define AGS_DSSI_BROWSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DSSI_BROWSER, AgsDssiBrowserClass))
#define AGS_IS_DSSI_BROWSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DSSI_BROWSER))
#define AGS_IS_DSSI_BROWSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DSSI_BROWSER))
#define AGS_DSSI_BROWSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DSSI_BROWSER, AgsDssiBrowserClass))

typedef struct _AgsDssiBrowser AgsDssiBrowser;
typedef struct _AgsDssiBrowserClass AgsDssiBrowserClass;

struct _AgsDssiBrowser
{
  GtkVBox vbox;

  gchar *path;

  GtkHBox *plugin;
  GtkVBox *description;

  GtkWidget *preview;
};

struct _AgsDssiBrowserClass
{
  GtkVBoxClass vbox;
};

GType ags_dssi_browser_get_type(void);

gchar* ags_dssi_browser_get_plugin_filename(AgsDssiBrowser *dssi_browser);
gchar* ags_dssi_browser_get_plugin_effect(AgsDssiBrowser *dssi_browser);

GtkWidget* ags_dssi_browser_combo_box_controls_new();
GtkWidget* ags_dssi_browser_preview_new();

AgsDssiBrowser* ags_dssi_browser_new();

#endif /*__AGS_DSSI_BROWSER_H__*/
