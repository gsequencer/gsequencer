/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_preset.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_midi.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/midi/ags_midi_file.h>

#include <libxml/tree.h>

#include <stdlib.h>
#include <string.h>

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
void ags_audio_dispose(GObject *gobject);
void ags_audio_finalize(GObject *gobject);

AgsUUID* ags_audio_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_has_resource(AgsConnectable *connectable);
gboolean ags_audio_is_ready(AgsConnectable *connectable);
void ags_audio_add_to_registry(AgsConnectable *connectable);
void ags_audio_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_xml_compose(AgsConnectable *connectable);
void ags_audio_xml_parse(AgsConnectable *connectable,
			 xmlNode *node);
gboolean ags_audio_is_connected(AgsConnectable *connectable);
void ags_audio_connect(AgsConnectable *connectable);
void ags_audio_disconnect(AgsConnectable *connectable);
void ags_audio_connect_connection(AgsConnectable *connectable,
				  GObject *connection);
void ags_audio_disconnect_connection(AgsConnectable *connectable,
				     GObject *connection);

void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);
void ags_audio_real_set_pads(AgsAudio *audio,
			     GType type,
			     guint channels, guint channels_old);

void ags_audio_real_duplicate_recall(AgsAudio *audio,
				    AgsRecallID *recall_id);
void ags_audio_real_resolve_recall(AgsAudio *audio,
				   AgsRecallID *recall_id);

void ags_audio_real_init_recall(AgsAudio *audio,
				AgsRecallID *recall_id, guint staging_flags);
void ags_audio_real_play_recall(AgsAudio *audio,
				AgsRecallID *recall_id, guint staging_flags);

void ags_audio_real_done_recall(AgsAudio *audio,
				AgsRecallID *recall_id);
void ags_audio_real_cancel_recall(AgsAudio *audio,
				  AgsRecallID *recall_id);

void ags_audio_real_cleanup_recall(AgsAudio *audio,
				   AgsRecallID *recall_id);

GList* ags_audio_real_start(AgsAudio *audio,
			    gint sound_scope);
void ags_audio_real_stop(AgsAudio *audio,
			 GList *recall_id, gint sound_scope);

GList* ags_audio_real_check_scope(AgsAudio *audio, gint sound_scope);

void ags_audio_real_recursive_run_stage(AgsAudio *audio,
					gint sound_scope, guint stage);

enum{
  SET_AUDIO_CHANNELS,
  SET_PADS,
  DUPLICATE_RECALL,
  RESOLVE_RECALL,
  INIT_RECALL,
  PLAY_RECALL,
  DONE_RECALL,
  CANCEL_RECALL,
  CLEANUP_RECALL,
  START,
  STOP,
  CHECK_SCOPE,
  RECURSIVE_RUN_STAGE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_OUTPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD,
  PROP_OUTPUT_SEQUENCER,
  PROP_INPUT_SEQUENCER,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_MAX_AUDIO_CHANNELS,
  PROP_MAX_OUTPUT_PADS,
  PROP_MAX_INPUT_PADS,
  PROP_AUDIO_CHANNELS,
  PROP_OUTPUT_PADS,
  PROP_OUTPUT_LINES,
  PROP_INPUT_PADS,
  PROP_INPUT_LINES,
  PROP_AUDIO_START_MAPPING,
  PROP_AUDIO_END_MAPPING,
  PROP_MIDI_START_MAPPING,
  PROP_MIDI_END_MAPPING,
  PROP_MIDI_CHANNEL,
  PROP_NUMERATOR,
  PROP_DENOMINATOR,
  PROP_TIME_SIGNATURE,
  PROP_IS_MINOR,
  PROP_SHARP_FLATS,
  PROP_OCTAVE,
  PROP_KEY,
  PROP_ABSOLUTE_KEY,
  PROP_OUTPUT,
  PROP_INPUT,
  PROP_PRESET,
  PROP_PLAYBACK_DOMAIN,
  PROP_NOTATION,
  PROP_AUTOMATION,
  PROP_WAVE,
  PROP_OUTPUT_AUDIO_FILE,
  PROP_INPUT_AUDIO_FILE,
  PROP_MIDI,
  PROP_OUTPUT_MIDI_FILE,
  PROP_INPUT_MIDI_FILE,
  PROP_RECALL_ID,
  PROP_RECYCLING_CONTEXT,
  PROP_RECALL_CONTAINER,
  PROP_PLAY,
  PROP_RECALL,
};

static gpointer ags_audio_parent_class = NULL;
static guint audio_signals[LAST_SIGNAL];

static pthread_mutex_t ags_audio_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
   * AgsAudio:output-soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-soundcard",
				   i18n_pspec("assigned output soundcard"),
				   i18n_pspec("The output soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudio:input-soundcard:
   *
   * The assigned #AgsSoundcard acting as default source.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-soundcard",
				   i18n_pspec("assigned input soundcard"),
				   i18n_pspec("The input soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD,
				  param_spec);
  
  /**
   * AgsAudio:output-sequencer:
   *
   * The assigned #AgsSequencer acting as default source.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-sequencer",
				   i18n_pspec("assigned output sequencer"),
				   i18n_pspec("The output sequencer it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SEQUENCER,
				  param_spec);

  /**
   * AgsAudio:input-sequencer:
   *
   * The assigned #AgsSequencer acting as default source.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-sequencer",
				   i18n_pspec("assigned input sequencer"),
				   i18n_pspec("The input sequencer it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SEQUENCER,
				  param_spec);
  
  /**
   * AgsAudio:samplerate:
   *
   * The samplerate.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudio:buffer-size:
   *
   * The buffer length.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudio:format:
   *
   * The format.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format"),
				 0,
				 G_MAXUINT32,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsAudio:max-audio-channels:
   *
   * The maximum audio channels count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("max-audio-channels",
				 i18n_pspec("maximum audio channels count"),
				 i18n_pspec("The maximum count of audio channels of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsAudio:max-output-pads:
   *
   * The maximum output pads count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("max-output-pads",
				 i18n_pspec("maximum output pads count"),
				 i18n_pspec("The maximum count of output pads of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_OUTPUT_PADS,
				  param_spec);

  /**
   * AgsAudio:max-input-pads:
   *
   * The maximum input pads count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("max-input-pads",
				 i18n_pspec("maximum input pads count"),
				 i18n_pspec("The maximum count of input pads of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAX_INPUT_PADS,
				  param_spec);
  
  /**
   * AgsAudio:audio-channels:
   *
   * The audio channels count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("audio channels count"),
				 i18n_pspec("The count of audio channels of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsAudio:output-pads:
   *
   * The output pads count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("output-pads",
				 i18n_pspec("output pads count"),
				 i18n_pspec("The count of output pads of audio"),
				 0,
				 G_MAXUINT32,
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("output-lines",
				 i18n_pspec("output lines count"),
				 i18n_pspec("The count of output lines of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_LINES,
				  param_spec);

  /**
   * AgsAudio:input-pads:
   *
   * The input pads count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("input-pads",
				 i18n_pspec("input pads count"),
				 i18n_pspec("The count of input pads of audio"),
				 0,
				 G_MAXUINT32,
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("input-lines",
				 i18n_pspec("input lines count"),
				 i18n_pspec("The count of input lines of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_LINES,
				  param_spec);

  /**
   * AgsAudio:audio-start-mapping:
   *
   * The audio start mapping.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsAudio:numerator:
   *
   * The numerator of time signature.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("numerator",
				 i18n_pspec("numerator"),
				 i18n_pspec("The numerator"),
				 0,
				 32,
				 4,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NUMERATOR,
				  param_spec);
  
  /**
   * AgsAudio:denominator:
   *
   * The denominator of time signature.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("denominator",
				 i18n_pspec("denominator"),
				 i18n_pspec("The denominator"),
				 0,
				 32,
				 4,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DENOMINATOR,
				  param_spec);

  /**
   * AgsAudio:time-signature:
   *
   * The time signature.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("time-signature",
				   i18n_pspec("time signature"),
				   i18n_pspec("The time signature"),
				   "4/4",
				   G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_TIME_SIGNATURE,
				  param_spec);

  /**
   * AgsAudio:is-minor:
   *
   * Is minor.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_boolean("is-minor",
				    i18n_pspec("is minor"),
				    i18n_pspec("Is minor"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IS_MINOR,
				  param_spec);

  /**
   * AgsAudio:sharp-flats:
   *
   * The sharp/flats count.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("sharp-flats",
				 i18n_pspec("sharp/flats"),
				 i18n_pspec("The sharp/flats count"),
				 0,
				 12,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SHARP_FLATS,
				  param_spec);
  
  /**
   * AgsAudio:octave:
   *
   * The octave lower.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_int("octave",
				i18n_pspec("octave"),
				i18n_pspec("The octave lower"),
				0,
				10,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OCTAVE,
				  param_spec);

  /**
   * AgsAudio:key:
   *
   * The key relative to octave.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("key",
				 i18n_pspec("relative key"),
				 i18n_pspec("The key relative to octave"),
				 0,
				 12,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_KEY,
				  param_spec);

  /**
   * AgsAudio:absolute-key:
   *
   * The absolute key lower.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_int("absolute-key",
				i18n_pspec("absolute key"),
				i18n_pspec("The absolute key lower"),
				0,
				128,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ABSOLUTE_KEY,
				  param_spec);

  /**
   * AgsAudio:output:
   *
   * The #AgsOutput it contains.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsAudio:preset:
   *
   * The assigned #GList-struct containing #AgsPreset information.
   * 
   * Since: 2.0.0
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
   * AgsAudio:playback-domain:
   *
   * The assigned #AgsPlaybackDomain.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("automation",
				    i18n_pspec("containing automation"),
				    i18n_pspec("The automation it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);
  
  /**
   * AgsAudio:wave:
   *
   * The #AgsWave it contains.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("wave",
				    i18n_pspec("containing wave"),
				    i18n_pspec("The wave it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE,
				  param_spec);

  /**
   * AgsAudio:output-audio-file:
   *
   * The assigned #AgsAudioFile acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-audio-file",
				   i18n_pspec("assigned output audio file"),
				   i18n_pspec("The output audio file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_AUDIO_FILE,
				  param_spec);
  
  /**
   * AgsAudio:input-audio-file:
   *
   * The assigned #AgsAudioFile acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-audio-file",
				   i18n_pspec("assigned input audio file"),
				   i18n_pspec("The input audio file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_AUDIO_FILE,
				  param_spec);

  /**
   * AgsAudio:midi:
   *
   * The #AgsMidi it contains.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("midi",
				    i18n_pspec("containing midi"),
				    i18n_pspec("The midi it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI,
				  param_spec);
  
  /**
   * AgsAudio:output-midi-file:
   *
   * The assigned #AgsMidiFile acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-midi-file",
				   i18n_pspec("assigned output midi file"),
				   i18n_pspec("The output midi file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_MIDI_FILE,
				  param_spec);
  
  /**
   * AgsAudio:input-midi-file:
   *
   * The assigned #AgsMidiFile acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-midi-file",
				   i18n_pspec("assigned input midi file"),
				   i18n_pspec("The input midi file it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_MIDI_FILE,
				  param_spec);
  
  /**
   * AgsAudio:recall-id:
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("recall",
				    i18n_pspec("containing recall"),
				    i18n_pspec("The recall it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL,
				  param_spec);

  
  /* AgsAudioClass */
  audio->set_audio_channels = ags_audio_real_set_audio_channels;
  audio->set_pads = ags_audio_real_set_pads;

  audio->duplicate_recall = ags_audio_real_duplicate_recall;
  audio->resolve_recall = ags_audio_real_resolve_recall;

  audio->init_recall = ags_audio_real_init_recall;
  audio->play_recall = ags_audio_real_play_recall;
  
  audio->done_recall = ags_audio_real_done_recall;
  audio->cancel_recall = ags_audio_real_cancel_recall;

  audio->cleanup_recall = ags_audio_real_cleanup_recall;

  audio->start = ags_audio_real_start;
  audio->stop = ags_audio_real_stop;

  audio->check_scope = ags_audio_real_check_scope;
  audio->recursive_run_stage = ags_audio_real_recursive_run_stage;
  
  /* signals */
  /**
   * AgsAudio::set-audio-channels:
   * @audio: the object to adjust the channels.
   * @audio_channels_new: new audio channel count
   * @audio_channels_old: old audio channel count
   *
   * The ::set-audio-channels signal notifies about changes in channel
   * alignment.
   *
   * Since: 2.0.0
   */
  audio_signals[SET_AUDIO_CHANNELS] = 
    g_signal_new("set-audio-channels",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_audio_channels),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsAudio::set-pads:
   * @audio: the object to adjust pads.
   * @channel_type: either #AGS_TYPE_INPUT or #AGS_TYPE_OUTPUT
   * @pads_new: new pad count
   * @pads_old: old pad count
   *
   * The ::set-pads signal notifies about changes in channel
   * alignment.
   *
   * Since: 2.0.0
   */
  audio_signals[SET_PADS] = 
    g_signal_new("set-pads",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsAudio::duplicate-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::duplicate-recall signal notifies about duplicated recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[DUPLICATE_RECALL] = 
    g_signal_new("duplicate-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, duplicate_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudio::resolve-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::resolve-recall signal notifies about resolved recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[RESOLVE_RECALL] = 
    g_signal_new("resolve-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, resolve_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

   /**
   * AgsAudio::init-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   * @staging_flags: the staging flags
   *
   * The ::init-recall signal notifies about initd recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[INIT_RECALL] = 
    g_signal_new("init-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, init_recall),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_UINT);
 
  /**
   * AgsAudio::play-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   * @staging_flags: the staging flags
   *
   * The ::play-recall signal notifies about playd recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[PLAY_RECALL] = 
    g_signal_new("play-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, play_recall),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_UINT);

  /**
   * AgsAudio::done-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::done-recall signal notifies about doned recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[DONE_RECALL] = 
    g_signal_new("done-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, done_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudio::cancel-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::cancel-recall signal notifies about canceld recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[CANCEL_RECALL] = 
    g_signal_new("cancel-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, cancel_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudio::cleanup-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::cleanup-recall signal notifies about cleanup recalls.
   *
   * Since: 2.0.0
   */
  audio_signals[CLEANUP_RECALL] = 
    g_signal_new("cleanup-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, cleanup_recall),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudio::start:
   * @audio: the #AgsAudio starts playing
   * @sound_scope: the affected scope
   *
   * The ::start signal is invoked while starting playback
   * of @audio.
   *
   * Returns: the #GList-struct containing #AgsRecallID
   * 
   * Since: 2.0.0
   */
  audio_signals[START] = 
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, start),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__INT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_INT);

  /**
   * AgsAudio::stop:
   * @audio: the #AgsAudio stops playing
   * @recall_id: a #GList-struct containing #AgsRecallID
   * @sound_scope: the affected scope
   *
   * The ::stop signal is invoked while stoping playback
   * of @audio.
   *
   * Since: 2.0.0
   */
  audio_signals[STOP] = 
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, stop),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_POINTER,
		 G_TYPE_INT);

  /**
   * AgsAudio::check-scope:
   * @audio: the #AgsAudio to check the scopes
   * @sound_scope: the affected scope
   *
   * The ::check-scope method returns the appropriate recall id of @sound_scope.
   *
   * Returns: the #GList-struct containing #AgsRecallID
   * 
   * Since: 2.0.0
   */
  audio_signals[CHECK_SCOPE] = 
    g_signal_new("check-scope",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, check_scope),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__INT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_INT);

  /**
   * AgsAudio::recursive-run-stage:
   * @audio: the #AgsAudio to run
   * @sound_scope: the affected scope
   * @staging_flags: the flags to set
   *
   * The ::recursive-run-stage signal is invoked while run staging
   * of @audio for @sound_scope.
   * 
   * Since: 2.0.0
   */
  audio_signals[RECURSIVE_RUN_STAGE] = 
    g_signal_new("recursive-run-stage",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, recursive_run_stage),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__INT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_INT,
		 G_TYPE_UINT);
}

void
ags_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_get_uuid;
  connectable->has_resource = ags_audio_has_resource;

  connectable->is_ready = ags_audio_is_ready;
  connectable->add_to_registry = ags_audio_add_to_registry;
  connectable->remove_from_registry = ags_audio_remove_from_registry;

  connectable->list_resource = ags_audio_list_resource;
  connectable->xml_compose = ags_audio_xml_compose;
  connectable->xml_parse = ags_audio_xml_parse;

  connectable->is_connected = ags_audio_is_connected;
  connectable->connect = ags_audio_connect;
  connectable->disconnect = ags_audio_disconnect;

  connectable->connect_connection = ags_audio_connect_connection;
  connectable->disconnect_connection = ags_audio_disconnect_connection;
}

