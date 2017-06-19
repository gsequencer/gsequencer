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

#include <ags/audio/ags_audio.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_registry.h>

#include <ags/audio/ags_audio_connection.h>
#include <ags/audio/ags_preset.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/thread/ags_audio_thread.h>

#include <ags/audio/file/ags_audio_file.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include <ags/i18n.h>

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
void ags_audio_dispose(GObject *gobject);
void ags_audio_finalize(GObject *gobject);

void ags_audio_real_check_connection(AgsAudio *audio);

void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);
void ags_audio_real_set_pads(AgsAudio *audio,
			     GType type,
			     guint channels, guint channels_old);
void ags_audio_real_done(AgsAudio *audio,
			 AgsRecallID *recall_id);

enum{
  CHECK_CONNECTION,
  SET_AUDIO_CHANNELS,
  SET_PADS,
  INIT_RUN,
  TACT,
  DONE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_SEQUENCER,
  PROP_MIDI_FILE,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_SEQUENCE_LENGTH,
  PROP_AUDIO_CHANNELS,
  PROP_INPUT_PADS,
  PROP_INPUT_LINES,
  PROP_OUTPUT_PADS,
  PROP_OUTPUT_LINES,
  PROP_AUDIO_START_MAPPING,
  PROP_AUDIO_END_MAPPING,
  PROP_MIDI_START_MAPPING,
  PROP_MIDI_END_MAPPING,
  PROP_MIDI_CHANNEL,
  PROP_AUDIO_CONNECTION,
  PROP_PRESET,
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
					    "AgsAudio", &ags_audio_info,
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

  gobject->dispose = ags_audio_dispose;
  gobject->finalize = ags_audio_finalize;

