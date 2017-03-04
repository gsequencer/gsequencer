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

#include <ags/audio/ags_playback.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_iterator_thread.h>
#include <ags/audio/thread/ags_recycling_thread.h>

/**
 * SECTION:ags_playback
 * @short_description: Outputting to soundcard context
 * @title: AgsPlayback
 * @section_id:
 * @include: ags/audio/ags_playback.h
 *
 * #AgsPlayback represents a context to output.
 */

void ags_playback_class_init(AgsPlaybackClass *playback);
void ags_playback_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playback_init(AgsPlayback *playback);
void ags_playback_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_playback_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_playback_disconnect(AgsConnectable *connectable);
void ags_playback_connect(AgsConnectable *connectable);
void ags_playback_dispose(GObject *gobject);
void ags_playback_finalize(GObject *gobject);

static gpointer ags_playback_parent_class = NULL;

enum{
  PROP_0,
  PROP_PLAYBACK_DOMAIN,
  PROP_SOURCE,
  PROP_AUDIO_CHANNEL,
};

GType
ags_playback_get_type (void)
{
  static GType ags_type_playback = 0;

  if(!ags_type_playback){
    static const GTypeInfo ags_playback_info = {
      sizeof (AgsPlaybackClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayback),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playback_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playback = g_type_register_static(G_TYPE_OBJECT,
					       "AgsPlayback\0",
					       &ags_playback_info,
					       0);

    g_type_add_interface_static(ags_type_playback,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_playback);
}

void
ags_playback_class_init(AgsPlaybackClass *playback)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_playback_parent_class = g_type_class_peek_parent(playback);

  /* GObjectClass */
  gobject = (GObjectClass *) playback;

  gobject->set_property = ags_playback_set_property;
  gobject->get_property = ags_playback_get_property;

  gobject->dispose = ags_playback_dispose;
  gobject->finalize = ags_playback_finalize;

  /* properties */
  /**
   * AgsPlayback:playback-domain:
   *
   * The parent playback domain.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("playback-domain\0",
				   "parent playback domain\0",
				   "The playback domain it is child of\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK_DOMAIN,
				  param_spec);

  /**
   * AgsPlayback:source:
   *
   * The assigned source.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("source\0",
				   "assigned source\0",
				   "The source it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /**
   * AgsPlayback:audio-channel:
   *
   * The assigned audio channel.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_uint("audio-channel\0",
				 "assigned audio channel\0",
				 "The audio channel it is assigned with\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
}

void
ags_playback_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_playback_connect;
  connectable->disconnect = ags_playback_disconnect;
}

void
ags_playback_init(AgsPlayback *playback)
{
  AgsConfig *config;

  gchar str, *str0, *str1;

  gdouble freq;
  guint samplerate, buffer_size;
  gboolean super_threaded_channel, super_threaded_recycling;
  
  /* config */
  config = ags_config_get_instance();

  /* thread model */
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model\0");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope\0");

  if(str0 != NULL && str1 != NULL){
    if(!g_ascii_strncasecmp(str0,
			    "super-threaded\0",
			    15)){
      if(!g_ascii_strncasecmp(str1,
			      "channel\0",
			      8)){
	super_threaded_channel = TRUE;
      }else if(!g_ascii_strncasecmp(str1,
				    "recycling\0",
				    10)){
	super_threaded_recycling = TRUE;
      }
    }
  }

  if(str0 != NULL){
    free(str0);
  }

  if(str1 != NULL){
    free(str1);
  }

  /* default flags */
  if(super_threaded_channel ||
     super_threaded_recycling){
    g_atomic_int_set(&(playback->flags),
		     AGS_PLAYBACK_SUPER_THREADED_CHANNEL);

    if(super_threaded_recycling){
      g_atomic_int_or(&(playback->flags),
		      AGS_PLAYBACK_SUPER_THREADED_RECYCLING);
    }
  }else{
    g_atomic_int_set(&(playback->flags),
		     0);
  }

  /* source */
  playback->source = NULL;
  playback->audio_channel = 0;

  /* samplerate and buffer size */
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate\0");
  }
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);

    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size\0");
  }
  
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);

    free(str);
  }

  /* playback domain */
  playback->playback_domain = NULL;

  /* source and audio channel */
  playback->source = NULL;

  playback->audio_channel = 0;
  
  /* thread frequency */
  freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  /* super threaded channel */
  playback->channel_thread = (AgsThread **) malloc(3 * sizeof(AgsThread *));

  playback->channel_thread[0] = NULL;
  playback->channel_thread[1] = NULL;
  playback->channel_thread[2] = NULL;

  if(super_threaded_channel ||
     super_threaded_recycling){
    playback->channel_thread[0] = (AgsThread *) ags_channel_thread_new(NULL,
								       NULL);
    playback->channel_thread[0]->freq = freq;
	
    playback->channel_thread[1] = (AgsThread *) ags_channel_thread_new(NULL,
								       NULL);
    playback->channel_thread[1]->freq = freq;

    playback->channel_thread[2] = (AgsThread *) ags_channel_thread_new(NULL,
								       NULL);
    playback->channel_thread[2]->freq = freq;
  }
  
  /* iterator thread */
  playback->iterator_thread = (AgsIteratorThread **) malloc(3 * sizeof(AgsIteratorThread *));

  playback->iterator_thread[0] = NULL;
  playback->iterator_thread[1] = NULL;
  playback->iterator_thread[2] = NULL;

  if(super_threaded_recycling){
    playback->iterator_thread[0] = ags_iterator_thread_new();
    playback->iterator_thread[1] = ags_iterator_thread_new();
    playback->iterator_thread[2] = ags_iterator_thread_new();
  }
  
  /* super threaded recycling */
  playback->recycling_thread = (AgsThread **) malloc(3 * sizeof(AgsThread *));

  playback->recycling_thread[0] = NULL;
  playback->recycling_thread[1] = NULL;
  playback->recycling_thread[2] = NULL;

  if(super_threaded_recycling){
    playback->recycling_thread[0] = (AgsThread *) ags_recycling_thread_new(NULL,
									   NULL);
    playback->recycling_thread[0]->freq = freq;
    
    playback->recycling_thread[1] = (AgsThread *) ags_recycling_thread_new(NULL,
									   NULL);
    playback->recycling_thread[1]->freq = freq;

    playback->recycling_thread[2] = (AgsThread *) ags_recycling_thread_new(NULL,
									   NULL);  
    playback->recycling_thread[2]->freq = freq;
  }
  
  /* recall id */
  playback->recall_id = (AgsRecallID **) malloc(3 * sizeof(AgsRecallID *));

  playback->recall_id[0] = NULL;
  playback->recall_id[1] = NULL;
  playback->recall_id[2] = NULL;
}