void
ags_audio_init(AgsAudio *audio)
{
  AgsMutexManager *mutex_manager;
  
  AgsConfig *config;
  
  gchar *str;
  gchar *str0, *str1;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  audio->flags = 0;
  audio->ability_flags = 0;
  audio->behaviour_flags = 0;
  memset(audio->staging_flags, 0, (AGS_SOUND_SCOPE_LAST + 1) * sizeof(guint));

  /* add audio mutex */
  audio->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  audio->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* uuid */
  audio->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio->uuid);
  
  /* config */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  config = ags_config_get_instance();
  
  /* base init */
  audio->output_soundcard = NULL;
  audio->output_soundcard_channel_map = NULL;

  audio->input_soundcard = NULL;
  audio->input_soundcard_channel_map = NULL;

  audio->output_sequencer = NULL;
  audio->input_sequencer = NULL;
  
  audio->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  audio->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  audio->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* read config */
  pthread_mutex_lock(application_mutex);
  
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

  pthread_mutex_unlock(application_mutex);

  /* bank */
  audio->bank_dim[0] = 0;
  audio->bank_dim[1] = 0;
  audio->bank_dim[2] = 0;

  /* channel allocation */
  audio->max_audio_channels = 0;

  audio->max_output_pads = 0;
  audio->max_input_pads = 0;

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

  /* time-signature */
  audio->numerator = 4;
  audio->denominator = 4;

  audio->time_signature = g_strdup("4/4");

  /* sharp/flats */
  audio->is_minor = FALSE;

  audio->sharp_flats = 0;

  /* octave */
  audio->octave = 0;
  audio->key = 0;
  
  audio->absolute_key = 0;
  
  /* channels */
  audio->output = NULL;
  audio->input = NULL;

  /* preset */
  audio->preset = NULL;

  /* playback domain */
  audio->playback_domain = (GObject *) ags_playback_domain_new(audio);
  g_object_ref(audio->playback_domain);

  /* synth generator */
  audio->synth_generator = NULL;
  
  /* notation and automation */
  audio->notation = NULL;
  audio->automation = NULL;
  
  /* wave */
  audio->wave = NULL;

  audio->output_audio_file = NULL;
  audio->input_audio_file = NULL;

  /* midi */
  audio->midi = NULL;

  audio->output_midi_file = NULL;
  audio->input_midi_file = NULL;
  
  /* recycling context */
  audio->recall_id = NULL;
  audio->recycling_context = NULL;

  audio->recall_container = NULL;

  /* play context */
  audio->play_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio->play_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio->play_mutex,
		     attr);

  audio->play = NULL;

  /* recall context */
  audio->recall_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio->recall_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio->recall_mutex,
		     attr);

  audio->recall = NULL;

  /* data */
  audio->machine_widget = NULL;
}

void
ags_audio_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsAudio *audio;

  pthread_mutex_t *audio_mutex;

  audio = AGS_AUDIO(gobject);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  switch(prop_id){
  case PROP_OUTPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      ags_audio_set_output_soundcard(audio,
				     (GObject *) soundcard);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      ags_audio_set_input_soundcard(audio,
				    (GObject *) soundcard);
    }
    break;
  case PROP_OUTPUT_SEQUENCER:
    {
      GObject *sequencer;

      sequencer = g_value_get_object(value);

      ags_audio_set_output_sequencer(audio,
				     (GObject *) sequencer);
    }
    break;
  case PROP_INPUT_SEQUENCER:
    {
      GObject *sequencer;

      sequencer = g_value_get_object(value);

      ags_audio_set_input_sequencer(audio,
				    (GObject *) sequencer);
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
  case PROP_MAX_AUDIO_CHANNELS:
    {
      guint max_audio_channels;

      max_audio_channels = g_value_get_uint(value);

      ags_audio_set_max_audio_channels(audio,
				       max_audio_channels);
    }
    break;
  case PROP_MAX_OUTPUT_PADS:
    {
      guint max_output_pads;

      max_output_pads = g_value_get_uint(value);

      ags_audio_set_max_pads(audio,
			     AGS_TYPE_OUTPUT,
			     max_output_pads);
    }
    break;
  case PROP_MAX_INPUT_PADS:
    {
      guint max_input_pads;

      max_input_pads = g_value_get_uint(value);

      ags_audio_set_max_pads(audio,
			     AGS_TYPE_INPUT,
			     max_input_pads);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      guint audio_channels;

      audio_channels = g_value_get_uint(value);

      ags_audio_set_audio_channels(audio,
				   audio_channels, 0);
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      guint output_pads;

      output_pads = g_value_get_uint(value);

      ags_audio_set_pads(audio,
			 AGS_TYPE_OUTPUT,
			 output_pads, 0);
    }
    break;
  case PROP_INPUT_PADS:
    {
      guint input_pads;

      input_pads = g_value_get_uint(value);

      ags_audio_set_pads(audio,
			 AGS_TYPE_INPUT,
			 input_pads, 0);
    }
    break;
  case PROP_AUDIO_START_MAPPING:
    {
      guint audio_start_mapping;

      audio_start_mapping = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);
      
      audio->audio_start_mapping = audio_start_mapping;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_AUDIO_END_MAPPING:
    {
      guint audio_end_mapping;

      audio_end_mapping = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->audio_end_mapping = audio_end_mapping;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_START_MAPPING:
    {
      guint midi_start_mapping;

      midi_start_mapping = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->midi_start_mapping = midi_start_mapping;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_END_MAPPING:
    {
      guint midi_end_mapping;

      midi_end_mapping = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->midi_end_mapping = midi_end_mapping;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_CHANNEL:
    {
      guint midi_channel;

      midi_channel = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->midi_channel = midi_channel;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_NUMERATOR:
    {
      guint numerator;

      numerator = g_value_get_uint(value);

      /* numerator and time signature */
      pthread_mutex_lock(audio_mutex);

      audio->numerator = numerator;

      g_free(audio->time_signature);
      audio->time_signature = g_strdup_printf("%u/%u",
					      audio->numerator,
					      audio->denominator);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_DENOMINATOR:
    {
      guint denominator;

      denominator = g_value_get_uint(value);

      /* denominator and time signature */
      pthread_mutex_lock(audio_mutex);

      audio->denominator = denominator;

      g_free(audio->time_signature);
      audio->time_signature = g_strdup_printf("%u/%u",
					      audio->numerator,
					      audio->denominator);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_IS_MINOR:
    {
      gboolean is_minor;

      is_minor = g_value_get_boolean(value);

      pthread_mutex_lock(audio_mutex);

      audio->is_minor = is_minor;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_SHARP_FLATS:
    {
      guint sharp_flats;

      sharp_flats = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->sharp_flats = sharp_flats;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OCTAVE:
    {
      gint octave;

      octave = g_value_get_int(value);

      pthread_mutex_lock(audio_mutex);

      audio->octave = octave;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_KEY:
    {
      guint key;

      key = g_value_get_uint(value);

      pthread_mutex_lock(audio_mutex);

      audio->key = key;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_ABSOLUTE_KEY:
    {
      gint absolute_key;

      absolute_key = g_value_get_int(value);

      pthread_mutex_lock(audio_mutex);

      audio->absolute_key = absolute_key;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_PRESET:
    {
      AgsPreset *preset;

      preset = (AgsPreset *) g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(preset == NULL ||
	 g_list_find(audio->preset,
		     preset) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_preset(audio,
			   (GObject *) preset);
    }
    break;
  case PROP_PLAYBACK_DOMAIN:
    {
      AgsPlaybackDomain *playback_domain;

      playback_domain = (AgsPlaybackDomain *) g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(audio->playback_domain == (GObject *) playback_domain){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      if(audio->playback_domain != NULL){
	g_object_unref(audio->playback_domain);
      }

      if(playback_domain != NULL){
	g_object_ref(playback_domain);
      }

      audio->playback_domain = (GObject *) playback_domain;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(notation == NULL ||
	 g_list_find(audio->notation,
		     notation) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_notation(audio,
			     (GObject *) notation);
    }
    break;
  case PROP_AUTOMATION:
    {
      AgsAutomation *automation;

      automation = (AgsAutomation *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(automation == NULL ||
	 g_list_find(audio->automation,
		     automation) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_automation(audio,
			       (GObject *) automation);
    }
    break;
  case PROP_WAVE:
    {
      AgsWave *wave;

      wave = (AgsWave *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(wave == NULL ||
	 g_list_find(audio->wave, wave) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_wave(audio,
			 (GObject *) wave);
    }
    break;
  case PROP_OUTPUT_AUDIO_FILE:
    {
      AgsAudioFile *output_audio_file;

      output_audio_file = (AgsAudioFile *) g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(audio->output_audio_file == (GObject *) output_audio_file){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      if(audio->output_audio_file != NULL){
	g_object_unref(audio->output_audio_file);
      }

      if(output_audio_file != NULL){
	g_object_ref(output_audio_file);
      }

      audio->output_audio_file = (GObject *) output_audio_file;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_FILE:
    {
      AgsAudioFile *input_audio_file;

      input_audio_file = (AgsAudioFile *) g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(audio->input_audio_file == (GObject *) input_audio_file){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      if(audio->input_audio_file != NULL){
	g_object_unref(audio->input_audio_file);
      }

      if(input_audio_file != NULL){
	g_object_ref(input_audio_file);
      }

      audio->input_audio_file = (GObject *) input_audio_file;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI:
    {
      AgsMidi *midi;

      midi = (AgsMidi *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(midi == NULL ||
	 g_list_find(audio->midi, midi) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_midi(audio,
			 (GObject *) midi);
    }
    break;
  case PROP_OUTPUT_MIDI_FILE:
    {
      AgsMidiFile *output_midi_file;

      output_midi_file = g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(audio->output_midi_file == output_midi_file){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      if(audio->output_midi_file != NULL){
	g_object_unref(audio->output_midi_file);
      }

      if(output_midi_file != NULL) {
	g_object_ref(output_midi_file);
      }

      audio->output_midi_file = output_midi_file;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_MIDI_FILE:
    {
      AgsMidiFile *input_midi_file;

      input_midi_file = g_value_get_object(value);

      pthread_mutex_lock(audio_mutex);

      if(audio->input_midi_file == input_midi_file){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      if(audio->input_midi_file != NULL){
	g_object_unref(audio->input_midi_file);
      }

      if(input_midi_file != NULL) {
	g_object_ref(input_midi_file);
      }

      audio->input_midi_file = input_midi_file;

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(recall_id == NULL ||
	 g_list_find(audio->recall_id, recall_id) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_recall_id(audio,
			      (GObject *) recall_id);
    }
    break;
  case PROP_RECYCLING_CONTEXT:
    {
      AgsRecyclingContext *recycling_context;

      recycling_context = (AgsRecyclingContext *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(recycling_context == NULL ||
	 g_list_find(audio->recycling_context, recycling_context) != NULL){
	pthread_mutex_unlock(audio_mutex);
	
	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_recycling_context(audio,
				      (GObject *) recycling_context);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_mutex);

      if(recall_container == NULL ||
	 g_list_find(audio->recall_container,
		     recall_container) != NULL){
	pthread_mutex_unlock(audio_mutex);

	return;
      }

      pthread_mutex_unlock(audio_mutex);

      ags_audio_add_recall_container(audio,
				     (GObject *) recall_container);
    }
    break;
  case PROP_PLAY:
    {
      AgsRecall *recall;

      pthread_mutex_t *play_mutex;

      /* get play mutex */
      pthread_mutex_lock(ags_audio_get_class_mutex());
  
      play_mutex = audio->play_mutex;
  
      pthread_mutex_unlock(ags_audio_get_class_mutex());

      /*  */
      recall = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(play_mutex);
      
      if(recall == NULL ||
	 g_list_find(audio->play,
		     recall) != NULL){
	pthread_mutex_unlock(play_mutex);
	
	return;
      }

      pthread_mutex_unlock(play_mutex);

      ags_audio_add_recall(audio,
			   (GObject *) recall,
			   TRUE);
    }
    break;
  case PROP_RECALL:
    {
      AgsRecall *recall;

      pthread_mutex_t *recall_mutex;

      /* get recall mutex */
      pthread_mutex_lock(ags_audio_get_class_mutex());
  
      recall_mutex = audio->recall_mutex;
  
      pthread_mutex_unlock(ags_audio_get_class_mutex());

      /*  */
      recall = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);
      
      if(recall == NULL ||
	 g_list_find(audio->recall,
		     recall) != NULL){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      pthread_mutex_unlock(recall_mutex);

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

  pthread_mutex_t *audio_mutex;

  audio = AGS_AUDIO(gobject);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  switch(prop_id){
  case PROP_OUTPUT_SOUNDCARD:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->output_soundcard);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->input_soundcard);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT_SEQUENCER:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->output_sequencer);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_SEQUENCER:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->input_sequencer);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->samplerate);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->buffer_size);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->format);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MAX_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->max_audio_channels);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MAX_OUTPUT_PADS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->max_output_pads);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MAX_INPUT_PADS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->max_input_pads);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->audio_channels);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT_PADS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->output_pads);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->output_lines);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_PADS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->input_pads);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_LINES:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->input_lines);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_AUDIO_START_MAPPING:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->audio_start_mapping);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_AUDIO_END_MAPPING:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->audio_end_mapping);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_START_MAPPING:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->midi_start_mapping);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_END_MAPPING:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->midi_end_mapping);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_MIDI_CHANNEL:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->midi_channel);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_NUMERATOR:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->numerator);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_DENOMINATOR:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->denominator);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_TIME_SIGNATURE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_string(value,
			 audio->time_signature);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_IS_MINOR:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_boolean(value,
			  audio->is_minor);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_SHARP_FLATS:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->sharp_flats);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OCTAVE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_int(value,
		      audio->octave);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_KEY:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_uint(value,
		       audio->key);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_ABSOLUTE_KEY:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_int(value,
		      audio->absolute_key);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->output);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->input);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_PRESET:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->preset));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_PLAYBACK_DOMAIN:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->playback_domain);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->notation));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_AUTOMATION:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->automation));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_WAVE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->wave));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT_AUDIO_FILE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->output_audio_file);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_FILE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->input_audio_file);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_OUTPUT_MIDI_FILE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->output_midi_file);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_INPUT_MIDI_FILE:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_object(value,
			 audio->input_midi_file);

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->recall_id));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_RECYCLING_CONTEXT:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->recycling_context));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      pthread_mutex_lock(audio_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->recall_container));

      pthread_mutex_unlock(audio_mutex);
    }
    break;
  case PROP_PLAY:
    {
      pthread_mutex_t *play_mutex;

      /* get play mutex */
      pthread_mutex_lock(ags_audio_get_class_mutex());
  
      play_mutex = audio->play_mutex;
  
      pthread_mutex_unlock(ags_audio_get_class_mutex());

      /*  */
      pthread_mutex_lock(play_mutex);
      
      g_value_set_pointer(value,
			  g_list_copy(audio->play));

      pthread_mutex_unlock(play_mutex);
    }
    break;
  case PROP_RECALL:
    {
      pthread_mutex_t *recall_mutex;

      /* get recall mutex */
      pthread_mutex_lock(ags_audio_get_class_mutex());
  
      recall_mutex = audio->recall_mutex;
  
      pthread_mutex_unlock(ags_audio_get_class_mutex());

      /*  */
      pthread_mutex_lock(recall_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio->recall));

      pthread_mutex_unlock(recall_mutex);
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

  pthread_mutex_t *play_mutex, *recall_mutex;

  audio = AGS_AUDIO(gobject);
  
  /* soundcard */
  if(audio->output_soundcard != NULL){    
    g_object_unref(audio->output_soundcard);

    audio->output_soundcard = NULL;
  }

  if(audio->input_soundcard != NULL){    
    g_object_unref(audio->input_soundcard);

    audio->input_soundcard = NULL;
  }

  /* sequencer */
  if(audio->output_sequencer != NULL){
    g_object_unref(audio->output_sequencer);

    audio->output_sequencer = NULL;
  }

  if(audio->input_sequencer != NULL){
    g_object_unref(audio->input_sequencer);

    audio->input_sequencer = NULL;
  }

  /* channels */
  ags_audio_set_audio_channels(audio,
			       0, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     0, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     0, 0);
  
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

  /* playback domain */
  if(audio->playback_domain != NULL){
    g_object_run_dispose(audio->playback_domain);

    audio->playback_domain = NULL;
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

  /* wave */
  if(audio->wave != NULL){
    list = audio->wave;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->wave,
		     g_object_unref);

    audio->wave = NULL;
  }

  /* output audio file */
  if(audio->output_audio_file != NULL){
    g_object_unref(audio->output_audio_file);

    audio->output_audio_file = NULL;
  }
  
  /* input audio file */
  if(audio->input_audio_file != NULL){
    g_object_unref(audio->input_audio_file);

    audio->input_audio_file = NULL;
  }

  /* midi */
  if(audio->midi != NULL){
    list = audio->midi;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(audio->midi,
		     g_object_unref);

    audio->midi = NULL;
  }

  /* output midi file */
  if(audio->output_midi_file != NULL){
    g_object_unref(audio->output_midi_file);

    audio->output_midi_file = NULL;
  }
  
  /* input midi file */
  if(audio->input_midi_file != NULL){
    g_object_unref(audio->input_midi_file);

    audio->input_midi_file = NULL;
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
  if(audio->recall_container != NULL){
    list = audio->recall_container;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(audio->recall_container,
		     g_object_unref);

    audio->recall_container = NULL;
  }

  /* play */
  if(audio->play != NULL){
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;

    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* run dispose and unref */
    pthread_mutex_lock(play_mutex);

    list = audio->play;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(audio->play,
		     g_object_unref);

    audio->play = NULL;

    pthread_mutex_unlock(play_mutex);
  }

  /* recall */
  if(audio->recall != NULL){
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;

    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* run dispose and unref */
    pthread_mutex_lock(recall_mutex);
    
    list = audio->recall;
  
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(audio->recall,
		     g_object_unref);

    audio->recall = NULL;

    pthread_mutex_unlock(recall_mutex);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->dispose(gobject);
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;

  GList *list;

  audio = AGS_AUDIO(gobject);

  pthread_mutex_destroy(audio->obj_mutex);
  free(audio->obj_mutex);

  pthread_mutexattr_destroy(audio->obj_mutexattr);
  free(audio->obj_mutexattr);

  /* soundcard */
  if(audio->output_soundcard != NULL){
    g_object_unref(audio->output_soundcard);
  }

  if(audio->input_soundcard != NULL){
    g_object_unref(audio->input_soundcard);
  }

  /* sequencer */
  if(audio->output_sequencer != NULL){
    g_object_unref(audio->output_sequencer);
  }

  if(audio->input_sequencer != NULL){
    g_object_unref(audio->input_sequencer);
  }

  /* channels */
  ags_audio_set_audio_channels(audio,
			       0, 0);

  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     0, 0);
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     0, 0);
  
  /* preset */
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

  /* wave */
  if(audio->wave != NULL){
    list = audio->wave;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->wave,
		     g_object_unref);
  }

  /* output audio file */
  if(audio->output_audio_file != NULL){
    g_object_unref(audio->output_audio_file);
  }

  /* input audio file */
  if(audio->input_audio_file != NULL){
    g_object_unref(audio->input_audio_file);
  }
  
  /* midi */
  if(audio->midi != NULL){
    list = audio->midi;

    while(list != NULL){
      g_object_set(list->data,
		   "audio", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio->midi,
		     g_object_unref);
  }

  /* output midi file */
  if(audio->output_midi_file != NULL){
    g_object_unref(audio->output_midi_file);
  }

  /* input midi file */
  if(audio->input_midi_file != NULL){
    g_object_unref(audio->input_midi_file);
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
  
  /* recall container */
  if(audio->recall_container != NULL){
    g_list_free_full(audio->recall_container,
		     g_object_unref);
  }

  /* play context */
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

  pthread_mutex_destroy(audio->play_mutex);
  free(audio->play_mutex);

  pthread_mutexattr_destroy(audio->play_mutexattr);
  free(audio->play_mutexattr);

  /* recall context */
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

  pthread_mutex_destroy(audio->recall_mutex);
  free(audio->recall_mutex);

  pthread_mutexattr_destroy(audio->recall_mutexattr);
  free(audio->recall_mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_get_uuid(AgsConnectable *connectable)
{
  AgsAudio *audio;
  
  AgsUUID *ptr;

  pthread_mutex_t *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(audio_mutex);

  ptr = audio->uuid;

  pthread_mutex_unlock(audio_mutex);
  
  return(ptr);
}

gboolean
ags_audio_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_audio_is_ready(AgsConnectable *connectable)
{
  AgsAudio *audio;
  
  gboolean is_ready;

  pthread_mutex_t *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(audio_mutex);

  is_ready = (((AGS_AUDIO_ADDED_TO_REGISTRY & (audio->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(audio_mutex);
  
  return(is_ready);
}

void
ags_audio_add_to_registry(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;
  
  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio = AGS_AUDIO(connectable);

  ags_audio_set_flags(audio, AGS_AUDIO_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = audio->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) audio);
    
    ags_registry_add_entry(registry,
			   entry);
  }
  
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
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_audio_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_xml_parse(AgsConnectable *connectable,
		    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_is_connected(AgsConnectable *connectable)
{
  AgsAudio *audio;
  
  gboolean is_connected;

  pthread_mutex_t *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(audio_mutex);

  is_connected = (((AGS_AUDIO_CONNECTED & (audio->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_mutex);
  
  return(is_connected);
}

void
ags_audio_connect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;

  GList *list;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio = AGS_AUDIO(connectable);

  ags_audio_set_flags(audio, AGS_AUDIO_CONNECTED);

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

  /* connect notation */
  list = audio->notation;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
  
  /* connect automation */
  list = audio->automation;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  /* connect wave */
  list = audio->wave;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
  
  /* connect midi */
  list = audio->midi;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  /* connect recall ids */
  list = audio->recall_id;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = audio->recall_container;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = audio->play;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->recall;

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

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio = AGS_AUDIO(connectable);
  
  ags_audio_unset_flags(audio, AGS_AUDIO_CONNECTED);
  
#ifdef AGS_DEBUG
  g_message("disconnecting audio");
#endif

  /* connect channels */
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

  /* connect recall ids */
  list = audio->recall_id;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = audio->recall_container;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
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

  /* connect notation */
  list = audio->notation;
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_audio_connect_connection(AgsConnectable *connectable,
			     GObject *connection)
{
  //TODO:JK: implement me
}

void
ags_audio_disconnect_connection(AgsConnectable *connectable,
				GObject *connection)
{
  //TODO:JK: implement me
}

/**
 * ags_audio_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_audio_get_class_mutex()
{
  return(&ags_audio_class_mutex);
}

/**
 * ags_audio_set_flags:
 * @audio: the #AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Enable a feature of AgsAudio.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_flags(AgsAudio *audio, guint flags)
{
  guint audio_flags;
  
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check flags */
  pthread_mutex_lock(audio_mutex);
  
  audio_flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  if((AGS_AUDIO_NO_OUTPUT & flags) != 0 &&
     (AGS_AUDIO_NO_OUTPUT & audio_flags) == 0){
    ags_audio_set_pads(audio,
		       AGS_TYPE_OUTPUT,
		       0, 0);
  }

  if((AGS_AUDIO_NO_INPUT & flags) != 0 &&
     (AGS_AUDIO_NO_INPUT & audio_flags) == 0){
    ags_audio_set_pads(audio,
		       AGS_TYPE_INPUT,
		       0, 0);
  }
  
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) != 0 &&
     (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) == 0){
    AgsChannel *channel;
    AgsRecycling *first_recycling, *last_recycling;

    GObject *soundcard;
    
    pthread_mutex_t *channel_mutex;

    pthread_mutex_lock(audio_mutex);

    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
  
      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      soundcard = channel->output_soundcard;
      
      pthread_mutex_unlock(channel_mutex);
      
      /* add recycling */
      first_recycling =
	last_recycling = ags_recycling_new(soundcard);
      g_object_ref(first_recycling);
      g_object_set(first_recycling,
		   "channel", channel,
		   NULL);
	  
      ags_channel_reset_recycling(channel,
				  first_recycling, last_recycling);
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }

  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (flags)) != 0 &&
     (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) == 0){
    AgsChannel *channel;
    AgsRecycling *recycling;
    AgsRecycling *first_recycling, *last_recycling;

    GObject *soundcard;
    
    pthread_mutex_t *channel_mutex;

    pthread_mutex_lock(audio_mutex);

    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
  
      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      soundcard = channel->output_soundcard;
      recycling = channel->first_recycling;
      
      pthread_mutex_unlock(channel_mutex);
      
      /* add recycling */
      if(recycling == NULL){
	first_recycling =
	  last_recycling = ags_recycling_new(soundcard);
	g_object_ref(first_recycling);
	g_object_set(first_recycling,
		     "channel", channel,
		     NULL);
	  
	ags_channel_set_recycling(channel,
				  first_recycling, last_recycling,
				  TRUE, TRUE);
      }
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(audio_mutex);

  audio->flags |= flags;
  
  pthread_mutex_unlock(audio_mutex);
}
    
/**
 * ags_audio_unset_flags:
 * @audio: the #AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Disable a feature of AgsAudio.
 *
 * Since: 2.0.0
 */
void
ags_audio_unset_flags(AgsAudio *audio, guint flags)
{
  guint audio_flags;
  
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check flags */
  pthread_mutex_lock(audio_mutex);
  
  audio_flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0 &&
     (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
    AgsChannel *channel;
    AgsRecycling *first_recycling;
    
    pthread_mutex_t *channel_mutex;

    pthread_mutex_lock(audio_mutex);

    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
  
      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* unset recycling */
      pthread_mutex_lock(channel_mutex);

      first_recycling = channel->first_recycling;

      channel->first_recycling =
	channel->last_recycling = NULL;
      
      pthread_mutex_unlock(channel_mutex);
      
      /* remove recycling */
      g_object_run_dispose(first_recycling);
      g_object_unref(first_recycling);
	  
      ags_channel_set_recycling(channel,
				NULL, NULL,
				TRUE, TRUE);
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }
  
  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0 &&
     (AGS_AUDIO_INPUT_HAS_RECYCLING & (flags)) == 0){
    AgsChannel *channel, *link;
    AgsRecycling *first_recycling;
    
    pthread_mutex_t *channel_mutex;

    pthread_mutex_lock(audio_mutex);

    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
  
      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      link = channel->link;
      
      pthread_mutex_unlock(channel_mutex);
      
      /* unset recycling */
      if(link == NULL){
	pthread_mutex_lock(channel_mutex);

	first_recycling = channel->first_recycling;

	channel->first_recycling =
	  channel->last_recycling = NULL;
      
	pthread_mutex_unlock(channel_mutex);
      
	/* remove recycling */
	g_object_run_dispose(first_recycling);
	g_object_unref(first_recycling);
	  
	ags_channel_set_recycling(channel,
				  NULL, NULL,
				  TRUE, TRUE);
      }
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(audio_mutex);

  audio->flags &= (~flags);
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_set_ability_flags:
 * @audio: the #AgsAudio
 * @ability_flags: see enum AgsSoundAbilityFlags
 *
 * Enable an ability of AgsAudio.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_ability_flags(AgsAudio *audio, guint ability_flags)
{
  AgsChannel *output, *input;  
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *main_loop;
  AgsAudioLoop *audio_loop;
  
  AgsApplicationContext *application_context;
  
  GObject *soundcard;
  
  guint samplerate, buffer_size;
  guint audio_ability_flags;
  gboolean super_threaded_audio;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *playback_domain_mutex;

  auto void ags_audio_set_ability_flags_channel(AgsChannel *channel, guint ability_flags);

  void ags_audio_set_ability_flags_channel(AgsChannel *channel, guint ability_flags){
    pthread_mutex_t *channel_mutex;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
      
      channel_mutex = channel->obj_mutex;
      
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* set ability flags */
      ags_channel_set_ability_flags(channel, ability_flags);

      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }
  
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check flags */
  pthread_mutex_lock(audio_mutex);

  audio_ability_flags = audio->ability_flags;

  soundcard = audio->output_soundcard;
  
  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;

  output = audio->output;
  input = audio->input;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
  
  pthread_mutex_unlock(audio_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_playback_domain_get_class_mutex());
  
  playback_domain_mutex = playback_domain->obj_mutex;
  
  pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

  /* get super-threaded flags */
  pthread_mutex_lock(playback_domain_mutex);

  super_threaded_audio = ((AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO & (g_atomic_int_get(&(playback_domain->flags)))) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(playback_domain_mutex);
  
  /* notation ability */  
  if(super_threaded_audio){
    /* find audio loop */
    audio_loop = ags_thread_find_type(main_loop,
				      AGS_TYPE_AUDIO_LOOP);
    
    if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_PLAYBACK & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(soundcard,
					  audio);
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   audio_thread,
					   AGS_SOUND_SCOPE_PLAYBACK);    

      /* set thread child */
      ags_thread_add_child(audio_loop,
			   audio_thread);
    }

    if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_NOTATION & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(soundcard,
					  audio);
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   audio_thread,
					   AGS_SOUND_SCOPE_NOTATION);    

      /* set thread child */
      ags_thread_add_child(audio_loop,
			   audio_thread);
    }

    /* sequencer ability */
    if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_SEQUENCER & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(soundcard,
					  audio);
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   audio_thread,
					   AGS_SOUND_SCOPE_SEQUENCER);

      /* set thread child */
      ags_thread_add_child(audio_loop,
			   audio_thread);
    }

    /* wave ability */
    if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_WAVE & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(soundcard,
					  audio);
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   audio_thread,
					   AGS_SOUND_SCOPE_WAVE);

      /* set thread child */
      ags_thread_add_child(audio_loop,
			   audio_thread);
    }

    /* midi ability */
    if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_MIDI & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(soundcard,
					  audio);
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   audio_thread,
					   AGS_SOUND_SCOPE_MIDI);

      /* set thread child */
      ags_thread_add_child(audio_loop,
			   audio_thread);
    }
  }
  
  /* channel */
  ags_audio_set_ability_flags_channel(output, ability_flags);  
  ags_audio_set_ability_flags_channel(input, ability_flags);
  
  /* set flags */
  pthread_mutex_lock(audio_mutex);

  audio->ability_flags |= ability_flags;
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_ability_flags:
 * @audio: the #AgsAudio
 * @flags: see enum AgsSoundAbilityFlags
 *
 * Disable an ability of AgsAudio.
 *
 * Since: 2.0.0
 */