  /* properties */
  /**
   * AgsAudio:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudio:sequencer:
   *
   * The assigned #AgsSequencer acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("sequencer",
				   i18n_pspec("assigned sequencer"),
				   i18n_pspec("The sequencer it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER,
				  param_spec);

  /**
   * AgsAudio:midi-file:
   *
   * The assigned #AgsMidiFile acting as default sink.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("midi-file",
				   i18n_pspec("assigned midi_file"),
				   i18n_pspec("The midi file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_FILE,
				  param_spec);
  
  /**
   * AgsAudio:samplerate:
   *
   * The samplerate.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudio:buffer-size:
   *
   * The buffer length.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudio:format:
   *
   * The format.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsAudio:sequence-length:
   *
   * The sequence length.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("sequence-length",
				 i18n_pspec("sequence-length"),
				 i18n_pspec("The sequence length"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCE_LENGTH,
				  param_spec);

  /**
   * AgsAudio:audio-channels:
   *
   * The audio channels count.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("audio channels count"),
				 i18n_pspec("The count of audio channels of audio"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("input-pads",
				 i18n_pspec("input pads count"),
				 i18n_pspec("The count of input pads of audio"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("input-lines",
				 i18n_pspec("input lines count"),
				 i18n_pspec("The count of input lines of audio"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("output-pads",
				 i18n_pspec("output pads count"),
				 i18n_pspec("The count of output pads of audio"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_uint("output-lines",
				 i18n_pspec("output lines count"),
				 i18n_pspec("The count of output lines of audio"),
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_LINES,
				  param_spec);

  /**
   * AgsAudio:audio-start-mapping:
   *
   * The audio start mapping.
   * 
   * Since: 0.7.3
   */
  param_spec = g_param_spec_uint("audio-start-mapping",
				 i18n_pspec("audio start mapping"),
				 i18n_pspec("The audio start mapping"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_START_MAPPING,
				  param_spec);

  /**
   * AgsAudio:audio-end-mapping:
   *
   * The audio end mapping.
   * 
   * Since: 0.7.3
   */
  param_spec = g_param_spec_uint("audio-end-mapping",
				 i18n_pspec("audio end mapping"),
				 i18n_pspec("The audio end mapping"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_END_MAPPING,
				  param_spec);

  /**
   * AgsAudio:midi-start-mapping:
   *
   * The midi start mapping.
   * 
   * Since: 0.7.3
   */
  param_spec =  g_param_spec_uint("midi-start-mapping",
				  i18n_pspec("midi start mapping range"),
				  i18n_pspec("The midi mapping range's start"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_START_MAPPING,
				  param_spec);

  /**
   * AgsAudio:midi-end-mapping:
   *
   * The midi end mapping.
   * 
   * Since: 0.7.3
   */
  param_spec = g_param_spec_uint("midi-end-mapping",
				 i18n_pspec("midi end mapping range"),
				 i18n_pspec("The midi mapping range's start"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_END_MAPPING,
				  param_spec);

  /**
   * AgsAudio:midi-channel:
   *
   * The midi channel.
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_uint("midi-channel",
				 i18n_pspec("midi channel"),
				 i18n_pspec("The midi channel"),
				 0,
				 16,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_CHANNEL,
				  param_spec);

  /**
   * AgsAudio:audio-connection:
   *
   * The assigned #GList-struct containing #AgsAudioConnection information.
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_object("audio-connection",
				   i18n_pspec("audio connection"),
				   i18n_pspec("The audio connection information"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CONNECTION,
				  param_spec);

  /**
   * AgsAudio:preset:
   *
   * The assigned #GList-struct containing #AgsPreset information.
   * 
   * Since: 0.8.5
   */
  param_spec = g_param_spec_object("preset",
				   i18n_pspec("preset"),
				   i18n_pspec("The preset"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);
    
  /**
   * AgsAudio:output:
   *
   * The #AgsOutput it contains.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("output",
				   i18n_pspec("containing output"),
				   i18n_pspec("The output it contains"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("input",
				   i18n_pspec("containing input"),
				   i18n_pspec("The input it contains"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("playback-domain",
				   i18n_pspec("assigned playback domain"),
				   i18n_pspec("The assigned playback domain"),
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
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("notation",
				    i18n_pspec("containing notation"),
				    i18n_pspec("The notation it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /**
   * AgsAudio:automation:
   *
   * The #AgsAutomation it contains.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("automation",
				    i18n_pspec("containing automation"),
				    i18n_pspec("The automation it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);

  /**
   * AgsAudio:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("recall-id",
				    i18n_pspec("assigned recall id"),
				    i18n_pspec("The assigned recall id"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT,
				  param_spec);

  /**
   * AgsAudio:recycling-context:
   *
   * The assigned #AgsRecyclingContext.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("recycling-context",
				    i18n_pspec("assigned recycling context"),
				    i18n_pspec("The assigned recall id"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT,
				  param_spec);

  /**
   * AgsAudio:recall-container:
   *
   * The #AgsRecallContainer it contains in container-context.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("recall-container",
				    i18n_pspec("containing recall container"),
				    i18n_pspec("The recall container it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsAudio:play:
   *
   * The #AgsRecall it contains in play-context.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("play",
				    i18n_pspec("containing play"),
				    i18n_pspec("The play it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /**
   * AgsAudio:recall:
   *
   * The #AgsRecall it contains in recall-context.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_pointer("recall",
				    i18n_pspec("containing recall"),
				    i18n_pspec("The recall it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);

  
  /* AgsAudioClass */
  audio->check_connection = ags_audio_real_check_connection;

  audio->set_audio_channels = ags_audio_real_set_audio_channels;
  audio->set_pads = ags_audio_real_set_pads;

  audio->init_run = NULL;
  audio->tact = NULL;
  audio->done = ags_audio_real_done;

  /* signals */
  /**
   * AgsAudio::check-connection:
   * @audio: the object to init run.
   *
   * The ::check-connection checks your audio connections.
   *
   * Since: 0.7.65
   */
  audio_signals[CHECK_CONNECTION] = 
    g_signal_new("check-connection",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, check_connection),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

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
    g_signal_new("set-audio-channels",
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
    g_signal_new("set-pads",
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
    g_signal_new("init-run",
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
    g_signal_new("tact",
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
    g_signal_new("done",
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
  AgsConfig *config;
  
  AgsMutexManager *mutex_manager;

  gchar *str;
  gchar *str0, *str1;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert audio mutex */
  //FIXME:JK: memory leak
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) audio,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* config */
  config = ags_config_get_instance();
  
  /* base init */
  audio->flags = 0;

  audio->soundcard = NULL;

  audio->sequencer = NULL;
  audio->midi_file = NULL;
  
  audio->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  audio->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  audio->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  audio->sequence_length = 0;
  
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
    audio->samplerate = g_ascii_strtoull(str,
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
    audio->buffer_size = g_ascii_strtoull(str,
					  NULL,
					  10);

    free(str);
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    audio->format = g_ascii_strtoull(str,
				     NULL,
				     10);

    free(str);
  }

  /* channel allocation */
  audio->audio_channels = 0;

  audio->output_pads = 0;
  audio->output_lines = 0;
  audio->input_pads = 0;
  audio->input_lines = 0;

  /* midi mapping */
  audio->audio_start_mapping = 0;
  audio->audio_end_mapping = 0;

  audio->midi_start_mapping = 0;
  audio->midi_end_mapping = 0;

  audio->midi_channel = 0;

  /* mapping */
  audio->audio_connection = NULL;
  
  /* preset */
  audio->preset = NULL;

  /* channels */
  audio->output = NULL;
  audio->input = NULL;

  /* playback domain */
  audio->playback_domain = (GObject *) ags_playback_domain_new();
  g_object_ref(audio->playback_domain);
  g_object_set(audio->playback_domain,
	       "domain", audio,
	       NULL);

  /* notation and automation */
  audio->notation = NULL;
  audio->automation = NULL;

  /* recycling context */
  audio->recall_id = NULL;
  audio->recycling_context = NULL;

  /* recall */
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio->recall_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio->recall_mutex,
		     attr);

  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio->play_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio->play_mutex,
		     attr);

  audio->container = NULL;
  
  audio->recall = NULL;
  audio->play = NULL;

  audio->recall_remove= NULL;
  audio->play_remove = NULL;

  /* data */
  audio->machine = NULL;
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

      soundcard = g_value_get_object(value);

      ags_audio_set_soundcard(audio, (GObject *) soundcard);
    }
    break;
  case PROP_SEQUENCER:
    {
      GObject *sequencer;

      sequencer = g_value_get_object(value);

      if(audio->sequencer == sequencer){
	return;
      }

      if(audio->sequencer != NULL){
	g_object_unref(audio->sequencer);
      }

      if(sequencer != NULL) {
	g_object_ref(sequencer);
      }

      audio->sequencer = sequencer;
    }
    break;
  case PROP_MIDI_FILE:
    {
      GObject *midi_file;

      midi_file = g_value_get_object(value);

      if(audio->midi_file == midi_file){
	return;
      }

      if(audio->midi_file != NULL){
	g_object_unref(audio->midi_file);
      }

      if(midi_file != NULL) {
	g_object_ref(midi_file);
      }

      audio->midi_file = midi_file;
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_audio_set_samplerate(audio,
			       samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_audio_set_buffer_size(audio,
				  buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      ags_audio_set_format(audio,
			   format);
    }
    break;
  case PROP_SEQUENCE_LENGTH:
    {
      guint sequence_length;

      sequence_length = g_value_get_uint(value);

      ags_audio_set_sequence_length(audio,
				    sequence_length);
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
  case PROP_AUDIO_START_MAPPING:
    {
      guint audio_start_mapping;

      audio_start_mapping = g_value_get_uint(value);

      audio->audio_start_mapping = audio_start_mapping;
    }
    break;
  case PROP_AUDIO_END_MAPPING:
    {
      guint audio_end_mapping;

      audio_end_mapping = g_value_get_uint(value);

      audio->audio_end_mapping = audio_end_mapping;
    }
    break;
  case PROP_MIDI_START_MAPPING:
    {
      guint midi_start_mapping;

      midi_start_mapping = g_value_get_uint(value);

      audio->midi_start_mapping = midi_start_mapping;
    }
    break;
  case PROP_MIDI_END_MAPPING:
    {
      guint midi_end_mapping;

      midi_end_mapping = g_value_get_uint(value);

      audio->midi_end_mapping = midi_end_mapping;
    }
    break;
  case PROP_MIDI_CHANNEL:
    {
      guint midi_channel;

      midi_channel = g_value_get_uint(value);

      audio->midi_channel = midi_channel;
    }
    break;
  case PROP_AUDIO_CONNECTION:
    {
      AgsAudioConnection *audio_connection;

      audio_connection = (AgsAudioConnection *) g_value_get_object(value);

      if(audio_connection == NULL ||
	 g_list_find(audio->audio_connection, audio_connection) != NULL){
	return;
      }

      ags_audio_add_audio_connection(audio,
				     (GObject *) audio_connection);
    }
    break;
  case PROP_PRESET:
    {
      AgsPreset *preset;

      preset = (AgsPreset *) g_value_get_object(value);

      if(preset == NULL ||
	 g_list_find(audio->preset, preset) != NULL){
	return;
      }

      ags_audio_add_preset(audio,
			   (GObject *) preset);
    }
    break;
  case PROP_PLAYBACK_DOMAIN:
    {
      AgsPlaybackDomain *playback_domain;

      playback_domain = (AgsPlaybackDomain *) g_value_get_object(value);

      if(audio->playback_domain == (GObject *) playback_domain){
	return;
      }

      if(audio->playback_domain != NULL){
	g_object_unref(audio->playback_domain);
      }

      if(playback_domain != NULL){
	g_object_ref(playback_domain);
      }

      audio->playback_domain = (GObject *) playback_domain;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_pointer(value);

      if(notation == NULL ||
	 g_list_find(audio->notation, notation) != NULL){
	return;
      }

      ags_audio_add_notation(audio,
			     (GObject *) notation);
    }
    break;
  case PROP_AUTOMATION:
    {
      AgsAutomation *automation;

      automation = (AgsAutomation *) g_value_get_pointer(value);

      if(automation == NULL ||
	 g_list_find(audio->automation, automation) != NULL){
	return;
      }

      ags_audio_add_automation(audio,
			       (GObject *) automation);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_pointer(value);

      if(recall_id == NULL ||
	 g_list_find(audio->recall_id, recall_id) != NULL){
	return;
      }

      ags_audio_add_recall_id(audio,
			      (GObject *) recall_id);
    }
    break;
  case PROP_RECYCLING_CONTEXT:
    {
      AgsRecyclingContext *recycling_context;

      recycling_context = (AgsRecyclingContext *) g_value_get_pointer(value);

      if(recycling_context == NULL ||
	 g_list_find(audio->recycling_context, recycling_context) != NULL){
	return;
      }

      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_pointer(value);

      if(recall_container == NULL ||
	 g_list_find(audio->container, recall_container) != NULL){
	return;
      }

      ags_audio_add_recall_container(audio,
				     (GObject *) recall_container);
    }
    break;
  case PROP_PLAY:
    {
      AgsRecall *recall;

      gboolean play_added;
      
      recall = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(audio->play_mutex);
      
      play_added = (g_list_find(audio->play, recall) != NULL) ? TRUE: FALSE;

      pthread_mutex_unlock(audio->play_mutex);
      
      if(recall == NULL ||
	 play_added){
	return;
      }

      ags_audio_add_recall(audio,
			   (GObject *) recall,
			   TRUE);
    }
    break;
  case PROP_RECALL:
    {
      AgsRecall *recall;

      gboolean recall_added;
      
      recall = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(audio->recall_mutex);
      
      recall_added = (g_list_find(audio->recall, recall) != NULL) ? TRUE: FALSE;

      pthread_mutex_unlock(audio->recall_mutex);

      if(recall == NULL ||
	 recall_added){
	return;
      }

      ags_audio_add_recall(audio,
			   (GObject *) recall,
			   FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
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
  case PROP_SEQUENCER:
    {
      g_value_set_object(value, audio->sequencer);
    }
    break;
  case PROP_MIDI_FILE:
    {
      g_value_set_object(value, audio->midi_file);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, audio->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, audio->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, audio->format);
    }
    break;
  case PROP_SEQUENCE_LENGTH:
    {
      g_value_set_uint(value, audio->sequence_length);
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
  case PROP_AUDIO_START_MAPPING:
    {
      g_value_set_uint(value, audio->audio_start_mapping);
    }
    break;
  case PROP_AUDIO_END_MAPPING:
    {
      g_value_set_uint(value, audio->audio_end_mapping);
    }
    break;
  case PROP_MIDI_START_MAPPING:
    {
      g_value_set_uint(value, audio->midi_start_mapping);
    }
    break;
  case PROP_MIDI_END_MAPPING:
    {
      g_value_set_uint(value, audio->midi_end_mapping);
    }
    break;
  case PROP_MIDI_CHANNEL:
    {
      g_value_set_uint(value, audio->midi_channel);
    }
    break;
  case PROP_OUTPUT:
    {
      g_value_set_object(value, audio->output);
    }
    break;
  case PROP_INPUT:
    {
      g_value_set_object(value, audio->input);
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
      pthread_mutex_lock(audio->play_mutex);
      
      g_value_set_pointer(value, g_list_copy(audio->play));

      pthread_mutex_unlock(audio->play_mutex);
    }
    break;
  case PROP_RECALL:
    {
      pthread_mutex_lock(audio->recall_mutex);
      
      g_value_set_pointer(value, g_list_copy(audio->recall));

      pthread_mutex_unlock(audio->recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_audio_dispose(GObject *gobject)
{
  AgsAudio *audio;

  GList *list, *list_next;

  audio = AGS_AUDIO(gobject);
  
  /* soundcard */
  if(audio->soundcard != NULL){    
    g_object_unref(audio->soundcard);

    audio->soundcard = NULL;
  }

  /* sequencer */
  if(audio->sequencer != NULL){
    g_object_unref(audio->sequencer);

    audio->sequencer = NULL;
  }

  /* midi file */
  if(audio->midi_file != NULL){
    g_object_unref(audio->midi_file);

    audio->midi_file = NULL;
  }

  /* audio connection */
  if(audio->audio_connection != NULL){
    list = audio->audio_connection;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->audio_connection,
		     g_object_unref);

    audio->audio_connection = NULL;
  }

  /* preset */
  if(audio->preset != NULL){
    list = audio->preset;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->preset,
		     g_object_unref);

    audio->preset = NULL;
  }

  /* notation */
  if(audio->notation != NULL){
    list = audio->notation;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->notation,
		     g_object_unref);

    audio->notation = NULL;
  }
  
  /* automation */
  if(audio->automation != NULL){
    list = audio->automation;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->automation,
		     g_object_unref);

    audio->automation = NULL;
  }

  /* recall id */
  if(audio->recall_id != NULL){
    list = audio->recall_id;
    
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
      
      list = list_next;
    }

    
    g_list_free_full(audio->recall_id,
		     g_object_unref);

    audio->recall_id = NULL;
  }
  
  /* recycling context */
  if(audio->recycling_context != NULL){
    list = audio->recycling_context;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->recycling_context,
		     g_object_unref);

    audio->recycling_context = NULL;
  }
  
  /* recall container */
  if(audio->container != NULL){
    list = audio->container;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(audio->container,
		     g_object_unref);

    audio->container = NULL;
  }

  /* recall */
  if(audio->recall != NULL){
    pthread_mutex_lock(audio->recall_mutex);
    
    list = audio->recall;
  
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(audio->recall,
		     g_object_unref);

    audio->recall = NULL;

    pthread_mutex_unlock(audio->recall_mutex);
  }

  /* play */
  if(audio->play != NULL){
    pthread_mutex_lock(audio->play_mutex);

    list = audio->play;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(audio->play,
		     g_object_unref);

    audio->play = NULL;

    pthread_mutex_unlock(audio->play_mutex);
  }
  
  /* remove */
  if(audio->recall_remove != NULL){
    g_list_free_full(audio->recall_remove,
		     g_object_unref);
  }

  if(audio->play_remove != NULL){
    g_list_free_full(audio->play_remove,
		     g_object_unref);
  }
  
  audio->recall_remove = NULL;
  audio->play_remove = NULL;
  
  /* channels */
  ags_audio_set_audio_channels(audio,
			       0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     0);

  /* playback domain */
  if(audio->playback_domain != NULL){
    g_object_run_dispose(audio->playback_domain);

    audio->playback_domain = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->dispose(gobject);
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *application_mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  audio = AGS_AUDIO(gobject);

  if(audio->soundcard != NULL){
    g_object_unref(audio->soundcard);
  }

  if(audio->sequencer != NULL){
    g_object_unref(audio->sequencer);
  }

  if(audio->midi_file != NULL){
    g_object_unref(audio->midi_file);
  }
  
  /* audio connection */
  if(audio->audio_connection != NULL){
    list = audio->audio_connection;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->audio_connection,
		     g_object_unref);
  }

  /* audio connection */
  if(audio->preset != NULL){
    list = audio->preset;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->preset,
		     g_object_unref);
  }
  
  /* playback domain */
  if(audio->playback_domain != NULL){
    g_object_unref(audio->playback_domain);
  }
  
  /* notation */
  if(audio->notation != NULL){
    list = audio->notation;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->notation,
		     g_object_unref);
  }
  
  /* automation */
  if(audio->automation != NULL){
    list = audio->automation;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->automation,
		     g_object_unref);
  }
  
  /* recall id */
  if(audio->recall_id != NULL){
    g_list_free_full(audio->recall_id,
		     g_object_unref);
  }
  
  /* recycling context */
  if(audio->recycling_context != NULL){
    g_list_free_full(audio->recycling_context,
		     g_object_unref);
  }
  
  /* recall */
  if(audio->container != NULL){
    g_list_free_full(audio->container,
		     g_object_unref);
  }

  if(audio->recall != NULL){
    list = audio->recall;

    while(list != NULL){
      if(AGS_IS_RECALL_AUDIO(list->data) ||
	 AGS_IS_RECALL_AUDIO_RUN(list->data)){
	g_object_set(list->data,
		     "audio", NULL,
		     NULL);
      }
    
      list = list->next;
    }

    g_list_free_full(audio->recall,
		     g_object_unref);
  }

  if(audio->play != NULL){
    list = audio->play;

    while(list != NULL){
      if(AGS_IS_RECALL_AUDIO(list->data) ||
	 AGS_IS_RECALL_AUDIO_RUN(list->data)){
	g_object_set(list->data,
		     "audio", NULL,
		     NULL);
      }
    
      list = list->next;
    }

    g_list_free_full(audio->play,
		     g_object_unref);
  }
  
  /* remove */
  if(audio->recall_remove != NULL){
    g_list_free_full(audio->recall_remove,
		     g_object_unref);
  }

  if(audio->play_remove != NULL){
    g_list_free_full(audio->play_remove,
		     g_object_unref);
  }
  
  /* channels */
  ags_audio_set_audio_channels(audio,
			       0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     0);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

void
ags_audio_add_to_registry(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;

  AgsServer *server;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;
  
  audio = AGS_AUDIO(connectable);

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(audio->soundcard));

  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

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
  audio->flags |= AGS_AUDIO_CONNECTED;
  
#ifdef AGS_DEBUG
  g_message("connecting audio");
#endif

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
  /* empty */
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
  AgsMutexManager *mutex_manager;
  
  guint samplerate;
  guint buffer_size;
  guint format;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);
  
  /* audio */
  if(audio->soundcard == soundcard){
    pthread_mutex_unlock(mutex);
    return;
  }

  if(audio->soundcard != NULL){
    g_object_unref(audio->soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  audio->soundcard = (GObject *) soundcard;

  if(soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      NULL,
			      &samplerate,
			      &buffer_size,
			      &format);
    g_object_set(audio,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);
  }
  
  /* playback domain */
  if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1] != NULL){
    g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
		 "soundcard", soundcard,
		 NULL);
  }

  if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2] != NULL){
    g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
		 "soundcard", soundcard,
		 NULL);
  }
  
  /* recall */
  list = audio->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard", soundcard,
		 NULL);
    
    list = list->next;
  }
  
  list = audio->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "soundcard", soundcard,
		 NULL);
    
    list = list->next;
  }
  
  /* input */
  channel = audio->input;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "soundcard", soundcard,
		 NULL);
    
    channel = channel->next;
  }

  /* output */
  channel = audio->output;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "soundcard", soundcard,
		 NULL);
    
    channel = channel->next;
  }
  
  pthread_mutex_unlock(mutex);
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
    //TODO:JK: implement me
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
    //TODO:JK: implement me
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
ags_audio_real_check_connection(AgsAudio *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_check_connection(AgsAudio *audio)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* lock */
  pthread_mutex_lock(mutex);

  if(!AGS_IS_AUDIO(audio)){
    pthread_mutex_unlock(mutex);
    return;
  }

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[CHECK_CONNECTION], 0);
  g_object_unref((GObject *) audio);

  /* unlock */
  pthread_mutex_unlock(mutex);
}

