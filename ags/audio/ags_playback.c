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

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_note.h>

#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_iterator_thread.h>
#include <ags/audio/thread/ags_recycling_thread.h>

#include <math.h>

#include <ags/i18n.h>

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

/**
 * SECTION:ags_playback
 * @short_description: Outputting to soundcard context
 * @title: AgsPlayback
 * @section_id:
 * @include: ags/audio/ags_playback.h
 *
 * #AgsPlayback represents a context to output.
 */

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
					       "AgsPlayback",
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("playback-domain",
				   i18n_pspec("parent playback domain"),
				   i18n_pspec("The playback domain it is child of"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("assigned source"),
				   i18n_pspec("The source it is assigned with"),
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("audio-channel",
				 i18n_pspec("assigned audio channel"),
				 i18n_pspec("The audio channel it is assigned with"),
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
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *thread_model, *super_threaded_scope;

  gboolean super_threaded_channel;
  guint i;
  
  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* config */
  config = ags_config_get_instance();

  /* thread model */
  super_threaded_channel = FALSE;
  
  pthread_mutex_lock(application_mutex);

  thread_model = ags_config_get_value(config,
				      AGS_CONFIG_THREAD,
				      "model");

  if(thread_model != NULL &&
     !g_ascii_strncasecmp(thread_model,
			  "super-threaded",
			  15)){
    super_threaded_scope = ags_config_get_value(config,
						AGS_CONFIG_THREAD,
						"super-threaded-scope");
    if(super_threaded_scope != NULL &&
       (!g_ascii_strncasecmp(super_threaded_scope,
			     "channel",
			     8))){
      super_threaded_channel = TRUE;
    }
    
    g_free(super_threaded_scope);
  }

  g_free(thread_model);

  pthread_mutex_unlock(application_mutex);

  /* default flags */
  if(super_threaded_channel){
    g_atomic_int_set(&(playback->flags),
		     AGS_PLAYBACK_SUPER_THREADED_CHANNEL);
  }else{
    g_atomic_int_set(&(playback->flags),
		     0);
  }

  /* source */
  playback->source = NULL;
  playback->audio_channel = 0;

  playback->play_note = ags_note_new();
  g_object_ref(playback->play_note);
  
  /* playback domain */
  playback->playback_domain = NULL;
  
  /* super threaded channel */
  playback_domain->channel_thread = (AgsThread **) malloc(AGS_SOUND_SCOPE_LAST * sizeof(AgsThread *));

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    playback->channel_thread[i] = NULL;
  }

  /* recall id */
  playback->recall_id = (AgsRecallID **) malloc(AGS_SOUND_SCOPE_LAST * sizeof(AgsRecallID *));

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    playback->recall_id[i] = NULL;
  }
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

	AGS_NOTE(playback->play_note)->y = AGS_CHANNEL(source)->pad;
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
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_run_dispose(playback->channel_thread[i]);
	g_object_unref(playback->channel_thread[i]);

	playback->channel_thread[i] = NULL;
      }
    }
  }
    
  /* recall id */
  if(playback->recall_id != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
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
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_run_dispose(playback->channel_thread[i]);
	g_object_unref(playback->channel_thread[i]);
      }
    }
    
    free(playback->channel_thread);
  }
  
  /* recall id */
  if(playback->recall_id != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
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
 * Since: 2.0.0
 */
void
ags_playback_set_channel_thread(AgsPlayback *playback,
				AgsThread *thread,
				guint scope)
{
  if(!AGS_IS_PLAYBACK(playback) ||
     scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }

  if(playback->channel_thread[scope] != NULL){
    if(ags_thread_is_running(playback->channel_thread[sound_scope])){
      ags_thread_stop(playback->channel_thread[sound_scope]);
    }
    
    g_object_run_dispose(playback->channel_thread[sound_scope]);
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
 * Since: 2.0.0
 */
AgsThread*
ags_playback_get_channel_thread(AgsPlayback *playback,
				guint scope)
{
  if(!AGS_IS_PLAYBACK(playback) ||
     playback->channel_thread == NULL ||
     scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  return(playback->channel_thread[scope]);
}

/**
 * ags_playback_set_recall_id:
 * @playback: the #AgsPlayback
 * @recall_id: the #AgsRecallID
 * @scope: the scope of the recall id to set
 * 
 * Set recall id of appropriate scope.
 * 
 * Since: 2.0.0
 */
void
ags_playback_set_recall_id(AgsPlayback *playback,
			   AgsRecallID *recall_id,
			   guint scope)
{
  if(playback == NULL ||
     scope >= AGS_SOUND_SCOPE_LAST){
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
 * Since: 2.0.0
 */
AgsRecallID*
ags_playback_get_recall_id(AgsPlayback *playback,
			   guint scope)
{
  if(playback == NULL ||
     playback->recall_id == NULL ||
     scope >= AGS_SOUND_SCOPE_LAST){
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
 * Since: 1.0.0
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
 * Since: 1.0.0
 */
AgsPlayback*
ags_playback_new()
{
  AgsPlayback *playback;

  playback = (AgsPlayback *) g_object_new(AGS_TYPE_PLAYBACK,
					  NULL);
  
  return(playback);
}