void
ags_audio_unset_ability_flags(AgsAudio *audio, guint ability_flags)
{
  AgsChannel *output, *input;  
  AgsPlaybackDomain *playback_domain;

  AgsThread *main_loop;
  AgsThread *audio_loop;

  AgsApplicationContext *application_context;
  
  guint audio_ability_flags;
  
  pthread_mutex_t *audio_mutex;

  auto void ags_audio_unset_ability_flags_channel(AgsChannel *channel, guint ability_flags);

  void ags_audio_unset_ability_flags_channel(AgsChannel *channel, guint ability_flags){
    pthread_mutex_t *channel_mutex;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());
      
      channel_mutex = channel->obj_mutex;
      
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* set ability flags */
      ags_channel_unset_ability_flags(channel, ability_flags);

      /* iterate */
      pthread_mutex_lock(channel_mutex);

      channel = channel->next;
      
      pthread_mutex_unlock(channel_mutex);
    }
  }

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* check flags */
  pthread_mutex_lock(audio_mutex);
  
  audio_ability_flags = audio->ability_flags;

  output = audio->output;
  input = audio->input;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

  pthread_mutex_unlock(audio_mutex);

  /* find audio loop */
  audio_loop = ags_thread_find_type(main_loop,
				    AGS_TYPE_AUDIO_LOOP);

  /* playback ability */
  if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_PLAYBACK & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;

    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_PLAYBACK);
    ags_thread_remove_child(audio_loop,
			    audio_thread);
    
    ags_playback_domain_set_audio_thread(playback_domain,
					 AGS_SOUND_SCOPE_PLAYBACK,
					 NULL);
  }

  /* notation ability */
  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_NOTATION & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_NOTATION);
    ags_thread_remove_child(audio_loop,
			    audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 AGS_SOUND_SCOPE_NOTATION,
					 NULL);
  }

  /* sequencer ability */
  if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_SEQUENCER & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_SEQUENCER);
    ags_thread_remove_child(audio_loop,
			    audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 AGS_SOUND_SCOPE_SEQUENCER,
					 NULL);
  }

  /* wave ability */
  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_WAVE & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_WAVE);
    ags_thread_remove_child(audio_loop,
			    audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 AGS_SOUND_SCOPE_WAVE,
					 NULL);
  }

  /* midi ability */
  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_MIDI & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							AGS_SOUND_SCOPE_MIDI);
    ags_thread_remove_child(audio_loop,
			    audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 AGS_SOUND_SCOPE_MIDI,
					 NULL);
  }

  /* channel */
  ags_audio_unset_ability_flags_channel(output, ability_flags);  
  ags_audio_unset_ability_flags_channel(input, ability_flags);
  
  /* unset flags */
  pthread_mutex_lock(audio_mutex);

  audio->ability_flags &= (~ability_flags);
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_set_behaviour_flags:
 * @audio: the #AgsAudio
 * @behaviour_flags: the behaviour flags
 * 
 * Set behaviour flags.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_behaviour_flags(AgsAudio *audio, guint behaviour_flags)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(audio_mutex);

  audio->behaviour_flags |= behaviour_flags;

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_behaviour_flags:
 * @audio: the #AgsAudio
 * @behaviour_flags: the behaviour flags
 * 
 * Unset behaviour flags.
 * 
 * Since: 2.0.0
 */
