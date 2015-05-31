/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_audio.h>

#include <ags/lib/ags_list.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_task_thread.h>

#include <ags/server/ags_server_application_context.h>
#include <ags/server/ags_server.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/task/ags_audio_set_recycling.h>

#include <stdlib.h>
#include <stdio.h>

/**
 * SECTION:ags_audio
 * @short_description: A container of channels organizing them as input or output
 * @title: AgsAudio
 * @section_id:
 * @include: ags/audio/ags_audio.h
 *
 * #AgsAudio organizes #AgsChannel objects either as input or output and
 * is responsible of their alignment. The class can contain #AgsRecall objects
 * in order to perform computation on all channels or in audio context.
 * Therefor exists #AgsRecyclingContext acting as tree context.
 *
 * At least one #AgsRecallID is assigned to it and has one more if
 * %AGS_AUDIO_OUTPUT_HAS_RECYCLING is set as flag.
 *
 * If %AGS_AUDIO_HAS_NOTATION is set as flag one #AgsNotation is allocated per audio
 * channel.
 */

void ags_audio_class_init(AgsAudioClass *audio_class);
void ags_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_init(AgsAudio *audio);
void ags_audio_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_audio_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_audio_add_to_registry(AgsConnectable *connectable);
void ags_audio_remove_from_registry(AgsConnectable *connectable);
void ags_audio_connect(AgsConnectable *connectable);
void ags_audio_disconnect(AgsConnectable *connectable);
void ags_audio_finalize(GObject *gobject);

void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);
void ags_audio_real_set_pads(AgsAudio *audio,
			     GType type,
			     guint channels, guint channels_old);

enum{
  SET_AUDIO_CHANNELS,
  SET_PADS,
  SET_LINES,
  INIT_RUN,
  TACT,
  DONE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_AUDIO_CHANNELS,
  PROP_INPUT_PADS,
  PROP_INPUT_LINES,
  PROP_OUTPUT_PADS,
  PROP_OUTPUT_LINES,
  PROP_OUTPUT,
  PROP_INPUT,
  PROP_PLAYBACK_DOMAIN,
  PROP_NOTATION,
  PROP_AUTOMATION,
  PROP_RECALL_ID,
  PROP_RECYCLING_CONTEXT,
  PROP_RECALL_CONTAINER,
  PROP_PLAY,
  PROP_RECALL,
};

static gpointer ags_audio_parent_class = NULL;

static guint audio_signals[LAST_SIGNAL];

