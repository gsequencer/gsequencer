/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_play_wave_audio.h>

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_play_wave_audio_class_init(AgsPlayWaveAudioClass *play_wave_audio);
void ags_play_wave_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_wave_audio_init(AgsPlayWaveAudio *play_wave_audio);
void ags_play_wave_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_play_wave_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_play_wave_audio_dispose(GObject *gobject);
void ags_play_wave_audio_finalize(GObject *gobject);

void ags_play_wave_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_play_wave_audio
 * @short_description: play audio wave
 * @title: AgsPlayWaveAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_wave_audio.h
 *
 * The #AgsPlayWaveAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_WAVE_LOOP,
  PROP_WAVE_LOOP_START,
  PROP_WAVE_LOOP_END,
};

static gpointer ags_play_wave_audio_parent_class = NULL;
static AgsPluginInterface *ags_play_wave_parent_plugin_interface;

static const gchar *ags_play_wave_audio_plugin_name = "ags-play-wave";
static const gchar *ags_play_wave_audio_specifier[] = {
  "./wave_loop[0]",
  "./wave_loop_start[0]",
  "./wave_loop_end[0]",
};
static const gchar *ags_play_wave_audio_control_port[] = {
  "1/3",
  "2/3",
  "3/3",
};

GType
ags_play_wave_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_wave_audio = 0;

    static const GTypeInfo ags_play_wave_audio_info = {
      sizeof (AgsPlayWaveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayWaveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_wave_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_wave_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsPlayWaveAudio",
						      &ags_play_wave_audio_info,
						      0);

    g_type_add_interface_static(ags_type_play_wave_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_wave_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_play_wave_audio_class_init(AgsPlayWaveAudioClass *play_wave_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_play_wave_audio_parent_class = g_type_class_peek_parent(play_wave_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_audio;

  gobject->set_property = ags_play_wave_audio_set_property;
  gobject->get_property = ags_play_wave_audio_get_property;

  gobject->dispose = ags_play_wave_audio_dispose;
  gobject->finalize = ags_play_wave_audio_finalize;

  /* properties */
  /**
   * AgsPlayWaveAudio:wave-loop:
   *
   * Count until loop-end and start at loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-loop",
				   i18n_pspec("wave loop playing"),
				   i18n_pspec("Play wave in a endless loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP,
				  param_spec);

  /**
   * AgsPlayWaveAudio:wave-loop-start:
   *
   * The wave's loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave_loop_start",
				   i18n_pspec("start beat of loop"),
				   i18n_pspec("The start beat of the wave loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP_START,
				  param_spec);

  /**
   * AgsPlayWaveAudio:wave-loop-end:
   *
   * The wave's loop-end.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-loop-end",
				   i18n_pspec("end beat of wave loop"),
				   i18n_pspec("The end beat of the wave loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP_END,
				  param_spec);
}

void
ags_play_wave_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_wave_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_wave_audio_set_ports;
}

void
ags_play_wave_audio_init(AgsPlayWaveAudio *play_wave_audio)
{
  GList *port;

  AGS_RECALL(play_wave_audio)->name = "ags-play-wave";
  AGS_RECALL(play_wave_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_audio)->xml_type = "ags-play-wave-audio";

  port = NULL;

  /* wave loop */
  play_wave_audio->wave_loop = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_play_wave_audio_plugin_name,
					    "specifier", ags_play_wave_audio_specifier[0],
					    "control-port", ags_play_wave_audio_control_port[0],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_BOOLEAN,
					    "port-value-size", sizeof(gboolean),
					    "port-value-length", 1,
					    NULL);
  g_object_ref(play_wave_audio->wave_loop);
  
  play_wave_audio->wave_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, play_wave_audio->wave_loop);
  g_object_ref(play_wave_audio->wave_loop);
  
  /* wave-loop-start  */
  play_wave_audio->wave_loop_start = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_play_wave_audio_plugin_name,
						  "specifier", ags_play_wave_audio_specifier[1],
						  "control-port", ags_play_wave_audio_control_port[1],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_UINT64,
						  "port-value-size", sizeof(guint64),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(play_wave_audio->wave_loop_start);
  
  play_wave_audio->wave_loop_start->port_value.ags_port_uint = 0;

  /* add to port */
  port = g_list_prepend(port, play_wave_audio->wave_loop_start);
  g_object_ref(play_wave_audio->wave_loop_start);
  
  /* wave-loop-end */
  play_wave_audio->wave_loop_end = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_play_wave_audio_plugin_name,
						"specifier", ags_play_wave_audio_specifier[2],
						"control-port", ags_play_wave_audio_control_port[2],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT64,
						"port-value-size", sizeof(guint64),
						"port-value-length", 1,
						NULL);
  g_object_ref(play_wave_audio->wave_loop_end);
  
  play_wave_audio->wave_loop_end->port_value.ags_port_uint = 64;

  /* add to port */
  port = g_list_prepend(port, play_wave_audio->wave_loop_end);
  g_object_ref(play_wave_audio->wave_loop_end);

  /* set port */
  AGS_RECALL(play_wave_audio)->port = port;
}

void
ags_play_wave_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayWaveAudio *play_wave_audio;

  pthread_mutex_t *recall_mutex;
  
  play_wave_audio = AGS_PLAY_WAVE_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_WAVE_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == play_wave_audio->wave_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_audio->wave_loop != NULL){
	g_object_unref(G_OBJECT(play_wave_audio->wave_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_wave_audio->wave_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == play_wave_audio->wave_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_audio->wave_loop_start != NULL){
	g_object_unref(G_OBJECT(play_wave_audio->wave_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_wave_audio->wave_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == play_wave_audio->wave_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_audio->wave_loop_end != NULL){
	g_object_unref(G_OBJECT(play_wave_audio->wave_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_wave_audio->wave_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_wave_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayWaveAudio *play_wave;
  
  pthread_mutex_t *recall_mutex;

  play_wave = AGS_PLAY_WAVE_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_WAVE_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave->wave_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave->wave_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave->wave_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_wave_audio_dispose(GObject *gobject)
{
  AgsPlayWaveAudio *play_wave_audio;

  play_wave_audio = AGS_PLAY_WAVE_AUDIO(gobject);

  /* wave */
  if(play_wave_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop));

    play_wave_audio->wave_loop = NULL;
  }

  if(play_wave_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop_start));

    play_wave_audio->wave_loop_start = NULL;
  }

  if(play_wave_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop_end));

    play_wave_audio->wave_loop_end = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_parent_class)->dispose(gobject);
}

void
ags_play_wave_audio_finalize(GObject *gobject)
{
  AgsPlayWaveAudio *play_wave_audio;

  play_wave_audio = AGS_PLAY_WAVE_AUDIO(gobject);

  /* wave */
  if(play_wave_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop));
  }

  if(play_wave_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop_start));
  }

  if(play_wave_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(play_wave_audio->wave_loop_end));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_audio_parent_class)->finalize(gobject);
}

void
ags_play_wave_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./wave-loop[0]",
		19)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-loop-start[0]",
		      24)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop-start", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-loop-end[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop-end", AGS_PORT(port->data),
		   NULL);
    }
    
    port = port->next;
  }
}

/**
 * ags_play_wave_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsPlayWaveAudio
 *
 * Returns: the new #AgsPlayWaveAudio
 *
 * Since: 2.0.0
 */
AgsPlayWaveAudio*
ags_play_wave_audio_new(AgsAudio *audio)
{
  AgsPlayWaveAudio *play_wave_audio;

  play_wave_audio = (AgsPlayWaveAudio *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO,
						      "audio", audio,
						      NULL);

  return(play_wave_audio);
}