void
ags_audio_unset_behaviour_flags(AgsAudio *audio, guint behaviour_flags)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(audio_mutex);

  audio->behaviour_flags &= (~behaviour_flags);

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_set_staging_flags:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Set staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_staging_flags(AgsAudio *audio, gint sound_scope,
			    guint staging_flags)
{
  guint i;
  
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_flags[i] |= staging_flags;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_flags[sound_scope] |= staging_flags;
  }

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_staging_flags:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Unset staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_audio_unset_staging_flags(AgsAudio *audio, gint sound_scope,
			      guint staging_flags)
{
  guint i;
  
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_flags[i] &= (~staging_flags);
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_flags[sound_scope] &= (~staging_flags);
  }

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_set_max_audio_channels:
 * @audio: the #AgsAudio
 * @max_audio_channels: maximum of audio channels
 * 
 * Set maximum audio channels.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_max_audio_channels(AgsAudio *audio,
				 guint max_audio_channels)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set max audio channels */
  pthread_mutex_lock(audio_mutex);

  audio->max_audio_channels = max_audio_channels;

  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_set_max_pads:
 * @audio: the #AgsAudio
 * @channel_type: the #GType of channel, either AGS_TYPE_OUTPUT or AGS_TYPE_INPUT
 * @max_audio_channels: maximum of audio channels
 * 
 * Set maximum pads of @channel_type.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_max_pads(AgsAudio *audio,
		       GType channel_type,
		       guint max_pads)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set max pads */
  pthread_mutex_lock(audio_mutex);

  if(g_type_is_a(channel_type,
		 AGS_TYPE_OUTPUT)){
    audio->max_output_pads = max_pads;
  }else if(g_type_is_a(channel_type,
		       AGS_TYPE_INPUT)){
    audio->max_input_pads = max_pads;
  }

  pthread_mutex_unlock(audio_mutex);
}