void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsMutexManager *mutex_manager;
  
  gboolean alloc_recycling;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *prev_mutex, *prev_pad_mutex, *current_mutex;
  
  auto void ags_audio_set_audio_channels_init_parameters(GType type);
  auto void ags_audio_set_audio_channels_grow(GType type);
  auto void ags_audio_set_audio_channels_shrink_zero();
  auto void ags_audio_set_audio_channels_shrink();
  auto void ags_audio_set_audio_channels_grow_notation();
  auto void ags_audio_set_audio_channels_shrink_notation();
  auto void ags_audio_set_audio_channels_shrink_automation();
  
  void ags_audio_set_audio_channels_init_parameters(GType type){
    if(type == AGS_TYPE_OUTPUT){
      link_recycling = FALSE;

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;

	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  if((AGS_AUDIO_SYNC & (audio->flags)) != 0 && (AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	    set_sync_link = FALSE;
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	    set_async_link = TRUE;
	    set_sync_link = FALSE;
	  }else{
#ifdef AGS_DEBUG
	    g_message("ags_audio_set_audio_channels - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined");
#endif
	    set_sync_link = FALSE;
	    set_async_link = FALSE;
	  }
	}
      }
    }else{
      set_sync_link = FALSE;
      set_async_link = FALSE;
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & audio->flags) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;
      }
      
      if((AGS_AUDIO_ASYNC & audio->flags) != 0 && alloc_recycling){
	link_recycling = TRUE;
      }else{
	link_recycling = FALSE;
      }
    }    
  }
  
  void ags_audio_set_audio_channels_grow(GType type){
    AgsChannel *channel, *start, *current, *pad_next;
    AgsRecycling *first_recycling, *last_recycling;
	  
    guint pads;
    guint i, j;

    if(type == AGS_TYPE_OUTPUT){
      /* AGS_TYPE_OUTPUT */
      pads = audio->output_pads;
      
      start = audio->output;
    }else{
      /* AGS_TYPE_INPUT */
      pads = audio->input_pads;

      start = audio->input;
    }

    /* grow */
    for(j = 0; j < pads; j++){
      if(audio_channels_old != 0){
	pad_next = ags_channel_nth(start, j * audio_channels)->next_pad;
      }else{
	pad_next = NULL;
      }

      for(i = audio_channels_old; i < audio_channels; i++){
	channel = (AgsChannel *) g_object_new(type,
					      "audio", (GObject *) audio,
					      "soundcard", audio->soundcard,
					      "samplerate", audio->samplerate,
					      "buffer-size", audio->buffer_size,
					      NULL);
	g_object_ref(channel);
	
	if(start == NULL){
	  start = channel;

	  if(type == AGS_TYPE_OUTPUT){
	    audio->output = channel;
	  }else{
	    audio->input = channel;
	  }
	}

	if(j * audio_channels + i != 0){
	  /* set prev */
	  channel->prev = ags_channel_nth(start, j * audio_channels + i - 1);

	  pthread_mutex_lock(application_mutex);
	  
	  prev_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) channel->prev);
	  
	  pthread_mutex_unlock(application_mutex);
	  
	  pthread_mutex_lock(prev_mutex);  

	  if(audio_channels_old != 0 &&
	     i == audio_channels - 1){
	    channel->next = pad_next;
	  }
	  
	  channel->prev->next = channel;

	  pthread_mutex_unlock(prev_mutex);
	}

	if(j != 0){
	  /* set prev pad */
	  channel->prev_pad = ags_channel_pad_nth(ags_channel_nth(start,
								  i),
						  j - 1);

	  pthread_mutex_lock(application_mutex);
	  
	  prev_pad_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) channel->prev_pad);
	  
	  pthread_mutex_unlock(application_mutex);

	  pthread_mutex_lock(prev_pad_mutex);
	  
	  channel->prev_pad->next_pad = channel;

	  pthread_mutex_unlock(prev_pad_mutex);
	}

	/* set indices */
	channel->pad = j;
	channel->audio_channel = i;
	channel->line = j * audio->audio_channels + i;

	/* reset nested AgsRecycling tree */
	if(alloc_recycling){
	  first_recycling =
	    last_recycling = ags_recycling_new(audio->soundcard);
	  g_object_ref(first_recycling);
	  g_object_set(first_recycling,
		       "channel", channel,
		       NULL);
	  
	  ags_channel_set_recycling(channel,
				    first_recycling, last_recycling,
				    TRUE, TRUE);
	}else if(set_sync_link){
	  AgsChannel *input;

	  input = ags_channel_nth(audio->input,
				  channel->line);
	  
	  /* set sync link */
	  if(input != NULL){
	    first_recycling = input->first_recycling;
	    last_recycling = input->last_recycling;
	    
	    ags_channel_set_recycling(channel,
				      first_recycling, last_recycling,
				      TRUE, TRUE);
	  }
	}else if(set_async_link){
	  AgsChannel *input, *input_pad_last;

	  input = ags_channel_nth(audio->input,
				  i);

	  /* set async link */
	  if(input != NULL){
	    input_pad_last = ags_channel_pad_last(input);

	    first_recycling = input->first_recycling;
	    last_recycling = input_pad_last->last_recycling;
	    
	    ags_channel_set_recycling(channel,
				      first_recycling, last_recycling,
				      TRUE, TRUE);
	  }
	}
      }
    }
  }
  
  void ags_audio_set_audio_channels_shrink_zero(){
    AgsChannel *channel, *start, *channel_next;
    
    gboolean first_run;
    
    GError *error;

    start =
      channel = audio->output;
    first_run = TRUE;

    error = NULL;

  ags_audio_set_audio_channel_shrink_zero0:

    while(channel != NULL){
      error =  NULL;
      ags_channel_set_link(channel, NULL,
			   &error);

      if(error != NULL){
	g_error("%s", error->message);
      }
      
      channel = channel->next;
    }

    channel = start;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_run_dispose(channel);
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
    AgsChannel *channel, *start;
    AgsChannel *channel0, *channel1;
    AgsRecycling *recycling;

    guint pads, i, j;
    gboolean first_run;
    
    GError *error;

    start =
      channel = audio->output;
    pads = audio->output_pads;
    first_run = TRUE;

  ags_audio_set_audio_channel_shrink0:

    for(i = 0; i < pads; i++){
      channel = ags_channel_nth(channel, audio_channels);

      for(j = audio_channels; j < audio->audio_channels; j++){
	error = NULL;
	ags_channel_set_link(channel, NULL,
			     &error);
	
	if(error != NULL){
	  g_error("%s", error->message);
	}
	
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
      
	  g_object_run_dispose(channel0);
	  g_object_unref((GObject *) channel0);

	  channel0 = channel1;
	}

	channel->next = channel1;

	if(channel1 != NULL){
	  channel1->prev = channel;
	}else{
	  break;
	}
	
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
    g_message("ags_audio_set_audio_channels_grow_notation\n");
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
      list->data = (gpointer) ags_notation_new((GObject *) audio,
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

  void ags_audio_set_audio_channels_shrink_automation(){
    GList *automation, *automation_next;

    automation = audio->automation;

    while(automation != NULL){
      automation_next = automation->next;

      if(AGS_AUTOMATION(automation->data)->channel_type != G_TYPE_NONE){
	if(audio_channels == 0 ||
	   AGS_AUTOMATION(automation->data)->line % audio_channels_old >= audio_channels){
	  ags_audio_remove_automation(audio,
				      automation->data);
	}
      }

      automation = automation_next;
    }
  }
  
  /* entry point */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* grow / shrink */
  if(audio_channels > audio_channels_old){
    AgsPlaybackDomain *playback_domain;
    AgsChannel *current;

    guint i;
    
    /* grow audio channels */
    if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0){
      ags_audio_set_audio_channels_grow_notation();
    }

    if(audio->input_pads > 0 &&
       (AGS_AUDIO_NO_INPUT & (audio->flags)) == 0){
      ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
      ags_audio_set_audio_channels_grow(AGS_TYPE_INPUT);
    }

    audio->input_lines = audio_channels * audio->input_pads;

    if(audio->output_pads > 0 &&
       (AGS_AUDIO_NO_OUTPUT & (audio->flags)) == 0){
      ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
      ags_audio_set_audio_channels_grow(AGS_TYPE_OUTPUT);
    }

    audio->output_lines = audio_channels * audio->output_pads;

    /* grow playback domain */
    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
    current = ags_channel_nth(audio->output,
			      audio->audio_channels);

    if(current != NULL){
      for(i = 0; i < audio_channels - audio_channels_old; i++){
	playback_domain->playback = g_list_append(playback_domain->playback,
						  current->playback);
	
	current = current->next;
      }
    }
  }else if(audio_channels < audio_channels_old){
    AgsPlaybackDomain *playback_domain;
    AgsPlayback *playback;
    AgsChannel *current;

    GList *list;
    
    guint i, j;
    
    /* shrink audio channels */
    ags_audio_set_audio_channels_shrink_automation();
    
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_shrink_notation();

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero();
    }else{
      ags_audio_set_audio_channels_shrink();
    }

    audio->input_lines = audio_channels * audio->input_pads;
    audio->output_lines = audio_channels * audio->output_pads;

    /* shrink playback domain */
    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
    list = playback_domain->playback;
    
    for(j = 0; j < audio->output_pads; j++){
      for(i = 0; i < audio->audio_channels - audio_channels; i++){
	playback = g_list_nth_prev(g_list_last(list),
				   (audio->output_pads - j - 1) * audio_channels)->data;
	playback_domain->playback = g_list_remove(playback_domain->playback,
						  playback);
	g_object_run_dispose(playback);
	g_object_unref(playback);
      }
    }
  }

  /* apply new sizes */
  audio->audio_channels = audio_channels;
  // input_lines must be set earlier because set_sync_link needs it
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* lock */
  pthread_mutex_lock(mutex);

  if(!AGS_IS_AUDIO(audio)){
    pthread_mutex_unlock(mutex);
    return;
  }

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio->audio_channels);
  g_object_unref((GObject *) audio);

  /* unlock */
  pthread_mutex_unlock(mutex);
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
  AgsChannel *channel;
  
  AgsMutexManager *mutex_manager;
  
  gboolean alloc_recycling, link_recycling, set_sync_link, set_async_link;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *prev_mutex, *prev_pad_mutex;

  auto void ags_audio_set_pads_init_parameters();
  auto void ags_audio_set_pads_grow();
  auto void ags_audio_set_pads_unlink_all(AgsChannel *channel);
  auto void ags_audio_set_pads_shrink_zero(AgsChannel *channel);
  auto void ags_audio_set_pads_shrink(AgsChannel *channel);
  auto void ags_audio_set_pads_alloc_notation();
  auto void ags_audio_set_pads_free_notation();
  auto void ags_audio_set_pads_add_notes();
  auto void ags_audio_set_pads_remove_notes();
  auto void ags_audio_set_pads_shrink_automation();
  
  void ags_audio_set_pads_init_parameters(){
    alloc_recycling = FALSE;
    link_recycling = FALSE;
    set_sync_link = FALSE;
    set_async_link = FALSE;
    
    if(type == AGS_TYPE_OUTPUT){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  if((AGS_AUDIO_SYNC & (audio->flags)) != 0 && (AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	    set_async_link = TRUE;
	  }else{
#ifdef AGS_DEBUG
	    g_message("ags_audio_set_pads - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined");
#endif
	  }
	}
      }
    }else{      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & audio->flags) != 0){
	alloc_recycling = TRUE;
      }
      
      if((AGS_AUDIO_ASYNC & audio->flags) != 0 && alloc_recycling){
	link_recycling = TRUE;
      }
    }    
  }

  void ags_audio_set_pads_grow(){
    AgsChannel *start, *channel;
    AgsRecycling *first_recycling, *last_recycling;

    guint i, j;
    
    if(type == AGS_TYPE_OUTPUT){
      start = audio->output;
    }else{
      start = audio->input;
    }

    for(j = pads_old; j < pads; j++){
      for(i = 0; i < audio->audio_channels; i++){
	channel = (AgsChannel *) g_object_new(type,
					      "audio", (GObject *) audio,
					      "soundcard", audio->soundcard,
					      "samplerate", audio->samplerate,
					      "buffer-size", audio->buffer_size,
					      NULL);
	g_object_ref(channel);
	
	if(start == NULL){
	  /* set first channel in AgsAudio */
	  if(type == AGS_TYPE_OUTPUT){
	    start = 
	      audio->output = channel;
	  }else{
	    start = 
	      audio->input = channel;
	  }
	}

	if(j * audio->audio_channels + i != 0){
	  /* set prev */
	  channel->prev = ags_channel_nth(start, j * audio->audio_channels + i - 1);
	  
	  pthread_mutex_lock(application_mutex);
	  
	  prev_mutex = ags_mutex_manager_lookup(mutex_manager,
						(GObject *) channel->prev);
	  
	  pthread_mutex_unlock(application_mutex);
	  
	  pthread_mutex_lock(prev_mutex);
	  
	  channel->prev->next = channel;

	  pthread_mutex_unlock(prev_mutex);
	}
	
	if(j != 0){
	  /* set prev pad */
	  channel->prev_pad = ags_channel_pad_nth(ags_channel_nth(start,
								  i),
						  j - 1);

	  pthread_mutex_lock(application_mutex);
	  
	  prev_pad_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) channel->prev_pad);
	  
	  pthread_mutex_unlock(application_mutex);

	  pthread_mutex_lock(prev_pad_mutex);
	  
	  channel->prev_pad->next_pad = channel;
	  
	  pthread_mutex_unlock(prev_pad_mutex);
	}

	/* set indices */
	channel->pad = j;
	channel->audio_channel = i;
	channel->line = j * audio->audio_channels + i;

	/* reset nested AgsRecycling tree */
	if(alloc_recycling){
	  first_recycling =
	    last_recycling = ags_recycling_new(audio->soundcard);
	  g_object_ref(first_recycling);
	  g_object_set(first_recycling,
		       "channel", channel,
		       NULL);
	  
	  ags_channel_set_recycling(channel,
				    first_recycling, last_recycling,
				    TRUE, TRUE);
	}else if(set_sync_link){
	  AgsChannel *input;

	  input = ags_channel_nth(audio->input,
				  channel->line);
	  
	  /* set sync link */
	  if(input != NULL){
	    first_recycling = input->first_recycling;
	    last_recycling = input->last_recycling;
	    
	    ags_channel_set_recycling(channel,
				      first_recycling, last_recycling,
				      TRUE, TRUE);
	  }
	}else if(set_async_link){
	  AgsChannel *input, *input_pad_last;

	  input = ags_channel_nth(audio->input,
				  i);

	  /* set async link */
	  if(input != NULL){
	    input_pad_last = ags_channel_pad_last(input);

	    first_recycling = input->first_recycling;
	    last_recycling = input_pad_last->last_recycling;
	    
	    ags_channel_set_recycling(channel,
				      first_recycling, last_recycling,
				      TRUE, TRUE);
	  }
	}
      }
    }
  }

  void ags_audio_set_pads_unlink_all(AgsChannel *channel){
    GError *error;

    while(channel != NULL){
      error = NULL;
      ags_channel_set_link(channel, NULL,
			   &error);

      if(error != NULL){
	g_error("%s", error->message);
      }

      channel = channel->next;
    }
  }
  
  void ags_audio_set_pads_shrink_zero(AgsChannel *channel){
    AgsChannel *channel_next;

    GError *error;
    
    while(channel != NULL){
      channel_next = channel->next;

      error = NULL;
      ags_channel_set_link(channel, NULL,
			   &error);

      if(error != NULL){
	g_error("%s", error->message);
      }
      
      g_object_run_dispose(channel);
      g_object_unref((GObject *) channel);

      channel = channel_next;
    }
  }

  void ags_audio_set_pads_shrink(AgsChannel *channel){
    AgsChannel *current;

    guint i;

    current = channel;
    
    if(channel != NULL &&
       channel->prev_pad != NULL){
      channel = channel->prev_pad;
    }else{
      channel = NULL;
    }
    
    ags_audio_set_pads_shrink_zero(current);

    /* remove pads */
    if(channel != NULL){
      current = channel;
      
      for(i = 0; i < audio->audio_channels; i++){
	current->next_pad = NULL;
	
	/* iterate */
	current = current->next;
      }

      /* remove channel */
      current = ags_channel_nth(channel,
				audio->audio_channels - 1);
      current->next = NULL;
    }
  }

  void ags_audio_set_pads_alloc_notation(){
    GList *list;
    
    guint i;

#ifdef AGS_DEBUG
    g_message("ags_audio_set_pads_alloc_notation\n");
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

      list->data = (gpointer) ags_notation_new((GObject *) audio,
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

  void ags_audio_set_pads_shrink_automation(){
    GList *automation, *automation_next;

    automation = audio->automation;

    while(automation != NULL){
      automation_next = automation->next;

      if(AGS_AUTOMATION(automation->data)->channel_type == type){
	if(AGS_AUTOMATION(automation->data)->line >= pads * audio->audio_channels){
	  ags_audio_remove_automation(audio,
				      automation->data);
	}
      }
      
      automation = automation_next;
    }
  }
  
  /* entry point */
  ags_audio_set_pads_init_parameters();

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  if(g_type_is_a(type, AGS_TYPE_OUTPUT)){
    /* output */
    pads_old = audio->output_pads;

    if(pads_old == pads){
      return;
    }

    if(audio->audio_channels == 0){
      audio->output_pads = pads;
      return;
    }

    /* grow or shrink */
    if(pads > audio->output_pads){
      AgsPlaybackDomain *playback_domain;
      AgsChannel *current;

      guint i, j;

      /* instantiate notation */
      if(pads_old == 0){
	if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) == 0){
	  ags_audio_set_pads_alloc_notation();
	}
      }

      if((AGS_AUDIO_NO_OUTPUT & (audio->flags)) == 0){
	/* grow channels */
	ags_audio_set_pads_grow();
      
	/* alloc playback domain */
	playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
	current = audio->output;

	current = ags_channel_pad_nth(current,
				      pads_old);

	for(j = pads_old; j < pads; j++){
	  for(i = 0; i < audio->audio_channels; i++){
	    playback_domain->playback = g_list_append(playback_domain->playback,
						      current->playback);
	
	    current = current->next;
	  }
	}
      }
    }else if(pads == 0){
      GList *list;
      
      if((AGS_AUDIO_NO_OUTPUT & (audio->flags)) == 0){
	ags_audio_set_pads_shrink_automation();      

	if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	   audio->notation != NULL){
	  ags_audio_set_pads_free_notation();
	}

	channel = audio->output;
      
	/* unlink and remove */
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink_zero(channel);
      
	audio->output = NULL;

	/* remove playback domain */
	list = AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback;

	while(list != NULL){
	  g_object_run_dispose(list->data);

	  list = list->next;
	}
      
	g_list_free_full(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback,
			 g_object_unref);

	AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback = NULL;
      }
    }else if(pads < audio->output_pads){
      AgsPlaybackDomain *playback_domain;

      guint i;
      
      channel = audio->output;

      if((AGS_AUDIO_NO_OUTPUT & (audio->flags)) == 0){
	ags_audio_set_pads_shrink_automation();      
	ags_audio_set_pads_remove_notes();

	channel = ags_channel_pad_nth(channel,
				      pads);
      
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink(channel);

	playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

	for(i = 0; i < audio->output_pads - pads; i++){
	  AgsPlayback *playback;
	  GList *list;

	  list = g_list_last(playback_domain->playback);

	  if(list != NULL){
	    playback = list->data;
	    playback_domain->playback = g_list_remove(playback_domain->playback,
						      playback);
	    g_object_run_dispose(playback);
	    g_object_unref(playback);
	  }
	}
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
    if(pads_old == pads){
      return;
    }

    if(audio->audio_channels == 0){
      audio->input_pads = pads;
      return;
    }

    /* grow or shrink */
    if(pads > pads_old){	 
      /* instantiate notation */
      if(pads_old == 0){
	if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
	  ags_audio_set_pads_alloc_notation();
	}
      }

      /* grow channels */
      if((AGS_AUDIO_NO_INPUT & (audio->flags)) == 0){
	ags_audio_set_pads_grow();
      }
    }else if(pads < pads_old){
      channel = audio->input;
	
      if((AGS_AUDIO_NO_INPUT & (audio->flags)) == 0){
	ags_audio_set_pads_shrink_automation();
      
	if(pads == 0){
	  /* shrink channels */
	  ags_audio_set_pads_unlink_all(channel);
	  ags_audio_set_pads_shrink_zero(channel);
	
	  audio->input = NULL;  
	}else{
	  channel = ags_channel_pad_nth(channel,
					pads);
	
	  /* shrink channels */
	  ags_audio_set_pads_unlink_all(channel);
	  ags_audio_set_pads_shrink(channel);
	}
      }
    }

    /* apply new allocation */
    audio->input_pads = pads;
    audio->input_lines = pads * audio->audio_channels;
  }else{
    g_warning("unknown channel type");
  }
}