GType
ags_audio_get_type (void)
{
  static GType ags_type_audio = 0;

  if(!ags_type_audio){
    static const GTypeInfo ags_audio_info = {
      sizeof(AgsAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio = g_type_register_static(G_TYPE_OBJECT,
					    "AgsAudio\0", &ags_audio_info,
					    0);

    g_type_add_interface_static(ags_type_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_audio);
}

void
ags_audio_class_init(AgsAudioClass *audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_parent_class = g_type_class_peek_parent(audio);

  /* GObjectClass */
  gobject = (GObjectClass *) audio;

  gobject->set_property = ags_audio_set_property;
  gobject->get_property = ags_audio_get_property;

  gobject->finalize = ags_audio_finalize;

  /* properties */
  /**
   * AgsAudio:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "assigned soundcard\0",
				   "The soundcard it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudio:audio-channels:
   *
   * The audio channels count.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_uint("audio-channels\0",
				  "audio channels count\0",
				  "The count of audio channels of audio\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);


  /**
   * AgsAudio:input-pads:
   *
   * The input pads count.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_uint("input-pads\0",
				  "input pads count\0",
				  "The count of input pads of audio\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_PADS,
				  param_spec);

  /**
   * AgsAudio:input-lines:
   *
   * The input lines count.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_uint("input-lines\0",
				  "input lines count\0",
				  "The count of input lines of audio\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_LINES,
				  param_spec);

  /**
   * AgsAudio:output-pads:
   *
   * The output pads count.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_uint("output-pads\0",
				  "output pads count\0",
				  "The count of output pads of audio\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_PADS,
				  param_spec);

  /**
   * AgsAudio:output-lines:
   *
   * The output lines count.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_uint("output-lines\0",
				  "output lines count\0",
				  "The count of output lines of audio\0",
				  0,
				  65535,
				  0,
				  G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_LINES,
				  param_spec);


  /**
   * AgsAudio:output:
   *
   * The #AgsOutput it contains.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("output\0",
				   "containing output\0",
				   "The output it contains\0",
				   AGS_TYPE_OUTPUT,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT,
				  param_spec);
  
  /**
   * AgsAudio:input:
   *
   * The #AgsInput it contains.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("input\0",
				   "containing input\0",
				   "The input it contains\0",
				   AGS_TYPE_INPUT,
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT,
				  param_spec);

  /**
   * AgsAudio:playback-domain:
   *
   * The assigned #AgsPlaybackDomain.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("playback-domain\0",
				   "assigned playback domain\0",
				   "The assigned playback domain\0",
				   AGS_TYPE_PLAYBACK_DOMAIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK_DOMAIN,
				  param_spec);

  /**
   * AgsAudio:notation:
   *
   * The #AgsNotation it contains.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("notation\0",
				   "containing notation\0",
				   "The notation it contains\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /**
   * AgsAudio:automation:
   *
   * The #AgsAutomation it contains.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("automation\0",
				   "containing automation\0",
				   "The automation it contains\0",
				   AGS_TYPE_AUTOMATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);

  /**
   * AgsAudio:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("recall-id\0",
				   "assigned recall id\0",
				   "The assigned recall id\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT,
				  param_spec);
  
  /**
   * AgsAudio:recycling-context:
   *
   * The assigned #AgsRecyclingContext.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("recycling-context\0",
				   "assigned recycling context\0",
				   "The assigned recall id\0",
				   AGS_TYPE_RECYCLING_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT,
				  param_spec);

  /**
   * AgsAudio:recall-container:
   *
   * The #AgsRecallContainer it contains in container-context.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("recall-container\0",
				   "containing recall container\0",
				   "The recall container it contains\0",
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);
  
  /**
   * AgsAudio:play:
   *
   * The #AgsRecall it contains in play-context.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("play\0",
				   "containing play\0",
				   "The play it contains\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /**
   * AgsAudio:recall:
   *
   * The #AgsRecall it contains in recall-context.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("recall\0",
				   "containing recall\0",
				   "The recall it contains\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);
    
  /* AgsAudioClass */
  audio->set_audio_channels = ags_audio_real_set_audio_channels;
  audio->set_pads = ags_audio_real_set_pads;

  audio->init_run = NULL;
  audio->tact = NULL;
  audio->done = NULL;

  /* signals */
  /**
   * AgsAudio::set-audio-channels:
   * @audio: the object to adjust the channels.
   * @audio_channels_new: new audio channel count
   * @audio_channels_old: old audio channel count
   *
   * The ::set-audio-channels signal notifies about changes in channel
   * alignment.
   */
  audio_signals[SET_AUDIO_CHANNELS] = 
    g_signal_new("set-audio-channels\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_audio_channels),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsAudio::set-pads:
   * @audio: the object to adjust pads.
   * @type: either #AGS_TYPE_INPUT or #AGS_TYPE_OUTPUT
   * @pads_new: new pad count
   * @pads_old: old pad count
   *
   * The ::set-pads signal notifies about changes in channel
   * alignment.
   */
  audio_signals[SET_PADS] = 
    g_signal_new("set-pads\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsAudio::init-run:
   * @audio: the object to init run.
   *
   * The ::init-run signal is invoked during dynamic initialization of recalls
   * of @audio.
   *
   * Returns: the current #AgsRecallID
   */
  audio_signals[INIT_RUN] = 
    g_signal_new("init-run\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, init_run),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__VOID,
		 G_TYPE_OBJECT, 0);

  /**
   * AgsAudio::tact:
   * @audio: the object playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::tact signal is invoked during playback of recalls
   * of @audio.
   */
  audio_signals[TACT] = 
    g_signal_new("tact\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, tact),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudio::done:
   * @audio: the object done playing.
   * @recall_id: the appropriate #AgsRecallID
   *
   * The ::done signal is invoked while terminating playback
   * of @audio.
   */
  audio_signals[DONE] = 
    g_signal_new("done\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, done),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_audio_add_to_registry;
  connectable->remove_from_registry = ags_audio_remove_from_registry;

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_audio_connect;
  connectable->disconnect = ags_audio_disconnect;
}

void
ags_audio_init(AgsAudio *audio)
{
  audio->flags = 0;

  audio->soundcard = NULL;

  audio->sequence_length = 0;
  audio->audio_channels = 0;
  audio->frequence = 0;

  audio->output_pads = 0;
  audio->output_lines = 0;
  audio->input_pads = 0;
  audio->input_lines = 0;

  audio->output = NULL;
  audio->input = NULL;

  audio->playback_domain = ags_playback_domain_new();
  AGS_PLAYBACK_DOMAIN(audio->playback_domain)->domain = audio;

  audio->notation = NULL;
  audio->automation = NULL;

  audio->recall_id = NULL;
  audio->recycling_context = NULL;

  audio->container = NULL;
  audio->recall = NULL;
  audio->play = NULL;

  audio->recall_remove= NULL;
  audio->play_remove = NULL;

  audio->machine_widget = NULL;
}

void
ags_audio_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsAudio *audio;

  audio = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_audio_set_soundcard(audio, (GObject *) soundcard);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      guint audio_channels;

      audio_channels = g_value_get_uint(value);

      ags_audio_set_audio_channels(audio,
				   audio_channels);
    }
    break;
  case PROP_INPUT_PADS:
    {
      guint input_pads;

      input_pads = g_value_get_uint(value);

      ags_audio_set_pads(audio, AGS_TYPE_INPUT,
			 input_pads);
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      guint output_pads;

      output_pads = g_value_get_uint(value);

      ags_audio_set_pads(audio, AGS_TYPE_OUTPUT,
			 output_pads);
    }
    break;
  case PROP_PLAYBACK_DOMAIN:
    {
      AgsPlaybackDomain *playback_domain;

      playback_domain = (AgsPlaybackDomain *) g_value_get_object(value);

      if(audio->playback_domain == playback_domain){
	return;
      }

      if(audio->playback_domain != NULL){
	g_object_unref(audio->playback_domain);
      }
      
      if(playback_domain != NULL){
	g_object_ref(playback_domain);
      }

      audio->playback_domain = playback_domain;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(notation == NULL ||
	 g_list_find(audio->notation, notation) != NULL){
	return;
      }

      ags_audio_add_notation(audio,
			     notation);
    }
    break;
  case PROP_AUTOMATION:
    {
      AgsAutomation *automation;

      automation = (AgsAutomation *) g_value_get_object(value);

      if(automation == NULL ||
	 g_list_find(audio->automation, automation) != NULL){
	return;
      }

      ags_audio_add_automation(audio,
			       automation);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(recall_id == NULL ||
	 g_list_find(audio->recall_id, recall_id) != NULL){
	return;
      }

      ags_audio_add_recall_id(audio,
			      recall_id);
    }
    break;
  case PROP_RECYCLING_CONTEXT:
    {
      AgsRecyclingContext *recycling_context;

      recycling_context = (AgsRecyclingContext *) g_value_get_object(value);

      if(recycling_context == NULL ||
	 g_list_find(audio->recycling_context, recycling_context) != NULL){
	return;
      }

      ags_audio_add_recycling_context(audio,
				      recycling_context);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      if(recall_container == NULL ||
	 g_list_find(audio->container, recall_container) != NULL){
	return;
      }

      ags_audio_add_recall_container(audio,
				     recall_container);
    }
    break;
  case PROP_PLAY:
    {
      AgsRecall *recall;

      recall = (AgsRecall *) g_value_get_object(value);

      if(recall == NULL ||
	 g_list_find(audio->play, recall) != NULL){
	return;
      }

      ags_audio_add_recall(audio,
			   recall,
			   TRUE);
    }
    break;
  case PROP_RECALL:
    {
      AgsRecall *recall;

      recall = (AgsRecall *) g_value_get_object(value);

      if(recall == NULL ||
	 g_list_find(audio->recall, recall) != NULL){
	return;
      }

      ags_audio_add_recall(audio,
			   recall,
			   FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsAudio *audio;

  audio = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, audio->soundcard);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, audio->audio_channels);
    }
    break;
  case PROP_INPUT_PADS:
    {
      g_value_set_uint(value, audio->input_pads);
    }
    break;
  case PROP_INPUT_LINES:
    {
      g_value_set_uint(value, audio->input_lines);
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      g_value_set_uint(value, audio->output_pads);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      g_value_set_uint(value, audio->output_lines);
    }
    break;
  case PROP_INPUT:
    {
      g_value_set_object(value, audio->input);
    }
    break;
  case PROP_OUTPUT:
    {
      g_value_set_object(value, audio->output);
    }
    break;
  case PROP_PLAYBACK_DOMAIN:
    {
      g_value_set_object(value, audio->playback_domain);
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_pointer(value, g_list_copy(audio->notation));
    }
    break;
  case PROP_AUTOMATION:
    {
      g_value_set_pointer(value, g_list_copy(audio->automation));
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_pointer(value, g_list_copy(audio->recall_id));
    }
    break;
  case PROP_RECYCLING_CONTEXT:
    {
      g_value_set_pointer(value, g_list_copy(audio->recycling_context));
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_value_set_pointer(value, g_list_copy(audio->container));
    }
    break;
  case PROP_PLAY:
    {
      g_value_set_pointer(value, g_list_copy(audio->play));
    }
    break;
  case PROP_RECALL:
    {
      g_value_set_pointer(value, g_list_copy(audio->recall));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;

  audio = AGS_AUDIO(gobject);

  if(audio->soundcard != NULL)
    g_object_unref(audio->soundcard);

  /* output */
  channel = audio->output;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref((GObject *) channel);
  }

  /* input */
  channel = audio->input;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref(channel);
  }

  /* free some lists */
  ags_list_free_and_unref_link(audio->notation);

  ags_list_free_and_unref_link(audio->recall_id);

  ags_list_free_and_unref_link(audio->recall);
  ags_list_free_and_unref_link(audio->play);

  ags_list_free_and_unref_link(audio->recall_remove);
  ags_list_free_and_unref_link(audio->play_remove);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

void
ags_audio_add_to_registry(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  AgsServer *server;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRegistryEntry *entry;
  GList *list;
  
  audio = AGS_AUDIO(connectable);

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(audio->soundcard));

  server = NULL;
  list = application_context->sibling;

  while(list != NULL){
    if(AGS_IS_SERVER_APPLICATION_CONTEXT(list->data)){
      server = AGS_SERVER_APPLICATION_CONTEXT(list->data)->server;
      break;
    }

    list = list->next;
  }
  
  entry = ags_registry_entry_alloc(server->registry);
  g_value_set_object(&(entry->entry),
		     (gpointer) audio);
  ags_registry_add(server->registry,
		   entry);

  /* add play */
  list = audio->play;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
  
  /* add recall */
  list = audio->recall;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* add output */
  channel = audio->output;

  while(channel != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  /* add input */
  channel = audio->input;

  while(channel != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }
}

void
ags_audio_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_audio_connect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;
  GList *list;

  audio = AGS_AUDIO(connectable);

  if((AGS_AUDIO_CONNECTED & (audio->flags)) != 0){
    return;
  }
  
#ifdef AGS_DEBUG
  g_message("connecting audio\0");
#endif

  audio->flags |= AGS_AUDIO_CONNECTED;

  /* connect channels */
  channel = audio->output;

  while(channel != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  /* connect recall ids */
  list = audio->recall_id;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = audio->container;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = audio->recall;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect remove recalls */
  list = audio->recall_remove;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play_remove;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect notation */
  list = audio->notation;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_audio_disconnect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;
  GList *list;

  audio = AGS_AUDIO(connectable);
  
  if((AGS_AUDIO_CONNECTED & (audio->flags)) == 0){
    return;
  }

  audio->flags &= (~AGS_AUDIO_CONNECTED);

  /* disconnect channels */
  channel = audio->output;

  while(channel != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  /* disconnect recall ids */
  list = audio->recall_id;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* disconnect recall containers */
  list = audio->container;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* disconnect recalls */
  list = audio->recall;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* disconnect remove recalls */
  list = audio->recall_remove;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play_remove;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* disconnect notation */
  list = audio->notation;
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

/**
 * ags_audio_set_flags:
 * @audio: an AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Enable a feature of AgsAudio.
 *
 * Since: 0.4
 */
void
ags_audio_set_flags(AgsAudio *audio, guint flags)
{
  auto GParameter* ags_audio_set_flags_set_recycling_parameter(GType type);
  auto void ags_audio_set_flags_add_recycling_task(GParameter *parameter);

  GParameter* ags_audio_set_flags_set_recycling_parameter(GType type){
    AgsChannel *channel, *start_channel, *end_channel;
    AgsRecycling *recycling, *recycling_next, *start_recycling, *end_recycling;
    GParameter *parameter;
    int i;

    parameter = g_new(GParameter, 4 * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      start_channel =
	channel = ags_channel_nth(((g_type_is_a(type, AGS_TYPE_INPUT)) ? audio->input: audio->output), i);
      end_channel = NULL;

      start_recycling =
	recycling = NULL;
      end_recycling = NULL;
	  
      if(channel != NULL){
	start_recycling = 
	  recycling = ags_recycling_new(audio->soundcard);
	goto ags_audio_set_flags_OUTPUT_RECYCLING;
      }

      while(channel != NULL){
	recycling->next = ags_recycling_new(audio->soundcard);
      ags_audio_set_flags_OUTPUT_RECYCLING:
	recycling->next->prev = recycling;
	recycling = recycling->next;
	    	    
	channel = channel->next_pad;
      }

      end_channel = ags_channel_pad_last(start_channel);
      end_recycling = recycling;

      /* setting up parameters */
      parameter[i].name = "start_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_channel);

      parameter[i].name = "end_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_channel);

      parameter[i].name = "start_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_recycling);

      parameter[i].name = "end_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_recycling);
    }

    return(parameter);
  }
  void ags_audio_set_flags_add_recycling_task(GParameter *parameter){
    AgsAudioSetRecycling *audio_set_recycling;
    AgsThread *main_loop, *current;
    AgsTaskThread *task_thread;
    AgsApplicationContext *application_context;

    application_context = ags_soundcard_get_application_context(audio->soundcard);
    main_loop = application_context->main_loop;
    task_thread = ags_thread_find_type(main_loop,
				       AGS_TYPE_TASK_THREAD);
    
    /* create set recycling task */
    audio_set_recycling = ags_audio_set_recycling_new(audio,
						      parameter);

    /* append AgsAudioSetRecycling */
    ags_task_thread_append_task(task_thread,
				AGS_TASK(audio_set_recycling));
  }

  if(audio == NULL || !AGS_IS_AUDIO(audio)){
    return;
  }

  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags))){
    GParameter *parameter;
        
    /* check if output has already recyclings */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if(audio->output_pads > 0){
	parameter = ags_audio_set_flags_set_recycling_parameter(AGS_TYPE_OUTPUT);
	ags_audio_set_flags_add_recycling_task(parameter);
      }
      
      audio->flags |= AGS_AUDIO_OUTPUT_HAS_RECYCLING;
    }

    /* check if input has already recyclings */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      if(audio->input_pads > 0){
	parameter = ags_audio_set_flags_set_recycling_parameter(AGS_TYPE_INPUT);
	ags_audio_set_flags_add_recycling_task(parameter);
      }

      audio->flags |= AGS_AUDIO_INPUT_HAS_RECYCLING;
    }
  }

  //TODO:JK: automatization of setting recycling_context root
}
    

/**
 * ags_audio_unset_flags:
 * @audio: an AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Disable a feature of AgsAudio.
 *
 * Since: 0.4
 */
void
ags_audio_unset_flags(AgsAudio *audio, guint flags)
{
  auto GParameter* ags_audio_unset_flags_set_recycling_parameter(GType type);
  auto void ags_audio_unset_flags_add_recycling_task(GParameter *parameter);

  GParameter* ags_audio_unset_flags_set_recycling_parameter(GType type){
    AgsChannel *channel, *start_channel, *end_channel;
    AgsRecycling *recycling, *recycling_next, *start_recycling, *end_recycling;
    GParameter *parameter;
    int i;

    parameter = g_new(GParameter, 4 * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      start_channel = ags_channel_nth(((g_type_is_a(type, AGS_TYPE_INPUT)) ? audio->input: audio->output), i);
      end_channel = ags_channel_pad_last(start_channel);

      start_recycling = NULL;
      end_recycling = NULL;

      /* setting up parameters */
      parameter[i].name = "start_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_channel);

      parameter[i].name = "end_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_channel);

      parameter[i].name = "start_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_recycling);

      parameter[i].name = "end_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_recycling);
    }

    return(parameter);
  }
  void ags_audio_unset_flags_add_recycling_task(GParameter *parameter){
    AgsAudioSetRecycling *audio_set_recycling;
    AgsThread *main_loop, *current;
    AgsTaskThread *task_thread;
    AgsApplicationContext *application_context;

    application_context = ags_soundcard_get_application_context(audio->soundcard);
    main_loop = application_context->main_loop;
    task_thread = ags_thread_find_type(main_loop,
				       AGS_TYPE_TASK_THREAD);
    
    /* create set recycling task */
    audio_set_recycling = ags_audio_set_recycling_new(audio,
						      parameter);

    /* append AgsAudioSetRecycling */
    ags_task_thread_append_task(task_thread,
				AGS_TASK(audio_set_recycling));
  }

  if(audio == NULL || !AGS_IS_AUDIO(audio)){
    return;
  }
  
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags))){
    GParameter *parameter;
    
    /* check if input has already no recyclings */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){      
      if(audio->input_pads > 0){
	parameter = ags_audio_unset_flags_set_recycling_parameter(AGS_TYPE_INPUT);
	ags_audio_unset_flags_add_recycling_task(parameter);
	
	audio->flags &= (~AGS_AUDIO_INPUT_HAS_RECYCLING);
      }
      
      /* check if output has already recyclings */
      if(audio->output_pads > 0){
	parameter = ags_audio_unset_flags_set_recycling_parameter(AGS_TYPE_OUTPUT);
	ags_audio_unset_flags_add_recycling_task(parameter);
	
	audio->flags &= (~AGS_AUDIO_OUTPUT_HAS_RECYCLING);
      }
    }
  }
}

