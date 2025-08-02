/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/plugin/ags_audio_unit_plugin.h>

#include <ags/plugin/ags_plugin_port.h>

#include <string.h>
#include <math.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

void ags_audio_unit_plugin_class_init(AgsAudioUnitPluginClass *audio_unit_plugin);
void ags_audio_unit_plugin_init (AgsAudioUnitPlugin *audio_unit_plugin);
void ags_audio_unit_plugin_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_plugin_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_plugin_finalize(GObject *gobject);

gpointer ags_audio_unit_plugin_instantiate(AgsBasePlugin *base_plugin,
					   guint samplerate,
					   guint buffer_size);

void ags_audio_unit_plugin_connect_port(AgsBasePlugin *base_plugin,
					gpointer plugin_handle,
					guint port_index,
					gpointer data_location);
void ags_audio_unit_plugin_activate(AgsBasePlugin *base_plugin,
				    gpointer plugin_handle);
void ags_audio_unit_plugin_deactivate(AgsBasePlugin *base_plugin,
				      gpointer plugin_handle);
void ags_audio_unit_plugin_run(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle,
			       snd_seq_event_t *seq_event,
			       guint frame_count);
void ags_audio_unit_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_audio_unit_plugin
 * @short_description: The audio_unit plugin class
 * @title: AgsAudioUnitPlugin
 * @section_id:
 * @include: ags/plugin/ags_audio_unit_plugin.h
 *
 * The #AgsAudioUnitPlugin loads/unloads a AudioUnit plugin.
 */

enum{
  PROP_0,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_audio_unit_plugin_parent_class = NULL;
static guint audio_unit_plugin_signals[LAST_SIGNAL];

GHashTable *ags_audio_unit_plugin_process_data = NULL;

GType
ags_audio_unit_plugin_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_plugin = 0;

    static const GTypeInfo ags_audio_unit_plugin_info = {
      sizeof (AgsAudioUnitPluginClass),
      NULL, /* audio_unit_init */
      NULL, /* audio_unit_finalize */
      (GClassInitFunc) ags_audio_unit_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioUnitPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_plugin_init,
    };

    ags_type_audio_unit_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
							"AgsAudioUnitPlugin",
							&ags_audio_unit_plugin_info,
							0);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_plugin);
  }

  return(g_define_type_id__static);
}

void
ags_audio_unit_plugin_class_init(AgsAudioUnitPluginClass *audio_unit_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_unit_plugin_parent_class = g_type_class_peek_parent(audio_unit_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_plugin;

  gobject->set_property = ags_audio_unit_plugin_set_property;
  gobject->get_property = ags_audio_unit_plugin_get_property;

  gobject->finalize = ags_audio_unit_plugin_finalize;

  /* properties */

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) audio_unit_plugin;

  base_plugin->instantiate = ags_audio_unit_plugin_instantiate;

  base_plugin->connect_port = ags_audio_unit_plugin_connect_port;

  base_plugin->activate = ags_audio_unit_plugin_activate;
  base_plugin->deactivate = ags_audio_unit_plugin_deactivate;

  base_plugin->run = ags_audio_unit_plugin_run;

  base_plugin->load_plugin = ags_audio_unit_plugin_load_plugin;

  /* AgsAudioUnitPluginClass */
}

void
ags_audio_unit_plugin_init(AgsAudioUnitPlugin *audio_unit_plugin)
{
  audio_unit_plugin->component = NULL;
  audio_unit_plugin->audio_unit = NULL;
}

void
ags_audio_unit_plugin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  GRecMutex *base_plugin_mutex;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_plugin_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  GRecMutex *base_plugin_mutex;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_plugin_finalize(GObject *gobject)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_plugin_parent_class)->finalize(gobject);
}

void
ags_audio_unit_plugin_async_instantiate(AgsBasePlugin *base_plugin)
{
  gchar *str;

  str = [((AVAudioUnitComponent *) AGS_AUDIO_UNIT_PLUGIN(base_plugin)->component)->name UTF8String];
  
  retval = g_strdup(str);
  
  [AVAudioUnit instantiateWithComponentDescription:(((AVAudioUnitComponent *) AGS_AUDIO_UNIT_PLUGIN(base_plugin)->component)->audioComponentDescription), options:0 completionHandler:^(AVAudioUnit * _Nullable audio_unit, NSError * _Nullable error){
      AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager;
      
      AgsAudioUnitNewQueue *audio_unit_new_queue;

      gchar *plugin_name;

      gint64 creation_timestamp;

      GRecMutex *audio_unit_new_queue_manager_mutex;
      
      if(error->code != noErr){
	g_warning("failed to instantiate Audio Unit %d", error->code);

	return;
      }

      plugin_name = [audio_unit->name UTF8String];

      creation_timestamp = g_get_monotonic_time();

      /* new queue */
      audio_unit_new_queue = ags_audio_unit_new_queue_alloc();

      audio_unit_new_queue->in_use = FALSE;

      audio_unit_new_queue->creation_timestamp = creation_timestamp;

      audio_unit_new_queue->plugin_name = g_strdup(plugin_name);
      
      audio_unit_new_queue->audio_unit = audio_unit;

      /* audio unit new queue manager */
      audio_unit_new_queue_manager = ags_audio_unit_new_queue_manager_get_instance();

      audio_unit_new_queue_manager_mutex = AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_GET_OBJ_MUTEX(audio_unit_new_queue_manager);

      g_rec_mutex_lock(audio_unit_new_queue_manager_mutex);

      audio_unit_new_queue_manager->audio_unit_new_queue = g_list_prepend(audio_unit_new_queue_manager->audio_unit_new_queue,
									  audio_unit_new_queue);

      g_rec_mutex_unlock(audio_unit_new_queue_manager_mutex);
    }];
}

void
ags_audio_unit_plugin_connect_port(AgsBasePlugin *base_plugin,
				   gpointer plugin_handle,
				   guint port_index,
				   gpointer data_location)
{  
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_activate(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_deactivate(AgsBasePlugin *base_plugin,
				 gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_run(AgsBasePlugin *base_plugin,
			  gpointer plugin_handle,
			  snd_seq_event_t *seq_event,
			  guint frame_count)
{
  //TODO:JK: implement me
}

/**
 * ags_audio_unit_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Create a new instance of #AgsAudioUnitPlugin
 *
 * Returns: the new #AgsAudioUnitPlugin
 *
 * Since: 8.1.2
 */
AgsAudioUnitPlugin*
ags_audio_unit_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  audio_unit_plugin = (AgsAudioUnitPlugin *) g_object_new(AGS_TYPE_AUDIO_UNIT_PLUGIN,
							  "filename", filename,
							  "effect", effect,
							  "effect-index", effect_index,
							  NULL);

  return(audio_unit_plugin);
}