/**
 * ags_audio_set_pads:
 * @audio: the #AgsAudio
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  guint pads_old;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if(!AGS_IS_AUDIO(audio)){
    pthread_mutex_unlock(mutex);
    return;
  }

  /* emit */
  g_object_ref((GObject *) audio);
  pads_old = ((g_type_is_a(type, AGS_TYPE_OUTPUT)) ? audio->output_pads: audio->input_pads);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		type, pads, pads_old);
  g_object_unref((GObject *) audio);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_set_samplerate:
 * @audio: the #AgsAudio
 * @samplerate: the samplerate
 *
 * Sets samplerate.
 *
 * Since: 0.7.3
 */
void
ags_audio_set_samplerate(AgsAudio *audio, guint samplerate)
{
  AgsConfig *config;
  
  gchar *str0, *str1;

  gdouble freq;
  
  auto void ags_audio_set_samplerate_channel(AgsChannel *channel);

  void ags_audio_set_samplerate_channel(AgsChannel *channel){
    while(channel != NULL){
      ags_channel_set_samplerate(channel, samplerate);
      
      channel = channel->next;
    }
  }
  
  audio->samplerate = samplerate;

  /*  */
  config = ags_config_get_instance();  

  freq = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

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
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[0],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
		     "frequency", freq,
		     NULL);
      }
    }
  }

  g_free(str0);
  g_free(str1);
  
  ags_audio_set_samplerate_channel(audio->output);
  ags_audio_set_samplerate_channel(audio->input);
}