void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsChannel *channel, *prev_pad, *input, *input_pad_last;
  AgsRecycling *recycling;
  GObject *parent; // of recycling
  GList *list;
  guint pads, i, j;
  gboolean alloc_recycling;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput
  void ags_audio_set_audio_channels_init_parameters(GType type){
    if(type == AGS_TYPE_OUTPUT){
      link_recycling = FALSE;

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;

	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  input = ags_channel_nth(audio->input, audio->audio_channels);

	  if((AGS_AUDIO_SYNC & (audio->flags)) != 0 && (AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	    set_sync_link = FALSE;
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	    set_async_link = TRUE;
	    set_sync_link = FALSE;
	    input_pad_last = ags_channel_nth(input, audio->input_lines - (audio_channels - audio->audio_channels));
	  }else{
#ifdef AGS_DEBUG
	    g_message("ags_audio_set_audio_channels - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined\0");
#endif
	    set_sync_link = FALSE;
	    set_async_link = FALSE;
	  }
	}
      }
    }else{
      set_sync_link = FALSE;
      set_async_link = FALSE;
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & audio->flags) != 0)
	alloc_recycling = TRUE;
      else
	alloc_recycling = FALSE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0 && alloc_recycling)
	link_recycling = TRUE;
      else
	link_recycling = FALSE;
    }    
  }
  void ags_audio_set_audio_channels_grow_one(GType type){
    channel = (AgsChannel *) g_object_new(type,
					  "audio\0", (GObject *) audio,
					  NULL);

    if(type == AGS_TYPE_OUTPUT){
      /* AGS_TYPE_OUTPUT */
      audio->output = channel;
      pads = audio->output_pads;
    }else{
      /* AGS_TYPE_INPUT */
      audio->input = channel;
      pads = audio->input_pads;
    }

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->soundcard);

      channel->first_recycling->channel = (GObject *) channel;
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;

      input = ags_channel_nth(input, audio_channels);
    }else if(set_async_link){

      /* set async link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < pads; j++){
      channel->next =
	channel->next_pad = (AgsChannel *) g_object_new(type,
							"audio\0", (GObject *) audio,
							NULL);
      channel->next->prev = channel;
      channel = channel->next;
      channel->prev_pad = channel->prev;

      channel->pad = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->soundcard);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	
	input = input->next;
      }
      /* set async link is illegal here */
    }
  }
  void ags_audio_set_audio_channels_grow(GType type){
    AgsChannel *prev_channel, *prev_pad, *next_channel, *start;
    AgsRecycling *prev_channel_next_recycling;

    if(type == AGS_TYPE_OUTPUT){
      //      prev_pad = audio->output;
      prev_channel = ags_channel_nth(audio->output, audio->audio_channels - 1);
      pads = audio->output_pads;
    }else{
      //      prev_pad = audio->input;
      prev_channel = ags_channel_nth(audio->input, audio->audio_channels - 1);
      pads = audio->input_pads;
    }

    /*
     * linking with prev_channel is done later else linked channels would be lost
     */
    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    channel->audio_channel = audio->audio_channels;
    channel->line = audio->audio_channels;

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->soundcard);

      if(link_recycling){
	prev_channel->last_recycling->next = recycling;
	recycling->prev = prev_channel->last_recycling;
      }

      channel->first_recycling->channel = (GObject *) channel;
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      /* set async link*/
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    //    i = 1;

    /*
     * link now the new allocated channels
     */
    next_channel = prev_channel->next;

    prev_channel->next = start;
    start->prev = prev_channel;
    prev_channel = next_channel;

    for(j = audio->audio_channels + 1; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);
      channel->next->prev = channel;
      channel = channel->next;

      if(type == AGS_TYPE_OUTPUT){
	channel->prev_pad = ags_channel_pad_last(ags_channel_nth(audio->output,
								 audio->audio_channels));
      }else{
	channel->prev_pad = ags_channel_pad_last(ags_channel_nth(audio->input,
								 audio->audio_channels));
      }

      channel->prev_pad->next_pad = channel;

      channel->pad = 0;
      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->soundcard);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }else if(set_async_link){
	/* set async link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }

    if(pads == 1)
      return;

    /*
     * the main part of allocation
     */
    for(i = 1; i < pads -1; i++){
      channel->next = prev_channel;
      prev_channel->prev = channel;

      /* set async link is illegal here */

      for(j = 0; j < audio->audio_channels -1; j++){
	prev_channel->line = i * audio_channels + j;

	prev_channel = prev_channel->next;
      }

      prev_channel->line = i * audio_channels + j;

      // prev_channel = prev_channel->next; will be called later

      if(link_recycling){
	recycling = prev_channel->last_recycling;
      }

      prev_pad = start;

      /* alloc new AgsChannel's */
      start =
	channel = (AgsChannel *) g_object_new(type,
					      "audio\0", (GObject *) audio,
					      NULL);

      next_channel = prev_channel->next;

      prev_channel->next = start;
      start->prev = prev_channel;
      prev_channel = next_channel;

      j++;

      goto ags_audio_set_audio_channels_grow0;

      for(; j < audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type,
						    "audio\0", (GObject *) audio,
						    NULL);

	channel->next->prev = channel;
	channel = channel->next;

      ags_audio_set_audio_channels_grow0:

	prev_pad->next_pad = channel;
	channel->prev_pad = prev_pad;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = (i * audio_channels) + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->soundcard);

	  if(link_recycling){
	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	    recycling = recycling->next;
	  }

	  channel->first_recycling->channel = (GObject *) channel;
	}else if(set_sync_link){
	  /* set sync link */
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;
	  input = input->next;
	}
	/* set async link is illegal here */

	prev_pad = prev_pad->next;
      }

      if(link_recycling){
	recycling->next = prev_channel->first_recycling;
	prev_channel->first_recycling->prev = recycling;
      }
    }

    channel->next = prev_channel;
    prev_channel->prev = channel;

    /* set async link is illegal here */

    for(j = 0; j < audio->audio_channels -1; j++){
      prev_channel->line = i * audio_channels + j;

      prev_channel = prev_channel->next;

      if(link_recycling)
	recycling = recycling->next;
    }

    prev_channel->line = i * audio_channels + j;

    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    prev_channel->next = start;
    start->prev = prev_channel;
    j++;

    goto ags_audio_set_audio_channels_grow1;

    for(; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);

      channel->next->prev = channel;
      channel = channel->next;

    ags_audio_set_audio_channels_grow1:

      prev_pad->next_pad = channel;
      channel->prev_pad = prev_pad;

      channel->pad = i;
      channel->audio_channel = j;
      channel->line = (i * audio_channels) + j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->soundcard);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }
      /* set async link is illegal here */

      prev_pad = prev_pad->next;
    }
  }
  void ags_audio_set_audio_channels_shrink_zero(){
    AgsChannel *start, *channel_next;
    AgsRecycling *prev_recycling, *next_recycling;
    gboolean first_run;
    GError *error;

    start =
      channel = audio->output;
    first_run = TRUE;

    error = NULL;

  ags_audio_set_audio_channel_shrink_zero0:

    while(channel != NULL){
      ags_channel_set_link(channel, NULL, &error);
      ags_channel_set_recycling(channel,
				NULL, NULL,
				TRUE, TRUE);

      channel = channel->next;
    }

    channel = start;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }

    if(first_run){
      start =
	channel = audio->input;
      first_run = FALSE;
      goto ags_audio_set_audio_channel_shrink_zero0;
    }

    audio->output = NULL;
    audio->input = NULL;
  }
  void ags_audio_set_audio_channels_shrink(){
    AgsChannel *channel0, *channel1, *start;
    AgsRecycling *recycling;
    gboolean first_run;
    GError *error;

    start =
      channel = audio->output;
    pads = audio->output_pads;
    first_run = TRUE;

    error = NULL;

  ags_audio_set_audio_channel_shrink0:

    for(i = 0; i < pads; i++){
      channel = ags_channel_nth(channel, audio_channels);

      for(j = audio_channels; j < audio->audio_channels; j++){
	ags_channel_set_link(channel, NULL, &error);
	channel = channel->next;
      }
    }

    channel = start;

    if(i < pads){
      for(i = 0; ; i++){
	for(j = 0; j < audio_channels -1; j++){
	  channel->pad = i;
	  channel->audio_channel = j;
	  channel->line = i * audio_channels + j;

	  channel = channel->next;
	}

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio_channels + j;

	channel0 = channel->next;
	
	for(; j < audio->audio_channels; j++){
	  channel1 = channel0->next;

	  g_object_unref((GObject *) channel0);

	  channel0 = channel1;
	}

	channel->next = channel1;

	if(channel1 != NULL)
	  channel1->prev = channel;
	else
	  break;

	channel = channel1;
      }
    }

    if(first_run){
      first_run = FALSE;
      start =
	channel = audio->input;
      pads = audio->input_pads;

      goto ags_audio_set_audio_channel_shrink0;
    }
  }
  void ags_audio_set_audio_channels_grow_notation(){
    GList *list;
    guint i;

    i = audio->audio_channels;

#ifdef AGS_DEBUG
    g_message("ags_audio_set_audio_channels_grow_notation\n\0");
#endif

    if(audio->audio_channels == 0){
      audio->notation =
	list = g_list_alloc();
      goto ags_audio_set_audio_channels_grow_notation0;
    }else{
      list = g_list_nth(audio->notation, audio->audio_channels - 1);
    }

    for(; i < audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;

    ags_audio_set_audio_channels_grow_notation0:
      list->data = (gpointer) ags_notation_new(audio,
					       i);
    } 
  }
  void ags_audio_set_audio_channels_shrink_notation(){
    GList *list, *list_next;

    list = g_list_nth(audio->notation, audio_channels);

    if(audio_channels == 0){
      audio->notation = NULL;
    }else{
      list->prev->next = NULL;
    }

    while(list != NULL){
      list_next = list->next;

      g_object_unref((GObject *) list->data);
      g_list_free1(list);

      list = list_next;
    }
  }

  /* entry point */
  if(audio_channels > audio->audio_channels){
    AgsPlaybackDomain *playback_domain;
    AgsChannel *current;

    /* grow audio channels*/
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_grow_notation();

    /* grow one */
    if(audio->audio_channels == 0){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_OUTPUT);
      }

      audio->audio_channels = 1;
      audio->output_lines = audio->output_pads;
    }

    /* grow more than one */
    if(audio_channels > 1){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio_channels * audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
     	ags_audio_set_audio_channels_grow(AGS_TYPE_OUTPUT);
      }
    }

    /* alloc playback domain */
    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
    current = ags_channel_nth(audio->output,
			      pads - audio->output_pads);

    for(i = 0; i < audio_channels - audio_channels_old; i++){
      playback_domain->playback = g_list_append(playback_domain->playback,
						current->playback);

      current = current->next;
    }

  }else if(audio_channels < audio->audio_channels){
    AgsPlaybackDomain *playback_domain;
    AgsChannel *current;

    /* shrink audio channels */
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_shrink_notation();

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero();
    }else{
      ags_audio_set_audio_channels_shrink();
    }

    audio->input_lines = audio_channels * audio->input_pads;

    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

    for(i = 0; i < audio->audio_channels - audio_channels; i++){
      AgsPlayback *playback;

      playback = g_list_last(playback_domain->playback);
      playback_domain->playback = g_list_remove(playback_domain->playback,
						playback);
    }
  }

  audio->audio_channels = audio_channels;
  // input_lines must be set earlier because set_sync_link needs it
  audio->output_lines = audio_channels * audio->output_pads;
}

