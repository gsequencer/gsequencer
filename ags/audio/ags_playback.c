/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_note.h>

#include <ags/audio/thread/ags_channel_thread.h>

#include <math.h>

#include <ags/i18n.h>

void ags_playback_class_init(AgsPlaybackClass *playback);
void ags_playback_init(AgsPlayback *playback);
void ags_playback_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_playback_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_playback_dispose(GObject *gobject);
void ags_playback_finalize(GObject *gobject);

/**
 * SECTION:ags_playback
 * @short_description: Outputting channel to soundcard
 * @title: AgsPlayback
 * @section_id:
 * @include: ags/audio/ags_playback.h
 *
 * #AgsPlayback represents a context to output. It provides you the channel processing threads
 * per #AgsSoundScope-enum.
 * 
 * The assigned #AgsChannelThread calls ags_channel_recursive_run_stage() for you.
 */

static gpointer ags_playback_parent_class = NULL;

enum{
  PROP_0,
  PROP_PLAYBACK_DOMAIN,
  PROP_CHANNEL,
  PROP_AUDIO_CHANNEL,
  PROP_PLAY_NOTE,
};

GType
ags_playback_get_type (void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_playback = 0;

    static const GTypeInfo ags_playback_info = {
      sizeof(AgsPlaybackClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPlayback),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_init,
    };

    ags_type_playback = g_type_register_static(G_TYPE_OBJECT,
					       "AgsPlayback",
					       &ags_playback_info,
					       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_playback);
  }

  return g_define_type_id__volatile;
}

