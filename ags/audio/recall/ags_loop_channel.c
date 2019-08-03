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

#include <ags/audio/recall/ags_loop_channel.h>

#include <ags/libags.h>

#include <math.h>

void ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel);
void ags_loop_channel_init(AgsLoopChannel *loop_channel);
void ags_loop_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_dispose(GObject *gobject);
void ags_loop_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_loop_channel
 * @short_description: loops channel
 * @title: AgsLoopChannel
 * @section_id:
 * @include: ags/audio/recall/ags_loop_channel.h
 *
 * The #AgsLoopChannel class provides ports to the effect processor.
 */

static gpointer ags_loop_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_DELAY_AUDIO,
};

GType
ags_loop_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_loop_channel = 0;

    static const GTypeInfo ags_loop_channel_info = {
      sizeof (AgsLoopChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_init,
    };
    
    ags_type_loop_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsLoopChannel",
						   &ags_loop_channel_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_loop_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_loop_channel_parent_class = g_type_class_peek_parent(loop_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) loop_channel;

  gobject->set_property = ags_loop_channel_set_property;
  gobject->get_property = ags_loop_channel_get_property;

  gobject->dispose = ags_loop_channel_dispose;
  gobject->finalize = ags_loop_channel_finalize;

  /* properties */
  /**
   * AgsLoopChannel:delay-audio:
   *
   * The assigned #AgsDelayAudio.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("delay-audio",
				   "assigned delay audio",
				   "The delay audio it is assigned with",
				   AGS_TYPE_DELAY_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO,
				  param_spec);
}

void
ags_loop_channel_init(AgsLoopChannel *loop_channel)
{
  AGS_RECALL(loop_channel)->name = "ags-loop";
  AGS_RECALL(loop_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(loop_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(loop_channel)->xml_type = "ags-loop-channel";

  loop_channel->delay_audio = NULL;
}

void
ags_loop_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  pthread_mutex_t *recall_mutex;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(loop_channel);

  switch(prop_id){
  case PROP_DELAY_AUDIO:
    {
      AgsDelayAudio *delay_audio;

      delay_audio = (AgsDelayAudio *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(loop_channel->delay_audio == delay_audio){      
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(loop_channel->delay_audio != NULL){
	g_object_unref(G_OBJECT(loop_channel->delay_audio));
      }

      if(delay_audio != NULL){
	g_object_ref(G_OBJECT(delay_audio));
      }

      loop_channel->delay_audio = delay_audio;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  pthread_mutex_t *recall_mutex;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(loop_channel);
  
  switch(prop_id){
  case PROP_DELAY_AUDIO:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, loop_channel->delay_audio);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_dispose(GObject *gobject)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  /* delay audio */
  if(loop_channel->delay_audio != NULL){
    g_object_unref(G_OBJECT(loop_channel->delay_audio));

    loop_channel->delay_audio = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_parent_class)->dispose(gobject);
}

void
ags_loop_channel_finalize(GObject *gobject)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  /* delay audio */
  if(loop_channel->delay_audio != NULL){
    g_object_unref(G_OBJECT(loop_channel->delay_audio));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_parent_class)->finalize(gobject);
}

/**
 * ags_loop_channel_new:
 * @source: the #AgsChannel 
 *
 * Create a new instance of #AgsLoopChannel
 *
 * Returns: the new #AgsLoopChannel
 *
 * Since: 2.0.0
 */
AgsLoopChannel*
ags_loop_channel_new(AgsChannel *source)
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						 "source", source,
						 NULL);

  return(loop_channel);
}