/**
 * ags_audio_set_audio_channels:
 * @audio: the #AgsAudio
 * @audio_channels: new audio channels
 *
 * Resize audio channels AgsInput will be allocated first.
 *
 * Since: 0.3
 */
void
ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio->audio_channels);
  g_object_unref((GObject *) audio);
}

/*
 * resize
 * AgsInput has to be allocated first
 */
void
ags_audio_real_set_pads(AgsAudio *audio,
			GType type,
			guint pads, guint pads_old)
{
  AgsChannel *start, *channel, *prev_pad, *input, *input_pad_last;
  AgsRecycling *recycling;
  AgsRecycling *old_first_recycling, *first_recycling;
  AgsRecyclingContext *recycling_context, *old_recycling_context;
  GList *playback;
  GList *recall_id;
  GList *recycling_prev, *recycling_iter;
  guint i, j;
  gboolean alloc_recycling, link_recycling, set_sync_link, update_async_link, set_async_link;
  void ags_audio_set_pads_grow_one(){
    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    if(alloc_recycling){
      channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->soundcard);

      channel->first_recycling->channel = (GObject *) channel;
    }else if(set_sync_link){
      input = audio->input;
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      input = audio->input;
      input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);

      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < audio->audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);
      channel->next->prev = channel;
      channel = channel->next;

      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->soundcard);

	channel->first_recycling->channel = (GObject *) channel;
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;

	input = input->next;
      }else if(set_async_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }
  }
  void ags_audio_set_pads_grow(){
    AgsChannel *start_pad, *end_pad;
    AgsChannel *prev_pad, *prev_pad_start;
    AgsRecycling *recycling, *last_recycling;
    GList *recycling_next, *recycling_current;
    GList *recall_id;

    if(type == AGS_TYPE_OUTPUT){
      prev_pad = ags_channel_pad_last(audio->output);
    }else{
      prev_pad = ags_channel_pad_last(audio->input);
    }

    prev_pad_start = prev_pad;
    start_pad = prev_pad;

    if(alloc_recycling){
      if(link_recycling){
	channel = prev_pad;
	recycling_next = NULL;

	while(channel != NULL){
	  recycling_next = g_list_prepend(recycling_next,
					  channel->last_recycling->next);

	  channel = channel->next;
	}

	recycling_next = g_list_reverse(recycling_next);
      }
    }else if(set_sync_link){
      if(pads_old != 0){
	input = ags_channel_pad_nth(audio->input, pads_old);
      }
    }

    channel = ags_channel_last(prev_pad);

    for(i = pads_old; i < pads; i++){
      prev_pad_start = prev_pad;

      if(alloc_recycling){
	if(link_recycling){
	  recycling_current = NULL;

	  while(prev_pad != NULL){
	    recycling_current = g_list_prepend(recycling_current,
					       prev_pad->first_recycling);

	    prev_pad = prev_pad->next;
	  }

	  prev_pad = prev_pad_start;

	  recycling_current = g_list_reverse(recycling_current);
	  recycling_iter = recycling_current;
	}
      }else if(set_async_link){
	input = audio->input;
	input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);
      }

      prev_pad = prev_pad_start;

      for(j = 0; j < audio->audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type,
						    "audio\0", (GObject *) audio,
						    NULL);
	channel->next->prev = channel;
	channel = channel->next;

	channel->prev_pad = prev_pad;
	prev_pad->next_pad = channel;
	prev_pad = prev_pad->next;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio->audio_channels + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->soundcard);

	  if(link_recycling){
	    recycling = recycling_iter->data;
	    recycling_iter = recycling_iter->next;

	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	  }

	  channel->first_recycling->channel = (GObject *) channel;
	}else if(set_sync_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;

	  input = input->next;
	}else if(set_async_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input_pad_last->last_recycling;

	  input = input->next;
	  input_pad_last = input_pad_last->next;
	}
      }

      if(alloc_recycling){
	if(link_recycling){
	  g_list_free(recycling_current);
	}
      }
    }

    if(alloc_recycling){
      if(link_recycling){
	channel = start_pad;
	end_pad = ags_channel_pad_last(channel);

	while(recycling_next != NULL){
	  end_pad->last_recycling->next = recycling_next->data;

	  if(end_pad->last_recycling->next != NULL){
	    end_pad->last_recycling->next->prev = end_pad->last_recycling;
	  }

	  end_pad = end_pad->next;
	  recycling_next = recycling_next->next;
	}
      }
    }
  }
  void ags_audio_set_pads_unlink_zero(){
    AgsChannel *start;
    GError *error;

    start = channel;

    while(channel != NULL){
      error = NULL;
      ags_channel_set_link(channel, NULL, &error);
      ags_channel_set_recycling(channel,
				NULL, NULL,
				TRUE, TRUE);

      if(error != NULL){
	g_error("%s\0", error->message);
      }

      channel = channel->next;
    }

    channel = start;
  }
  void ags_audio_set_pads_unlink(){
    AgsChannel *start;

    start = channel;
    channel = ags_channel_nth(channel, pads * audio->audio_channels);

    ags_audio_set_pads_unlink_zero();

    channel = start;
  }
  void ags_audio_set_pads_shrink_zero(){
    AgsChannel *channel_next;
    GError *error;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }
  }
  void ags_audio_set_pads_shrink(){
    AgsChannel *start;

    start = channel;

    channel = ags_channel_nth(channel, pads * audio->audio_channels);
    ags_audio_set_pads_shrink_zero();

    /* remove pads */
    channel = ags_channel_nth(start, (pads - 1) * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      channel->next_pad = NULL;

      /* iterate */
      channel = channel->next;
    }

    /* remove channel */
    channel = ags_channel_nth(start, (pads - 1) * audio->audio_channels + audio->audio_channels - 1);
    channel->next = NULL;

    channel = start;
  }
  void ags_audio_set_pads_alloc_notation(){
    GList *list;
    guint i;

#ifdef AGS_DEBUG
    g_message("ags_audio_set_pads_alloc_notation\n\0");
#endif

    if(audio->audio_channels > 0){
      audio->notation =
	list = g_list_alloc();
      i = 0;
      goto ags_audio_set_pads_alloc_notation0;
    }else{
      return;
    }

    for(; i < audio->audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;
    ags_audio_set_pads_alloc_notation0:

      list->data = (gpointer) ags_notation_new(audio,
					       i);
    }
  }
  void ags_audio_set_pads_free_notation(){
    GList *list, *list_next;

    if(audio->audio_channels > 0){
      list = audio->notation;
      audio->notation = NULL;
    }else{
      return;
    }

    while(list != NULL){
      list_next = list->next;

      g_object_unref(G_OBJECT(list->data));
      g_list_free1(list);

      list = list_next;
    }
  }
  void ags_audio_set_pads_add_notes(){
    /* -- useless --
       GList *list;

       list = audio->notation;

       while(list != NULL){
       AGS_NOTATION(list->data)->pads = pads;

       list = list->next;
       }
    */
  }
  void ags_audio_set_pads_remove_notes(){
    AgsNotation *notation;
    GList *notation_i, *note, *note_next;

    notation_i = audio->notation;

    while(notation_i != NULL){
      notation = AGS_NOTATION(notation_i->data);
      note = notation->notes;

      while(note != NULL){
	note_next = note->next;

	if(AGS_NOTE(note->data)->y >= pads){
	  if(note->prev != NULL)
	    note->prev->next = note_next;
	  else
	    notation->notes = note_next;

	  if(note_next != NULL)
	    note_next->prev = note->prev;

	  free(note->data);
	  g_list_free1(note);
	}

	note = note_next;
      }

      notation_i = notation_i->next;
    }
  }
  
  /* entry point */
  alloc_recycling = FALSE;
  link_recycling = FALSE;
  set_sync_link = FALSE;
  update_async_link = FALSE;
  set_async_link = FALSE;

  if(g_type_is_a(type, AGS_TYPE_OUTPUT)){
    /* output */
    pads_old = audio->output_pads;
    link_recycling = FALSE;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->output_pads = pads;
      return;
    }

    /* init grow parameters */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
      alloc_recycling = TRUE;
    }else if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      if((AGS_AUDIO_SYNC & audio->flags) != 0 && (AGS_AUDIO_ASYNC & audio->flags) == 0){
	set_sync_link = TRUE;
      }else if((AGS_AUDIO_ASYNC & audio->flags) == 0){
	input = audio->input;
      }
    }

    /* grow one */
    if(pads_old == 0){
      AgsPlaybackDomain *playback_domain;
      AgsChannel *current;

      /* alloc notation */
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation == NULL &&
	 (AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
	ags_audio_set_pads_alloc_notation();
      }

      /*  */
      ags_audio_set_pads_grow_one();

      /* populate playback domain */
      playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

      channel = start;
      audio->output = start;

      current = start;

      for(i = 0; i < audio->audio_channels; i++){
	playback_domain->playback = g_list_append(playback_domain->playback,
						  current->playback);
	
	current = current->next;
      }

      pads_old =
	audio->output_pads = 1;
    }else{
      channel = audio->output;
    }

    /* grow or shrink */
    if(pads > audio->output_pads){
      AgsPlaybackDomain *playback_domain;
      AgsChannel *current;

      ags_audio_set_pads_grow();
    }else if(pads == 0){
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation != NULL){
	ags_audio_set_pads_free_notation();
      }

      /* unlink and remove */
      ags_audio_set_pads_unlink_zero();

      ags_audio_set_pads_shrink_zero();
      audio->output = NULL;

      /* remove playback domain */
      g_list_free(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback);

      AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback = NULL;
    }else if(pads < audio->output_pads){
      AgsPlaybackDomain *playback_domain;

      ags_audio_set_pads_remove_notes();

      ags_audio_set_pads_unlink();

      ags_audio_set_pads_shrink();

      playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

      for(i = 0; i < audio->output_pads - pads; i++){
	AgsPlayback *playback;

	playback = g_list_last(playback_domain->playback);
	playback_domain->playback = g_list_remove(playback_domain->playback,
						  playback);
      }

    }

    /* apply new size */
    audio->output_pads = pads;
    audio->output_lines = pads * audio->audio_channels;

    if((AGS_AUDIO_SYNC & audio->flags) != 0 && (AGS_AUDIO_ASYNC & audio->flags) == 0){
      //TODO:JK: fix me
      //      audio->input_pads = pads;
      //      audio->input_lines = pads * audio->audio_channels;
    }
  }else if(g_type_is_a(type, AGS_TYPE_INPUT)){
    /* input */
    pads_old = audio->input_pads;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->input_pads = pads;
      return;
    }

    /* init grow parameters */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      alloc_recycling = TRUE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0){
	link_recycling = TRUE;

	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
	  update_async_link = TRUE;
      }
    }
    
    /* grow one */
    if(pads_old == 0){
      AgsChannel *current;
      AgsRecallID *current_recall_id, *default_recall_id;
      
      /* alloc notation */
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation == NULL &&
	 (AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) == 0){
	ags_audio_set_pads_alloc_notation();
      }

      /* add first channel */
      ags_audio_set_pads_grow_one();

      /* add recall id */
      current = start;
	
      while(current != start->next_pad){
	recall_id = audio->recall_id;

	while(recall_id != NULL){
	  current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					   "recycling\0", current->first_recycling,
					   "recycling-context\0", AGS_RECALL_ID(recall_id->data)->recycling_context,
					   NULL);
	  
	  ags_channel_add_recall_id(current,
				    current_recall_id);
	    
	  recall_id = recall_id->next;
	}
	  
	current = current->next;
      }

      /* apply current allocation */
      channel = start;
      audio->input = start;

      pads_old =
	audio->input_pads = 1;
    }else{
      channel = audio->input;
    }

    /* grow or shrink */
    if(pads >= 1){
      if(pads > audio->input_pads){
	AgsChannel *prev;
	AgsChannel *current;
	AgsRecallID *current_recall_id;

	if(link_recycling){
	  prev = ags_channel_pad_last(channel);
	  recycling_prev = NULL;

	  while(prev != NULL){
	    recycling = prev->first_recycling;

	    recycling_prev = g_list_append(recycling_prev,
					   recycling);

	    prev = prev->next;
	  }
	}
	
	recycling_iter = recycling_prev;

	/* grow channels */
	ags_audio_set_pads_grow();

	/* add recall id */
	current = ags_channel_pad_nth(audio->input,
				      audio->input_pads);
	
	while(current != NULL){
	  recall_id = audio->recall_id;

	  while(recall_id != NULL){
	    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					     "recycling\0", current->first_recycling,
					     "recycling-context\0", AGS_RECALL_ID(recall_id->data)->recycling_context,
					     NULL);
	  
	    ags_channel_add_recall_id(current,
				      current_recall_id);
	    
	    recall_id = recall_id->next;
	  }

	  current = current->next;
	}
      }else if(pads < audio->input_pads){
	/* shrink channels */
	ags_audio_set_pads_unlink();

	ags_audio_set_pads_shrink();
      }
    }else if(pads == 0){
      /* shrink channels */
      ags_audio_set_pads_unlink_zero();
      
      ags_audio_set_pads_shrink_zero();
      audio->input = NULL;  
    }

    /* apply new allocation */
    audio->input_pads = pads;
    audio->input_lines = pads * audio->audio_channels;

    /* update recycling context of default recall id */
    recall_id = audio->recall_id;

    while(recall_id != NULL){
      if((AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	channel = ags_channel_nth(audio->input,
				  AGS_CHANNEL(AGS_RECYCLING(AGS_RECALL_ID(recall_id->data)->recycling)->channel)->line);
      }else{
	channel = ags_channel_nth(audio->input,
				  AGS_CHANNEL(AGS_RECYCLING(AGS_RECALL_ID(recall_id->data)->recycling)->channel)->audio_channel);
      }

      old_first_recycling =
	first_recycling = channel->first_recycling;
    
      old_recycling_context = (AgsRecyclingContext *) AGS_RECALL_ID(recall_id->data)->recycling_context;

      if((old_first_recycling != NULL &&
	  ags_recycling_context_find(old_recycling_context,
				     old_first_recycling) != -1) ||
	 (first_recycling->parent == NULL ||
	  ags_recycling_context_find_parent(old_recycling_context,
					    first_recycling->parent) == -1)){
	recall_id = recall_id->next;
	
	continue;
      }
  
      recycling_context = ags_recycling_context_reset_recycling(old_recycling_context,
								NULL, NULL,
								channel->first_recycling, ags_channel_pad_last(channel)->last_recycling);

      ags_audio_add_recycling_context(audio,
				      recycling_context);

      while(channel != NULL){
	ags_channel_recursive_reset_recycling_context(channel,
						      old_recycling_context,
						      recycling_context);

	channel = channel->next_pad;
      }
      
      recall_id = recall_id->next;
    }
  }else{
    g_warning("unknown channel type\0");
  }
}

