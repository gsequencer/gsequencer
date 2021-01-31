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

#include <ags/audio/fx/ags_fx_playback_channel_processor.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>
#include <ags/audio/fx/ags_fx_playback_recycling.h>

#include <ags/i18n.h>

void ags_fx_playback_channel_processor_class_init(AgsFxPlaybackChannelProcessorClass *fx_playback_channel_processor);
void ags_fx_playback_channel_processor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fx_playback_channel_processor_init(AgsFxPlaybackChannelProcessor *fx_playback_channel_processor);
void ags_fx_playback_channel_processor_dispose(GObject *gobject);
void ags_fx_playback_channel_processor_finalize(GObject *gobject);

void ags_fx_playback_channel_processor_connect(AgsConnectable *connectable);
void ags_fx_playback_channel_processor_disconnect(AgsConnectable *connectable);

void ags_fx_playback_channel_processor_source_recycling_changed_callback(AgsChannel *channel,
									 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
									 AgsRecycling *new_start_region, AgsRecycling *new_end_region,
									 AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
									 AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
									 AgsFxPlaybackChannelProcessor *fx_playback_channel_processor);

void ags_fx_playback_channel_processor_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_playback_channel_processor
 * @short_description: fx playback channel processor
 * @title: AgsFxPlaybackChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_channel_processor.h
 *
 * The #AgsFxPlaybackChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_channel_processor_parent_class = NULL;
static AgsConnectableInterface* ags_fx_playback_channel_processor_parent_connectable_interface;

const gchar *ags_fx_playback_channel_processor_plugin_name = "ags-fx-playback";

