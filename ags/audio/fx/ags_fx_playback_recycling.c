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

#include <ags/audio/fx/ags_fx_playback_recycling.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>
#include <ags/audio/fx/ags_fx_playback_channel_processor.h>
#include <ags/audio/fx/ags_fx_playback_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_playback_recycling_class_init(AgsFxPlaybackRecyclingClass *fx_playback_recycling);
void ags_fx_playback_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fx_playback_recycling_init(AgsFxPlaybackRecycling *fx_playback_recycling);
void ags_fx_playback_recycling_dispose(GObject *gobject);
void ags_fx_playback_recycling_finalize(GObject *gobject);

void ags_fx_playback_recycling_connect(AgsConnectable *connectable);
void ags_fx_playback_recycling_disconnect(AgsConnectable *connectable);

void ags_fx_playback_recycling_notify_source_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);

void ags_fx_playback_recycling_add_audio_signal_callback(AgsRecycling *recycling,
							 AgsAudioSignal *audio_signal,
							 AgsFxPlaybackRecycling *fx_playback_recycling);
void ags_fx_playback_recycling_remove_audio_signal_callback(AgsRecycling *recycling,
							    AgsAudioSignal *audio_signal,
							    AgsFxPlaybackRecycling *fx_playback_recycling);

/**
 * SECTION:ags_fx_playback_recycling
 * @short_description: fx playback recycling
 * @title: AgsFxPlaybackRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_recycling.h
 *
 * The #AgsFxPlaybackRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_recycling_parent_class = NULL;
static AgsConnectableInterface* ags_fx_playback_recycling_parent_connectable_interface;

const gchar *ags_fx_playback_recycling_plugin_name = "ags-fx-playback";

GType
ags_fx_playback_recycling_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_playback_recycling = 0;

    static const GTypeInfo ags_fx_playback_recycling_info = {
      sizeof (AgsFxPlaybackRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxPlaybackRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fx_playback_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fx_playback_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							    "AgsFxPlaybackRecycling",
							    &ags_fx_playback_recycling_info,
							    0);
    
    g_type_add_interface_static(ags_type_fx_playback_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_playback_recycling);
  }

  return(g_define_type_id__static);
}

void
ags_fx_playback_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fx_playback_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fx_playback_recycling_connect;
  connectable->disconnect = ags_fx_playback_recycling_disconnect;
}

void
ags_fx_playback_recycling_class_init(AgsFxPlaybackRecyclingClass *fx_playback_recycling)
{
  GObjectClass *gobject;

  ags_fx_playback_recycling_parent_class = g_type_class_peek_parent(fx_playback_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_recycling;

  gobject->dispose = ags_fx_playback_recycling_dispose;
  gobject->finalize = ags_fx_playback_recycling_finalize;
}

void
ags_fx_playback_recycling_init(AgsFxPlaybackRecycling *fx_playback_recycling)
{
  AGS_RECALL(fx_playback_recycling)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_recycling)->xml_type = "ags-fx-playback-recycling";

  AGS_RECALL(fx_playback_recycling)->child_type = AGS_TYPE_FX_PLAYBACK_AUDIO_SIGNAL;
}

void
ags_fx_playback_recycling_dispose(GObject *gobject)
{
  AgsFxPlaybackRecycling *fx_playback_recycling;
  
  fx_playback_recycling = AGS_FX_PLAYBACK_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_playback_recycling_finalize(GObject *gobject)
{
  AgsFxPlaybackRecycling *fx_playback_recycling;
  
  fx_playback_recycling = AGS_FX_PLAYBACK_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_recycling_parent_class)->finalize(gobject);
}


void
ags_fx_playback_recycling_connect(AgsConnectable *connectable)
{
  AgsChannel *source;
  AgsFxPlaybackRecycling *fx_playback_recycling;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_fx_playback_recycling_parent_connectable_interface->connect(connectable);

  /* recall channel run */
  fx_playback_recycling = AGS_FX_PLAYBACK_RECYCLING(connectable);

  g_signal_connect(fx_playback_recycling, "notify::source",
		   G_CALLBACK(ags_fx_playback_recycling_notify_source_callback), NULL);
  
  /* get some fields */
  g_object_get(fx_playback_recycling,
	       "source", &source,
	       NULL);

  /* source */
  if(source != NULL){
    g_signal_connect(source, "add-audio-signal",
		     G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback), fx_playback_recycling);
    
    g_signal_connect(source, "remove-audio-signal",
		     G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback), fx_playback_recycling);

    g_object_unref(source);
  }
}