GType
ags_playback_flags_get_type()
{
  static gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_PLAYBACK_SINGLE_THREADED, "AGS_PLAYBACK_SINGLE_THREADED", "playback-single-threaded" },
      { AGS_PLAYBACK_SUPER_THREADED_CHANNEL, "AGS_PLAYBACK_SUPER_THREADED_CHANNEL", "playback-super-threaded-channel" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsPlaybackFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("playback-domain",
				   i18n_pspec("parent playback domain"),
				   i18n_pspec("The playback domain it is child of"),
				   AGS_TYPE_PLAYBACK_DOMAIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK_DOMAIN,
				  param_spec);

  /**
   * AgsPlayback:channel:
   *
   * The assigned channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("assigned channel"),
				   i18n_pspec("The channel it is assigned with"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsPlayback:audio-channel:
   *
   * The assigned audio channel.
   * 
   * Since: 3.0.0
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

  /**
   * AgsPlayback:play-note:
   *
   * The assigned note.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("play-note",
				   i18n_pspec("assigned note to play"),
				   i18n_pspec("The note to do playback"),
				   AGS_TYPE_NOTE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_NOTE,
				  param_spec);
}

void
ags_playback_init(AgsPlayback *playback)
{
  AgsConfig *config;

  gchar *thread_model, *super_threaded_scope;

  gboolean super_threaded_channel;
  guint i;
  
  playback->flags = 0;  
  playback->connectable_flags = 0;
  
  /* add playback mutex */
  g_rec_mutex_init(&(playback->obj_mutex)); 

  /* config */
  config = ags_config_get_instance();
  
  /* thread model */
  super_threaded_channel = FALSE;
  
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

  /* default flags */
  if(super_threaded_channel){
    playback->flags |= AGS_PLAYBACK_SUPER_THREADED_CHANNEL;
  }

  /* channel */
  playback->channel = NULL;
  playback->audio_channel = 0;

  playback->play_note = (GObject *) ags_note_new();
  g_object_ref(playback->play_note);
  
  /* playback domain */
  playback->playback_domain = NULL;
  
  /* super threaded channel */
  playback->channel_thread = (AgsThread **) malloc(AGS_SOUND_SCOPE_LAST * sizeof(AgsThread *));

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
  
  GRecMutex *playback_mutex;

  playback = AGS_PLAYBACK(gobject);

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  switch(prop_id){
  case PROP_PLAYBACK_DOMAIN:
    {
      GObject *playback_domain;

      playback_domain = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(playback_mutex);
      
      if((GObject *) playback->playback_domain == playback_domain){  
	g_rec_mutex_unlock(playback_mutex);
	
	return;
      }

      if(playback->playback_domain != NULL){
	g_object_unref(G_OBJECT(playback->playback_domain));
      }

      if(playback_domain != NULL){
	g_object_ref(G_OBJECT(playback_domain));
      }

      playback->playback_domain = (GObject *) playback_domain;

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      GObject *channel;

      channel = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(playback_mutex);
      
      if(channel == playback->channel){
	g_rec_mutex_unlock(playback_mutex);

	return;
      }

      if(playback->channel != NULL){
	g_object_unref(G_OBJECT(playback->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));

	AGS_NOTE(playback->play_note)->y = AGS_CHANNEL(channel)->pad;
      }

      playback->channel = (GObject *) channel;

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(playback_mutex);
      
      playback->audio_channel = g_value_get_uint(value);

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_PLAY_NOTE:
    {
      GObject *note;

      note = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(playback_mutex);
      
      if(note == playback->play_note){
	g_rec_mutex_unlock(playback_mutex);

	return;
      }

      if(playback->play_note != NULL){
	g_object_unref(G_OBJECT(playback->play_note));
      }

      if(note != NULL){
	g_object_ref(G_OBJECT(note));
      }

      playback->play_note = (GObject *) note;

      g_rec_mutex_unlock(playback_mutex);
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

  GRecMutex *playback_mutex;

  playback = AGS_PLAYBACK(gobject);

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  switch(prop_id){
  case PROP_PLAYBACK_DOMAIN:
    {
      g_rec_mutex_lock(playback_mutex);
      
      g_value_set_object(value,
			 playback->playback_domain);

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_CHANNEL:
    {
      g_rec_mutex_lock(playback_mutex);
      
      g_value_set_object(value,
			 playback->channel);

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(playback_mutex);
      
      g_value_set_uint(value,
		       playback->audio_channel);      

      g_rec_mutex_unlock(playback_mutex);
    }
    break;
  case PROP_PLAY_NOTE:
    {
      g_rec_mutex_lock(playback_mutex);
      
      g_value_set_object(value,
			 playback->play_note);

      g_rec_mutex_unlock(playback_mutex);
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
    ags_playback_domain_remove_playback((AgsPlaybackDomain *) playback->playback_domain,
					(GObject *) playback, ((AGS_IS_OUTPUT(playback->channel)) ? AGS_TYPE_OUTPUT: AGS_TYPE_INPUT));
  }

  /* channel */
  if(playback->channel != NULL){
    AgsChannel *channel;

    channel = (AgsChannel *) playback->channel;
    
    playback->channel = NULL;
    
    g_object_unref(channel);
  }
  
  /* channel thread */
  if(playback->channel_thread != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_run_dispose((GObject *) playback->channel_thread[i]);
	g_object_unref((GObject *) playback->channel_thread[i]);

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
    if(playback->channel != NULL){
      ags_playback_domain_remove_playback((AgsPlaybackDomain *) playback->playback_domain,
					  (GObject *) playback, ((AGS_IS_OUTPUT(playback->channel)) ? AGS_TYPE_OUTPUT: AGS_TYPE_INPUT));
    }else{
      gpointer tmp;

      tmp = playback->playback_domain;

      playback->playback_domain = NULL;

      g_object_unref(tmp);
    }
  }

  /* channel */
  if(playback->channel != NULL){
    AgsChannel *channel;

    channel = (AgsChannel *) playback->channel;
    
    playback->channel = NULL;
    
    g_object_unref(channel);
  }
  
  /* channel thread */
  if(playback->channel_thread != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(playback->channel_thread[i] != NULL){
	g_object_run_dispose((GObject *) playback->channel_thread[i]);
	g_object_unref((GObject *) playback->channel_thread[i]);
      }
    }
    
    free(playback->channel_thread);

    playback->channel_thread = NULL;
  }
  
  /* recall id */
  if(playback->recall_id != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(playback->recall_id[i] != NULL){
	g_object_unref(playback->recall_id[i]);
      }
    }
    
    free(playback->recall_id);

    playback->recall_id = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_playback_parent_class)->finalize(gobject);
}

/**
 * ags_playback_test_flags:
 * @playback: the #AgsPlayback
 * @flags: the flags
 *
 * Test @flags to be set on @playback.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_playback_test_flags(AgsPlayback *playback, AgsPlaybackFlags flags)
{
  gboolean retval;  
  
  GRecMutex *playback_mutex;

  if(!AGS_IS_PLAYBACK(playback)){
    return(FALSE);
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* test */
  g_rec_mutex_lock(playback_mutex);

  retval = (flags & (playback->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(playback_mutex);

  return(retval);
}

/**
 * ags_playback_set_flags:
 * @playback: the #AgsPlayback
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_playback_set_flags(AgsPlayback *playback, AgsPlaybackFlags flags)
{
  GRecMutex *playback_mutex;

  if(!AGS_IS_PLAYBACK(playback)){
    return;
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* set flags */
  g_rec_mutex_lock(playback_mutex);

  playback->flags |= flags;

  g_rec_mutex_unlock(playback_mutex);
}

/**
 * ags_playback_unset_flags:
 * @playback: the #AgsPlayback
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_playback_unset_flags(AgsPlayback *playback, AgsPlaybackFlags flags)
{
  GRecMutex *playback_mutex;

  if(!AGS_IS_PLAYBACK(playback)){
    return;
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* set flags */
  g_rec_mutex_lock(playback_mutex);

  playback->flags &= (~flags);

  g_rec_mutex_unlock(playback_mutex);
}

/**
 * ags_playback_set_channel_thread:
 * @playback: the #AgsPlayback
 * @thread: the #AgsChannelThread
 * @sound_scope: the scope of the thread to set
 * 
 * Set channel thread of appropriate scope.
 * 
 * Since: 3.0.0
 */
void
ags_playback_set_channel_thread(AgsPlayback *playback,
				AgsThread *thread,
				gint sound_scope)
{
  GRecMutex *playback_mutex;

  if(!AGS_IS_PLAYBACK(playback) ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* unset old */
  g_rec_mutex_lock(playback_mutex);

  if(playback->channel_thread[sound_scope] != NULL){
    if(ags_thread_test_status_flags(playback->channel_thread[sound_scope], AGS_THREAD_STATUS_RUNNING)){
      ags_thread_stop(playback->channel_thread[sound_scope]);
    }
    
    g_object_run_dispose((GObject *) playback->channel_thread[sound_scope]);
    g_object_unref((GObject *) playback->channel_thread[sound_scope]);
  }

  /* set new */
  if(thread != NULL){
    g_object_ref(thread);
  }
  
  playback->channel_thread[sound_scope] = thread;

  g_rec_mutex_unlock(playback_mutex);
}

/**
 * ags_playback_get_channel_thread:
 * @playback: the #AgsPlayback
 * @sound_scope: the scope of the thread to get
 * 
 * Get channel thread of appropriate scope.
 * 
 * Returns: (transfer full): the matching #AgsThread or %NULL
 * 
 * Since: 3.0.0
 */
AgsThread*
ags_playback_get_channel_thread(AgsPlayback *playback,
				gint sound_scope)
{
  AgsThread *channel_thread;
  
  GRecMutex *playback_mutex;

  if(!AGS_IS_PLAYBACK(playback) ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* get channel thread */
  g_rec_mutex_lock(playback_mutex);

  channel_thread = (playback->channel_thread != NULL) ? playback->channel_thread[sound_scope]: NULL;

  if(channel_thread != NULL){
    g_object_ref(channel_thread);
  }
  
  g_rec_mutex_unlock(playback_mutex);
  
  return(channel_thread);
}

/**
 * ags_playback_set_recall_id:
 * @playback: the #AgsPlayback
 * @recall_id: the #AgsRecallID
 * @sound_scope: the scope of the recall id to set
 * 
 * Set recall id of appropriate scope.
 * 
 * Since: 3.0.0
 */
void
ags_playback_set_recall_id(AgsPlayback *playback,
			   AgsRecallID *recall_id,
			   gint sound_scope)
{
  GRecMutex *playback_mutex;
  
  if(!AGS_IS_PLAYBACK(playback) ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* unref old */
  g_rec_mutex_lock(playback_mutex);

  if(playback->recall_id[sound_scope] != NULL){
    g_object_unref(playback->recall_id[sound_scope]);
  }

  /* ref new */
  if(recall_id != NULL){
    g_object_ref(recall_id);
  }

  /* set recall id */
  playback->recall_id[sound_scope] = recall_id;

  g_rec_mutex_unlock(playback_mutex);
}

/**
 * ags_playback_get_recall_id:
 * @playback: the #AgsPlayback
 * @sound_scope: the scope of the recall id to get
 * 
 * Get recall id of appropriate scope.
 * 
 * Returns: (transfer full): the matching #AgsRecallID or %NULL
 * 
 * Since: 3.0.0
 */
AgsRecallID*
ags_playback_get_recall_id(AgsPlayback *playback,
			   gint sound_scope)
{
  AgsRecallID *recall_id;

  GRecMutex *playback_mutex;
  
  if(!AGS_IS_PLAYBACK(playback) ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  /* get playback mutex */
  playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback);

  /* get recall id */
  g_rec_mutex_lock(playback_mutex);

  if(playback->recall_id == NULL){
    g_rec_mutex_unlock(playback_mutex);
    
    return(NULL);
  }
  
  recall_id = playback->recall_id[sound_scope];

  if(recall_id != NULL){
    g_object_ref(recall_id);
  }

  g_rec_mutex_unlock(playback_mutex);
  
  return(recall_id);
}

/**
 * ags_playback_find_channel:
 * @playback: (element-type AgsAudio.Playback) (transfer none): the #GList-struct containing #AgsPlayback
 * @channel: the #AgsChannel
 * 
 * Find @channel in @playback.
 *
 * Returns: (transfer none): the matching playback
 *
 * Since: 3.0.0
 */
AgsPlayback*
ags_playback_find_channel(GList *playback,
			  GObject *channel)
{
  GRecMutex *playback_mutex;

  while(playback != NULL){
    /* get playback mutex */
    playback_mutex = AGS_PLAYBACK_GET_OBJ_MUTEX(playback->data);

    /* check channel */
    g_rec_mutex_lock(playback_mutex);
    
    if(AGS_PLAYBACK(playback->data)->channel == channel){
      g_rec_mutex_unlock(playback_mutex);
      
      return(playback->data);
    }

    g_rec_mutex_unlock(playback_mutex);

    /* iterate */
    playback = playback->next;
  }

  return(NULL);
}

/**
 * ags_playback_new:
 * @channel: the #AgsChannel
 *
 * Instantiate a playback object and assign @channel.
 *
 * Returns: the new #AgsPlayback
 *
 * Since: 3.0.0
 */
AgsPlayback*
ags_playback_new(GObject *channel)
{
  AgsPlayback *playback;

  playback = (AgsPlayback *) g_object_new(AGS_TYPE_PLAYBACK,
					  "channel", channel,
					  NULL);
  
  return(playback);
}