/**
 * ags_audio_set_buffer_size:
 * @audio: the #AgsAudio
 * @buffer_size: the buffer length
 *
 * Sets buffer length.
 *
 * Since: 0.7.3
 */
void
ags_audio_set_buffer_size(AgsAudio *audio, guint buffer_size)
{
  AgsConfig *config;
  
  gchar *str0, *str1;

  gdouble freq;

  auto void ags_audio_set_buffer_size_channel(AgsChannel *channel);

  void ags_audio_set_buffer_size_channel(AgsChannel *channel){
    while(channel != NULL){
      ags_channel_set_buffer_size(channel, buffer_size);
      
      channel = channel->next;
    }
  }
  
  audio->buffer_size = buffer_size;

  /*  */
  config = ags_config_get_instance();
  
  freq = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

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
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[0],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
		     "frequency", freq,
		     NULL);
	g_object_set(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
		     "frequency", freq,
		     NULL);
      }
    }
  }

  g_free(str0);
  g_free(str1);

  ags_audio_set_buffer_size_channel(audio->output);
  ags_audio_set_buffer_size_channel(audio->input);
}

/**
 * ags_audio_set_format:
 * @audio: the #AgsAudio
 * @format: the format
 *
 * Sets buffer length.
 *
 * Since: 0.7.45
 */
