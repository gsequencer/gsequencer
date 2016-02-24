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

#include <ags/plugin/ags_ladspa_plugin.h>

void ags_ladspa_plugin_class_init(AgsLadspaPluginClass *ladspa_plugin);
void ags_ladspa_plugin_init (AgsLadspaPlugin *ladspa_plugin);
void ags_ladspa_plugin_finalize(GObject *gobject);

gpointer ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin);
void ags_ladspa_plugin_connect_port(AgsBasePlugin *base_plugin,
				    gpointer plugin_handle,
				    guint port_index,
				    gpointer data_location);
void ags_ladspa_plugin_activate(AgsBasePlugin *base_plugin,
				gpointer plugin_handle);
void ags_ladspa_plugin_deactivate(AgsBasePlugin *base_plugin,
				  gpointer plugin_handle);
void ags_ladspa_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_ladspa_plugin
 * @short_description: The ladspa plugin class
 * @title: AgsLadspaPlugin
 * @section_id:
 * @include: ags/object/ags_ladspa_plugin.h
 *
 * The #AgsLadspaPlugin loads/unloads a Ladspa plugin.
 */

static gpointer ags_ladspa_plugin_parent_class = NULL;

GType
ags_ladspa_plugin_get_type (void)
{
  static GType ags_type_ladspa_plugin = 0;

  if(!ags_type_ladspa_plugin){
    static const GTypeInfo ags_ladspa_plugin_info = {
      sizeof (AgsLadspaPluginClass),
      NULL, /* ladspa_init */
      NULL, /* ladspa_finalize */
      (GClassInitFunc) ags_ladspa_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_plugin_init,
    };

    ags_type_ladspa_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						    "AgsLadspaPlugin\0",
						    &ags_ladspa_plugin_info,
						    0);
  }

  return (ags_type_ladspa_plugin);
}

void
ags_ladspa_plugin_class_init(AgsLadspaPluginClass *ladspa_plugin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ladspa_plugin_parent_class = g_type_class_peek_parent(ladspa_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_plugin;

  gobject->finalize = ags_ladspa_plugin_finalize;
}

void
ags_ladspa_plugin_init(AgsLadspaPlugin *ladspa_plugin)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_finalize(GObject *gobject)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = AGS_LADSPA_PLUGIN(gobject);
}

gpointer
ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_connect_port(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle,
			       guint port_index,
			       gpointer data_location)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_activate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_deactivate(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
}

/**
 * ags_ladspa_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Creates an #AgsLadspaPlugin
 *
 * Returns: a new #AgsLadspaPlugin
 *
 * Since: 0.7.6
 */
AgsLadspaPlugin*
ags_ladspa_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = (AgsLadspaPlugin *) g_object_new(AGS_TYPE_LADSPA_PLUGIN,
						   "filename\0", filename,
						   "effect\0", effect,
						   "effect-index\0", effect_index,
						   NULL);

  return(ladspa_plugin);
}
