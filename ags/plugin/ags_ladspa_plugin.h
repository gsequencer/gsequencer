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

#ifndef __AGS_LADSPA_PLUGIN_H__
#define __AGS_LADSPA_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ladspa.h>

#define AGS_TYPE_LADSPA_PLUGIN                (ags_ladspa_plugin_get_type())
#define AGS_LADSPA_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LADSPA_PLUGIN, AgsLadspaPlugin))
#define AGS_LADSPA_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LADSPA_PLUGIN, AgsLadspaPluginClass))
#define AGS_IS_LADSPA_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LADSPA_PLUGIN))
#define AGS_IS_LADSPA_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LADSPA_PLUGIN))
#define AGS_LADSPA_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LADSPA_PLUGIN, AgsLadspaPluginClass))

#define AGS_LADSPA_PLUGIN_DESCRIPTOR(ptr) ((LADSPA_Descriptor *)(ptr))

typedef struct _AgsLadspaPlugin AgsLadspaPlugin;
typedef struct _AgsLadspaPluginClass AgsLadspaPluginClass;

struct _AgsLadspaPlugin
{
  AgsBasePlugin base_plugin;

  guint unique_id;
};

struct _AgsLadspaPluginClass
{
  AgsBasePluginClass base_plugin;
};

GType ags_ladspa_plugin_get_type(void);

AgsLadspaPlugin* ags_ladspa_plugin_new(gchar *filename, gchar *effect, guint effect_index);

#endif /*__AGS_LADSPA_PLUGIN_H__*/