void
ags_audio_set_format(AgsAudio *audio, guint format)
{
  auto void ags_audio_set_format_channel(AgsChannel *channel);

  void ags_audio_set_format_channel(AgsChannel *channel){
    while(channel != NULL){
      ags_channel_set_format(channel, format);
      
      channel = channel->next;
    }
  }
  
  audio->format = format;

  ags_audio_set_format_channel(audio->output);
  ags_audio_set_format_channel(audio->input);
}

/**
 * ags_audio_set_sequence_length:
 * @audio: the #AgsAudio
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
 * ags_audio_add_audio_connection:
 * @audio: the #AgsAudio
 * @audio_connection: an #AgsAudioConnection
 *
 * Adds an audio connection.
 *
 * Since: 0.7.65
 */
void
ags_audio_add_audio_connection(AgsAudio *audio,
			       GObject *audio_connection)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add audio connection */
  pthread_mutex_lock(mutex);

  g_object_ref(audio_connection);
  audio->audio_connection = g_list_prepend(audio->audio_connection, audio_connection);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_audio_connection:
 * @audio: the #AgsAudio
 * @audio_connection: an #AgsAudioConnection
 *
 * Removes an audio connection.
 *
 * Since: 0.7.65
 */
void
ags_audio_remove_audio_connection(AgsAudio *audio,
				  GObject *audio_connection)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove audio connection */
  pthread_mutex_lock(mutex);

  audio->audio_connection = g_list_remove(audio->audio_connection, audio_connection);
  g_object_unref(audio_connection);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Adds an preset.
 *
 * Since: 0.8.5
 */
void
ags_audio_add_preset(AgsAudio *audio,
		     GObject *preset)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add preset */
  pthread_mutex_lock(mutex);

  g_object_ref(preset);
  audio->preset = g_list_prepend(audio->preset, preset);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Removes an preset.
 *
 * Since: 0.8.5
 */
void
ags_audio_remove_preset(AgsAudio *audio,
			GObject *preset)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove preset */
  pthread_mutex_lock(mutex);

  audio->preset = g_list_remove(audio->preset, preset);
  g_object_unref(preset);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_add_notation(AgsAudio *audio, GObject *notation)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add recall id */
  pthread_mutex_lock(mutex);

  g_object_ref(notation);
  audio->notation = g_list_prepend(audio->notation, notation);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_remove_notation(AgsAudio *audio, GObject *notation)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recall id */
  pthread_mutex_lock(mutex);

  audio->notation = g_list_remove(audio->notation, notation);
  g_object_unref(notation);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 0.7.0
 */
void
ags_audio_add_automation(AgsAudio *audio, GObject *automation)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add recall id */
  pthread_mutex_lock(mutex);

  g_object_ref(automation);
  audio->automation = g_list_prepend(audio->automation, automation);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 0.7.0
 */
void
ags_audio_remove_automation(AgsAudio *audio, GObject *automation)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recall id */
  pthread_mutex_lock(mutex);

  audio->automation = g_list_remove(audio->automation, automation);
  g_object_unref(automation);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add recall id */
  pthread_mutex_lock(mutex);

  g_object_ref(recall_id);
  audio->recall_id = g_list_prepend(audio->recall_id, recall_id);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recall id */
  pthread_mutex_lock(mutex);

  audio->recall_id = g_list_remove(audio->recall_id, recall_id);
  g_object_unref(recall_id);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Adds a recycling container.
 *
 * Since: 0.4
 */
void
ags_audio_add_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add recycling container */
  pthread_mutex_lock(mutex);

  g_object_ref(recycling_context);
  audio->recycling_context = g_list_prepend(audio->recycling_context, recycling_context);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Removes a recycling container.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recycling container */
  pthread_mutex_lock(mutex);

  audio->recycling_context = g_list_remove(audio->recycling_context, recycling_context);
  g_object_unref(recycling_context);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* add recall container */
  pthread_mutex_lock(mutex);
  
  g_object_ref(recall_container);
  audio->container = g_list_prepend(audio->container, recall_container);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_remove_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recall container */
  pthread_mutex_lock(mutex);

  audio->container = g_list_remove(audio->container, recall_container);
  g_object_unref(recall_container);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_add_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Adds a recall.
 *
 * Since: 0.4
 */
void
ags_audio_add_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  if(audio == NULL ||
     recall == NULL){
    return;
  }
  
  /* add recall */
  g_object_ref(G_OBJECT(recall));

  if(play){
    pthread_mutex_lock(audio->play_mutex);
    
    audio->play = g_list_prepend(audio->play, recall);

    pthread_mutex_unlock(audio->play_mutex);
  }else{
    pthread_mutex_lock(audio->recall_mutex);

    audio->recall = g_list_prepend(audio->recall, recall);

    pthread_mutex_unlock(audio->recall_mutex);
  }
}

/**
 * ags_audio_remove_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play: %TRUE if simple playback.
 *
 * Removes a recall.
 *
 * Since: 0.4
 */
void
ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  if(audio == NULL ||
     recall == NULL){
    return;
  }

  /* remove recall */
  if(play){
    pthread_mutex_lock(audio->play_mutex);

    audio->play = g_list_remove(audio->play, recall);

    pthread_mutex_unlock(audio->play_mutex);
  }else{
    pthread_mutex_lock(audio->recall_mutex);

    audio->recall = g_list_remove(audio->recall, recall);

    pthread_mutex_unlock(audio->recall_mutex);
  }

  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_audio_init_run:
 * @audio: the #AgsAudio
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if(!AGS_IS_AUDIO(audio)){
    pthread_mutex_unlock(mutex);
    return(NULL);
  }

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[INIT_RUN], 0,
		&recall_id);
  g_object_unref((GObject *) audio);
  
  pthread_mutex_unlock(mutex);

  return(recall_id);
}