void
ags_playback_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayback *playback;
  
  playback = AGS_PLAYBACK(gobject);

  switch(prop_id){
  case PROP_PLAYBACK_DOMAIN:
    {
      GObject *playback_domain;

      playback_domain = (GObject *) g_value_get_object(value);

      if((GObject *) playback->playback_domain == playback_domain){
	return;
      }

      if(playback->playback_domain != NULL){
	g_object_unref(G_OBJECT(playback->playback_domain));
      }

      if(playback_domain != NULL){
	g_object_ref(G_OBJECT(playback_domain));
      }

      playback->playback_domain = (GObject *) playback_domain;
    }
    break;
  case PROP_SOURCE:
    {
      GObject *source;

      source = (GObject *) g_value_get_object(value);

      if(source == playback->source){
	return;
      }

      if(playback->source != NULL){
	g_object_unref(G_OBJECT(playback->source));
      }

      if(source != NULL){
	g_object_ref(G_OBJECT(source));

	if(AGS_IS_CHANNEL(source) &&
	   ((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0 ||
	    (AGS_PLAYBACK_SUPER_THREADED_RECYCLING & (g_atomic_int_get(&(playback->flags)))) != 0)){
	  gdouble freq;
	  
	  /* thread frequency */
	  freq = ceil((gdouble) AGS_CHANNEL(source)->samplerate / (gdouble) AGS_CHANNEL(source)->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

	  /* channel thread */
	  g_object_set(playback->channel_thread[0],
		       "frequency\0", freq,
		       "channel\0", source,
		       NULL);

	  g_object_set(playback->channel_thread[1],
		       "frequency\0", freq,
		       "channel\0", source,
		       NULL);

	  g_object_set(playback->channel_thread[2],
		       "frequency\0", freq,
		       "channel\0", source,
		       NULL);

	  if((AGS_PLAYBACK_SUPER_THREADED_RECYCLING & (g_atomic_int_get(&(playback->flags)))) != 0){
	    /* recycling thread */
	    g_object_set(playback->recycling_thread[0],
			 "frequency\0", freq,
			 NULL);

	    g_object_set(playback->recycling_thread[1],
			 "frequency\0", freq,
			 NULL);

	    g_object_set(playback->recycling_thread[2],
			 "frequency\0", freq,
			 NULL);

	    /* iterator thread */
	    g_object_set(playback->iterator_thread[0],
			 "frequency\0", freq,
			 NULL);

	    g_object_set(playback->iterator_thread[1],
			 "frequency\0", freq,
			 NULL);

	    g_object_set(playback->iterator_thread[2],
			 "frequency\0", freq,
			 NULL);
	  }
	}
      }

      playback->source = (GObject *) source;
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      playback->audio_channel = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayback *playback;

  playback = AGS_PLAYBACK(gobject);

  switch(prop_id){
  case PROP_PLAYBACK_DOMAIN:
    {
      g_value_set_object(value,
			 playback->playback_domain);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value,
			  playback->source);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value,
		       playback->audio_channel);      
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_dispose(GObject *gobject)
{
  AgsPlayback *playback;

  guint i;
  
  playback = AGS_PLAYBACK(gobject);

  /* playback domain */
  if(playback->playback_domain != NULL){
    g_object_unref(playback->playback_domain);

    playback->playback_domain = NULL;
  }

  /* source */
  if(playback->source != NULL){
    g_object_unref(playback->source);

    playback->source = NULL;
  }
  
  /* channel thread */
  if(playback->channel_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_unref(playback->channel_thread[i]);

	playback->channel_thread[i] = NULL;
      }
    }
  }
  
  /* recycling thread */
  if(playback->recycling_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->recycling_thread[i] != NULL){
	g_object_unref(playback->recycling_thread[i]);

	playback->recycling_thread[i] = NULL;
      }
    }
    
  }
  
  /* iterator thread */
  if(playback->iterator_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->iterator_thread[i] != NULL){
	g_object_unref(playback->iterator_thread[i]);

	playback->iterator_thread[i] = NULL;
      }
    }
    
  }
  
  /* recall id */
  if(playback->recall_id != NULL){
    for(i = 0; i < 3; i++){
      if(playback->recall_id[i] != NULL){
	g_object_unref(playback->recall_id[i]);

	playback->recall_id[i] = NULL;
      }
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_playback_parent_class)->dispose(gobject);
}