/**
 * ags_audio_set_pads:
 * @audio: an #AgsAudio
 * @type: AGS_TYPE_INPUT or AGS_TYPE_OUTPUT
 * @pads: new pad count
 *
 * Sets pad count for the apropriate @type
 *
 * Since: 0.3
 */
void
ags_audio_set_pads(AgsAudio *audio, GType type, guint pads)
{
  guint pads_old;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  pads_old = ((g_type_is_a(type, AGS_TYPE_OUTPUT)) ? audio->output_pads: audio->input_pads);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		type, pads, pads_old);
  g_object_unref((GObject *) audio);
}

/**
 * ags_audio_init_run:
 * @audio: an #AgsAudio
 *
 * Is emitted as audio is initialized.
 *
 * Returns: the current #AgsRecallID
 *
 * Since: 0.4
 */
AgsRecallID*
ags_audio_init_run(AgsAudio *audio)
{
  AgsRecallID *recall_id;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[INIT_RUN], 0,
		&recall_id);
  g_object_unref((GObject *) audio);

  return(recall_id);
}

/**
 * ags_audio_tact:
 * @audio: an #AgsAudio
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as audio is played.
 *
 * Since: 0.4
 */
void
ags_audio_tact(AgsAudio *audio, AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[TACT], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

/**
 * ags_audio_done:
 * @audio: an #AgsAudio
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as playing audio is done.
 *
 * Since: 0.4
 */
void
ags_audio_done(AgsAudio *audio, AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[DONE], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

/**
 * ags_audio_set_sequence_length:
 * @audio: an #AgsAudio
 * @sequence_length: the sequence length
 *
 * Sets sequence length.
 *
 * Since: 0.4
 */
void
ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length)
{
  audio->sequence_length = sequence_length;
}

/**
 * ags_audio_add_notation:
 * @audio: an #AgsAudio
 * @notation: the #AgsNotation
 *
 * Adds a notation.
 *
 * Since: 0.4
 */
void
ags_audio_add_notation(AgsAudio *audio,
		       GObject *notation)
{
  g_object_ref(notation);
  audio->automation = g_list_prepend(audio->notation,
				     notation);
}

/**
 * ags_audio_remove_notation:
 * @audio: an #AgsAudio
 * @notation: the #AgsNotation
 *
 * Removes a notation.
 *
 * Since: 0.4
 */
void
ags_audio_remove_notation(AgsAudio *audio,
			  GObject *notation)
{
  g_object_unref(notation);
  audio->automation = g_list_remove(audio->notation,
				    notation);
}

/**
 * ags_audio_add_automation:
 * @audio: an #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Adds an automation.
 *
 * Since: 0.4.3
 */
void
ags_audio_add_automation(AgsAudio *audio,
			 GObject *automation)
{
  g_object_ref(automation);
  audio->automation = g_list_prepend(audio->automation,
				     automation);
}

/**
 * ags_audio_remove_automation:
 * @audio: an #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Removes an automation.
 *
 * Since: 0.4.3
 */
void
ags_audio_remove_automation(AgsAudio *audio,
			    GObject *automation)
{
  g_object_unref(automation);
  audio->automation = g_list_remove(audio->automation,
				    automation);
}

/**
 * ags_audio_add_recall_id:
 * @audio: an #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */
  g_object_ref(recall_id);
  audio->recall_id = g_list_prepend(audio->recall_id, recall_id);
}

