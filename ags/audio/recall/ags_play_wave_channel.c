/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_play_wave_channel.h>

#include <ags/i18n.h>

void ags_play_wave_channel_class_init(AgsPlayWaveChannelClass *play_wave_channel);
void ags_play_wave_channel_init(AgsPlayWaveChannel *play_wave_channel);
void ags_play_wave_channel_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_play_wave_channel_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_play_wave_channel_dispose(GObject *gobject);
void ags_play_wave_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_play_wave_channel
 * @short_description: play channel wave
 * @title: AgsPlayWaveChannel
 * @section_id:
 * @include: ags/audio/recall/ags_play_wave_channel.h
 *
 * The #AgsPlayWaveChannel class provides ports to the effect processor.
 */

static gpointer ags_play_wave_channel_parent_class = NULL;

static const gchar *ags_play_wave_channel_plugin_name = "ags-play-wave";
static const gchar *ags_play_wave_channel_specifier[] = {
  "./do-playback[0]",
  "./x-offset[0]", 
};
static const gchar *ags_play_wave_channel_control_port[] = {
  "1/2",
  "2/2",
};

enum{
  PROP_0,
  PROP_WAVE,
  PROP_DO_PLAYBACK,
  PROP_X_OFFSET,
};

GType
ags_play_wave_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_wave_channel = 0;

    static const GTypeInfo ags_play_wave_channel_info = {
      sizeof (AgsPlayWaveChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_wave_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsPlayWaveChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_wave_channel_init,
    };

    ags_type_play_wave_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsPlayWaveChannel",
							&ags_play_wave_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_wave_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_play_wave_channel_class_init(AgsPlayWaveChannelClass *play_wave_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_wave_channel_parent_class = g_type_class_peek_parent(play_wave_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) play_wave_channel;

  gobject->set_property = ags_play_wave_channel_set_property;
  gobject->get_property = ags_play_wave_channel_get_property;

  gobject->dispose = ags_play_wave_channel_dispose;
  gobject->finalize = ags_play_wave_channel_finalize;

  /* properties */
  /**
   * AgsPlayWaveChannel:wave:
   *
   * The wave containing the notes.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("wave",
				   i18n_pspec("assigned AgsWave"),
				   i18n_pspec("The AgsWave containing notes"),
				   AGS_TYPE_WAVE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE,
				  param_spec);

  /**
   * AgsPlayWaveChannel:do-playback:
   *
   * The do-playback port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("do-playback",
				   i18n_pspec("do playback"),
				   i18n_pspec("The do playback control"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_PLAYBACK,
				  param_spec);

  /**
   * AgsPlayWaveChannel:x-offset:
   *
   * The x-offset port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("x-offset",
				   i18n_pspec("x offset"),
				   i18n_pspec("The x offset control"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_OFFSET,
				  param_spec);
}

void
ags_play_wave_channel_init(AgsPlayWaveChannel *play_wave_channel)
{
  GList *port;

  AGS_RECALL(play_wave_channel)->name = "ags-play-wave";
  AGS_RECALL(play_wave_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_wave_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_wave_channel)->xml_type = "ags-play-wave-channel";

  /* fields */
  play_wave_channel->wave = NULL;

  /* port */
  port = NULL;

  /* do playback */
  play_wave_channel->do_playback = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_play_wave_channel_plugin_name,
						"specifier", ags_play_wave_channel_specifier[0],
						"control-port", ags_play_wave_channel_control_port[0],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_BOOLEAN,
						NULL);
  g_object_ref(play_wave_channel->do_playback);
  
  play_wave_channel->do_playback->port_value.ags_port_boolean = TRUE;

  /* add to port */
  port = g_list_prepend(port, play_wave_channel->do_playback);
  g_object_ref(play_wave_channel->do_playback);

  /* x offset */
  play_wave_channel->x_offset = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_play_wave_channel_plugin_name,
					     "specifier", ags_play_wave_channel_specifier[1],
					     "control-port", ags_play_wave_channel_control_port[1],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     NULL);
  g_object_ref(play_wave_channel->x_offset);
  
  play_wave_channel->x_offset->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, play_wave_channel->x_offset);
  g_object_ref(play_wave_channel->x_offset);
  
  /* set port */
  AGS_RECALL(play_wave_channel)->port = port;
}