void
ags_playback_finalize(GObject *gobject)
{
  AgsPlayback *playback;

  guint i;
  
  playback = AGS_PLAYBACK(gobject);

  /* playback domain */
  if(playback->playback_domain != NULL){
    g_object_unref(playback->playback_domain);
  }

  /* source */
  if(playback->source != NULL){
    g_object_unref(playback->source);
  }
  
  /* channel thread */
  if(playback->channel_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_unref(playback->channel_thread[i]);
      }
    }
    
    free(playback->channel_thread);
  }
  
  /* recycling thread */
  if(playback->recycling_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->recycling_thread[i] != NULL){
	g_object_unref(playback->recycling_thread[i]);
      }
    }
    
    free(playback->recycling_thread);
  }
  
  /* iterator thread */
  if(playback->iterator_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback->iterator_thread[i] != NULL){
	g_object_unref(playback->iterator_thread[i]);
      }
    }
    
    free(playback->iterator_thread);
  }
  
  /* recall id */
  if(playback->recall_id != NULL){
    for(i = 0; i < 3; i++){
      if(playback->recall_id[i] != NULL){
	g_object_unref(playback->recall_id[i]);
      }
    }
    
    free(playback->recall_id);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_playback_parent_class)->finalize(gobject);
}

void
ags_playback_connect(AgsConnectable *connectable)
{
  AgsPlayback *playback;

  playback = AGS_PLAYBACK(connectable);

  if((AGS_PLAYBACK_CONNECTED & (playback->flags)) != 0){
    return;
  }

  playback->flags |= AGS_PLAYBACK_CONNECTED;
}

void
ags_playback_disconnect(AgsConnectable *connectable)
{
  AgsPlayback *playback;

  playback = AGS_PLAYBACK(connectable);


  if((AGS_PLAYBACK_CONNECTED & (playback->flags)) == 0){
    return;
  }

  playback->flags &= (~AGS_PLAYBACK_CONNECTED);
}

/**
 * ags_playback_set_channel_thread:
 * @playback: the #AgsPlayback
 * @thread: the #AgsChannelThread
 * @scope: the scope of the thread to set
 * 
 * Set channel thread of appropriate scope.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_set_channel_thread(AgsPlayback *playback,
				AgsThread *thread,
				guint scope)
{
  if(playback == NULL ||
     scope > 2){
    return;
  }

  if(playback->channel_thread[scope] != NULL){
    g_object_unref(playback->channel_thread[scope]);
  }

  if(thread != NULL){
    g_object_ref(thread);
  }
  
  playback->channel_thread[scope] = thread;
}

/**
 * ags_playback_get_channel_thread:
 * @playback: the #AgsPlayback
 * @scope: the scope of the thread to get
 * 
 * Get channel thread of appropriate scope.
 * 
 * Returns: the matching #AgsThread or %NULL
 * 
 * Since: 0.7.122.7
 */
AgsThread*
ags_playback_get_channel_thread(AgsPlayback *playback,
				guint scope)
{
  if(playback == NULL ||
     playback->channel_thread == NULL ||
     scope > 2){
    return(NULL);
  }

  return(playback->channel_thread[scope]);
}