void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GObject *output_soundcard, *input_soundcard;
  
  gboolean alloc_recycling;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput
  gboolean alloc_pattern;

  guint bank_dim[3];

  guint audio_flags;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint output_pads, input_pads;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *prev_mutex, *prev_pad_mutex, *current_mutex;
  
  auto void ags_audio_set_audio_channels_init_parameters(GType type);
  auto void ags_audio_set_audio_channels_grow(GType type);
  auto void ags_audio_set_audio_channels_shrink_zero();
  auto void ags_audio_set_audio_channels_shrink();

  auto void ags_audio_set_audio_channels_shrink_notation();
  auto void ags_audio_set_audio_channels_shrink_automation();
  auto void ags_audio_set_audio_channels_shrink_wave();
  auto void ags_audio_set_audio_channels_shrink_midi();
  
  void ags_audio_set_audio_channels_init_parameters(GType type){
    alloc_pattern = FALSE;
    
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
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;
      }
      
      if((AGS_AUDIO_ASYNC & (audio->flags)) != 0 && alloc_recycling){
	link_recycling = TRUE;
      }else{
	link_recycling = FALSE;
      }

      if((AGS_SOUND_ABILITY_SEQUENCER & (audio->ability_flags)) != 0){
	alloc_pattern = TRUE;
	
	bank_dim[0] = audio->bank_dim[0];
	bank_dim[1] = audio->bank_dim[1];
	bank_dim[2] = audio->bank_dim[2];
      }
    }    
  }
  
  void ags_audio_set_audio_channels_grow(GType type){
    AgsChannel *channel, *start, *current, *pad_next;
    AgsRecycling *first_recycling, *last_recycling;
	  
    guint pads;
    guint i, j;

    pthread_mutex_lock(audio_mutex);
    
    if(type == AGS_TYPE_OUTPUT){
      /* AGS_TYPE_OUTPUT */
      pads = audio->output_pads;
      
      start = audio->output;
    }else{
      /* AGS_TYPE_INPUT */
      pads = audio->input_pads;

      start = audio->input;
    }

    pthread_mutex_unlock(audio_mutex);

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
					      "output-soundcard", output_soundcard,
					      "input-soundcard", input_soundcard,
					      "samplerate", samplerate,
					      "buffer-size", buffer_size,
					      NULL);
	g_object_ref(channel);
	
	if(alloc_pattern){
	  channel->pattern = g_list_alloc();
	  channel->pattern->data = (gpointer) ags_pattern_new();
	  g_object_ref(channel->pattern->data);
	  
	  ags_pattern_set_dim((AgsPattern *) channel->pattern->data,
			      bank_dim[0], bank_dim[1], bank_dim[2]);
	}

	if(start == NULL){
	  pthread_mutex_lock(audio_mutex);
	  
	  start = channel;

	  if(type == AGS_TYPE_OUTPUT){
	    audio->output = channel;
	  }else{
	    audio->input = channel;
	  }

	  pthread_mutex_unlock(audio_mutex);
	}

	if(j * audio_channels + i != 0){
	  /* set prev */
	  channel->prev = ags_channel_nth(start, j * audio_channels + i - 1);

	  /* get prev mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	  prev_mutex = channel->prev->obj_mutex;
	  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* set next and prev->next */
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

	  /* get prev pad mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	  prev_pad_mutex = channel->prev_pad->obj_mutex;
	  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* set prev pad next pad */
	  pthread_mutex_lock(prev_pad_mutex);
	  
	  channel->prev_pad->next_pad = channel;

	  pthread_mutex_unlock(prev_pad_mutex);
	}

	/* set indices */
	channel->pad = j;
	channel->audio_channel = i;
	channel->line = j * audio_channels + i;

	/* reset nested AgsRecycling tree */
	if(alloc_recycling){
	  first_recycling =
	    last_recycling = ags_recycling_new(output_soundcard);
	  g_object_ref(first_recycling);
	  g_object_set(first_recycling,
		       "channel", channel,
		       NULL);
	  
	  ags_channel_set_recycling(channel,
				    first_recycling, last_recycling,
				    TRUE, TRUE);
	}else if(set_sync_link){
	  AgsChannel *input;

	  pthread_mutex_lock(audio_mutex);

	  input = audio->input;
	  
	  pthread_mutex_unlock(audio_mutex);

	  input = ags_channel_nth(input,
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

	  pthread_mutex_lock(audio_mutex);

	  input = audio->input;
	  
	  pthread_mutex_unlock(audio_mutex);

	  input = ags_channel_nth(input,
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

    pthread_mutex_lock(audio_mutex);
	  
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

    pthread_mutex_unlock(audio_mutex);
  }
  
  void ags_audio_set_audio_channels_shrink(){
    AgsChannel *channel, *start;
    AgsChannel *channel0, *channel1;
    AgsRecycling *recycling;

    guint pads, i, j;
    gboolean first_run;
    
    GError *error;

    pthread_mutex_lock(audio_mutex);

    start =
      channel = audio->output;
    pads = audio->output_pads;

    pthread_mutex_unlock(audio_mutex);

    first_run = TRUE;

  ags_audio_set_audio_channel_shrink0:

    for(i = 0; i < pads; i++){
      channel = ags_channel_nth(channel, audio_channels);

      for(j = audio_channels; j < audio_channels_old; j++){
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
	
	for(; j < audio_channels_old; j++){
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
    
  void ags_audio_set_audio_channels_shrink_notation(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list_start = g_list_copy(audio->notation);

    while(list != NULL){
      if(AGS_NOTATION(list->data)->audio_channel >= audio_channels){
	ags_audio_remove_notation(audio,
				  list->data);

	g_object_run_dispose((GObject *) list->data);
	g_object_unref((GObject *) list->data);
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    pthread_mutex_unlock(audio_mutex);
  }

  void ags_audio_set_audio_channels_shrink_automation(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list_start = g_list_copy(audio->automation);

    while(list != NULL){
      if(AGS_AUTOMATION(list->data)->line % audio_channels_old >= audio_channels){
	ags_audio_remove_automation(audio,
				    list->data);

	g_object_run_dispose((GObject *) list->data);
	g_object_unref((GObject *) list->data);
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    if(audio_channels == 0){
      audio->automation = NULL;
    }

    pthread_mutex_unlock(audio_mutex);
  }
  
  void ags_audio_set_audio_channels_shrink_wave(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list_start = g_list_copy(audio->wave);

    while(list != NULL){
      if(AGS_WAVE(list->data)->audio_channel >= audio_channels){
	ags_audio_remove_wave(audio,
			      list->data);

	g_object_run_dispose((GObject *) list->data);
	g_object_unref((GObject *) list->data);
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    pthread_mutex_unlock(audio_mutex);
  }

  void ags_audio_set_audio_channels_shrink_midi(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list_start = g_list_copy(audio->midi);

    while(list != NULL){
      if(AGS_MIDI(list->data)->audio_channel >= audio_channels){
	ags_audio_remove_midi(audio,
			      list->data);

	g_object_run_dispose((GObject *) list->data);
	g_object_unref((GObject *) list->data);
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    pthread_mutex_unlock(audio_mutex);
  }
  
  /*
   * entry point
   */
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* grow / shrink */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  output_soundcard = audio->output_soundcard;
  input_soundcard = audio->input_soundcard;
  
  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;
  format = audio->format;

  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  pthread_mutex_unlock(audio_mutex);

  if(audio_channels > audio_channels_old){
    AgsPlaybackDomain *playback_domain;
    AgsPlayback *playback;
    AgsChannel *current;

    guint i;
    
    pthread_mutex_t *current_mutex;
    pthread_mutex_t *playback_domain_mutex;
    
    /* grow input channels */
    if(audio->input_pads > 0 &&
       (AGS_AUDIO_NO_INPUT & (audio_flags)) == 0){
      ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
      ags_audio_set_audio_channels_grow(AGS_TYPE_INPUT);
    }

    /* apply new sizes */
    pthread_mutex_lock(audio_mutex);
    
    audio->input_lines = audio_channels * audio->input_pads;

    pthread_mutex_unlock(audio_mutex);

    /* grow output channels */
    if(audio->output_pads > 0 &&
       (AGS_AUDIO_NO_OUTPUT & (audio_flags)) == 0){
      ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
      ags_audio_set_audio_channels_grow(AGS_TYPE_OUTPUT);
    }

    /* apply new sizes and get some fields */
    pthread_mutex_lock(audio_mutex);
    
    audio->output_lines = audio_channels * audio->output_pads;

    audio->audio_channels = audio_channels;

    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

    pthread_mutex_unlock(audio_mutex);

    /* get playback domain mutex */
    pthread_mutex_lock(ags_playback_domain_get_class_mutex());
	  
    playback_domain_mutex = playback_domain->obj_mutex;
	  
    pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

    /* add output playback */
    pthread_mutex_lock(audio_mutex);
    
    current = audio->output;
    
    pthread_mutex_unlock(audio_mutex);

    while(current != NULL){
      current = ags_channel_nth(current,
				audio_channels_old);

      for(i = 0; i < audio_channels - audio_channels_old; i++){
	/* get current mutex */
	pthread_mutex_lock(ags_current_get_class_mutex());
	  
	current_mutex = current->obj_mutex;
	  
	pthread_mutex_unlock(ags_current_get_class_mutex());

	/* playback */
	pthread_mutex_lock(current_mutex);

	playback = current->playback;

	pthread_mutex_unlock(current_mutex);

	/* append */
	ags_playback_domain_add_playback(playback_domain,
					 playback, AGS_TYPE_OUTPUT);
	
	/* iterate */
	pthread_mutex_lock(current_mutex);

	current = current->next;

	pthread_mutex_unlock(current_mutex);
      }
    }

    /* add input playback */
    pthread_mutex_lock(audio_mutex);
    
    current = audio->input;
    
    pthread_mutex_unlock(audio_mutex);

    while(current != NULL){
      current = ags_channel_nth(current,
				audio_channels_old);

      for(i = 0; i < audio_channels - audio_channels_old; i++){
	/* get current mutex */
	pthread_mutex_lock(ags_current_get_class_mutex());
	  
	current_mutex = current->obj_mutex;
	  
	pthread_mutex_unlock(ags_current_get_class_mutex());

	/* playback */
	pthread_mutex_lock(current_mutex);

	playback = current->playback;

	pthread_mutex_unlock(current_mutex);

	/* append */
	ags_playback_domain_add_playback(playback_domain,
					 playback, AGS_TYPE_INPUT);

	/* iterate */
	pthread_mutex_lock(current_mutex);

	current = current->next;

	pthread_mutex_unlock(current_mutex);
      }
    }
  }else if(audio_channels < audio_channels_old){
    AgsPlaybackDomain *playback_domain;
    AgsPlayback *playback;
    AgsChannel *current;

    GList *list, *list_start;
    
    guint i, j;
    
    pthread_mutex_t *playback_domain_mutex;

    /* shrink audio channels */
    ags_audio_set_audio_channels_shrink_automation();
    
    if((AGS_SOUND_ABILITY_NOTATION & (audio->ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_notation();
    }

    if((AGS_SOUND_ABILITY_WAVE & (audio->ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_wave();
    }

    if((AGS_SOUND_ABILITY_MIDI & (audio->ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_midi();
    }

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero();
    }else{
      ags_audio_set_audio_channels_shrink();
    }

    /* apply new sizes */
    pthread_mutex_lock(audio_mutex);

    audio->audio_channels = audio_channels;
    
    audio->input_lines = audio_channels * audio->input_pads;
    audio->output_lines = audio_channels * audio->output_pads;

    playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

    pthread_mutex_unlock(audio_mutex);

    /* get playback domain mutex */
    pthread_mutex_lock(ags_playback_domain_get_class_mutex());
	  
    playback_domain_mutex = playback_domain->obj_mutex;
	  
    pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

    /* shrink output playback domain */
    pthread_mutex_lock(playback_domain_mutex);

    list_start = g_list_copy(playback_domain->output_playback);
    
    pthread_mutex_unlock(playback_domain_mutex);
    
    for(j = 0; j < output_pads; j++){
      list = g_list_nth(list_start,
			(j + 1) * audio_channels);
      
      for(i = 0; i < audio_channels_old - audio_channels; i++){
	playback = list->data;

	ags_playback_domain_remove_playback(playback_domain,
					    playback, AGS_TYPE_OUTPUT);
	
	g_object_run_dispose(playback);
	g_object_unref(playback);

	/* iterate */
	list = list->next;
      }
    }

    g_list_free(list_start);

    /* shrink input playback domain */
    pthread_mutex_lock(playback_domain_mutex);

    list_start = g_list_copy(playback_domain->input_playback);
    
    pthread_mutex_unlock(playback_domain_mutex);
    
    for(j = 0; j < input_pads; j++){
      list = g_list_nth(list_start,
			(j + 1) * audio_channels);
      
      for(i = 0; i < audio_channels_old - audio_channels; i++){
	playback = list->data;

	ags_playback_domain_remove_playback(playback_domain,
					    playback, AGS_TYPE_INPUT);
	
	g_object_run_dispose(playback);
	g_object_unref(playback);

	/* iterate */
	list = list->next;
      }
    }

    g_list_free(list_start);
  }
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsAudio::set-audio-channels");

    /* add message */
    message = ags_message_envelope_alloc(audio,
					 NULL,
					 doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* audio channels */
    message->parameter_name[0] = "audio-channels";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     audio_channels);

    /* audio channels old */
    message->parameter_name[1] = "audio-channels-old";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     audio_channels_old);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
}

/**
 * ags_audio_set_audio_channels:
 * @audio: the #AgsAudio
 * @audio_channels: new audio channels
 *
 * Resize audio channels AgsInput will be allocated first.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old)
{
  pthread_mutex_t *audio_mutex;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* lock */
  pthread_mutex_lock(audio_mutex);

  audio_channels_old = audio->audio_channels;

  pthread_mutex_unlock(audio_mutex);

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) audio);

  /* unlock */
  //TODO:JK: remove comment above
}

/*
 * resize
 * AgsInput has to be allocated first
 */
void
ags_audio_real_set_pads(AgsAudio *audio,
			GType channel_type,
			guint pads, guint pads_old)
{
  AgsChannel *channel;
  AgsPlaybackDomain *playback_domain;
  
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GObject *output_soundcard;
  GObject *input_soundcard;
  
  guint bank_dim[3];

  guint audio_flags;
  guint audio_channels;
  guint output_pads, input_pads;
  guint samplerate;
  guint buffer_size;
  guint format;
  gboolean alloc_recycling, link_recycling, set_sync_link, set_async_link;
  gboolean alloc_pattern;
 
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *current_mutex;
  pthread_mutex_t *prev_mutex, *prev_pad_mutex;
  pthread_mutex_t *playback_domain_mutex;

  auto void ags_audio_set_pads_init_parameters();
  auto void ags_audio_set_pads_grow();
  auto void ags_audio_set_pads_unlink_all(AgsChannel *channel);
  auto void ags_audio_set_pads_shrink_zero(AgsChannel *channel);
  auto void ags_audio_set_pads_shrink(AgsChannel *channel);
  auto void ags_audio_set_pads_free_notation();
  auto void ags_audio_set_pads_remove_notes();
  auto void ags_audio_set_pads_shrink_automation();
  auto void ags_audio_set_pads_free_wave();
  auto void ags_audio_set_pads_free_midi();
  
  void ags_audio_set_pads_init_parameters(){
    alloc_recycling = FALSE;
    link_recycling = FALSE;
    set_sync_link = FALSE;
    set_async_link = FALSE;

    alloc_pattern = FALSE;
    
    if(channel_type == AGS_TYPE_OUTPUT){
      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	  if((AGS_AUDIO_SYNC & (audio_flags)) != 0 && (AGS_AUDIO_ASYNC & (audio_flags)) == 0){
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	    set_async_link = TRUE;
	  }else{
#ifdef AGS_DEBUG
	    g_message("ags_audio_set_pads - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined");
#endif
	  }
	}
      }
    }else{      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	alloc_recycling = TRUE;
      }
      
      if((AGS_AUDIO_ASYNC & (audio_flags)) != 0 && alloc_recycling){
	link_recycling = TRUE;
      }

      pthread_mutex_lock(audio_mutex);
      
      if((AGS_SOUND_ABILITY_SEQUENCER & (audio->ability_flags)) != 0){
	alloc_pattern = TRUE;

	bank_dim[0] = audio->bank_dim[0];
	bank_dim[1] = audio->bank_dim[1];
	bank_dim[2] = audio->bank_dim[2];
      }      

      pthread_mutex_unlock(audio_mutex);
    }    
  }

  void ags_audio_set_pads_grow(){
    AgsChannel *start, *channel;
    AgsRecycling *first_recycling, *last_recycling;

    guint i, j;

    pthread_mutex_lock(audio_mutex);

    if(channel_type == AGS_TYPE_OUTPUT){
      start = audio->output;
    }else{
      start = audio->input;
    }

    pthread_mutex_unlock(audio_mutex);

    for(j = pads_old; j < pads; j++){
      for(i = 0; i < audio_channels; i++){
	channel = (AgsChannel *) g_object_new(channel_type,
					      "audio", (GObject *) audio,
					      "output-soundcard", output_soundcard,
					      "input-soundcard", input_soundcard,
					      "samplerate", samplerate,
					      "buffer-size", buffer_size,
					      "format", format,
					      NULL);
	g_object_ref(channel);

	if(alloc_pattern){
	  channel->pattern = g_list_alloc();
	  channel->pattern->data = (gpointer) ags_pattern_new();
	  g_object_ref(channel->pattern->data);
	  
	  ags_pattern_set_dim((AgsPattern *) channel->pattern->data,
			      bank_dim[0], bank_dim[1], bank_dim[2]);
	}
	
	if(start == NULL){
	  /* set first channel in AgsAudio */
	  pthread_mutex_lock(audio_mutex);
	  
	  if(channel_type == AGS_TYPE_OUTPUT){
	    start = 
	      audio->output = channel;
	  }else{
	    start = 
	      audio->input = channel;
	  }

	  pthread_mutex_unlock(audio_mutex);
	}

	if(j * audio_channels + i != 0){
	  /* set prev */
	  channel->prev = ags_channel_nth(start, j * audio_channels + i - 1);

	  /* get prev mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	  prev_mutex = channel->prev->obj_mutex;
	  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* set prev->next */
	  pthread_mutex_lock(prev_mutex);
	  
	  channel->prev->next = channel;

	  pthread_mutex_unlock(prev_mutex);
	}
	
	if(j != 0){
	  /* set prev pad */
	  channel->prev_pad = ags_channel_pad_nth(ags_channel_nth(start,
								  i),
						  j - 1);

	  /* get prev pad mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	  prev_pad_mutex = channel->prev_pad->obj_mutex;
	  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* set prev_pad->next_pad */
	  pthread_mutex_lock(prev_pad_mutex);
	  
	  channel->prev_pad->next_pad = channel;
	  
	  pthread_mutex_unlock(prev_pad_mutex);
	}

	/* set indices */
	channel->pad = j;
	channel->audio_channel = i;
	channel->line = j * audio_channels + i;

	/* reset nested AgsRecycling tree */
	if(alloc_recycling){
	  first_recycling =
	    last_recycling = ags_recycling_new(output_soundcard);
	  g_object_ref(first_recycling);
	  g_object_set(first_recycling,
		       "channel", channel,
		       NULL);
	  
	  ags_channel_set_recycling(channel,
				    first_recycling, last_recycling,
				    TRUE, TRUE);
	}else if(set_sync_link){
	  AgsChannel *input;

	  pthread_mutex_lock(audio_mutex);

	  input = audio->input;
	  
	  pthread_mutex_unlock(audio_mutex);

	  input = ags_channel_nth(input,
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

	  pthread_mutex_lock(audio_mutex);

	  input = audio->input;
	  
	  pthread_mutex_unlock(audio_mutex);

	  input = ags_channel_nth(input,
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
      
      for(i = 0; i < audio_channels; i++){
	current->next_pad = NULL;
	
	/* iterate */
	current = current->next;
      }

      /* remove channel */
      current = ags_channel_nth(channel,
				audio_channels - 1);
      current->next = NULL;
    }
  }

  void ags_audio_set_pads_free_notation(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);
    
    list = 
      list_start = g_list_copy(audio->notation);
    
    while(list != NULL){
      ags_audio_remove_notation(audio,
				list->data);
      
      g_object_run_dispose((GObject *) list->data);
      g_object_unref((GObject *) list->data);
      
      list = list->next;
    }

    g_list_free(audio->notation);
    g_list_free(list_start);
    
    audio->notation = NULL;

    pthread_mutex_unlock(audio_mutex);
  }
  
  void ags_audio_set_pads_remove_notes(){
    GList *notation;
    GList *note_start, *note;

    pthread_mutex_lock(audio_mutex);

    notation = audio->notation;

    while(notation != NULL){
      note =
	note_start = g_list_copy(AGS_NOTATION(notation->data)->note);

      while(note != NULL){
	if(AGS_NOTE(note->data)->y >= pads){
	  AGS_NOTATION(notation->data)->note = g_list_remove(AGS_NOTATION(notation->data)->note,
							     note->data);
	  
	  g_object_unref(note->data);
	}

	note = note->next;
      }

      g_list_free(note_start);
      
      notation = notation->next;
    }

    pthread_mutex_unlock(audio_mutex);
  }

  void ags_audio_set_pads_shrink_automation(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list = 
      list_start = g_list_copy(audio->automation);
    
    while(list != NULL){
      if(AGS_AUTOMATION(list->data)->channel_type == channel_type &&
	 AGS_AUTOMATION(list->data)->line >= pads * audio_channels){
	ags_audio_remove_automation(audio,
				    list->data);
	
	g_object_run_dispose((GObject *) list->data);
	g_object_unref((GObject *) list->data);
      }
      
      list = list->next;
    }

    g_list_free(list_start);

    pthread_mutex_unlock(audio_mutex);
  }

  void ags_audio_set_pads_free_wave(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);

    list = 
      list_start = g_list_copy(audio->wave);
    
    while(list != NULL){
      ags_audio_remove_wave(audio,
			    list->data);
      
      g_object_run_dispose((GObject *) list->data);
      g_object_unref((GObject *) list->data);
      
      list = list->next;
    }

    g_list_free(audio->wave);
    g_list_free(list_start);
    
    audio->wave = NULL;

    pthread_mutex_unlock(audio_mutex);
  }
  
  void ags_audio_set_pads_free_midi(){
    GList *list_start, *list;

    pthread_mutex_lock(audio_mutex);
    
    list = 
      list_start = g_list_copy(audio->midi);
    
    while(list != NULL){
      ags_audio_remove_midi(audio,
			    list->data);

      g_object_run_dispose((GObject *) list->data);
      g_object_unref((GObject *) list->data);
      
      list = list->next;
    }

    g_list_free(audio->midi);
    g_list_free(list_start);
    
    audio->midi = NULL;

    pthread_mutex_unlock(audio_mutex);
  }

  if(!(g_type_is_a(channel_type,
		   AGS_TYPE_OUTPUT) ||
       g_type_is_a(channel_type,
		   AGS_TYPE_INPUT))){
    g_warning("unknown channel type");

    return;
  }
  
  /*
   * entry point
   */
  if(pads_old == pads){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  audio_channels = audio->audio_channels;

  output_soundcard = audio->output_soundcard;
  input_soundcard = audio->input_soundcard;
  
  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;
  format = audio->format;
  
  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);
  
  pthread_mutex_unlock(audio_mutex);

  /* get playback domain mutex */
  pthread_mutex_lock(ags_playback_domain_get_class_mutex());

  playback_domain_mutex = playback_domain->obj_mutex;
  
  pthread_mutex_unlock(ags_playback_domain_get_class_mutex());
  
  /* init boolean parameters */
  ags_audio_set_pads_init_parameters();

  if(g_type_is_a(channel_type,
		 AGS_TYPE_OUTPUT) &&
     (AGS_AUDIO_NO_OUTPUT & (audio_flags)) == 0){
    /* output */
    if(audio_channels != 0){
      /* grow or shrink */
      if(pads > output_pads){
	AgsChannel *current;

	guint i, j;

	/* grow channels */
	ags_audio_set_pads_grow();

	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	  
	current = audio->output;

	pthread_mutex_unlock(audio_mutex);

	/* alloc playback domain */
	current = ags_channel_pad_nth(current,
				      pads_old);

	for(j = pads_old; j < pads; j++){
	  for(i = 0; i < audio_channels; i++){
	    AgsPlayback *playback;
	    
	    /* get current mutex */
	    pthread_mutex_lock(ags_current_get_class_mutex());
	  
	    current_mutex = current->obj_mutex;
	  
	    pthread_mutex_unlock(ags_current_get_class_mutex());

	    /* playback */
	    pthread_mutex_lock(current_mutex);

	    playback = current->playback;

	    pthread_mutex_unlock(current_mutex);

	    /* append */
	    ags_playback_domain_add_playback(playback_domain,
					     playback, AGS_TYPE_OUTPUT);
	
	    /* iterate */
	    pthread_mutex_lock(current_mutex);

	    current = current->next;

	    pthread_mutex_unlock(current_mutex);
	  }
	}
      }else if(pads == 0){	
	GList *list, *list_start;
      
	ags_audio_set_pads_shrink_automation();      

	if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (audio->behaviour_flags)) != 0){
	  if((AGS_SOUND_ABILITY_NOTATION & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_notation();
	  }

	  if((AGS_SOUND_ABILITY_WAVE & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_wave();
	  }

	  if((AGS_SOUND_ABILITY_MIDI & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_midi();
	  }
	}
	
	pthread_mutex_lock(audio_mutex);

	channel = audio->output;

	pthread_mutex_unlock(audio_mutex);
      
	/* unlink and remove */
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink_zero(channel);
      
	pthread_mutex_lock(audio_mutex);
	
	audio->output = NULL;

	pthread_mutex_unlock(audio_mutex);

	/* remove playback */
	pthread_mutex_lock(playback_domain_mutex);

	list = 
	  list_start = g_list_copy(playback_domain->output_playback);

	pthread_mutex_unlock(playback_domain_mutex);

	while(list != NULL){
	  AgsPlayback *playback;

	  playback = list->data;

	  /* remove playback */
	  ags_playback_domain_remove_playback(playback_domain,
					      playback, AGS_TYPE_OUTPUT);

	  g_object_run_dispose(playback);
	  g_object_unref(playback);

	  /* iterate */
	  list = list->next;
	}

	g_list_free(list_start);
      }else if(pads < output_pads){
	GList *list, *list_start;

	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	
	channel = audio->output;

	pthread_mutex_unlock(audio_mutex);

	ags_audio_set_pads_shrink_automation();      
	ags_audio_set_pads_remove_notes();

	channel = ags_channel_pad_nth(channel,
				      pads);
      
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink(channel);

	/* remove playback */
	pthread_mutex_lock(playback_domain_mutex);
	  
	list = 
	  list_start = g_list_copy(playback_domain->output_playback);

	pthread_mutex_unlock(playback_domain_mutex);

	list = g_list_nth(list,
			  pads * audio_channels);
	  
	while(list != NULL){
	  AgsPlayback *playback;

	  playback = list->data;

	  /* remove playback */
	  ags_playback_domain_remove_playback(playback_domain,
					      playback, AGS_TYPE_OUTPUT);
	    
	  g_object_run_dispose(playback);
	  g_object_unref(playback);

	  /* iterate */
	  list = list->next;
	}

	g_list_free(list_start);
      }
    }
    
    /* apply new size */
    audio->output_pads = pads;
    audio->output_lines = pads * audio_channels;

    //    if((AGS_AUDIO_SYNC & audio_flags) != 0 && (AGS_AUDIO_ASYNC & audio_flags) == 0){
    //TODO:JK: fix me
    //      input_pads = pads;
    //      audio->input_lines = pads * audio_channels;
    //    }
  }else if(g_type_is_a(channel_type,
		       AGS_TYPE_INPUT) &&
	   (AGS_AUDIO_NO_INPUT & (audio_flags)) == 0){
    /* input */
    if(audio_channels != 0){
      /* grow or shrink */
      if(pads > pads_old){
	AgsChannel *current;

	guint i, j;

	/* grow channels */
	ags_audio_set_pads_grow();

	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	  
	current = audio->input;

	pthread_mutex_unlock(audio_mutex);

	/* alloc playback domain */
	current = ags_channel_pad_nth(current,
				      pads_old);

	for(j = pads_old; j < pads; j++){
	  for(i = 0; i < audio_channels; i++){
	    AgsPlayback *playback;
	    
	    /* get current mutex */
	    pthread_mutex_lock(ags_current_get_class_mutex());
	  
	    current_mutex = current->obj_mutex;
	  
	    pthread_mutex_unlock(ags_current_get_class_mutex());

	    /* playback */
	    pthread_mutex_lock(current_mutex);

	    playback = current->playback;

	    pthread_mutex_unlock(current_mutex);

	    /* append */
	    ags_playback_domain_add_playback(playback_domain,
					     playback, AGS_TYPE_INPUT);
	
	    /* iterate */
	    pthread_mutex_lock(current_mutex);

	    current = current->next;

	    pthread_mutex_unlock(current_mutex);
	  }
	}
      }else if(pads == 0){
	GList *list, *list_start;
	
	ags_audio_set_pads_shrink_automation();

	if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (audio->behaviour_flags)) != 0){
	  if((AGS_SOUND_ABILITY_NOTATION & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_notation();
	  }

	  if((AGS_SOUND_ABILITY_WAVE & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_wave();
	  }

	  if((AGS_SOUND_ABILITY_MIDI & (audio->ability_flags)) != 0){
	    ags_audio_set_pads_free_midi();
	  }
	}

	pthread_mutex_lock(audio_mutex);

	channel = audio->input;

	pthread_mutex_unlock(audio_mutex);

	/* shrink channels */
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink_zero(channel);
	
	pthread_mutex_lock(audio_mutex);

	audio->input = NULL;  

	pthread_mutex_unlock(audio_mutex);

	/* remove playback */
	pthread_mutex_lock(playback_domain_mutex);

	list = 
	  list_start = g_list_copy(playback_domain->input_playback);

	pthread_mutex_unlock(playback_domain_mutex);

	while(list != NULL){
	  AgsPlayback *playback;

	  playback = list->data;

	  /* remove playback */
	  ags_playback_domain_remove_playback(playback_domain,
					      playback, AGS_TYPE_INPUT);

	  g_object_run_dispose(playback);
	  g_object_unref(playback);

	  /* iterate */
	  list = list->next;
	}

	g_list_free(list_start);
      }else if(pads < pads_old){
	GList *list, *list_start;

	/* get some fields */
	pthread_mutex_lock(audio_mutex);

	channel = audio->input;

	pthread_mutex_unlock(audio_mutex);
	
	ags_audio_set_pads_shrink_automation();
      
	channel = ags_channel_pad_nth(channel,
				      pads);
	
	/* shrink channels */
	ags_audio_set_pads_unlink_all(channel);
	ags_audio_set_pads_shrink(channel);

	/* remove playback */
	pthread_mutex_lock(playback_domain_mutex);
	  
	list = 
	  list_start = g_list_copy(playback_domain->input_playback);

	pthread_mutex_unlock(playback_domain_mutex);

	list = g_list_nth(list,
			  pads * audio_channels);
	  
	while(list != NULL){
	  AgsPlayback *playback;

	  playback = list->data;

	  /* remove playback */
	  ags_playback_domain_remove_playback(playback_domain,
					      playback, AGS_TYPE_INPUT);
	    
	  g_object_run_dispose(playback);
	  g_object_unref(playback);

	  /* iterate */
	  list = list->next;
	}

	g_list_free(list_start);
      }
    }

    /* apply new allocation */
    audio->input_pads = pads;
    audio->input_lines = pads * audio_channels;
  }
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsAudio::set-pads");

    /* add message */
    message = ags_message_envelope_alloc(audio,
					 NULL,
					 doc);
    /* set parameter */
    message->n_params = 3;

    message->parameter_name = (gchar *) malloc(4 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    3);

    /* channel type */
    message->parameter_name[0] = "channel-type";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_ULONG);
    g_value_set_ulong(&(message->value[0]),
		      channel_type);

    /* pads */
    message->parameter_name[1] = "pads";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     pads);

    /* pads old */
    message->parameter_name[2] = "pads-old";
    
    g_value_init(&(message->value[2]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[2]),
		     pads_old);

    /* terminate string vector */
    message->parameter_name[3] = NULL;
    
    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
}

/**
 * ags_audio_set_pads:
 * @audio: the #AgsAudio
 * @channel_type: AGS_TYPE_INPUT or AGS_TYPE_OUTPUT
 * @pads: new pad count
 *
 * Set pad count for the apropriate @channel_type
 *
 * Since: 2.0.0
 */
void
ags_audio_set_pads(AgsAudio *audio,
		   GType channel_type,
		   guint pads, guint pads_old)
{
  pthread_mutex_t *audio_mutex;
  
  g_return_if_fail(AGS_IS_AUDIO(audio));

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* emit */
  pthread_mutex_lock(audio_mutex);

  pads_old = ((g_type_is_a(channel_type, AGS_TYPE_OUTPUT)) ? audio->output_pads: audio->input_pads);

  pthread_mutex_unlock(audio_mutex);

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		channel_type, pads, pads_old);
  g_object_unref((GObject *) audio);  
}

/**
 * ags_audio_set_output_soundcard:
 * @audio: the #AgsAudio
 * @soundcard: an #AgsSoundcard
 *
 * Set the output soundcard object of @audio.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_output_soundcard(AgsAudio *audio,
			       GObject *soundcard)
{
  AgsChannel *channel;
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *audio_thread;
  
  GObject *old_soundcard;
  
  GList *list;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint i;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *playback_domain_mutex;
  pthread_mutex_t *play_mutex, *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* old soundcard */
  pthread_mutex_lock(audio_mutex);

  old_soundcard = audio->output_soundcard;

  pthread_mutex_unlock(audio_mutex);
  
  if(old_soundcard == soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  pthread_mutex_lock(audio_mutex);
  
  audio->output_soundcard = (GObject *) soundcard;

  pthread_mutex_unlock(audio_mutex);

  if(soundcard != NULL){
    /* get presets */    
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      NULL,
			      &samplerate,
			      &buffer_size,
			      &format);

    /* apply presets */
    g_object_set(audio,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);
  }

  /* output */
  pthread_mutex_lock(audio_mutex);
  
  channel = audio->output;

  pthread_mutex_unlock(audio_mutex);

  while(channel != NULL){
    GObject *current_soundcard;

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
  
    channel_mutex = channel->obj_mutex;

    pthread_mutex_unlock(ags_channel_get_class_mutex());
  
    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    current_soundcard = channel->output_soundcard;

    pthread_mutex_unlock(channel_mutex);

    /* reset */
    if(current_soundcard == old_soundcard){
      g_object_set(G_OBJECT(channel),
		   "output-soundcard", soundcard,
		   NULL);
    }

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  /* input */
  pthread_mutex_lock(audio_mutex);
  
  channel = audio->input;

  pthread_mutex_unlock(audio_mutex);

  while(channel != NULL){
    GObject *current_soundcard;

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
  
    channel_mutex = channel->obj_mutex;

    pthread_mutex_unlock(ags_channel_get_class_mutex());
  
    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    current_soundcard = channel->output_soundcard;

    pthread_mutex_unlock(channel_mutex);

    /* reset */
    if(current_soundcard == old_soundcard){
      g_object_set(G_OBJECT(channel),
		   "output-soundcard", soundcard,
		   NULL);
    }

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  /* playback domain - audio thread */
  pthread_mutex_lock(audio_mutex);

  playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

  pthread_mutex_unlock(audio_mutex);

  /* get playback domain mutex */
  pthread_mutex_lock(ags_playback_domain_get_class_mutex());

  playback_domain_mutex = playback_domain->obj_mutex;
  
  pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

  /* audio thread - output soundcard */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    pthread_mutex_lock(playback_domain_mutex);
    
    audio_thread = playback_domain->audio_thread[i];

    pthread_mutex_unlock(playback_domain_mutex);
    
    if(audio_thread != NULL){
      /* set output soundcard */
      g_object_set(audio_thread,
		   "output-soundcard", soundcard,
		   NULL);
    }
  }
  
  /* get play mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  play_mutex = audio->play_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* play context */
  pthread_mutex_lock(play_mutex);
  
  list = audio->play;
  
  while(list != NULL){
    if(AGS_RECALL(list->data)->output_soundcard == old_soundcard){
      g_object_set(G_OBJECT(list->data),
		   "output-soundcard", soundcard,
		   NULL);
    }
    
    list = list->next;
  }
  
  pthread_mutex_unlock(play_mutex);

  /* get recall mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  recall_mutex = audio->recall_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* recall context */
  pthread_mutex_lock(recall_mutex);

  list = audio->recall;
  
  while(list != NULL){
    if(AGS_RECALL(list->data)->output_soundcard == old_soundcard){
      g_object_set(G_OBJECT(list->data),
		   "output-soundcard", soundcard,
		   NULL);
    }
    
    list = list->next;
  } 

  pthread_mutex_unlock(recall_mutex);

  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_audio_set_input_soundcard:
 * @audio: the #AgsAudio
 * @soundcard: an #AgsSoundcard
 *
 * Set the input soundcard object on audio.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_input_soundcard(AgsAudio *audio,
			      GObject *soundcard)
{
  AgsChannel *channel;

  GObject *old_soundcard;
  
  GList *list;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *play_mutex, *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* old soundcard */
  pthread_mutex_lock(audio_mutex);

  old_soundcard = audio->input_soundcard;

  pthread_mutex_unlock(audio_mutex);
  
  if(old_soundcard == soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  pthread_mutex_lock(audio_mutex);
  
  audio->input_soundcard = (GObject *) soundcard;

  pthread_mutex_unlock(audio_mutex);  

  /* output */
  pthread_mutex_lock(audio_mutex);
  
  channel = audio->output;

  pthread_mutex_unlock(audio_mutex);

  while(channel != NULL){
    GObject *current_soundcard;

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
  
    channel_mutex = channel->obj_mutex;

    pthread_mutex_unlock(ags_channel_get_class_mutex());
  
    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    current_soundcard = channel->input_soundcard;

    pthread_mutex_unlock(channel_mutex);

    /* reset */
    if(current_soundcard == old_soundcard){
      g_object_set(G_OBJECT(channel),
		   "input-soundcard", soundcard,
		   NULL);
    }

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  /* input */
  pthread_mutex_lock(audio_mutex);
  
  channel = audio->input;

  pthread_mutex_unlock(audio_mutex);

  while(channel != NULL){
    GObject *current_soundcard;

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
  
    channel_mutex = channel->obj_mutex;

    pthread_mutex_unlock(ags_channel_get_class_mutex());
  
    /* get some fields */
    pthread_mutex_lock(channel_mutex);
    
    current_soundcard = channel->input_soundcard;

    pthread_mutex_unlock(channel_mutex);

    /* reset */
    if(current_soundcard == old_soundcard){
      g_object_set(G_OBJECT(channel),
		   "input-soundcard", soundcard,
		   NULL);
    }

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }

  /* get play mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  play_mutex = audio->play_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* play context */
  pthread_mutex_lock(play_mutex);
  
  list = audio->play;
  
  while(list != NULL){
    if(AGS_RECALL(list->data)->input_soundcard == old_soundcard){
      g_object_set(G_OBJECT(list->data),
		   "input-soundcard", soundcard,
		   NULL);
    }
    
    list = list->next;
  }
  
  pthread_mutex_unlock(play_mutex);

  /* get recall mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  recall_mutex = audio->recall_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* recall context */
  pthread_mutex_lock(recall_mutex);

  list = audio->recall;
  
  while(list != NULL){
    if(AGS_RECALL(list->data)->input_soundcard == old_soundcard){
      g_object_set(G_OBJECT(list->data),
		   "input-soundcard", soundcard,
		   NULL);
    }
    
    list = list->next;
  } 

  pthread_mutex_unlock(recall_mutex);

  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_audio_set_output_sequencer:
 * @audio: the #AgsAudio
 * @sequencer: the output sequencer
 * 
 * Set output sequencer.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_output_sequencer(AgsAudio *audio,
			       GObject *sequencer)
{
  GObject *old_sequencer;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* old sequencer */
  pthread_mutex_lock(audio_mutex);

  old_sequencer = audio->output_sequencer;

  pthread_mutex_unlock(audio_mutex);
  
  if(old_sequencer == sequencer){
    return;
  }

  /* ref and set new sequencer */
  if(sequencer != NULL){
    g_object_ref(sequencer);
  }
  
  pthread_mutex_lock(audio_mutex);
  
  audio->output_sequencer = (GObject *) sequencer;

  pthread_mutex_unlock(audio_mutex);  

  /* unref old sequencer */
  if(old_sequencer != NULL){
    g_object_unref(old_sequencer);
  }
}

/**
 * ags_audio_set_input_sequencer:
 * @audio: the #AgsAudio
 * @sequencer: the input sequencer
 * 
 * Set input sequencer.
 * 
 * Since: 2.0.0
 */
void
ags_audio_set_input_sequencer(AgsAudio *audio,
			      GObject *sequencer)
{
  GObject *old_sequencer;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  /* old sequencer */
  pthread_mutex_lock(audio_mutex);

  old_sequencer = audio->input_sequencer;

  pthread_mutex_unlock(audio_mutex);
  
  if(old_sequencer == sequencer){
    return;
  }

  /* ref and set new sequencer */
  if(sequencer != NULL){
    g_object_ref(sequencer);
  }
  
  pthread_mutex_lock(audio_mutex);
  
  audio->input_sequencer = (GObject *) sequencer;

  pthread_mutex_unlock(audio_mutex);  

  /* unref old sequencer */
  if(old_sequencer != NULL){
    g_object_unref(old_sequencer);
  }
}

/**
 * ags_audio_set_samplerate:
 * @audio: the #AgsAudio
 * @samplerate: the samplerate
 *
 * Sets samplerate.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_samplerate(AgsAudio *audio, guint samplerate)
{
  AgsChannel *output, *input;
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *audio_thread;

  gdouble frequency;
  guint i;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *playback_domain_mutex;
  
  auto void ags_audio_set_samplerate_channel(AgsChannel *channel);

  void ags_audio_set_samplerate_channel(AgsChannel *channel){
    pthread_mutex_t *channel_mutex;
    
    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* set samplerate */
      g_object_set(channel,
		   "samplerate", samplerate,
		    NULL);

      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set samplerate */
  pthread_mutex_lock(audio_mutex);

  audio->samplerate = samplerate;

  frequency = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  output = audio->output;
  input = audio->input;

  playback_domain = audio->playback_domain;
  
  pthread_mutex_unlock(audio_mutex);

  if(playback_domain != NULL){
    /* get playback domain mutex */
    pthread_mutex_lock(ags_playback_domain_get_class_mutex());

    playback_domain_mutex = playback_domain->obj_mutex;
  
    pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

    /* audio thread - frequency */
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      pthread_mutex_lock(playback_domain_mutex);
    
      audio_thread = playback_domain->audio_thread[i];

      pthread_mutex_unlock(playback_domain_mutex);
    
      if(audio_thread != NULL){
	/* apply new frequency */
	g_object_set(audio_thread,
		     "frequency", frequency,
		     NULL);
      }
    }
  }
  
  /* set samplerate output/input */
  ags_audio_set_samplerate_channel(output);
  ags_audio_set_samplerate_channel(input);
}

/**
 * ags_audio_set_buffer_size:
 * @audio: the #AgsAudio
 * @buffer_size: the buffer length
 *
 * Sets buffer length.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_buffer_size(AgsAudio *audio, guint buffer_size)
{
  AgsChannel *output, *input;
  AgsPlaybackDomain *playback_domain;

  AgsThread *audio_thread;
  
  gdouble frequency;
  guint i;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *playback_domain_mutex;

  auto void ags_audio_set_buffer_size_channel(AgsChannel *channel);

  void ags_audio_set_buffer_size_channel(AgsChannel *channel){
    pthread_mutex_t *channel_mutex;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* set buffer size */
      g_object_set(channel,
		   "buffer-size", buffer_size,
		   NULL);
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set buffer size */
  pthread_mutex_lock(audio_mutex);

  audio->buffer_size = buffer_size;

  output = audio->output;
  input = audio->input;

  playback_domain = audio->playback_domain;

  frequency = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  
  pthread_mutex_unlock(audio_mutex);

  /* get playback domain mutex */
  pthread_mutex_lock(ags_playback_domain_get_class_mutex());

  playback_domain_mutex = playback_domain->obj_mutex;
  
  pthread_mutex_unlock(ags_playback_domain_get_class_mutex());

  /* audio thread - frequency */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    pthread_mutex_lock(playback_domain_mutex);
    
    audio_thread = playback_domain->audio_thread[i];

    pthread_mutex_unlock(playback_domain_mutex);

    if(audio_thread != NULL){
      /* apply new frequency */
      g_object_set(playback_domain->audio_thread[i],
		   "frequency", frequency,
		   NULL);
    }
  }  

  /* set buffer size output/input */
  ags_audio_set_buffer_size_channel(output);
  ags_audio_set_buffer_size_channel(input);
}

/**
 * ags_audio_set_format:
 * @audio: the #AgsAudio
 * @format: the format
 *
 * Sets buffer length.
 *
 * Since: 2.0.0
 */
void
ags_audio_set_format(AgsAudio *audio, guint format)
{
  AgsChannel *output, *input;
  
  pthread_mutex_t *audio_mutex;

  auto void ags_audio_set_format_channel(AgsChannel *channel);

  void ags_audio_set_format_channel(AgsChannel *channel){
    pthread_mutex_t *channel_mutex;

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());

      /* set format */
      g_object_set(channel,
		   "format", format,
		   NULL);
      
      /* iterate */
      pthread_mutex_lock(channel_mutex);
      
      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }
  }  

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* set format */
  pthread_mutex_lock(audio_mutex);

  audio->format = format;

  output = audio->output;
  input = audio->input;

  pthread_mutex_unlock(audio_mutex);

  /* set format output/input */
  ags_audio_set_format_channel(output);
  ags_audio_set_format_channel(input);
}

/**
 * ags_audio_add_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Adds an preset.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_preset(AgsAudio *audio,
		     GObject *preset)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_PRESET(preset)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add preset */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->preset,
		 preset) == NULL){
    g_object_ref(preset);
    audio->preset = g_list_prepend(audio->preset,
				   preset);

    g_object_set(preset,
		 "audio", audio,
		 NULL);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Removes an preset.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_preset(AgsAudio *audio,
			GObject *preset)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_PRESET(preset)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove preset */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->preset,
		 preset) != NULL){
    audio->preset = g_list_remove(audio->preset,
				  preset);

    g_object_set(preset,
		 "audio", NULL,
		 NULL);

    g_object_unref(preset);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 *
 * Adds a notation.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_notation(AgsAudio *audio, GObject *notation)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_NOTATION(notation)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add notation */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->notation,
		 notation) == NULL){
    g_object_ref(notation);
    audio->notation = ags_notation_add(audio->notation,
				       notation);

    g_object_set(notation,
		 "audio", audio,
		 NULL);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 *
 * Removes a notation.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_notation(AgsAudio *audio, GObject *notation)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_NOTATION(notation)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove notation */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->notation,
		 notation) != NULL){
    audio->notation = g_list_remove(audio->notation,
				    notation);

    g_object_set(notation,
		 "audio", NULL,
		 NULL);

    g_object_unref(notation);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Adds an automation.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_automation(AgsAudio *audio, GObject *automation)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recall id */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->automation,
		 automation) != NULL){
    g_object_ref(automation);
    audio->automation = ags_automation_add(audio->automation,
					   automation);

    g_object_set(automation,
		 "audio", audio,
		 NULL);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Removes an automation.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_automation(AgsAudio *audio, GObject *automation)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove automation */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->automation,
		 automation) != NULL){
    audio->automation = g_list_remove(audio->automation,
				      automation);

    g_object_set(automation,
		 "audio", NULL,
		 NULL);

    g_object_unref(automation);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_wave:
 * @audio: the #AgsAudio
 * @wave: the #AgsWave
 *
 * Adds a wave.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_wave(AgsAudio *audio, GObject *wave)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recall id */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->wave,
		 wave) != NULL){
    g_object_ref(wave);
    audio->wave = ags_wave_add(audio->wave,
			       wave);

    g_object_set(wave,
		 "audio", audio,
		 NULL);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_wave:
 * @audio: the #AgsAudio
 * @wave: the #AgsWave
 *
 * Removes a wave.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_wave(AgsAudio *audio, GObject *wave)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove wave */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->wave,
		 wave) != NULL){
    audio->wave = g_list_remove(audio->wave,
				wave);

    g_object_set(wave,
		 "audio", NULL,
		 NULL);

    g_object_unref(wave);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_midi:
 * @audio: the #AgsAudio
 * @midi: the #AgsMidi
 *
 * Adds a midi.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_midi(AgsAudio *audio, GObject *midi)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_MIDI(midi)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recall id */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->midi,
		 midi) != NULL){
    g_object_ref(midi);
    audio->midi = ags_midi_add(audio->midi,
			       midi);

    g_object_set(midi,
		 "audio", audio,
		 NULL);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_midi:
 * @audio: the #AgsAudio
 * @midi: the #AgsMidi
 *
 * Removes a midi.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_midi(AgsAudio *audio, GObject *midi)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_MIDI(midi)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove midi */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->midi,
		 midi) != NULL){
    audio->midi = g_list_remove(audio->midi,
				midi);

    g_object_set(midi,
		 "audio", NULL,
		 NULL);

    g_object_unref(midi);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recall id */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_id,
		 recall_id) == NULL){
    g_object_ref(recall_id);
    audio->recall_id = g_list_prepend(audio->recall_id,
				      recall_id);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove recall id */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_id,
		 recall_id) != NULL){
    audio->recall_id = g_list_remove(audio->recall_id,
				     recall_id);
    g_object_unref(recall_id);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Adds a recycling context.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recycling context */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->recycling_context,
		 recycling_context) == NULL){
    g_object_ref(recycling_context);
    audio->recycling_context = g_list_prepend(audio->recycling_context,
					      recycling_context);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Removes a recycling context.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove recycling container */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->recycling_context,
		 recycling_context) != NULL){
    audio->recycling_context = g_list_remove(audio->recycling_context,
					     recycling_context);
    g_object_unref(recycling_context);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* add recall container */
  pthread_mutex_lock(audio_mutex);
  
  if(g_list_find(audio->recall_container,
		 recall_container) == NULL){
    g_object_ref(recall_container);
    audio->recall_container = g_list_prepend(audio->recall_container,
					     recall_container);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container)
{
  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* remove recall container */
  pthread_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_container,
		 recall_container) != NULL){
    audio->recall_container = g_list_remove(audio->recall_container,
					    recall_container);
    g_object_unref(recall_container);
  }
  
  pthread_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play_context: if %TRUE play context, else if %FALSE recall context
 *
 * Adds a recall to @audio.
 *
 * Since: 2.0.0
 */
