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

#ifndef __AGS_DSSI_PLUGIN_H__
#define __AGS_DSSI_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/plugin/ags_base_plugin.h>

#include <dssi.h>

#define AGS_TYPE_DSSI_PLUGIN                (ags_dssi_plugin_get_type())
#define AGS_DSSI_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DSSI_PLUGIN, AgsDssiPlugin))
#define AGS_DSSI_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DSSI_PLUGIN, AgsDssiPluginClass))
#define AGS_IS_DSSI_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DSSI_PLUGIN))
#define AGS_IS_DSSI_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DSSI_PLUGIN))
#define AGS_DSSI_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_DSSI_PLUGIN, AgsDssiPluginClass))

#define AGS_DSSI_PLUGIN_DESCRIPTOR(ptr) ((DSSI_Descriptor *)(ptr))

typedef struct _AgsDssiPlugin AgsDssiPlugin;
typedef struct _AgsDssiPluginClass AgsDssiPluginClass;

struct _AgsDssiPlugin
{
  AgsBasePlugin base_plugin;

  gchar *unique_id;
  
  gchar *program;
};

struct _AgsDssiPluginClass
{
  AgsBasePluginClass base_plugin;

  void (*change_program)(AgsDssiPlugin *dssi_plugin,
			 gpointer ladspa_handle,
			 guint bank_index,
			 guint program_index);
};

GType ags_dssi_plugin_get_type(void);

void ags_dssi_plugin_change_program(AgsDssiPlugin *dssi_plugin,
				    gpointer ladspa_handle,
				    guint bank_index,
				    guint program_index);

AgsDssiPlugin* ags_dssi_plugin_new(gchar *filename, gchar *effect, guint effect_index);

#endif /*__AGS_DSSI_PLUGIN_H__*/