/**
 * ags_audio_remove_recall_id:
 * @audio: an #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->recall_id = g_list_remove(audio->recall_id, recall_id);
  g_object_unref(recall_id);
}

/**
 * ags_audio_add_recycling_context:
 * @audio: an #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Adds a recycling container.
 *
 * Since: 0.4
 */
void
ags_audio_add_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  g_object_ref(recycling_context);
  audio->recycling_context = g_list_prepend(audio->recycling_context, recycling_context);
}

/**
 * ags_audio_remove_recycling_context:
 * @audio: an #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Removes a recycling container.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  audio->recycling_context = g_list_remove(audio->recycling_context, recycling_context);
  g_object_unref(recycling_context);
}

/**
 * ags_audio_add_recall_container:
 * @audio: an #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container)
{
  /*
   * TODO:JK: thread synchronisation
   */

  g_object_ref(recall_container);
  audio->container = g_list_prepend(audio->container, recall_container);
}

/**
 * ags_audio_remove_recall_container:
 * @audio: an #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->container = g_list_remove(audio->container, recall_container);
  g_object_unref(recall_container);
}

/**
 * ags_audio_add_recall:
 * @audio: an #AgsAudio
 * @recall_container: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Adds a recall.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  g_object_ref(G_OBJECT(recall));

  if(play){
    audio->play = g_list_append(audio->play, recall);
  }else{
    audio->recall = g_list_append(audio->recall, recall);
  }
}

/**
 * ags_audio_remove_recall:
 * @audio: an #AgsAudio
 * @recall_container: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Removes a recall.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  if(play){
    audio->play = g_list_remove(audio->play, recall);
  }else{
    audio->recall = g_list_remove(audio->recall, recall);
  }

  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_audio_duplicate_recall:
 * @audio: an #AgsAudio
 * @recall_id: an #AgsRecallID
 * 
 * Duplicate all #AgsRecall templates of this #AgsAudio.
 *
 * Since: 0.4
 */