/**
 * ags_audio_duplicate_recall:
 * @audio: the #AgsAudio
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

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(audio == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* initial checks */
  pthread_mutex_lock(mutex);
  
#ifdef AGS_DEBUG
  g_message("ags_audio_duplicate_recall: %s - audio.lines[%u,%u]\n", G_OBJECT_TYPE_NAME(audio->machine), audio->output_lines, audio->input_lines);  
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
    list_recall = g_list_copy(audio->play);
    list_recall_start =
      list_recall = g_list_reverse(list_recall);
  }else{
    list_recall = g_list_copy(audio->recall);
    list_recall_start =
      list_recall = g_list_reverse(list_recall);
  }

  /* notify run */  
  //  ags_recall_notify_dependency(AGS_RECALL(list_recall->data), AGS_RECALL_NOTIFY_RUN, 1);

  /* return if already played */
  if((AGS_RECALL_ID_PRE & (recall_id->flags)) != 0 ||
     (AGS_RECALL_ID_INTER & (recall_id->flags)) != 0 ||
     (AGS_RECALL_ID_POST & (recall_id->flags)) != 0){
    pthread_mutex_unlock(mutex);
    return;
  }

  ags_recall_id_set_run_stage(recall_id, 0);
  ags_recall_id_set_run_stage(recall_id, 1);
  ags_recall_id_set_run_stage(recall_id, 2);

  /* duplicate */
  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       AGS_IS_RECALL_AUDIO(recall) ||
       !((playback && (AGS_RECALL_PLAYBACK & (recall->flags)) != 0) ||
	 (sequencer && (AGS_RECALL_SEQUENCER & (recall->flags)) != 0) ||
	 (notation && (AGS_RECALL_NOTATION & (recall->flags)) != 0)) ||
       recall->recall_id != NULL){
      list_recall = list_recall->next;
      continue;
    }  

    /* duplicate template only once */
    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
      /* duplicate the recall */
      copy = ags_recall_duplicate(recall, recall_id);

      
      if(copy == NULL){
	/* iterate */    
	list_recall = list_recall->next;

	continue;
      }
    
      /* notify run */
      ags_recall_notify_dependency(copy, AGS_RECALL_NOTIFY_RUN, 1);

#ifdef AGS_DEBUG
      g_message("recall duplicated: %s\n", G_OBJECT_TYPE_NAME(copy));
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
      ags_audio_add_recall(audio,
			   (GObject *) copy,
			   ((recall_id->recycling_context->parent == NULL) ? TRUE: FALSE));

      /* connect */
      ags_connectable_connect(AGS_CONNECTABLE(copy));
    }

    /* iterate */
    list_recall = list_recall->next;
  }

  pthread_mutex_unlock(mutex);

  g_list_free(list_recall_start);
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(audio == NULL ||
     recall_id == NULL){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  /* check for init */
  
  /* return if already initialized */
  switch(stage){
  case 0:
    if((AGS_RECALL_ID_INIT_PRE & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
      return;
    }
    break;
  case 1:
    if((AGS_RECALL_ID_INIT_INTER & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
      return;
    }
    break;
  case 2:
    if((AGS_RECALL_ID_INIT_POST & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
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
	recall_id->flags |= AGS_RECALL_ID_INIT_PRE;
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));
    
	recall->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(recall);
	recall->flags &= (~AGS_RECALL_REMOVE);
      }else if(stage == 1){
	recall_id->flags |= AGS_RECALL_ID_INIT_INTER;
	ags_recall_run_init_inter(recall);
      }else{
	recall_id->flags |= AGS_RECALL_ID_INIT_POST;
	ags_recall_run_init_post(recall);
      }
    }
    
    list_recall = list_recall->next;
  }
  
  pthread_mutex_unlock(mutex);
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(audio == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  /* return if already duplicated */
  if((AGS_RECALL_ID_RESOLVE & (recall_id->flags)) != 0){
    pthread_mutex_unlock(mutex);
    return;
  }
  
  /* get the appropriate lists */
  if(recall_id->recycling_context->parent == NULL){
    list_recall = audio->play;
  }else{
    list_recall = audio->recall;
  }

  /* resolve */  
  while((list_recall = ags_recall_find_recycling_context(list_recall, (GObject *) recall_id->recycling_context)) != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    ags_recall_resolve_dependencies(recall);

    list_recall = list_recall->next;
  }

  pthread_mutex_unlock(mutex);
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* check if playing */
  pthread_mutex_lock(mutex);
  
  output = audio->output;

  while(output != NULL){
    playback = AGS_PLAYBACK(output->playback);

    if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(playback->flags)))) != 0 ||
       (AGS_PLAYBACK_SEQUENCER & (g_atomic_int_get(&(playback->flags)))) != 0 ||
       (AGS_PLAYBACK_NOTATION & (g_atomic_int_get(&(playback->flags)))) != 0){
  
      pthread_mutex_unlock(mutex);
      
      return(TRUE);
    }

    output = output->next;
  }
  
  pthread_mutex_unlock(mutex);

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
  GList *list_start, *list, *list_next;
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(audio == NULL ||
     recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  /* check for status */
  if((AGS_RECALL_ID_PRE & (recall_id->flags)) != 0 &&
     (AGS_RECALL_ID_INTER & (recall_id->flags)) != 0 &&
     (AGS_RECALL_ID_POST & (recall_id->flags)) != 0){
    recall_id->flags &= (~(AGS_RECALL_ID_PRE |
			   AGS_RECALL_ID_INTER |
			   AGS_RECALL_ID_POST));
  }
  
  /* return if already played */
  switch(stage){
  case 0:
    ags_recall_id_unset_run_stage(recall_id, 1);
    
    if((AGS_RECALL_ID_PRE & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
      return;
    }
    break;
  case 1:
    ags_recall_id_unset_run_stage(recall_id, 2);

    if((AGS_RECALL_ID_INTER & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
      return;
    }
    break;
  case 2:
    ags_recall_id_unset_run_stage(recall_id, 0);

    if((AGS_RECALL_ID_POST & (recall_id->flags)) != 0){
      pthread_mutex_unlock(mutex);
      return;
    }
    break;
  }

  ags_recall_id_set_run_stage(recall_id, stage);

  /* retrieve appropriate recalls */
  if(recall_id->recycling_context->parent == NULL){
    g_object_get(audio,
		 "play", &list_start,
		 NULL);
  }else{
    g_object_get(audio,
		 "recall", &list_start,
		 NULL);
  }
  
  pthread_mutex_unlock(mutex);

  list = list_start;
  
  /* play */
  while(list != NULL){
    guint recall_flags;
    
    list_next = list->next;

    recall = (AgsRecall *) list->data;

    if(recall == NULL ||
       !AGS_IS_RECALL(recall)){
      //      if(recall_id->recycling_context->parent != NULL){
      //	audio->recall = g_list_remove(audio->recall,
      //				      recall);
      //      }else{
      //	audio->play = g_list_remove(audio->play,
      //				    recall);
      //      }

      list = list_next;
      g_warning("recall == NULL");
      
      continue;
    }

    if(AGS_IS_RECALL_AUDIO(recall)){
      if(stage == 0){
	ags_recall_automate(recall);
      }

      list = list_next;
      
      continue;
    }

    if(recall->recall_id == NULL ||
       (recall->recall_id->recycling_context != recall_id->recycling_context)){
      list = list_next;

      continue;
    }

    recall_flags = recall->flags;
    
    if((AGS_RECALL_TEMPLATE & (recall_flags)) == 0){
      if((AGS_RECALL_HIDE & (recall_flags)) == 0){
	g_object_ref(recall);
	
	if(stage == 0){
	  AGS_RECALL_GET_CLASS(recall)->run_pre(recall);
	}else if(stage == 1){
	  AGS_RECALL_GET_CLASS(recall)->run_inter(recall);
	}else{
	  AGS_RECALL_GET_CLASS(recall)->run_post(recall);
	}

	g_object_unref(recall);
      }
    }
    
    list = list_next;
  }

  g_list_free(list_start);
}

/**
 * ags_audio_tact:
 * @audio: the #AgsAudio
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as audio is played.
 *
 * Since: 0.4
 */
void
ags_audio_tact(AgsAudio *audio, AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if(!AGS_IS_AUDIO(audio)){
    pthread_mutex_unlock(mutex);
    return;
  }

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[TACT], 0,
		recall_id);
  g_object_unref((GObject *) audio);
  
  pthread_mutex_unlock(mutex);
}

void
ags_audio_real_done(AgsAudio *audio,
		    AgsRecallID *recall_id)
{
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *list, *list_next;

  gboolean is_toplevel;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get context */
  pthread_mutex_lock(mutex);

  if(recall_id->recycling_context->parent == NULL){
    list = audio->play;
    
    is_toplevel = TRUE;
  }else{
    list = audio->recall;

    is_toplevel = FALSE;
  }

  /* remove recall, run dispose and unref */
 ags_audio_real_done_RECALL0:
  
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);
    
    if(recall->recall_id != NULL &&
       recall->recall_id->recycling_context == recall_id->recycling_context){
      g_object_run_dispose(recall);
      ags_audio_remove_recall(audio,
			      recall,
			      is_toplevel);
      g_object_unref(recall);
    }

    list = list_next;
  }

  if(is_toplevel &&
     (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
    list = audio->recall;
    is_toplevel = FALSE;
    
    goto ags_audio_real_done_RECALL0;
  }
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_done:
 * @audio: the #AgsAudio
 * @recall_id: the current #AgsRecallID
 *
 * Is emitted as playing audio is done.
 *
 * Since: 0.4
 */
void
ags_audio_done(AgsAudio *audio, AgsRecallID *recall_id)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(audio == NULL ||
     recall_id == NULL ||
     recall_id->recycling_context == NULL){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if(!(AGS_IS_AUDIO(audio) ||
       AGS_IS_RECALL_ID(recall_id))){
    pthread_mutex_unlock(mutex);
    return;
  }

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[DONE], 0,
		recall_id);
  g_object_unref((GObject *) audio);
  
  pthread_mutex_unlock(mutex);
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
  GList *list_start, *list, *list_next;
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
    
  if(audio == NULL || recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* cancel recalls */
  pthread_mutex_lock(mutex);

  /* retrieve appropriate recalls */
  if(recall_id->recycling_context->parent == NULL){
    g_object_get(audio,
		 "play", &list_start,
		 NULL);
  }else{
    g_object_get(audio,
		 "recall", &list_start,
		 NULL);
  }
  
  g_object_ref(recall_id);
  
  pthread_mutex_unlock(mutex);

  list = list_start;
  
  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if(recall == NULL ||
       !AGS_IS_RECALL(recall) ||
       (AGS_RECALL_TEMPLATE & (recall->flags)) ||
       recall->recall_id == NULL ||
       recall->recall_id->recycling_context != recall_id->recycling_context){
      list = list_next;

      continue;
    }

    ags_recall_cancel(recall);
    
    list = list_next;
  }  

  g_list_free(list_start);
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  if(audio == NULL || recall_id == NULL){
    return;
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* remove recalls */
  pthread_mutex_lock(mutex);

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
			    (GObject *) recall,
			    play);
    
    list = list_next;
  }

  audio->recall_id = g_list_remove(audio->recall_id,
				   recall_id);
  g_object_unref(recall_id);
  
  pthread_mutex_unlock(mutex);
}