void
ags_audio_add_recall(AgsAudio *audio,
		     GObject *recall, gboolean play_context)
{  
  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL(recall)){
    return;
  }
  
  if(play_context){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* add recall */
    pthread_mutex_lock(play_mutex);
    
    if(g_list_find(audio->play, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      audio->play = g_list_prepend(audio->play,
				   recall);
            
      if(AGS_IS_RECALL_AUDIO(recall) ||
	 AGS_IS_RECALL_AUDIO_RUN(recall)){
	g_object_set(recall,
		     "audio", audio,
		     NULL);
      }
    }

    pthread_mutex_unlock(play_mutex);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* add recall */
    pthread_mutex_lock(recall_mutex);

    if(g_list_find(audio->recall, recall) == NULL){
      g_object_ref(G_OBJECT(recall));
    
      audio->recall = g_list_prepend(audio->recall,
				     recall);
            
      if(AGS_IS_RECALL_AUDIO(recall) ||
	 AGS_IS_RECALL_AUDIO_RUN(recall)){
	g_object_set(recall,
		     "audio", audio,
		     NULL);
      }
    }

    pthread_mutex_unlock(recall_mutex);
  }
}

/**
 * ags_audio_remove_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play_context: if %TRUE play context, else if %FALSE recall context
 *
 * Removes a recall from @audio.
 *
 * Since: 2.0.0
 */