void
ags_audio_duplicate_recall(AgsAudio *audio,
			   AgsRecallID *recall_id)
{
  AgsRecall *recall, *copy;
  GList *list_recall_start, *list_recall;
  gboolean playback, sequencer, notation;
  
#ifdef AGS_DEBUG
  g_message("ags_audio_duplicate_recall: %s - audio.lines[%u,%u]\n\0", G_OBJECT_TYPE_NAME(audio->machine_widget), audio->output_lines, audio->input_lines);
#endif

  playback = FALSE;
  sequencer = FALSE;
  notation = FALSE;

  if((AGS_RECALL_ID_PLAYBACK & (recall_id->flags)) != 0){
    playback = TRUE;
  }

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    sequencer = TRUE;
  }

  if((AGS_RECALL_ID_NOTATION & (recall_id->flags)) != 0){
    notation = TRUE;
  }
  
  /* get the appropriate list */
  if(recall_id->recycling_context->parent == NULL){
    list_recall_start = 
      list_recall = audio->play;
  }else{
    list_recall_start =
      list_recall = audio->recall;
  }

  /* return if already duplicated */
  if((AGS_RECALL_ID_DUPLICATE & (recall_id->flags)) != 0){
    while(list_recall != NULL){
      /* notify run */
      ags_recall_notify_dependency(AGS_RECALL(list_recall->data), AGS_RECALL_NOTIFY_RUN, 1);

      list_recall = list_recall->next;
    }

    return;
  }else{
    //TODO:JK: optimize tree see deprecated AgsRunOrder
  }

  /* duplicate */
  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       AGS_IS_RECALL_AUDIO(recall) ||
       !((playback && (AGS_RECALL_PLAYBACK & (recall->flags)) != 0) ||
	 (sequencer && (AGS_RECALL_SEQUENCER & (recall->flags)) != 0) ||
	 (notation && (AGS_RECALL_NOTATION & (recall->flags)) != 0))){
      list_recall = list_recall->next;
      continue;
    }  

    /* duplicate template only once */
    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
      /* duplicate the recall */
      copy = ags_recall_duplicate(recall, recall_id);

      /* notify run */
      ags_recall_notify_dependency(copy, AGS_RECALL_NOTIFY_RUN, 1);

#ifdef AGS_DEBUG
      g_message("recall duplicated: %s\n\0", G_OBJECT_TYPE_NAME(copy));
#endif

      /* set appropriate flag */
      if(playback){
	ags_recall_set_flags(copy, AGS_RECALL_PLAYBACK);
      }else if(sequencer){
	ags_recall_set_flags(copy, AGS_RECALL_SEQUENCER);
      }else if(notation){
	ags_recall_set_flags(copy, AGS_RECALL_NOTATION);
      }

      /* append to AgsAudio */
      if(recall_id->recycling_context->parent == NULL)
	audio->play = g_list_append(audio->play, copy);
      else
	audio->recall = g_list_append(audio->recall, copy);
    
      /* connect */
      ags_connectable_connect(AGS_CONNECTABLE(copy));
    }

    /* iterate */
    list_recall = list_recall->next;
  }
}

/**
 * ags_audio_resolve_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to use
 *
 * Performs resolving of recalls.
 *
 * Since: 0.4
 */
void ags_audio_resolve_recall(AgsAudio *audio,
			      AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;  

  /* return if already duplicated */
  if((AGS_RECALL_ID_RESOLVE & (recall_id->flags)) != 0){
    return;
  }
    
  /* get the appropriate lists */
  if(recall_id->recycling_context->parent == NULL){
    list_recall = audio->play;
  }else{
    list_recall = audio->recall;
  }

  /* resolve */  
  while((list_recall = ags_recall_find_recycling_context(list_recall, recall_id->recycling_context)) != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    ags_recall_resolve_dependencies(recall);

    list_recall = list_recall->next;
  }
}

/**
 * ags_audio_init_recall:
 * @audio: the #AgsAudio
 * @stage: stage benning at 0 up to 2, or just -1
 * @recall_id: the #AgsRecallID to use or #NULL
 *
 * Initializes the recalls of @audio
 *
 * Since: 0.4
 */
void
ags_audio_init_recall(AgsAudio *audio, gint stage,
		      AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list_recall;

  /* return if already initialized */
  switch(stage){
  case 0:
    if((AGS_RECALL_ID_INIT_PRE & (recall_id->flags)) != 0){
      return;
    }
    break;
  case 1:
    if((AGS_RECALL_ID_INIT_INTER & (recall_id->flags)) != 0){
      return;
    }
    break;
  case 2:
    if((AGS_RECALL_ID_INIT_POST & (recall_id->flags)) != 0){
      return;
    }
    break;
  }

  /* retrieve appropriate recalls */
  if(recall_id->recycling_context->parent == NULL)
    list_recall = audio->play;
  else
    list_recall = audio->recall;

  /* init  */
  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if(recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context ||
       AGS_IS_RECALL_AUDIO(recall)){
      list_recall = list_recall->next;
      continue;
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
      if(stage == 0){
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));
    
	recall->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(recall);
	recall->flags &= (~AGS_RECALL_REMOVE);
      }else if(stage == 1){
	ags_recall_run_init_inter(recall);
      }else{
	ags_recall_run_init_post(recall);
      }
    }
    
    list_recall = list_recall->next;
  }
}

/**
 * ags_audio_is_playing:
 * @audio: the #AgsAudio
 *
 * Determine if #AgsAudio is playing.
 *
 * Returns: TRUE if it's playing otherwise FALSE
 *
 * Since: 0.4
 */
gboolean
ags_audio_is_playing(AgsAudio *audio)
{
  AgsChannel *output;
  AgsRecallID *recall_id;
  AgsPlayback *playback;
  
  output = audio->output;

  while(output != NULL){
    playback = AGS_PLAYBACK(output->playback);

    if((AGS_PLAYBACK_PLAYBACK & (playback->flags)) != 0 ||
       (AGS_PLAYBACK_SEQUENCER & (playback->flags)) != 0 ||
       (AGS_PLAYBACK_NOTATION & (playback->flags)) != 0){
      return(TRUE);
    }

    output = output->next;
  }

  return(FALSE);
}

/**
 * ags_audio_play:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 * @stage: valid range is from 0 up to 2
 *
 * Performs on single play call of appropriate stage.
 *
 * Since: 0.4
 */
void
ags_audio_play(AgsAudio *audio,
	       AgsRecallID *recall_id,
	       gint stage)
{
  AgsRecall *recall;
  GList *list, *list_next;

  /* return if already played */
  switch(stage){
  case 0:
    if((AGS_RECALL_ID_PRE & (recall_id->flags)) != 0){
      return;
    }
    break;
  case 1:
    if((AGS_RECALL_ID_INTER & (recall_id->flags)) != 0){
      return;
    }
    break;
  case 2:
    if((AGS_RECALL_ID_POST & (recall_id->flags)) != 0){
      return;
    }
    break;
  }

  /* retrieve appropriate recalls */
  if(recall_id->recycling_context->parent == NULL)
    list = audio->play;
  else
    list = audio->recall;

  /* play */
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if(recall == NULL){
      if(recall_id->recycling_context->parent != NULL){
	audio->recall = g_list_remove(audio->recall,
				      recall);
      }else{
	audio->play = g_list_remove(audio->play,
				    recall);
      }

      g_warning("recall == NULL\0");
      list = list_next;
      continue;
    }

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id == NULL ||
       (recall->recall_id->recycling_context != recall_id->recycling_context)){
      list = list_next;

      continue;
    }
    
    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0)
	ags_recall_run_pre(recall);
      else if(stage == 1)
	ags_recall_run_inter(recall);
      else
	ags_recall_run_post(recall);
    }

    list = list_next;
  }
}

/**
 * ags_audio_recursive_play_init:
 * @audio: the #AgsAudio object
 * @playback: if doing playback
 * @sequencer: if doing sequencer
 * @notation: if doing notation
 *
 * Initializes #AgsAudio in order to do playback, sequencer or notation.
 *
 * Returns: a list containing all #AgsRecallID
 *
 * Since: 0.4
 */