/**
 * ags_playback_set_iterator_thread:
 * @playback: the #AgsPlayback
 * @thread: the #AgsIteratorThread
 * @scope: the scope of the thread to set
 * 
 * Set iterator thread of appropriate scope.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_set_iterator_thread(AgsPlayback *playback,
				 AgsThread *thread,
				 guint scope)
{
  if(playback == NULL ||
     scope > 2){
    return;
  }

  if(playback->iterator_thread[scope] != NULL){
    g_object_unref(playback->iterator_thread[scope]);
  }

  if(thread != NULL){
    g_object_ref(thread);
  }
  
  playback->iterator_thread[scope] = thread;
}

/**
 * ags_playback_get_iterator_thread:
 * @playback: the #AgsPlayback
 * @scope: the scope of the thread to get
 * 
 * Get iterator thread of appropriate scope.
 * 
 * Returns: the matching #AgsThread or %NULL
 * 
 * Since: 0.7.122.7
 */
AgsThread*
ags_playback_get_iterator_thread(AgsPlayback *playback,
				 guint scope)
{
  if(playback == NULL ||
     playback->iterator_thread == NULL ||
     scope > 2){
    return(NULL);
  }

  return(playback->iterator_thread[scope]);
}

/**
 * ags_playback_set_recycling_thread:
 * @playback: the #AgsPlayback
 * @thread: the #AgsRecyclingThread
 * @scope: the scope of the thread to set
 * 
 * Set recycling thread of appropriate scope.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_set_recycling_thread(AgsPlayback *playback,
				  AgsThread *thread,
				  guint scope)
{
  if(playback == NULL ||
     scope > 2){
    return;
  }

  if(playback->recycling_thread[scope] != NULL){
    g_object_unref(playback->recycling_thread[scope]);
  }

  if(thread != NULL){
    g_object_ref(thread);
  }
  
  playback->recycling_thread[scope] = thread;
}

/**
 * ags_playback_get_recycling_thread:
 * @playback: the #AgsPlayback
 * @scope: the scope of the thread to get
 * 
 * Get recycling thread of appropriate scope.
 * 
 * Returns: the matching #AgsThread or %NULL
 * 
 * Since: 0.7.122.7
 */
AgsThread*
ags_playback_get_recycling_thread(AgsPlayback *playback,
				  guint scope)
{
  if(playback == NULL ||
     playback->recycling_thread == NULL ||
     scope > 2){
    return(NULL);
  }

  return(playback->recycling_thread[scope]);
}

/**
 * ags_playback_set_recall_id:
 * @playback: the #AgsPlayback
 * @recall_id: the #AgsRecallID
 * @scope: the scope of the recall id to set
 * 
 * Set recall id of appropriate scope.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_set_recall_id(AgsPlayback *playback,
			   AgsRecallID *recall_id,
			   guint scope)
{
  if(playback == NULL ||
     scope > 2){
    return;
  }

  if(playback->recall_id[scope] != NULL){
    g_object_unref(playback->recall_id[scope]);
  }

  if(recall_id != NULL){
    g_object_ref(recall_id);
  }
  
  playback->recall_id[scope] = recall_id;
}

/**
 * ags_playback_get_recall_id:
 * @playback: the #AgsPlayback
 * @scope: the scope of the recall id to get
 * 
 * Get recall id of appropriate scope.
 * 
 * Returns: the matching #AgsRecallID or %NULL
 * 
 * Since: 0.7.122.7
 */
AgsRecallID*
ags_playback_get_recall_id(AgsPlayback *playback,
			   guint scope)
{
  if(playback == NULL ||
     playback->recall_id == NULL ||
     scope > 2){
    return(NULL);
  }

  return(playback->recall_id[scope]);
}

/**
 * ags_playback_play_find_source:
 * @playback_play: a #GList containing #AgsPlayback
 * 
 * Find source
 *
 * Returns: the matching playback play
 *
 * Since: 0.4
 */
AgsPlayback*
ags_playback_find_source(GList *playback,
			 GObject *source)
{
  while(playback != NULL){
    if(AGS_PLAYBACK(playback->data)->source == source){
      return(playback->data);
    }

    playback = playback->next;
  }

  return(NULL);
}

/**
 * ags_playback_new:
 *
 * Creates an #AgsPlayback, refering to @application_context.
 *
 * Returns: a new #AgsPlayback
 *
 * Since: 0.4
 */
AgsPlayback*
ags_playback_new()
{
  AgsPlayback *playback;

  playback = (AgsPlayback *) g_object_new(AGS_TYPE_PLAYBACK,
					  NULL);
  
  return(playback);
}