void
ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play_context)
{
  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL(recall)){
    return;
  }
  
  if(play_context){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* add recall */
    pthread_mutex_lock(play_mutex);
    
    if(g_list_find(audio->play, recall) != NULL){
      audio->play = g_list_remove(audio->play,
				  recall);
            
      if(AGS_IS_RECALL_AUDIO(recall) ||
	 AGS_IS_RECALL_AUDIO_RUN(recall)){
	g_object_set(recall,
		     "audio", NULL,
		     NULL);
      }

      g_object_unref(G_OBJECT(recall));
    }

    pthread_mutex_unlock(play_mutex);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* add recall */
    pthread_mutex_lock(recall_mutex);

    if(g_list_find(audio->recall, recall) != NULL){
      audio->recall = g_list_remove(audio->recall,
				    recall);
            
      if(AGS_IS_RECALL_AUDIO(recall) ||
	 AGS_IS_RECALL_AUDIO_RUN(recall)){
	g_object_set(recall,
		     "audio", NULL,
		     NULL);
      }

      g_object_unref(G_OBJECT(recall));
    }

    pthread_mutex_unlock(recall_mutex);
  }
}

void
ags_audio_real_duplicate_recall(AgsAudio *audio,
				AgsRecallID *recall_id)
{
  AgsRecall *recall, *copy;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  gboolean play_context;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate list */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    play_context = FALSE;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* notify run */  
  //  ags_recall_notify_dependency(AGS_RECALL(list->data), AGS_RECALL_NOTIFY_RUN, 1);
  
  /* return if already played */
  pthread_mutex_lock(recall_id_mutex);

  if(ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_WAITING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_ACTIVE) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_PROCESSING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_TERMINATING)){
    pthread_mutex_unlock(recall_id_mutex);
    
    return;
  }

  ags_recall_id_set_state_flags(recall_id,
				AGS_SOUND_STATE_IS_WAITING);

  pthread_mutex_unlock(recall_id_mutex);
  
  /* duplicate */
  while(list != NULL){
    pthread_mutex_t *current_recall_mutex;
    
    recall = AGS_RECALL(list->data);

    /* get current recall mutex */
    pthread_mutex_lock(ags_recall_get_class_mutex());

    current_recall_mutex = recall->obj_mutex;
  
    pthread_mutex_unlock(ags_recall_get_class_mutex());

    /* some checks */
    pthread_mutex_lock(current_recall_mutex);
    
    if(AGS_IS_RECALL_AUDIO(recall) ||
       (AGS_RECALL_TEMPLATE & (recall->flags)) == 0 ||
       recall->recall_id != NULL ||
       !ags_recall_match_ability_flags_to_scope(recall,
						sound_scope)){
      list = list->next;

      pthread_mutex_unlock(current_recall_mutex);
      
      continue;
    }  

    pthread_mutex_unlock(current_recall_mutex);

    /* duplicate the recall */
    copy = ags_recall_duplicate(recall, recall_id,
				NULL, NULL, NULL);
      
    if(copy == NULL){
      /* iterate */    
      list = list->next;

      continue;
    }
    
#ifdef AGS_DEBUG
    g_message("recall duplicated: %s\n", G_OBJECT_TYPE_NAME(copy));
#endif

    /* set appropriate sound scope */
    copy->sound_scope = sound_scope;
      
    /* append to AgsAudio */
    ags_audio_add_recall(audio,
			 (GObject *) copy,
			 play_context);

    /* connect */
    ags_connectable_connect(AGS_CONNECTABLE(copy));

    /* notify run */
    ags_recall_notify_dependency(copy, AGS_RECALL_NOTIFY_RUN, 1);

    /* iterate */
    list = list->next;
  }

  g_list_free(list_start);
}


/**
 * ags_audio_duplicate_recall:
 * @audio: the #AgsAudio
 * @recall_id: an #AgsRecallID
 * 
 * Duplicate all #AgsRecall templates of @audio.
 *
 * Since: 2.0.0
 */
void
ags_audio_duplicate_recall(AgsAudio *audio,
			   AgsRecallID *recall_id)
{  
  g_return_val_if_fail(AGS_IS_AUDIO(audio), NULL);

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[DUPLICATE_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_resolve_recall(AgsAudio *audio,
			      AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);
  
  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* resolve */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    ags_recall_resolve_dependencies(recall);
    ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_audio_resolve_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to use
 *
 * Performs resolving of recalls.
 *
 * Since: 2.0.0
 */
void
ags_audio_resolve_recall(AgsAudio *audio,
			 AgsRecallID *recall_id)
{  
  g_return_val_if_fail(AGS_IS_AUDIO(audio), NULL);

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[RESOLVE_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_init_recall(AgsAudio *audio,
			   AgsRecallID *recall_id, guint staging_flags)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }
  
  /* init  */
  staging_flags = staging_mask & staging_flags;
  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* run init stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }
  
  g_list_free(list_start);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);
}

/**
 * ags_audio_init_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to use or #NULL
 * @staging_flags: staging flags, see #AgsSoundStagingFlags-enum
 *
 * Initializes the recalls of @audio
 *
 * Since: 2.0.0
 */
void
ags_audio_init_recall(AgsAudio *audio,
		      AgsRecallID *recall_id, guint staging_flags)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[INIT_RECALL], 0,
		recall_id, staging_flags);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_play_recall(AgsAudio *audio,
			   AgsRecallID *recall_id, guint staging_flags)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_mask = (AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST |
				     AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
				     AGS_SOUND_STAGING_AUTOMATE |
				     AGS_SOUND_STAGING_RUN_PRE |
				     AGS_SOUND_STAGING_RUN_INTER |
				     AGS_SOUND_STAGING_RUN_POST |
				     AGS_SOUND_STAGING_DO_FEEDBACK |
				     AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE |
				     AGS_SOUND_STAGING_FINI);

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;
  
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    return;
  }
  
  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* play  */
  staging_flags = staging_mask & staging_flags;
  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* play stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }
  
  g_list_free(list_start);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);
}

/**
 * ags_audio_play_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 * @staging_flags: staging flags, see #AgsSoundStagingFlags-enum
 *
 * Performs play for the specified @staging_flags.
 *
 * Since: 2.0.0
 */
void
ags_audio_play_recall(AgsAudio *audio,
		      AgsRecallID *recall_id, guint staging_flags)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[PLAY_RECALL], 0,
		recall_id, staging_flags);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_done_recall(AgsAudio *audio,
			   AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_flags = (AGS_SOUND_STAGING_DONE);  

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* done  */  
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* done stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }
  
  g_list_free(list_start);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);
}

/**
 * ags_audio_done_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Done processing audio data.
 *
 * Since: 2.0.0
 */
void
ags_audio_done_recall(AgsAudio *audio,
			AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[DONE_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_cancel_recall(AgsAudio *audio,
			     AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL);

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    return;
  }

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* cancel  */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* cancel stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);

    list = list->next;
  }

  g_list_free(list_start);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);
}

/**
 * ags_audio_cancel_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Cancel processing audio data.
 *
 * Since: 2.0.0
 */
void
ags_audio_cancel_recall(AgsAudio *audio,
			AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[CANCEL_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

void
ags_audio_real_cleanup_recall(AgsAudio *audio,
			     AgsRecallID *recall_id)
{
  AgsRecall *recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  gboolean play_context;
  gboolean remove_recycling_context;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_id_mutex;
  pthread_mutex_t *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());

  recall_id_mutex = recall_id->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_id_mutex);

  recycling_context = recall_id->recycling_context;

  sound_scope = recall_id->sound_scope;

  pthread_mutex_unlock(recall_id_mutex);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get staging flags */
  pthread_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  pthread_mutex_unlock(audio_mutex);

  /* get recycling context mutex */
  pthread_mutex_lock(ags_recycling_context_get_class_mutex());
  
  recycling_context_mutex = recycling_context->obj_mutex;
  
  pthread_mutex_unlock(ags_recycling_context_get_class_mutex());

  /* get parent recycling context */
  pthread_mutex_lock(recycling_context_mutex);

  parent_recycling_context = recycling_context->parent;

  remove_recycling_context = FALSE;
  
  if(recycling_context->recall_id == recall_id){
    remove_recycling_context = TRUE;
  }
  
  pthread_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    pthread_mutex_t *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    play_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy play context */
    pthread_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy(audio->play);

    pthread_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    pthread_mutex_t *recall_mutex;

    play_context = FALSE;
    
    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* copy recall context */
    pthread_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* cleanup  */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* remove recall */
    ags_audio_remove_recall(audio, recall, play_context);

    list = list->next;
  }

  g_list_free(list_start);

  /* remove recall id */
  ags_audio_remove_recall_id(audio, recall_id);

  /* remove recycling context */
  if(remove_recycling_context){
    ags_audio_remove_recycling_context(audio, recycling_context);
  }
}

/**
 * ags_audio_cleanup_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Cleanup processing audio data.
 *
 * Since: 2.0.0
 */
void
ags_audio_cleanup_recall(AgsAudio *audio,
			AgsRecallID *recall_id)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[CLEANUP_RECALL], 0,
		recall_id);
  g_object_unref((GObject *) audio);
}

GList*
ags_audio_real_start(AgsAudio *audio,
		     gint sound_scope)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsPlayback *playback;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context;
  
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GList *recall_id;

  guint audio_flags;
  guint audio_channels;
  guint output_pads;
  guint i;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *playback_mutex;

  if(sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  channel = audio->output;

  audio_flags = audio->flags;
  
  pthread_mutex_unlock(audio_mutex);

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) == 0){
    return(NULL);
  }
  
  /* initialize channel */
  recall_id = NULL;

  while(channel != NULL){
    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());

    channel_mutex = channel->obj_mutex;
  
    pthread_mutex_unlock(ags_channel_get_class_mutex());

    /* recycling context */
    current_recycling_context = ags_recycling_context_new(1);
    ags_audio_add_recycling_context(audio,
				    current_recycling_context);

    /* get recycling */
    pthread_mutex_lock(channel_mutex);

    recycling = channel->first_recycling;
    
    pthread_mutex_unlock(channel_mutex);

    /* set recycling */
    ags_recycling_context_replace(current_recycling_context,
				  recycling,
				  0);

    /* create recall id */
    current_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				     "recycling-context", current_recycling_context,
				     NULL);
    ags_recall_id_set_sound_scope(current_recall_id,  sound_scope);
    ags_audio_add_recall_id(audio,
			    current_recall_id);
    
    recall_id = g_list_prepend(recall_id,
			       current_recall_id);
        
    /* get playback */
    pthread_mutex_lock(channel_mutex);

    playback = channel->playback;
    
    pthread_mutex_unlock(channel_mutex);

    /* get playback mutex */
    pthread_mutex_lock(ags_playback_get_class_mutex());

    playback_mutex = playback->obj_mutex;
  
    pthread_mutex_unlock(ags_playback_get_class_mutex());
    
    /* set playback's recall id */
    pthread_mutex_lock(playback_mutex);
    
    playback->recall_id[sound_scope] = current_recall_id;

    pthread_mutex_unlock(playback_mutex);
    
    /* reset recall id */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, (AGS_SOUND_STAGING_CHECK_RT_DATA |
						  AGS_SOUND_STAGING_RUN_INIT_PRE |
						  AGS_SOUND_STAGING_RUN_INIT_INTER |
						  AGS_SOUND_STAGING_RUN_INIT_POST));

    /* iterate */
    pthread_mutex_lock(channel_mutex);
    
    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  recall_id = g_list_reverse(recall_id);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsAudio::start");

    /* add message */
    message = ags_message_envelope_alloc(audio,
					 NULL,
					 doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* recall id */
    message->parameter_name[0] = "recall-id";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[0]),
			recall_id);

    /* sound scope */
    message->parameter_name[1] = "sound-scope";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     sound_scope);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }

  return(recall_id);
}

/**
 * ags_audio_start:
 * @audio: the #AgsAudio
 * @sound_scope: the scope
 *
 * Is emitted as audio is started.
 *
 * Returns: the #GList-struct containing #AgsRecallID
 * 
 * Since: 2.0.0
 */
GList*
ags_audio_start(AgsAudio *audio,
		gint sound_scope)
{
  GList *list;
  
  g_return_val_if_fail(AGS_IS_AUDIO(audio), NULL);
  
  /* emit */
  list = NULL;
  
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[START], 0,
		sound_scope,
		&list);
  g_object_unref((GObject *) audio);

  return(list);
}

void
ags_audio_real_stop(AgsAudio *audio,
		    GList *recall_id, gint sound_scope)
{
  AgsChannel *channel;
  AgsPlayback *playback;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *playback_mutex;

  if(!AGS_IS_RECALL_ID(recall_id) ||
     sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }
  
  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  channel = audio->output;
  
  pthread_mutex_unlock(audio_mutex);
  
  while(channel != NULL){
    AgsRecallID *current_recall_id;
    
    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());

    channel_mutex = channel->obj_mutex;
  
    pthread_mutex_unlock(ags_channel_get_class_mutex());

    /* get current recall id */
    pthread_mutex_lock(channel_mutex);
    
    playback = channel->playback;

    pthread_mutex_unlock(channel_mutex);

    /* get playback mutex */
    pthread_mutex_lock(ags_playback_get_class_mutex());

    playback_mutex = playback->obj_mutex;
  
    pthread_mutex_unlock(ags_playback_get_class_mutex());

    /* recursive cancel */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, (AGS_SOUND_STAGING_CANCEL |
						  AGS_SOUND_STAGING_REMOVE));

    /* get/set recall id */
    pthread_mutex_lock(playback_mutex);
    
    current_recall_id = playback->recall_id[sound_scope];
    
    playback->recall_id[sound_scope] = NULL;

    pthread_mutex_unlock(playback_mutex);

    /* iterate */
    pthread_mutex_lock(channel_mutex);    
    
    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      "libags-audio");

  if(message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsAudio::stop");

    /* add message */
    message = ags_message_envelope_alloc(audio,
					 NULL,
					 doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* recall id */
    message->parameter_name[0] = "recall-id";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->value[0]),
		       recall_id);

    /* sound scope */
    message->parameter_name[1] = "sound-scope";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     sound_scope);

    /* terminate string vector */
    message->parameter_name[2] = NULL;

    /* add message */
    ags_message_delivery_add_message(message_delivery,
				     "libags-audio",
				     message);
  }
}

