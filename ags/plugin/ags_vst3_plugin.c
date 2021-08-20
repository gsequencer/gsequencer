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

#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/libags-vst.h>

#if defined(AGS_W32API)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void ags_vst3_plugin_class_init(AgsVst3PluginClass *vst3_plugin);
void ags_vst3_plugin_init (AgsVst3Plugin *vst3_plugin);
void ags_vst3_plugin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_plugin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_plugin_finalize(GObject *gobject);

gpointer ags_vst3_plugin_instantiate(AgsBasePlugin *base_plugin,
				     guint samplerate, guint buffer_size);
void ags_vst3_plugin_connect_port(AgsBasePlugin *base_plugin,
				  gpointer plugin_handle,
				  guint port_index,
				  gpointer data_location);
void ags_vst3_plugin_activate(AgsBasePlugin *base_plugin,
			      gpointer plugin_handle);
void ags_vst3_plugin_deactivate(AgsBasePlugin *base_plugin,
				gpointer plugin_handle);
void ags_vst3_plugin_run(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle,
			 snd_seq_event_t *seq_event,
			 guint frame_count);
void ags_vst3_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_vst3_plugin
 * @short_description: The vst3 plugin class
 * @title: AgsVst3Plugin
 * @section_id:
 * @include: ags/plugin/ags_vst3_plugin.h
 *
 * The #AgsVst3Plugin loads/unloads a Vst3 plugin.
 */

enum{
  PROP_0,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_vst3_plugin_parent_class = NULL;
static guint vst3_plugin_signals[LAST_SIGNAL];

GType
ags_vst3_plugin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_plugin = 0;

    static const GTypeInfo ags_vst3_plugin_info = {
      sizeof (AgsVst3PluginClass),
      NULL, /* vst3_init */
      NULL, /* vst3_finalize */
      (GClassInitFunc) ags_vst3_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_plugin_init,
    };

    ags_type_vst3_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						  "AgsVst3Plugin",
						  &ags_vst3_plugin_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_plugin);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_plugin_class_init(AgsVst3PluginClass *vst3_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_vst3_plugin_parent_class = g_type_class_peek_parent(vst3_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) vst3_plugin;

  gobject->set_property = ags_vst3_plugin_set_property;
  gobject->get_property = ags_vst3_plugin_get_property;

  gobject->finalize = ags_vst3_plugin_finalize;

  /* properties */

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) vst3_plugin;

  base_plugin->instantiate = ags_vst3_plugin_instantiate;

  base_plugin->connect_port = ags_vst3_plugin_connect_port;

  base_plugin->activate = ags_vst3_plugin_activate;
  base_plugin->deactivate = ags_vst3_plugin_deactivate;

  base_plugin->run = ags_vst3_plugin_run;

  base_plugin->load_plugin = ags_vst3_plugin_load_plugin;

  /* AgsVst3PluginClass */
}

void
ags_vst3_plugin_init(AgsVst3Plugin *vst3_plugin)
{
  //TODO:JK: implement me
}

void
ags_vst3_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Plugin *vst3_plugin;

  GRecMutex *base_plugin_mutex;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_plugin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Plugin *vst3_plugin;

  GRecMutex *base_plugin_mutex;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_plugin_finalize(GObject *gobject)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_vst3_plugin_parent_class)->finalize(gobject);
}

gpointer
ags_vst3_plugin_instantiate(AgsBasePlugin *base_plugin,
			    guint samplerate, guint buffer_size)
{
  //TODO:JK: implement me
  
  return(NULL);
}

void
ags_vst3_plugin_connect_port(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle,
			     guint port_index,
			     gpointer data_location)
{
  //TODO:JK: implement me
}

void
ags_vst3_plugin_activate(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_vst3_plugin_deactivate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_vst3_plugin_run(AgsBasePlugin *base_plugin,
		    gpointer plugin_handle,
		    snd_seq_event_t *seq_event,
		    guint frame_count)
{
  //TODO:JK: implement me
}

void
ags_vst3_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

/**
 * ags_vst3_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Create a new instance of #AgsVst3Plugin
 *
 * Returns: the new #AgsVst3Plugin
 *
 * Since: 3.10.2
 */
AgsVst3Plugin*
ags_vst3_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = (AgsVst3Plugin *) g_object_new(AGS_TYPE_VST3_PLUGIN,
					       "filename", filename,
					       "effect", effect,
					       "effect-index", effect_index,
					       NULL);

  return(vst3_plugin);
}