GList*
ags_audio_recursive_play_init(AgsAudio *audio,
			      gboolean playback, gboolean sequencer, gboolean notation)
{
  AgsChannel *channel;
  AgsRecallID *recall_id;
  GList *list, *list_start;
  gint stage;
  gboolean arrange_recall_id, duplicate_templates, resolve_dependencies;

  list = NULL;
  list_start = NULL;

  for(stage = 0; stage < 3; stage++){
    channel = audio->output;
    list = list_start;

    if(stage == 0){
      arrange_recall_id = TRUE;
      duplicate_templates = TRUE;
      resolve_dependencies = TRUE;
    }else{
      arrange_recall_id = FALSE;
      duplicate_templates = FALSE;
      resolve_dependencies = FALSE;
    }

    while(channel != NULL){
      if(stage == 0){
	recall_id = ags_channel_recursive_play_init(channel, stage,
						    arrange_recall_id, duplicate_templates,
						    playback, sequencer, notation,
						    resolve_dependencies,
						    NULL);
	
	list_start = g_list_append(list_start,
				   recall_id);
      }else{
	ags_channel_recursive_play_init(channel, stage,
					arrange_recall_id, duplicate_templates,
					playback, sequencer, notation,
					resolve_dependencies,
					AGS_RECALL_ID(list->data));

	list = list->next;
      }

      channel = channel->next;
    }
  }

  return(list_start);
}

/**
 * ags_audio_remove:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Remove processing audio data.
 *
 * Since: 0.4
 */
void
ags_audio_remove(AgsAudio *audio,
		 AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list, *list_next;
  gboolean play;

  if(recall_id == NULL){
    return;
  }
  
  if(recall_id->recycling_context->parent == NULL){
    list = audio->play;
    play = TRUE;
  }else{
    list = audio->recall;
    play = FALSE;
  }

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) ||
       recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;

      continue;
    }

    ags_recall_remove(recall);
    ags_audio_remove_recall(audio,
			    recall,
			    play);
    
    list = list_next;
  }

  audio->recall_id = g_list_remove(audio->recall_id,
				   recall_id);
  g_object_unref(recall_id);
}

/**
 * ags_audio_cancel:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Cancel processing audio data.
 *
 * Since: 0.4
 */
void
ags_audio_cancel(AgsAudio *audio,
		 AgsRecallID *recall_id)
{
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(recall_id == NULL){
    return;
  }

  if(recall_id->recycling_context->parent == NULL)
    list = audio->play;
  else
    list = audio->recall;

  g_object_ref(recall_id);

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) ||
       recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;

      continue;
    }

    g_object_ref(recall_id);
    g_object_ref(recall);
    ags_recall_cancel(recall);
    
    list = list_next;
  }
}

/**
 * ags_audio_set_soundcard:
 * @audio: the #AgsAudio
 * @soundcard: an #AgsSoundcard
 *
 * Sets a soundcard object on audio.
 *
 * Since: 0.4
 */
void
ags_audio_set_soundcard(AgsAudio *audio, GObject *soundcard)
{
  AgsChannel *channel;
  GList *list;

  /* audio */
  if(audio->soundcard == soundcard)
    return;

  if(audio->soundcard != NULL)
    g_object_unref(audio->soundcard);

  if(soundcard != NULL)
    g_object_ref(soundcard);

  audio->soundcard = (GObject *) soundcard;

  /* recall */
  list = audio->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard\0", soundcard,
		 NULL);
    
    list = list->next;
  }
  
  list = audio->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard\0", soundcard,
		 NULL);
    
    list = list->next;
  }
  
  /* input */
  channel = audio->input;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "soundcard\0", soundcard,
		 NULL);
    
    channel = channel->next;
  }

  /* output */
  channel = audio->output;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "soundcard\0", soundcard,
		 NULL);
    
    channel = channel->next;
  }
}

/**
 * ags_audio_open_files:
 * @audio: the #AgsAudio
 * @filenames: the files to open
 * @overwrite_channels: if existing channels should be assigned
 * @create_channels: if new channels should be created as not fitting if combined with @overwrite_channels
 *
 * Open some files.
 *
 * Since: 0.4
 */
void
ags_audio_open_files(AgsAudio *audio,
		     GSList *filenames,
		     gboolean overwrite_channels,
		     gboolean create_channels)
{
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  AgsAudioSignal *audio_signal_source_old;
  GList *audio_signal_list;
  guint i, j;
  guint list_length;
  GError *error;

  channel = audio->input;

  /* overwriting existing channels */
  if(overwrite_channels){
    if(channel != NULL){
      for(i = 0; i < audio->input_pads && filenames != NULL; i++){
	audio_file = ags_audio_file_new((gchar *) filenames->data,
					(GObject *) audio->soundcard,
					0, audio->audio_channels);
	if(!ags_audio_file_open(audio_file)){
	  filenames = filenames->next;
	  continue;
	}

	ags_audio_file_read_audio_signal(audio_file);
	ags_audio_file_close(audio_file);
	
	audio_signal_list = audio_file->audio_signal;
	
	for(j = 0; j < audio->audio_channels && audio_signal_list != NULL; j++){
	  /* create task */
	  error = NULL;

	  ags_channel_set_link(channel, NULL,
			       &error);

	  if(error != NULL){
	    g_message(error->message);
	  }

	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	  // FIXME:JK: create a task
	  channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
								 (gpointer) audio_signal_source_old);
	  channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								  audio_signal_list->data);

	  g_object_unref(G_OBJECT(audio_signal_source_old));

	  audio_signal_list = audio_signal_list->next;
	  channel = channel->next;
	}

	if(audio_file->channels < audio->audio_channels)
	  channel = ags_channel_nth(channel,
				    audio->audio_channels - audio_file->channels);
	
	filenames = filenames->next;
      }
    }
  }

  /* appending to channels */
  if(create_channels && filenames != NULL){
    list_length = g_slist_length(filenames);
    
    ags_audio_set_pads((AgsAudio *) audio, AGS_TYPE_INPUT,
		       list_length + AGS_AUDIO(audio)->input_pads);
    channel = ags_channel_nth(AGS_AUDIO(audio)->input,
			      (AGS_AUDIO(audio)->input_pads - list_length) * AGS_AUDIO(audio)->audio_channels);
    
    while(filenames != NULL){
      audio_file = ags_audio_file_new((gchar *) filenames->data,
				      (GObject *) audio->soundcard,
				      0, audio->audio_channels);
      if(!ags_audio_file_open(audio_file)){
	filenames = filenames->next;
	continue;
      }
      
      ags_audio_file_read_audio_signal(audio_file);
      ags_audio_file_close(audio_file);
	
      audio_signal_list = audio_file->audio_signal;
      
      for(j = 0; j < audio->audio_channels && audio_signal_list != NULL; j++){
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	
	channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
							       (gpointer) audio_signal_source_old);
	channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								audio_signal_list->data);
	
	g_object_unref(G_OBJECT(audio_signal_source_old));
	
	audio_signal_list = audio_signal_list->next;
	channel = channel->next;
      }
      
      if(audio->audio_channels > audio_file->channels)
	channel = ags_channel_nth(channel,
				  audio->audio_channels - audio_file->channels);
      
      filenames = filenames->next;
    }
  }
}

/**
 * ags_audio_find_port:
 * @audio: an #AgsAudio
 *
 * Retrieve all ports of #AgsAudio.
 *
 * Returns: a new #GList containing #AgsPort
 *
 * Since: 0.4
 */
GList*
ags_audio_find_port(AgsAudio *audio)
{
  GList *recall;
  GList *list;

  list = NULL;
 
  /* collect port of playing recall */
  recall = audio->play;
   
  while(recall != NULL){
    if(AGS_RECALL(recall->data)->port != NULL){
      if(list == NULL){
	list = g_list_copy(AGS_RECALL(recall->data)->port);
      }else{
	if(AGS_RECALL(recall->data)->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(AGS_RECALL(recall->data)->port));
	}
      }
    }
     
    recall = recall->next;
  }
 
  /* the same for true recall */
  recall = audio->recall;
   
  while(recall != NULL){
    if(AGS_RECALL(recall->data)->port != NULL){
      if(list == NULL){
	list = g_list_copy(AGS_RECALL(recall->data)->port);
      }else{
	if(AGS_RECALL(recall->data)->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(AGS_RECALL(recall->data)->port));
	}
      }
    }
       
    recall = recall->next;
  }
   
  /*  */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_audio_new:
 * @soundcard: an #AgsSoundcard
 *
 * Creates an #AgsAudio, with defaults of @soundcard.
 *
 * Returns: a new #AgsAudio
 *
 * Since: 0.3
 */
AgsAudio*
ags_audio_new(GObject *soundcard)
{
  AgsAudio *audio;

  audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
				    "soundcard\0", soundcard,
				    NULL);

  return(audio);
}