/**
 * ags_audio_stop:
 * @audio: the #AgsAudio
 * @recall_id: the current #AgsRecallID
 * @sound_scope: the scope
 *
 * Is emitted as playing audio is stopped.
 *
 * Since: 2.0.0
 */
void
ags_audio_stop(AgsAudio *audio,
	       GList *recall_id, gint sound_scope)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));
  
  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[STOP], 0,
		recall_id, sound_scope);
  g_object_unref((GObject *) audio);
}

GList*
ags_audio_real_check_scope(AgsAudio *audio, gint sound_scope)
{
  GList *list_start, *list;
  GList *recall_id;

  gint i;

  pthread_mutex_t *audio_mutex;

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get recall id */
  pthread_mutex_lock(audio_mutex);
  
  list = 
    list_start = g_list_copy(audio->recall_id);
  
  pthread_mutex_unlock(audio_mutex);

  /* iterate recall id */
  recall_id = NULL;

  if(sound_scope >= 0){
    while(list != NULL){
      /* check sound scope */
      if(ags_recall_id_check_sound_scope(list->data, sound_scope)){
	recall_id = g_list_prepend(recall_id,
				   list->data);
      }

      /* iterate */
      list = list->next;
    }
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      list = list_start;

      while(list != NULL){
	/* check sound scope */
	if(ags_recall_id_check_sound_scope(list->data, i)){
	  recall_id = g_list_prepend(recall_id,
				     list->data);
	}

	/* iterate */
	list = list->next;
      }
    }
  }

  g_list_free(list_start);

  /* reverse recall id */
  recall_id = g_list_reverse(recall_id);
  
  return(recall_id);
}

/**
 * ags_audio_check_scope:
 * @audio: the #AgsAudio
 * @sound_scope: the scope
 *
 * Check scope's recall id.
 * 
 * Returns: the scope's recall id of @audio
 * 
 * Since: 2.0.0
 */
GList*
ags_audio_check_scope(AgsAudio *audio, gint sound_scope)
{
  GList *recall_id;

  g_return_val_if_fail(AGS_IS_AUDIO(audio),
		       NULL);
  
  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[CHECK_SCOPE], 0,
		sound_scope,
		&recall_id);
  g_object_unref((GObject *) audio);

  return(recall_id);
}

/**
 * ags_audio_collect_all_audio_ports:
 * @audio: the #AgsAudio
 *
 * Retrieve all ports of #AgsAudio.
 *
 * Returns: a new #GList containing #AgsPort
 *
 * Since: 2.0.0
 */
GList*
ags_audio_collect_all_audio_ports(AgsAudio *audio)
{
  GList *recall_start, *recall;
  GList *list;

  pthread_mutex_t *recall_mutex, *play_mutex;
  pthread_mutex_t *mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  list = NULL;
 
  /* get play mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  play_mutex = audio->play_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* collect port of playing recall */
  pthread_mutex_lock(play_mutex);

  recall =
    recall_start = g_list_copy(audio->play);

  pthread_mutex_unlock(play_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    pthread_mutex_lock(ags_recall_get_class_mutex());

    mutex = current->obj_mutex;
  
    pthread_mutex_unlock(ags_recall_get_class_mutex());

    /* concat port */
    pthread_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy(current->port);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(current->port));
	}
      }
    }

    pthread_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }

  g_list_free(recall_start);
  
  /* get recall mutex */  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  recall_mutex = audio->recall_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* the same for true recall */
  pthread_mutex_lock(recall_mutex);

  recall =
    recall_start = g_list_copy(audio->recall);

  pthread_mutex_unlock(recall_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    pthread_mutex_lock(ags_recall_get_class_mutex());

    mutex = current->obj_mutex;
  
    pthread_mutex_unlock(ags_recall_get_class_mutex());

    /* concat port */
    pthread_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy(current->port);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy(current->port));
	}
      }
    }

    pthread_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }
   
  g_list_free(recall_start);

  /* reverse result */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_audio_collect_all_audio_ports_by_specifier_and_scope:
 * @audio: an #AgsAudio
 * @specifier: the port's name
 * @play_context: either %TRUE for play or %FALSE for recall
 *
 * Retrieve specified port of #AgsAudio
 *
 * Returns: a #GList-struct of #AgsPort if found, otherwise %NULL
 *
 * Since: 2.0.0
 */
GList*
ags_audio_collect_all_audio_ports_by_specifier_and_context(AgsAudio *audio,
							   gchar *specifier,
							   gboolean play_context)
{
  GList *recall_start, *recall;
  GList *port_start, *port;
  GList *list;
  
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }
  
  if(play_context){
    /* get play mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());
    
    recall_mutex = audio->play_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* get recall */
    pthread_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy(audio->play);
    
    pthread_mutex_unlock(recall_mutex);
  }else{
    /* get recall mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    recall_mutex = audio->recall_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* get recall */
    pthread_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy(audio->recall);
    
    pthread_mutex_unlock(recall_mutex);
  }
  
  /* collect port of playing recall */
  list = NULL;

  while(recall != NULL){
    AgsRecall *current;

    pthread_mutex_t *mutex;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    pthread_mutex_lock(ags_recall_get_class_mutex());

    mutex = current->obj_mutex;
  
    pthread_mutex_unlock(ags_recall_get_class_mutex());

    /* get port */
    pthread_mutex_lock(mutex);
    
    port =
      port_start = g_list_copy(current->port);

    pthread_mutex_unlock(mutex);

    /* check specifier */
    while((port = ags_port_find_specifier(port, specifier)) != NULL){
      AgsPort *current;

      current = AGS_PORT(port->data);
      list = g_list_prepend(list,
			    current);

      /* iterate - port */
      port = port->next;
    }

    g_list_free(port_start);

    /* iterate - recall */
    recall = recall->next;
  }

  g_list_free(recall_start);

  /* reverse result */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_audio_open_audio_file_as_channel:
 * @audio: the #AgsAudio
 * @filename: the files to open
 * @overwrite_channels: if existing channels should be assigned
 * @create_channels: if new channels should be created as not fitting if combined with @overwrite_channels
 *
 * Open some files.
 *
 * Since: 2.0.0
 */
void
ags_audio_open_audio_file_as_channel(AgsAudio *audio,
				     GSList *filename,
				     gboolean overwrite_channels,
				     gboolean create_channels)
{
  AgsChannel *channel;
  AgsAudioFile *audio_file;

  GObject *soundcard;
  
  GList *audio_signal;

  guint input_pads;
  guint audio_channels;
  guint i, j;
  guint list_length;

  GError *error;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     filename == NULL ||
     (!overwrite_channels &&
      !create_channels)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio fields */
  pthread_mutex_lock(audio_mutex);

  channel = audio->input;
  soundcard = audio->output_soundcard;
  
  input_pads = audio->input_pads;
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);
  
  /* overwriting existing channels */
  if(overwrite_channels){
    if(channel != NULL){
      for(i = 0; i < input_pads && filename != NULL; i++){
	audio_file = ags_audio_file_new((gchar *) filename->data,
					soundcard,
					-1);
	
	if(!ags_audio_file_open(audio_file)){
	  filename = filename->next;
	  
	  continue;
	}

	ags_audio_file_read_audio_signal(audio_file);
	ags_audio_file_close(audio_file);
	
	audio_signal = audio_file->audio_signal;
	
	for(j = 0; j < audio_channels && audio_signal != NULL; j++){
	  AgsRecycling *recycling;
	  
	  /* create task */
	  error = NULL;

	  ags_channel_set_link(channel, NULL,
			       &error);

	  if(error != NULL){
	    g_warning("%s", error->message);
	  }

	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	  channel_mutex = channel->obj_mutex;
	  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* get recycling */
	  pthread_mutex_lock(channel_mutex);

	  recycling = channel->first_recycling;
	  	  
	  pthread_mutex_unlock(channel_mutex);

	  /* get recycling mutex */
	  pthread_mutex_lock(ags_recycling_get_class_mutex());
	  
	  recycling_mutex = recycling->obj_mutex;
	  
	  pthread_mutex_unlock(ags_recycling_get_class_mutex());

	  /* replace template audio signal */
	  pthread_mutex_lock(recycling_mutex);
	  
	  AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(audio_signal->data)->recycling = (GObject *) recycling;

	  ags_recycling_add_audio_signal(recycling,
					 audio_signal->data);

	  pthread_mutex_unlock(recycling_mutex);

	  /* iterate */
	  audio_signal = audio_signal->next;

	  pthread_mutex_lock(channel_mutex);

	  channel = channel->next;

	  pthread_mutex_unlock(channel_mutex);
	}

	if(audio_file->file_audio_channels < audio_channels){
	  channel = ags_channel_nth(channel,
				    audio_channels - audio_file->file_audio_channels);
	}
	
	filename = filename->next;
      }
    }
  }

  /* appending to channels */
  if(create_channels && filename != NULL){
    list_length = g_slist_length(filename);
    
    ags_audio_set_pads((AgsAudio *) audio, AGS_TYPE_INPUT,
		       list_length + AGS_AUDIO(audio)->input_pads, 0);
    
    channel = ags_channel_nth(AGS_AUDIO(audio)->input,
			      (AGS_AUDIO(audio)->input_pads - list_length) * AGS_AUDIO(audio)->audio_channels);
    
    while(filename != NULL){
      audio_file = ags_audio_file_new((gchar *) filename->data,
				      soundcard,
				      -1);
      
      if(!ags_audio_file_open(audio_file)){
	filename = filename->next;
	continue;
      }
      
      ags_audio_file_read_audio_signal(audio_file);
      ags_audio_file_close(audio_file);
	
      audio_signal = audio_file->audio_signal;
      
      for(j = 0; j < audio_channels && audio_signal != NULL; j++){
	AgsRecycling *recycling;
	
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());
	  
	channel_mutex = channel->obj_mutex;
	  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* get recycling */
	pthread_mutex_lock(channel_mutex);

	recycling = channel->first_recycling;
	
	pthread_mutex_unlock(channel_mutex);

	/* get recycling mutex */
	pthread_mutex_lock(ags_recycling_get_class_mutex());
	
	recycling_mutex = recycling->obj_mutex;
	
	pthread_mutex_unlock(ags_recycling_get_class_mutex());

	/* replace template audio signal */
	pthread_mutex_lock(recycling_mutex);
	  
	AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal->data)->recycling = (GObject *) recycling;
	
	ags_recycling_add_audio_signal(recycling,
				       audio_signal->data);
	
	pthread_mutex_unlock(recycling_mutex);

	/* iterate */
	audio_signal = audio_signal->next;

	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }
      
      if(audio_channels > audio_file->file_audio_channels){
	channel = ags_channel_nth(channel,
				  audio_channels - audio_file->file_audio_channels);
      }
      
      filename = filename->next;
    }
  }
}

void
ags_audio_open_audio_file_as_wave(AgsAudio *audio,
				  const gchar *filename,
				  gboolean overwrite_channels,
				  gboolean create_channels)
{
  AgsChannel *channel;
  AgsAudioFile *audio_file;

  GObject *soundcard;
  
  GList *wave;

  guint audio_channels;

  GError *error;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     filename == NULL ||
     (!overwrite_channels &&
      !create_channels)){
    return;
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio fields */
  pthread_mutex_lock(audio_mutex);

  audio_channels = audio->audio_channels;

  soundcard = audio->output_soundcard;

  pthread_mutex_unlock(audio_mutex);

  /* open audio file */
  audio_file = ags_audio_file_new(filename,
				  soundcard,
				  -1);

  if(!ags_audio_file_open(audio_file)){
    return;
  }

  ags_audio_file_read_wave(audio_file,
			   0,
			   0.0, 0);
  ags_audio_file_close(audio_file);

  //TODO:JK: implement me
}

void
ags_audio_open_midi_file_as_midi(AgsAudio *audio,
				 const gchar *filename,
				 const gchar *instrument,
				 const gchar *track_name,
				 guint midi_channel)
{
  //TODO:JK: implement me
}

void
ags_audio_open_midi_file_as_notation(AgsAudio *audio,
				     const gchar *filename,
				     const gchar *instrument,
				     const gchar *track_name,
				     guint midi_channel)
{
  //TODO:JK: implement me
}

/**
 * ags_audio_recursive_set_property:
 * @audio: the #AgsAudio
 * @n_params: the count of elements in following arrays
 * @parameter_name: the parameter's name array
 * @value: the value array
 *
 * Recursive set property for #AgsAudio.
 *
 * Since: 2.0.0
 */
void
ags_audio_recursive_set_property(AgsAudio *audio,
				 gint n_params,
				 const gchar *parameter_name[], const GValue value[])
{
  AgsChannel *channel;
  
  auto void ags_audio_set_property(AgsAudio *audio,
				   gint n_params,
				   const gchar *parameter_name[], const GValue value[]);
  auto void ags_audio_recursive_set_property_down(AgsChannel *channel,
						  gint n_params,
						  const gchar *parameter_name[], const GValue value[]);
  auto void ags_audio_recursive_set_property_down_input(AgsChannel *channel,
							gint n_params,
							const gchar *parameter_name[], const GValue value[]);

  void ags_audio_set_property(AgsAudio *audio,
			      gint n_params,
			      const gchar *parameter_name[], const GValue value[]){
    guint i;

    for(i = 0; i < n_params; i++){
      g_object_set_property(G_OBJECT(audio),
			    parameter_name[i], &(value[i]));
    }
  }
  
  void ags_audio_recursive_set_property_down(AgsChannel *channel,
					     gint n_params,
					     const gchar *parameter_name[], const GValue value[]){
    if(channel == NULL){
      return;
    }

    ags_audio_set_property(AGS_AUDIO(channel->audio),
			   n_params,
			   parameter_name, value);
    
    ags_audio_recursive_set_property_down_input(channel,
						n_params,
						parameter_name, value);
  }
    
  void ags_audio_recursive_set_property_down_input(AgsChannel *channel,
						   gint n_params,
						   const gchar *parameter_name[], const GValue value[]){
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
      ags_audio_recursive_set_property_down(input->link,
					    n_params,
					    parameter_name, value);

      input = input->next;
    }
  }

  if(audio == NULL){
    return;
  }

  ags_audio_set_property(audio,
			 n_params,
			 parameter_name, value);

  if(audio->input != NULL){
    channel = audio->input;
    
    while(channel != NULL){
      ags_audio_recursive_set_property_down(channel->link,
					    n_params,
					    parameter_name, value);

      channel = channel->next;
    }
  }
}

void
ags_audio_real_recursive_run_stage(AgsAudio *audio,
				   gint sound_scope, guint staging_flags)
{
  AgsChannel *channel;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get channel */
  pthread_mutex_lock(audio_mutex);

  channel = audio->output;

  pthread_mutex_unlock(audio_mutex);

  /* initialize return value */
  while(channel != NULL){
    /* reset recursive channel stage */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, staging_flags);

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());
      
    channel_mutex = channel->obj_mutex;
  
    pthread_mutex_unlock(ags_channel_get_class_mutex());

    /* iterate */
    pthread_mutex_lock(channel_mutex);

    channel = channel->next;

    pthread_mutex_unlock(channel_mutex);
  }
}

/**
 * ags_audio_recursive_run_stage:
 * @audio: the #AgsAudio object
 * @sound_scope: the scope to reset
 * @staging_flags: the stage to enable
 *
 * Resets @audio's @sound_scope specified by @staging_flags.
 *
 * Since: 2.0.0
 */
void
ags_audio_recursive_run_stage(AgsAudio *audio,
			      gint sound_scope, guint staging_flags)
{  
  g_return_if_fail(AGS_IS_AUDIO(audio));
  
  /* emit */  
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[RECURSIVE_RUN_STAGE], 0,
		sound_scope, staging_flags);
  g_object_unref((GObject *) audio);
}

/**
 * ags_audio_new:
 * @output_soundcard: the #AgsSoundcard to use for output
 *
 * Creates an #AgsAudio, with defaults of @soundcard.
 *
 * Returns: a new #AgsAudio
 *
 * Since: 2.0.0
 */
AgsAudio*
ags_audio_new(GObject *output_soundcard)
{
  AgsAudio *audio;

  audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
				    "output-soundcard", output_soundcard,
				    NULL);

  return(audio);
}
