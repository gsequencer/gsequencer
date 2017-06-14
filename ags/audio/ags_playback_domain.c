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

#include <ags/audio/ags_playback_domain.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_thread.h>

#include <math.h>

#include <ags/i18n.h>

void ags_playback_domain_class_init(AgsPlaybackDomainClass *playback_domain);
void ags_playback_domain_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playback_domain_init(AgsPlaybackDomain *playback_domain);
void ags_playback_domain_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_playback_domain_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_playback_domain_disconnect(AgsConnectable *connectable);
void ags_playback_domain_connect(AgsConnectable *connectable);
void ags_playback_domain_dispose(GObject *gobject);
void ags_playback_domain_finalize(GObject *gobject);

/**
 * SECTION:ags_playback_domain
 * @short_description: Outputting to soundcard domain
 * @title: AgsPlaybackDomain
 * @section_id:
 * @include: ags/audio/ags_playback_domain.h
 *
 * #AgsPlaybackDomain represents a domain to output.
 */

static gpointer ags_playback_domain_parent_class = NULL;

enum{
  PROP_0,
  PROP_DOMAIN,
  PROP_PLAYBACK,
};

GType
ags_playback_domain_get_type (void)
{
  static GType ags_type_playback_domain = 0;

  if(!ags_type_playback_domain){
    static const GTypeInfo ags_playback_domain_info = {
      sizeof (AgsPlaybackDomainClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_domain_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlaybackDomain),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_domain_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playback_domain_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playback_domain = g_type_register_static(G_TYPE_OBJECT,
						      "AgsPlaybackDomain",
						      &ags_playback_domain_info,
						      0);

    g_type_add_interface_static(ags_type_playback_domain,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_playback_domain);
}

void
ags_playback_domain_class_init(AgsPlaybackDomainClass *playback_domain)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_playback_domain_parent_class = g_type_class_peek_parent(playback_domain);

  /* GObjectClass */
  gobject = (GObjectClass *) playback_domain;

  gobject->set_property = ags_playback_domain_set_property;
  gobject->get_property = ags_playback_domain_get_property;

  gobject->dispose = ags_playback_domain_dispose;
  gobject->finalize = ags_playback_domain_finalize;

  /* properties */
  /**
   * AgsPlaybackDomain:domain:
   *
   * The assigned domain.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("domain",
				   i18n_pspec("assigned domain"),
				   i18n_pspec("The domain it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOMAIN,
				  param_spec);

  /**
   * AgsPlaybackDomain:playback:
   *
   * The assigned playback.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("assigned playback"),
				   i18n_pspec("The playback it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);
}

void
ags_playback_domain_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_playback_domain_connect;
  connectable->disconnect = ags_playback_domain_disconnect;
}

void
ags_playback_domain_init(AgsPlaybackDomain *playback_domain)
{
  AgsConfig *config;

  gchar *str, *str0, *str1;

  gboolean super_threaded_audio;
  
  /* config */
  config = ags_config_get_instance();

  /* thread model */
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model");
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope");

  if(str0 != NULL && str1 != NULL){
    if(!g_ascii_strncasecmp(str0,
			    "super-threaded",
			    15)){
      if(!g_ascii_strncasecmp(str1,
			      "audio",
			      6) ||
	 !g_ascii_strncasecmp(str1,
			      "channel",
			      8) ||
	 !g_ascii_strncasecmp(str1,
			      "recycling",
			      10)){
	super_threaded_audio = TRUE;
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
  if(super_threaded_audio){
    g_atomic_int_set(&(playback_domain->flags),
		     AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO);
  }else{
    g_atomic_int_set(&(playback_domain->flags),
		     0);
  }

  /* domain */
  playback_domain->domain = NULL;

  /* super threaded audio */
  playback_domain->audio_thread = (AgsThread **) malloc(3 * sizeof(AgsThread *));

  playback_domain->audio_thread[0] = NULL;
  playback_domain->audio_thread[1] = NULL;
  playback_domain->audio_thread[2] = NULL;
  
  if(super_threaded_audio){
    guint samplerate, buffer_size;
    gdouble freq;
    
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

    /* samplerate */
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD,
			       "samplerate");

    if(str == NULL){
      str = ags_config_get_value(config,
				 AGS_CONFIG_SOUNDCARD_0,
				 "samplerate");
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
			       "buffer-size");

    if(str == NULL){
      str = ags_config_get_value(config,
				 AGS_CONFIG_SOUNDCARD_0,
				 "buffer-size");
    }
  
    if(str != NULL){
      buffer_size = g_ascii_strtoull(str,
				     NULL,
				     10);

      free(str);
    }

    /* thread frequency */
    freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

    /* playback audio thread */
    playback_domain->audio_thread[0] = (AgsThread *) ags_audio_thread_new(NULL,
									  NULL);
    g_object_ref(playback_domain->audio_thread[0]);
    playback_domain->audio_thread[0]->freq = freq;

    /* sequencer audio thread */
    playback_domain->audio_thread[1] = (AgsThread *) ags_audio_thread_new(NULL,
									  NULL);
    g_object_ref(playback_domain->audio_thread[1]);
    playback_domain->audio_thread[1]->freq = freq;

    /* notation audio thread */
    playback_domain->audio_thread[2] = (AgsThread *) ags_audio_thread_new(NULL,
									  NULL);
    g_object_ref(playback_domain->audio_thread[2]);
    playback_domain->audio_thread[2]->freq = freq;
  }

  /* playback */
  playback_domain->playback = NULL;
}

void
ags_playback_domain_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlaybackDomain *playback_domain;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(gobject);

  switch(prop_id){
  case PROP_DOMAIN:
    {
      GObject *domain;

      domain = (GObject *) g_value_get_object(value);

      if((GObject *) playback_domain->domain == domain){
	return;
      }

      if(playback_domain->domain != NULL){
	g_object_unref(G_OBJECT(playback_domain->domain));
      }

      if(domain != NULL){
	g_object_ref(G_OBJECT(domain));

	if(AGS_IS_AUDIO(domain) &&
	   (AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO & (g_atomic_int_get(&(playback_domain->flags)))) != 0){
	  gdouble freq;
	  
	  /* thread frequency */
	  freq = ceil((gdouble) AGS_AUDIO(domain)->samplerate / (gdouble) AGS_AUDIO(domain)->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

	  g_object_set(playback_domain->audio_thread[0],
		       "frequency", freq,
		       "audio", domain,
		       NULL);

	  g_object_set(playback_domain->audio_thread[1],
		       "frequency", freq,
		       "audio", domain,
		       NULL);

	  g_object_set(playback_domain->audio_thread[2],
		       "frequency", freq,
		       "audio", domain,
		       NULL);
	}
      }

      playback_domain->domain = (GObject *) domain;
    }
    break;
  case PROP_PLAYBACK:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_object(value);

      if(playback == NULL ||
	 g_list_find(playback_domain->playback, playback) != NULL){
	return;
      }

      ags_playback_domain_add_playback(playback_domain,
				       (GObject *) playback);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_domain_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = AGS_PLAYBACK_DOMAIN(gobject);

  switch(prop_id){
  case PROP_DOMAIN:
    {
      g_value_set_object(value,
			 playback_domain->domain);
    }
    break;
  case PROP_PLAYBACK:
    {
      g_value_set_pointer(value,
			  g_list_copy(playback_domain->playback));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_playback_domain_dispose(GObject *gobject)
{
  AgsPlaybackDomain *playback_domain;

  guint i;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(gobject);
  
  if(playback_domain->audio_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback_domain->audio_thread[i] != NULL){
	g_object_run_dispose(playback_domain->audio_thread[i]);
	g_object_unref(playback_domain->audio_thread[i]);

	playback_domain->audio_thread[i] = NULL;
      }
    }
  }

  /* domain */
  if(playback_domain->domain != NULL){
    g_object_unref(playback_domain->domain);

    playback_domain->domain = NULL;
  }

  /* playback */
  if(playback_domain->playback != NULL){
    g_list_free_full(playback_domain->playback,
		     g_object_unref);
    
    playback_domain->playback = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_playback_domain_parent_class)->dispose(gobject);
}

void
ags_playback_domain_finalize(GObject *gobject)
{
  AgsPlaybackDomain *playback_domain;

  guint i;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(gobject);

  /* audio thread */
  if(playback_domain->audio_thread != NULL){
    for(i = 0; i < 3; i++){
      if(playback_domain->audio_thread[i] != NULL){
	g_object_unref(playback_domain->audio_thread[i]);
      }
    }
    
    free(playback_domain->audio_thread);
  }

  /* domain */
  if(playback_domain->domain != NULL){
    g_object_unref(playback_domain->domain);
  }
  
  /* playback */
  g_list_free_full(playback_domain->playback,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_playback_domain_parent_class)->finalize(gobject);
}

void
ags_playback_domain_connect(AgsConnectable *connectable)
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = AGS_PLAYBACK_DOMAIN(connectable);

  if((AGS_PLAYBACK_DOMAIN_CONNECTED & (playback_domain->flags)) != 0){
    return;
  }

  playback_domain->flags |= AGS_PLAYBACK_DOMAIN_CONNECTED;
}

void
ags_playback_domain_disconnect(AgsConnectable *connectable)
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = AGS_PLAYBACK_DOMAIN(connectable);


  if((AGS_PLAYBACK_DOMAIN_CONNECTED & (playback_domain->flags)) == 0){
    return;
  }

  playback_domain->flags &= (~AGS_PLAYBACK_DOMAIN_CONNECTED);
}

/**
 * ags_playback_domain_set_audio_thread:
 * @playback_domain: the #AgsPlaybackDomain
 * @thread: the #AgsThread
 * @scope: the thread's scope
 * 
 * Set audio thread to specified scope.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_domain_set_audio_thread(AgsPlaybackDomain *playback_domain,
				     AgsThread *thread,
				     guint scope)
{
  if(playback_domain == NULL ||
     scope > 2){
    return;
  }

  if(playback_domain->audio_thread[scope] != NULL){
    g_object_unref(playback_domain->audio_thread[scope]);
  }

  if(thread != NULL){
    g_object_ref(thread);
  }
  
  playback_domain->audio_thread[scope] = thread;
}

/**
 * ags_playback_domain_set_audio_thread:
 * @playback_domain: the #AgsPlaybackDomain
 * @scope: the thread's scope
 * 
 * Get audio thread of specified scope.
 * 
 * Returns: the matching #AgsThread or %NULL
 * 
 * Since: 0.7.122.7
 */
AgsThread*
ags_playback_domain_get_audio_thread(AgsPlaybackDomain *playback_domain,
				     guint scope)
{
  if(playback_domain == NULL ||
     playback_domain->audio_thread == NULL ||
     scope > 2){
    return(NULL);
  }

  return(playback_domain->audio_thread[scope]);
}


/**
 * ags_playback_domain_add_playback:
 * @playback_domain: the #AgsPlaybackDomain
 * @playback: the #AgsPlayback
 * 
 * Add @playback to @playback_domain.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_domain_add_playback(AgsPlaybackDomain *playback_domain,
				 GObject *playback)
{
  if(playback_domain == NULL ||
     playback == NULL){
    return;
  }

  //TODO:JK: rather use prepend but needs refactoring
  g_object_ref(playback);
  playback_domain->playback = g_list_append(playback_domain->playback,
					    playback);
}

/**
 * ags_playback_domain_remove_playback:
 * @playback_domain: the #AgsPlaybackDomain
 * @playback: the #AgsPlayback
 * 
 * Remove @playback of @playback_domain.
 * 
 * Since: 0.7.122.7
 */
void
ags_playback_domain_remove_playback(AgsPlaybackDomain *playback_domain,
				    GObject *playback)
{
  if(playback_domain == NULL ||
     playback == NULL){
    return;
  }

  playback_domain->playback = g_list_remove(playback_domain->playback,
					    playback);
  g_object_unref(playback);
}

/**
 * ags_playback_domain_new:
 *
 * Creates an #AgsPlaybackDomain, refering to @application_context.
 *
 * Returns: a new #AgsPlaybackDomain
 *
 * Since: 0.4
 */
AgsPlaybackDomain*
ags_playback_domain_new()
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = (AgsPlaybackDomain *) g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
						       NULL);
  
  return(playback_domain);
}