void
ags_fx_playback_recycling_disconnect(AgsConnectable *connectable)
{
  AgsChannel *source;
  AgsFxPlaybackRecycling *fx_playback_recycling;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_fx_playback_recycling_parent_connectable_interface->disconnect(connectable);

  /* recall channel run */
  fx_playback_recycling = AGS_FX_PLAYBACK_RECYCLING(connectable);

  g_object_disconnect(fx_playback_recycling,
		      "any_signal::notify::source",
		      G_CALLBACK(ags_fx_playback_recycling_notify_source_callback),
		      NULL,
		      NULL);
  
  /* get some fields */
  g_object_get(fx_playback_recycling,
	       "source", &source,
	       NULL);

  /* source */
  if(source != NULL){
    g_object_disconnect(source,
			"any_signal::add-audio-signal",
			G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback),
			fx_playback_recycling,
			NULL);
    
    g_object_disconnect(source,
			"any_signal::remove-audio-signal",
			G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback),
			fx_playback_recycling,
			NULL);

    g_object_unref(source);
  }
}

void
ags_fx_playback_recycling_notify_source_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsRecycling *source;

  g_object_get(gobject,
	       "source", &source,
	       NULL);

  if(source == NULL){
    return;
  }

  if(ags_connectable_is_connected(AGS_CONNECTABLE(gobject))){
    g_signal_connect(source, "add-audio-signal",
		     G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback), gobject);
    
    g_signal_connect(source, "remove-audio-signal",
		     G_CALLBACK(ags_fx_playback_recycling_add_audio_signal_callback), gobject);
  }
  
  g_object_unref(source);
}

void
ags_fx_playback_recycling_add_audio_signal_callback(AgsRecycling *recycling,
						    AgsAudioSignal *audio_signal,
						    AgsFxPlaybackRecycling *fx_playback_recycling)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  AgsRecallID *recall_id;

  fx_playback_audio = NULL;

  fx_playback_channel_processor = NULL;
  
  recall_id = NULL;
  
  g_object_get(fx_playback_recycling,
	       "recall-id", &recall_id,
	       NULL);

  if(recall_id == NULL){
    return;
  }

  fx_playback_channel_processor = NULL;

  g_object_get(fx_playback_recycling,
	       "parent", &fx_playback_channel_processor,
	       NULL);

  fx_playback_audio = NULL;

  if(fx_playback_channel_processor != NULL){
    g_object_get(fx_playback_channel_processor,
		 "recall-audio", &fx_playback_audio,
		 NULL);
  }
  
  if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_MASTER)){
    ags_fx_playback_audio_add_master_audio_signal(fx_playback_audio,
						  audio_signal);    
  }else if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_FEED)){
    ags_fx_playback_audio_add_feed_audio_signal(fx_playback_audio,
						audio_signal);
  }
  
  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
  
  if(fx_playback_channel_processor != NULL){
    g_object_unref(fx_playback_channel_processor);
  }
}

void
ags_fx_playback_recycling_remove_audio_signal_callback(AgsRecycling *recycling,
						       AgsAudioSignal *audio_signal,
						       AgsFxPlaybackRecycling *fx_playback_recycling)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  AgsRecallID *recall_id;

  fx_playback_audio = NULL;

  fx_playback_channel_processor = NULL;

  recall_id = NULL;
  
  g_object_get(fx_playback_recycling,
	       "recall-id", &recall_id,
	       NULL);

  if(recall_id == NULL){
    return;
  }

  fx_playback_channel_processor = NULL;
  
  g_object_get(fx_playback_recycling,
	       "parent", &fx_playback_channel_processor,
	       NULL);

  fx_playback_audio = NULL;

  if(fx_playback_channel_processor != NULL){
    g_object_get(fx_playback_channel_processor,
		 "recall-audio", &fx_playback_audio,
		 NULL);
  }
  
  if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_MASTER)){
    ags_fx_playback_audio_remove_master_audio_signal(fx_playback_audio,
						     audio_signal);    
  }else if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_FEED)){
    ags_fx_playback_audio_remove_feed_audio_signal(fx_playback_audio,
						   audio_signal);
  }
  
  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
  
  if(fx_playback_channel_processor != NULL){
    g_object_unref(fx_playback_channel_processor);
  }
}

/**
 * ags_fx_playback_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxPlaybackRecycling
 *
 * Returns: the new #AgsFxPlaybackRecycling
 *
 * Since: 3.3.0
 */
AgsFxPlaybackRecycling*
ags_fx_playback_recycling_new(AgsRecycling *recycling)
{
  AgsFxPlaybackRecycling *fx_playback_recycling;

  fx_playback_recycling = (AgsFxPlaybackRecycling *) g_object_new(AGS_TYPE_FX_PLAYBACK_RECYCLING,
								  "source", recycling,
								  NULL);

  return(fx_playback_recycling);
}