void
ags_play_wave_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsPlayWaveChannel *play_wave_channel;

  GRecMutex *recall_mutex;

  play_wave_channel = AGS_PLAY_WAVE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_wave_channel);

  switch(prop_id){
  case PROP_WAVE:
    {
      AgsWave *wave;

      wave = (AgsWave *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_wave_channel->wave == wave){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_channel->wave != NULL){
	g_object_unref(play_wave_channel->wave);
      }

      if(wave != NULL){
	g_object_ref(wave);
      }

      play_wave_channel->wave = wave;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      AgsPort *do_playback;

      do_playback = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_wave_channel->do_playback == do_playback){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_channel->do_playback != NULL){
	g_object_unref(G_OBJECT(play_wave_channel->do_playback));
      }
      
      if(do_playback != NULL){
	g_object_ref(G_OBJECT(do_playback));
      }
      
      play_wave_channel->do_playback = do_playback;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_X_OFFSET:
    {
      AgsPort *x_offset;

      x_offset = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_wave_channel->x_offset == x_offset){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(play_wave_channel->x_offset != NULL){
	g_object_unref(G_OBJECT(play_wave_channel->x_offset));
      }
      
      if(x_offset != NULL){
	g_object_ref(G_OBJECT(x_offset));
      }
      
      play_wave_channel->x_offset = x_offset;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_wave_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsPlayWaveChannel *play_wave_channel;

  GRecMutex *recall_mutex;
  
  play_wave_channel = AGS_PLAY_WAVE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_wave_channel);

  switch(prop_id){
  case PROP_WAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave_channel->wave);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave_channel->do_playback);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_X_OFFSET:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_wave_channel->x_offset);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_wave_channel_dispose(GObject *gobject)
{
  AgsPlayWaveChannel *play_wave_channel;

  play_wave_channel = AGS_PLAY_WAVE_CHANNEL(gobject);

  /* wave */
  if(play_wave_channel->wave != NULL){
    g_object_unref(G_OBJECT(play_wave_channel->wave));

    play_wave_channel->wave = NULL;
  }

  /* do playback */
  if(play_wave_channel->do_playback != NULL){
    g_object_unref(play_wave_channel->do_playback);

    play_wave_channel->do_playback = NULL;
  }
  
  /* x-offset */
  if(play_wave_channel->x_offset != NULL){
    g_object_unref(play_wave_channel->x_offset);

    play_wave_channel->x_offset = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_channel_parent_class)->dispose(gobject);
}

void
ags_play_wave_channel_finalize(GObject *gobject)
{
  AgsPlayWaveChannel *play_wave_channel;

  play_wave_channel = AGS_PLAY_WAVE_CHANNEL(gobject);

  /* wave */
  if(play_wave_channel->wave != NULL){
    g_object_unref(G_OBJECT(play_wave_channel->wave));
  }

  /* do playback */
  if(play_wave_channel->do_playback != NULL){
    g_object_unref(play_wave_channel->do_playback);
  }
  
  /* x-offset */
  if(play_wave_channel->x_offset != NULL){
    g_object_unref(play_wave_channel->x_offset);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_wave_channel_parent_class)->finalize(gobject);
}

/**
 * ags_play_wave_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPlayWaveChannel
 *
 * Returns: the new #AgsPlayWaveChannel
 *
 * Since: 3.0.0
 */
AgsPlayWaveChannel*
ags_play_wave_channel_new(AgsChannel *source)
{
  AgsPlayWaveChannel *play_wave_channel;

  play_wave_channel = (AgsPlayWaveChannel *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL,
							  "source", source,
							  NULL);

  return(play_wave_channel);
}