GType
ags_fx_playback_channel_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_playback_channel_processor = 0;

    static const GTypeInfo ags_fx_playback_channel_processor_info = {
      sizeof (AgsFxPlaybackChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxPlaybackChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_channel_processor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fx_playback_channel_processor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fx_playback_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								    "AgsFxPlaybackChannelProcessor",
								    &ags_fx_playback_channel_processor_info,
								    0);
    
    g_type_add_interface_static(ags_type_fx_playback_channel_processor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_playback_channel_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_playback_channel_processor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fx_playback_channel_processor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fx_playback_channel_processor_connect;
  connectable->disconnect = ags_fx_playback_channel_processor_disconnect;
}

void
ags_fx_playback_channel_processor_class_init(AgsFxPlaybackChannelProcessorClass *fx_playback_channel_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_playback_channel_processor_parent_class = g_type_class_peek_parent(fx_playback_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_channel_processor;

  gobject->dispose = ags_fx_playback_channel_processor_dispose;
  gobject->finalize = ags_fx_playback_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_playback_channel_processor;
  
  recall->run_inter = ags_fx_playback_channel_processor_real_run_inter;
}

void
ags_fx_playback_channel_processor_init(AgsFxPlaybackChannelProcessor *fx_playback_channel_processor)
{
  AGS_RECALL(fx_playback_channel_processor)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_channel_processor)->xml_type = "ags-fx-playback-channel-processor";

  AGS_RECALL(fx_playback_channel_processor)->child_type = AGS_TYPE_FX_PLAYBACK_RECYCLING;
}

void
ags_fx_playback_channel_processor_dispose(GObject *gobject)
{
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  
  fx_playback_channel_processor = AGS_FX_PLAYBACK_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_playback_channel_processor_finalize(GObject *gobject)
{
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  
  fx_playback_channel_processor = AGS_FX_PLAYBACK_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_playback_channel_processor_connect(AgsConnectable *connectable)
{
  AgsChannel *source;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_fx_playback_channel_processor_parent_connectable_interface->connect(connectable);

  /* recall channel run */
  fx_playback_channel_processor = AGS_FX_PLAYBACK_CHANNEL_PROCESSOR(connectable);
  
  /* get some fields */
  g_object_get(fx_playback_channel_processor,
	       "source", &source,
	       NULL);

  /* source */
  if(source != NULL){
    g_signal_connect(source, "recycling-changed",
		     G_CALLBACK(ags_fx_playback_channel_processor_source_recycling_changed_callback), fx_playback_channel_processor);

    g_object_unref(source);
  }
}

void
ags_fx_playback_channel_processor_disconnect(AgsConnectable *connectable)
{
  AgsChannel *source;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_fx_playback_channel_processor_parent_connectable_interface->disconnect(connectable);

  /* recall channel run */
  fx_playback_channel_processor = AGS_FX_PLAYBACK_CHANNEL_PROCESSOR(connectable);
  
  /* get some fields */
  g_object_get(fx_playback_channel_processor,
	       "source", &source,
	       NULL);

  /* source */
  if(source != NULL){
    g_object_disconnect(source,
			"any_signal::recycling-changed",
			G_CALLBACK(ags_fx_playback_channel_processor_source_recycling_changed_callback),
			fx_playback_channel_processor,
			NULL);

    g_object_unref(source);
  }
}

void
ags_fx_playback_channel_processor_source_recycling_changed_callback(AgsChannel *channel,
								    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
								    AgsRecycling *new_start_region, AgsRecycling *new_end_region,
								    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
								    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
								    AgsFxPlaybackChannelProcessor *fx_playback_channel_processor)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsRecycling *source_recycling, *source_next_recycling, *source_end_recycling;

  GList *start_list, *list;

  fx_playback_audio = NULL;
  
  g_object_get(fx_playback_channel_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);

  /* remove old */
  if(old_start_changed_region != NULL){
    /* get end recycling */
    source_end_recycling = ags_recycling_next(old_end_changed_region);

    /*  */
    source_recycling = old_start_changed_region;
    g_object_ref(source_recycling);
    
    while(source_recycling != source_end_recycling){
      /* get children */
      start_list = NULL;
      
      g_object_get(fx_playback_channel_processor,
		   "child", &start_list,
		   NULL);

      list = start_list;
      
      while(list != NULL){
	AgsRecycling *current_source;
	AgsRecall *current_recall;

	current_recall = AGS_RECALL(list->data);

	current_source = NULL;
	
	g_object_get(current_recall,
		     "source", &current_source,
		     NULL);
	
	if(current_source == source_recycling){
	  GList *start_audio_signal, *audio_signal;

	  start_audio_signal = NULL;
	  
	  g_object_get(current_source,
		       "audio-signal", &start_audio_signal,
		       NULL);

	  audio_signal = start_audio_signal;
	  
	  while(audio_signal != NULL){
	    if(ags_audio_signal_test_flags(audio_signal->data, AGS_AUDIO_SIGNAL_MASTER)){
	      ags_fx_playback_audio_remove_master_audio_signal(fx_playback_audio,
							       audio_signal->data);    
	    }else if(ags_audio_signal_test_flags(audio_signal->data, AGS_AUDIO_SIGNAL_FEED)){
	      ags_fx_playback_audio_remove_feed_audio_signal(fx_playback_audio,
							     audio_signal->data);
	    }

	    audio_signal = audio_signal->next;
	  }	  

	  g_list_free_full(start_audio_signal,
			   (GDestroyNotify) g_object_unref);
	}

	list = list->next;
      }

      g_list_free_full(start_list,
		       (GDestroyNotify) g_object_unref);

      /* iterate */
      source_next_recycling = ags_recycling_next(source_recycling);

      g_object_unref(source_recycling);

      source_recycling = source_next_recycling;
    }

    /* unref */
    if(source_recycling != NULL){
      g_object_unref(source_recycling);
    }

    if(source_end_recycling != NULL){
      g_object_unref(source_end_recycling);
    }
  }
}

void
ags_fx_playback_channel_processor_real_run_inter(AgsRecall *recall)
{
  AgsChannel *source;
  
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsRecallID *recall_id;

  g_object_get(recall,
	       "source", &source,
	       "recall-id", &recall_id,
	       "recall-audio", &fx_playback_audio,
	       NULL);
  
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_PLAYBACK)){
    GList *start_feed_audio_signal, *feed_audio_signal;
    
    gboolean done;
    
    feed_audio_signal = 
      start_feed_audio_signal = ags_fx_playback_audio_get_feed_audio_signal(fx_playback_audio);

    done = TRUE;
    
    while(done && feed_audio_signal != NULL){
      AgsChannel *channel;
      AgsRecycling *recycling;      

      channel = NULL;
      recycling = NULL;
      
      g_object_get(feed_audio_signal->data,
		   "recycling", &recycling,
		   NULL);

      g_object_get(recycling,
		   "channel", &channel,
		   NULL);

      if(channel == source){
	done = FALSE;
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      if(recycling != NULL){
	g_object_unref(recycling);
      }
      
      feed_audio_signal = feed_audio_signal->next;
    }

    if(done){
      ags_recall_done(recall);
    }
    
    g_list_free_full(start_feed_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  if(source != NULL){
    g_object_unref(source);
  }
  
  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_playback_channel_processor_parent_class)->run_inter(recall);
}

/**
 * ags_fx_playback_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxPlaybackChannelProcessor
 *
 * Returns: the new #AgsFxPlaybackChannelProcessor
 *
 * Since: 3.3.0
 */
AgsFxPlaybackChannelProcessor*
ags_fx_playback_channel_processor_new(AgsChannel *channel)
{
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;

  fx_playback_channel_processor = (AgsFxPlaybackChannelProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_CHANNEL_PROCESSOR,
										 "source", channel,
										 NULL);

  return(fx_playback_channel_processor);
}