/**
 * ags_audio_find_port:
 * @audio: the #AgsAudio
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
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

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
    
  pthread_mutex_unlock(mutex);
  
  return(list);
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

  AgsMutexManager *mutex_manager;

  GObject *soundcard;
  
  GList *audio_signal_list;

  guint input_pads;
  guint audio_channels;
  guint i, j;
  guint list_length;

  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex, *channel_mutex, *recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio fields */
  pthread_mutex_lock(mutex);

  channel = audio->input;
  soundcard = audio->soundcard;
  
  input_pads = audio->input_pads;
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(mutex);
  
  /* overwriting existing channels */
  if(overwrite_channels){
    if(channel != NULL){
      for(i = 0; i < input_pads && filenames != NULL; i++){
	audio_file = ags_audio_file_new((gchar *) filenames->data,
					soundcard,
					0, audio_channels);
	if(!ags_audio_file_open(audio_file)){
	  filenames = filenames->next;
	  continue;
	}

	ags_audio_file_read_audio_signal(audio_file);
	ags_audio_file_close(audio_file);
	
	audio_signal_list = audio_file->audio_signal;
	
	for(j = 0; j < audio_channels && audio_signal_list != NULL; j++){
	  AgsRecycling *recycling;
	  
	  /* create task */
	  error = NULL;

	  ags_channel_set_link(channel, NULL,
			       &error);

	  if(error != NULL){
	    g_warning("%s", error->message);
	  }

	  /* lock channel and recycling */
	  pthread_mutex_lock(application_mutex);
  
	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) channel);

	  pthread_mutex_unlock(application_mutex);

	  /* get recycling */
	  pthread_mutex_lock(channel_mutex);

	  recycling = channel->first_recycling;
	  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						     (GObject *) recycling);
	  	  
	  pthread_mutex_unlock(channel_mutex);

	  /* replace template audio signal */
	  pthread_mutex_lock(recycling_mutex);
	  
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) recycling;

	  ags_recycling_add_audio_signal(recycling,
					 audio_signal_list->data);

	  pthread_mutex_unlock(recycling_mutex);

	  /* iterate */
	  audio_signal_list = audio_signal_list->next;

	  pthread_mutex_lock(channel_mutex);

	  channel = channel->next;

	  pthread_mutex_unlock(channel_mutex);
	}

	if(audio_file->channels < audio_channels)
	  channel = ags_channel_nth(channel,
				    audio_channels - audio_file->channels);
	
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
				      soundcard,
				      0, audio_channels);
      if(!ags_audio_file_open(audio_file)){
	filenames = filenames->next;
	continue;
      }
      
      ags_audio_file_read_audio_signal(audio_file);
      ags_audio_file_close(audio_file);
	
      audio_signal_list = audio_file->audio_signal;
      
      for(j = 0; j < audio_channels && audio_signal_list != NULL; j++){
	AgsRecycling *recycling;
	
	/* lock channel and recycling */
	pthread_mutex_lock(application_mutex);
  
	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);

	pthread_mutex_unlock(application_mutex);

	/* get recycling */
	pthread_mutex_lock(channel_mutex);

	recycling = channel->first_recycling;
	recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) recycling);
	
	pthread_mutex_unlock(channel_mutex);

	/* replace template audio signal */
	pthread_mutex_lock(recycling_mutex);
	  
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) recycling;
	
	ags_recycling_add_audio_signal(recycling,
				       audio_signal_list->data);
	
	pthread_mutex_unlock(recycling_mutex);

	/* iterate */
	audio_signal_list = audio_signal_list->next;

	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
      
      if(audio_channels > audio_file->channels)
	channel = ags_channel_nth(channel,
				  audio_channels - audio_file->channels);
      
      filenames = filenames->next;
    }
  }
}

/**
 * ags_audio_recursive_set_property:
 * @audio: the #AgsAudio
 * @parameter: the #GParameter-struct array containing properties
 * @n_params: the length of @parameter array
 *
 * Recursive set property for #AgsAudio.
 *
 * Since: 0.7.65
 */
void
ags_audio_recursive_set_property(AgsAudio *audio,
				 GParameter *parameter, gint n_params)
{
  AgsChannel *channel;
  
  auto void ags_audio_set_property(AgsAudio *audio,
				   GParameter *parameter, gint n_params);
  auto void ags_audio_recurisve_set_property_down(AgsChannel *channel,
						  GParameter *parameter, gint n_params);
  auto void ags_audio_recurisve_set_property_down_input(AgsChannel *channel,
							GParameter *parameter, gint n_params);

  void ags_audio_set_property(AgsAudio *audio,
			      GParameter *parameter, gint n_params){
    guint i;

    for(i = 0; i < n_params; i++){
      g_object_set_property(G_OBJECT(audio),
			    parameter[i].name, &(parameter[i].value));
    }
  }
  
  void ags_audio_recurisve_set_property_down(AgsChannel *channel,
					     GParameter *parameter, gint n_params){
    if(channel == NULL){
      return;
    }

    ags_audio_set_property(AGS_AUDIO(channel->audio),
			   parameter, n_params);
    
    ags_audio_recurisve_set_property_down_input(channel,
						parameter, n_params);
  }
    
  void ags_audio_recurisve_set_property_down_input(AgsChannel *channel,
						   GParameter *parameter, gint n_params){
    AgsAudio *audio;
    AgsChannel *input;
    
    if(channel == NULL){
      return;
    }

    audio = (AgsAudio *) channel->audio;

    if(audio == NULL){
      return;
    }
    
    input = ags_channel_nth(audio->input,
			    channel->audio_channel);

    while(input != NULL){      
      ags_audio_recurisve_set_property_down(input->link,
					    parameter, n_params);

      input = input->next;
    }
  }

  if(audio == NULL){
    return;
  }

  ags_audio_set_property(audio,
			 parameter, n_params);

  if(audio->input != NULL){
    channel = audio->input;

    while(channel != NULL){
      ags_audio_recurisve_set_property_down(channel->link,
					    parameter, n_params);

      channel = channel->next;
    }
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

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_start;
  gint stage;
  gboolean arrange_recall_id, duplicate_templates, resolve_dependencies;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex, *channel_mutex;

  if(audio == NULL){
    return(NULL);
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* recursive init playback */
  pthread_mutex_lock(mutex);

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

      /* iterate */
      pthread_mutex_lock(application_mutex);
      
      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);
      
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }
  
  pthread_mutex_unlock(mutex);

  return(list_start);
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
				    "soundcard", soundcard,
				    NULL);

  return(audio);
}
