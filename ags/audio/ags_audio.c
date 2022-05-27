/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_preset.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_automation.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_midi.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_sf2_synth_generator.h>
#include <ags/audio/ags_sfz_synth_generator.h>
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
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/task/ags_cancel_audio.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/midi/ags_midi_file.h>

#include <ags/audio/fx/ags_fx_playback_audio_processor.h>
#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_audio_processor.h>

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

void ags_audio_set_ability_flags_channel(AgsChannel *start_channel, guint ability_flags);
void ags_audio_unset_ability_flags_channel(AgsChannel *start_channel, guint ability_flags);

void ags_audio_set_audio_channels_grow(AgsAudio *audio,
				       GType channel_type,
				       guint audio_channels, guint audio_channels_old,
				       guint bank_dim_0, guint bank_dim_1, guint bank_dim_2,
				       gboolean add_recycling, gboolean add_pattern, gboolean add_synth_generator,
				       gboolean link_recycling,
				       gboolean set_sync_link, gboolean set_async_link);

void ags_audio_set_audio_channels_shrink_zero(AgsAudio *audio);
void ags_audio_set_audio_channels_shrink(AgsAudio *audio,
					 guint audio_channels, guint audio_channels_old);
void ags_audio_set_audio_channels_shrink_notation(AgsAudio *audio,
						  guint audio_channels, guint audio_channels_old);
void ags_audio_set_audio_channels_shrink_automation(AgsAudio *audio,
						    guint audio_channels, guint audio_channels_old);
void ags_audio_set_audio_channels_shrink_wave(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old);
void ags_audio_set_audio_channels_shrink_midi(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old);
      
void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);

void ags_audio_set_pads_grow(AgsAudio *audio,
			     GType channel_type,
			     guint pads, guint pads_old,
			     guint bank_dim_0, guint bank_dim_1, guint bank_dim_2,
			     gboolean add_recycling, gboolean add_pattern, gboolean add_synth_generator,
			     gboolean link_recycling,
			     gboolean set_sync_link, gboolean set_async_link);
void ags_audio_set_pads_unlink(AgsAudio *audio,
			       GType channel_type,
			       guint pads);
void ags_audio_set_pads_shrink_zero(AgsAudio *audio,
				    GType channel_type,
				    guint pads);
void ags_audio_set_pads_shrink(AgsAudio *audio,
			       GType channel_type,
			       guint pads);
void ags_audio_set_pads_remove_notes(AgsAudio *audio,
				     GType channel_type,
				     guint pads);
void ags_audio_set_pads_shrink_automation(AgsAudio *audio,
					  GType channel_type,
					  guint pads);
void ags_audio_set_pads_shrink_wave(AgsAudio *audio,
				    GType channel_type,
				    guint pads);
void ags_audio_set_pads_shrink_midi(AgsAudio *audio,
				    GType channel_type,
				    guint pads);

void ags_audio_real_set_pads(AgsAudio *audio,
			     GType channel_type,
			     guint channels, guint channels_old);

void ags_audio_real_set_output_soundcard(AgsAudio *audio, GObject *output_soundcard);

void ags_audio_real_set_input_soundcard(AgsAudio *audio, GObject *input_soundcard);

void ags_audio_real_set_output_sequencer(AgsAudio *audio, GObject *output_sequencer);

void ags_audio_real_set_input_sequencer(AgsAudio *audio, GObject *input_sequencer);

void ags_audio_set_samplerate_channel(AgsChannel *start_channel, guint samplerate);
void ags_audio_real_set_samplerate(AgsAudio *audio, guint samplerate);

void ags_audio_set_buffer_size_channel(AgsChannel *start_channel, guint buffer_size);
void ags_audio_real_set_buffer_size(AgsAudio *audio, guint buffer_size);

void ags_audio_set_format_channel(AgsChannel *start_channel, guint format);
void ags_audio_real_set_format(AgsAudio *audio, guint format);

void ags_audio_real_duplicate_recall(AgsAudio *audio,
				     AgsRecallID *recall_id,
				     guint pad, guint audio_channel,
				     guint line);
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

void ags_audio_recall_done_callback(AgsRecall *recall,
				    AgsAudio *audio);

GList* ags_audio_real_start(AgsAudio *audio,
			    gint sound_scope);
void ags_audio_real_stop(AgsAudio *audio,
			 GList *recall_id, gint sound_scope);

GList* ags_audio_real_check_scope(AgsAudio *audio, gint sound_scope);

void ags_audio_set_property_all(AgsAudio *audio,
				gint n_params,
				const gchar *parameter_name[], const GValue value[]);
void ags_audio_recursive_set_property_down(AgsChannel *channel,
					   gint n_params,
					   const gchar *parameter_name[], const GValue value[]);
void ags_audio_recursive_set_property_down_input(AgsChannel *channel,
						 gint n_params,
						 const gchar *parameter_name[], const GValue value[]);

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
  PROP_AUDIO_NAME,
  PROP_OUTPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD,
  PROP_OUTPUT_SEQUENCER,
  PROP_INPUT_SEQUENCER,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_BPM,
  PROP_MIN_AUDIO_CHANNELS,
  PROP_MAX_AUDIO_CHANNELS,
  PROP_MIN_OUTPUT_PADS,
  PROP_MAX_OUTPUT_PADS,
  PROP_MIN_INPUT_PADS,
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
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_OFFSET,
  PROP_OUTPUT,
  PROP_INPUT,
  PROP_PRESET,
  PROP_SYNTH_GENERATOR,
  PROP_SF2_SYNTH_GENERATOR,
  PROP_SFZ_SYNTH_GENERATOR,
  PROP_PLAYBACK_DOMAIN,
  PROP_CURSOR,
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

GType
ags_audio_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio);
  }

  return g_define_type_id__volatile;
}

GType
ags_audio_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_AUDIO_NO_OUTPUT, "AGS_AUDIO_NO_OUTPUT", "audio-no-output" },
      { AGS_AUDIO_NO_INPUT, "AGS_AUDIO_NO_INPUT", "audio-no-input" },
      { AGS_AUDIO_SYNC, "AGS_AUDIO_SYNC", "audio-sync" },
      { AGS_AUDIO_ASYNC, "AGS_AUDIO_ASYNC", "audio-async" },
      { AGS_AUDIO_OUTPUT_HAS_RECYCLING, "AGS_AUDIO_OUTPUT_HAS_RECYCLING", "audio-output-has-recycling" },
      { AGS_AUDIO_OUTPUT_HAS_SYNTH, "AGS_AUDIO_OUTPUT_HAS_SYNTH", "audio-output-has-synth" },
      { AGS_AUDIO_INPUT_HAS_RECYCLING, "AGS_AUDIO_INPUT_HAS_RECYCLING", "audio-input-has-recycling" },
      { AGS_AUDIO_INPUT_HAS_SYNTH, "AGS_AUDIO_INPUT_HAS_SYNTH", "audio-input-has-synth" },
      { AGS_AUDIO_INPUT_HAS_FILE, "AGS_AUDIO_INPUT_HAS_FILE", "audio-input-has-file" },
      { AGS_AUDIO_CAN_NEXT_ACTIVE, "AGS_AUDIO_CAN_NEXT_ACTIVE", "audio-can-next-active" },
      { AGS_AUDIO_SKIP_OUTPUT, "AGS_AUDIO_SKIP_OUTPUT", "audio-skip-output" },
      { AGS_AUDIO_SKIP_INPUT, "AGS_AUDIO_SKIP_INPUT", "audio-skip-input" },
      { AGS_AUDIO_BYPASS, "AGS_AUDIO_BYPASS", "audio-bypass" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAudioFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * AgsAudio:audio-name:
   *
   * The name of audio object.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("audio-name",
				   i18n_pspec("assigned name"),
				   i18n_pspec("The name of audio"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_NAME,
				  param_spec);

  /**
   * AgsAudio:output-soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsAudio:bpm:
   *
   * The bpm.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("bpm"),
				   i18n_pspec("The bpm"),
				   0.0,
				   G_MAXDOUBLE,
				   AGS_SOUNDCARD_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsAudio:min-audio-channels:
   *
   * The minimum audio channels count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("min-audio-channels",
				 i18n_pspec("minimum audio channels count"),
				 i18n_pspec("The minimum count of audio channels of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIN_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsAudio:max-audio-channels:
   *
   * The maximum audio channels count.
   * 
   * Since: 3.0.0
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
   * AgsAudio:min-output-pads:
   *
   * The minimum output pads count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("min-output-pads",
				 i18n_pspec("minimum output pads count"),
				 i18n_pspec("The minimum count of output pads of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIN_OUTPUT_PADS,
				  param_spec);

  /**
   * AgsAudio:max-output-pads:
   *
   * The maximum output pads count.
   * 
   * Since: 3.0.0
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
   * AgsAudio:min-input-pads:
   *
   * The minimum input pads count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("min-input-pads",
				 i18n_pspec("minimum input pads count"),
				 i18n_pspec("The minimum count of input pads of audio"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIN_INPUT_PADS,
				  param_spec);

  /**
   * AgsAudio:max-input-pads:
   *
   * The maximum input pads count.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsAudio:loop-start:
   *
   * The audio's loop start.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint64("loop-start",
				    i18n_pspec("loop start of audio"),
				    i18n_pspec("The loop start of audio"),
				    0.0,
				    G_MAXUINT64,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsAudio:loop-end:
   *
   * The audio's loop end.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint64("loop-end",
				    i18n_pspec("loop end of audio"),
				    i18n_pspec("The loop end of audio"),
				    0.0,
				    G_MAXUINT64,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsAudio:offset:
   *
   * The audio's offset.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint64("offset",
				    i18n_pspec("offset of audio"),
				    i18n_pspec("The offset of audio"),
				    0.0,
				    G_MAXUINT64,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OFFSET,
				  param_spec);

  /**
   * AgsAudio:output:
   *
   * The #AgsOutput it contains.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsAudio:preset: (type GList(AgsPreset)) (transfer full)
   *
   * The assigned #GList-struct containing #AgsPreset information.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("preset",
				    i18n_pspec("preset"),
				    i18n_pspec("The preset"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsAudio:synth-generator: (type GList(AgsSynthGenerator)) (transfer full)
   *
   * The assigned #GList-struct containing #AgsSynthGenerator information.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("synth-generator",
				    i18n_pspec("synth generator"),
				    i18n_pspec("The synth generator"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_GENERATOR,
				  param_spec);

  /**
   * AgsAudio:sf2-synth-generator: (type GList(AgsSF2SynthGenerator)) (transfer full)
   *
   * The assigned #GList-struct containing #AgsSF2SynthGenerator information.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_pointer("sf2-synth-generator",
				    i18n_pspec("SF2 synth generator"),
				    i18n_pspec("The SF2 synth generator"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SF2_SYNTH_GENERATOR,
				  param_spec);

  /**
   * AgsAudio:sfz-synth-generator: (type GList(AgsSFZSynthGenerator)) (transfer full)
   *
   * The assigned #GList-struct containing #AgsSFZSynthGenerator information.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_pointer("sfz-synth-generator",
				    i18n_pspec("SFZ synth generator"),
				    i18n_pspec("The SFZ synth generator"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SFZ_SYNTH_GENERATOR,
				  param_spec);
  
  /**
   * AgsAudio:playback-domain:
   *
   * The assigned #AgsPlaybackDomain.
   * 
   * Since: 3.0.0
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
   * AgsAudio:cursor: (type GList(GObject)) (transfer full)
   *
   * The #GObject implementing #AgsCursor interface.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("cursor",
				    i18n_pspec("cursor"),
				    i18n_pspec("The cursor object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CURSOR,
				  param_spec);

  /**
   * AgsAudio:notation: (type GList(AgsNotation)) (transfer full)
   *
   * The #AgsNotation it contains.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("notation",
				    i18n_pspec("containing notation"),
				    i18n_pspec("The notation it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /**
   * AgsAudio:automation: (type GList(AgsAutomation)) (transfer full)
   *
   * The #AgsAutomation it contains.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("automation",
				    i18n_pspec("containing automation"),
				    i18n_pspec("The automation it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION,
				  param_spec);
  
  /**
   * AgsAudio:wave: (type GList(AgsWave)) (transfer full)
   *
   * The #AgsWave it contains.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsAudio:midi: (type GList(AgsMidi)) (transfer full)
   *
   * The #AgsMidi it contains.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsAudio:recall-id: (type GList(AgsRecallID)) (transfer full)
   *
   * The assigned #AgsRecallID.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall-id",
				    i18n_pspec("assigned recall id"),
				    i18n_pspec("The assigned recall id"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsAudio:recycling-context: (type GList(AgsRecyclingContext)) (transfer full)
   *
   * The assigned #AgsRecyclingContext.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recycling-context",
				    i18n_pspec("assigned recycling context"),
				    i18n_pspec("The assigned recall id"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING_CONTEXT,
				  param_spec);

  /**
   * AgsAudio:recall-container: (type GList(AgsRecallContainer)) (transfer full)
   *
   * The #AgsRecallContainer it contains in container-context.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall-container",
				    i18n_pspec("containing recall container"),
				    i18n_pspec("The recall container it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsAudio:play: (type GList(AgsRecall)) (transfer full)
   *
   * The #AgsRecall it contains in play-context.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("play",
				    i18n_pspec("containing play"),
				    i18n_pspec("The play it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY,
				  param_spec);

  /**
   * AgsAudio:recall: (type GList(AgsRecall)) (transfer full)
   *
   * The #AgsRecall it contains in recall-context.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
   */
  audio_signals[SET_PADS] = 
    g_signal_new("set-pads",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_pads),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__POINTER_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsAudio::duplicate-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   * @pad: the pad
   * @audio_channel: the audio channel
   * @line: the line
   *
   * The ::duplicate-recall signal notifies about duplicated recalls.
   *
   * Since: 3.0.0
   */
  audio_signals[DUPLICATE_RECALL] = 
    g_signal_new("duplicate-recall",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, duplicate_recall),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__OBJECT_UINT_UINT_UINT,
		 G_TYPE_NONE, 4,
		 G_TYPE_OBJECT,
		 G_TYPE_UINT, G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsAudio::resolve-recall:
   * @audio: the #AgsAudio
   * @recall_id: the #AgsRecallID
   *
   * The ::resolve-recall signal notifies about resolved recalls.
   *
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_init(AgsAudio *audio)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *str0, *str1;

  audio->flags = 0;
  audio->connectable_flags = 0;
  audio->ability_flags = 0;
  audio->behaviour_flags = 0;
  memset(audio->staging_flags, 0, AGS_SOUND_SCOPE_LAST * sizeof(guint));

  memset(audio->staging_completed, FALSE, AGS_SOUND_SCOPE_LAST * sizeof(gboolean));

  /* audio mutex */
  g_rec_mutex_init(&(audio->obj_mutex));

  /* uuid */
  audio->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio->uuid);
  
  audio->audio_name = NULL;

  /* config */
  config = ags_config_get_instance();
  
  /* base init */
  audio->output_soundcard = NULL;
  audio->output_soundcard_channel_map = NULL;

  audio->input_soundcard = NULL;
  audio->input_soundcard_channel_map = NULL;

  audio->output_sequencer = NULL;
  audio->input_sequencer = NULL;
  
  audio->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio->format = ags_soundcard_helper_config_get_format(config);

  audio->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* bank */
  audio->bank_dim[0] = 0;
  audio->bank_dim[1] = 0;
  audio->bank_dim[2] = 0;

  /* channel allocation */
  audio->max_audio_channels = G_MAXUINT32;
  audio->min_audio_channels = 0;

  audio->max_output_pads = G_MAXUINT32;
  audio->min_output_pads = 0;

  audio->max_input_pads = G_MAXUINT32;
  audio->min_input_pads = 0;

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

  /* loop */
  audio->loop_start = 0;
  audio->loop_end = 0;
  audio->offset = 0;
  
  /* channels */
  audio->output = NULL;
  audio->input = NULL;

  /* preset */
  audio->preset = NULL;

  /* playback domain */
  audio->playback_domain = (GObject *) ags_playback_domain_new((GObject *) audio);
  g_object_ref(audio->playback_domain);

  /* synth generator */
  audio->synth_generator = NULL;
  audio->sf2_synth_generator = NULL;
  audio->sfz_synth_generator = NULL;

  /*
   * Storage objects
   */
  /* cursor */
  audio->cursor = NULL;

  /* notation */
  audio->notation = NULL;

  /* automation */
  audio->automation_port = NULL;
  
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
  g_rec_mutex_init(&(audio->play_mutex));

  audio->play = NULL;

  /* recall context */
  g_rec_mutex_init(&(audio->recall_mutex));

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

  GRecMutex *audio_mutex;

  audio = AGS_AUDIO(gobject);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  switch(prop_id){
  case PROP_AUDIO_NAME:
  {
    gchar *audio_name;

    audio_name = g_value_get_string(value);

    g_rec_mutex_lock(audio_mutex);
      
    audio->audio_name = g_strdup(audio_name);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    GObject *soundcard;

    soundcard = g_value_get_object(value);

    ags_audio_real_set_output_soundcard(audio,
					(GObject *) soundcard);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    GObject *soundcard;

    soundcard = g_value_get_object(value);

    ags_audio_real_set_input_soundcard(audio,
				       (GObject *) soundcard);
  }
  break;
  case PROP_OUTPUT_SEQUENCER:
  {
    GObject *sequencer;

    sequencer = g_value_get_object(value);

    ags_audio_real_set_output_sequencer(audio,
					(GObject *) sequencer);
  }
  break;
  case PROP_INPUT_SEQUENCER:
  {
    GObject *sequencer;

    sequencer = g_value_get_object(value);

    ags_audio_real_set_input_sequencer(audio,
				       (GObject *) sequencer);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    ags_audio_real_set_samplerate(audio,
				  samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    ags_audio_real_set_buffer_size(audio,
				   buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    ags_audio_real_set_format(audio,
			      format);
  }
  break;
  case PROP_BPM:
  {
    gdouble bpm;

    bpm = g_value_get_double(value);

    g_rec_mutex_lock(audio_mutex);
      
    audio->bpm = bpm;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIN_AUDIO_CHANNELS:
  {
    guint min_audio_channels;

    min_audio_channels = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);
      
    audio->min_audio_channels = min_audio_channels;

    g_rec_mutex_unlock(audio_mutex);
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
  case PROP_MIN_OUTPUT_PADS:
  {
    guint min_output_pads;

    min_output_pads = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);
      
    audio->min_output_pads = min_output_pads;

    g_rec_mutex_unlock(audio_mutex);
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
  case PROP_MIN_INPUT_PADS:
  {
    guint min_input_pads;

    min_input_pads = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);
      
    audio->min_input_pads = min_input_pads;

    g_rec_mutex_unlock(audio_mutex);
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

    g_rec_mutex_lock(audio_mutex);
      
    audio->audio_start_mapping = audio_start_mapping;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_AUDIO_END_MAPPING:
  {
    guint audio_end_mapping;

    audio_end_mapping = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->audio_end_mapping = audio_end_mapping;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_START_MAPPING:
  {
    guint midi_start_mapping;

    midi_start_mapping = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->midi_start_mapping = midi_start_mapping;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_END_MAPPING:
  {
    guint midi_end_mapping;

    midi_end_mapping = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->midi_end_mapping = midi_end_mapping;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_CHANNEL:
  {
    guint midi_channel;

    midi_channel = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->midi_channel = midi_channel;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_NUMERATOR:
  {
    guint numerator;

    numerator = g_value_get_uint(value);

    /* numerator and time signature */
    g_rec_mutex_lock(audio_mutex);

    audio->numerator = numerator;

    g_free(audio->time_signature);
    audio->time_signature = g_strdup_printf("%u/%u",
					    audio->numerator,
					    audio->denominator);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_DENOMINATOR:
  {
    guint denominator;

    denominator = g_value_get_uint(value);

    /* denominator and time signature */
    g_rec_mutex_lock(audio_mutex);

    audio->denominator = denominator;

    g_free(audio->time_signature);
    audio->time_signature = g_strdup_printf("%u/%u",
					    audio->numerator,
					    audio->denominator);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_IS_MINOR:
  {
    gboolean is_minor;

    is_minor = g_value_get_boolean(value);

    g_rec_mutex_lock(audio_mutex);

    audio->is_minor = is_minor;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SHARP_FLATS:
  {
    guint sharp_flats;

    sharp_flats = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->sharp_flats = sharp_flats;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OCTAVE:
  {
    gint octave;

    octave = g_value_get_int(value);

    g_rec_mutex_lock(audio_mutex);

    audio->octave = octave;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_KEY:
  {
    guint key;

    key = g_value_get_uint(value);

    g_rec_mutex_lock(audio_mutex);

    audio->key = key;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_ABSOLUTE_KEY:
  {
    gint absolute_key;

    absolute_key = g_value_get_int(value);

    g_rec_mutex_lock(audio_mutex);

    audio->absolute_key = absolute_key;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    guint64 loop_start;

    loop_start = g_value_get_uint64(value);

    g_rec_mutex_lock(audio_mutex);

    audio->loop_start = loop_start;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    guint64 loop_end;

    loop_end = g_value_get_uint64(value);

    g_rec_mutex_lock(audio_mutex);

    audio->loop_end = loop_end;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OFFSET:
  {
    guint64 offset;

    offset = g_value_get_uint64(value);

    g_rec_mutex_lock(audio_mutex);

    audio->offset = offset;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_PRESET:
  {
    AgsPreset *preset;

    preset = (AgsPreset *) g_value_get_pointer(value);

    ags_audio_add_preset(audio,
			 (GObject *) preset);
  }
  break;
  case PROP_SYNTH_GENERATOR:
  {
    AgsSynthGenerator *synth_generator;

    synth_generator = (AgsSynthGenerator *) g_value_get_pointer(value);

    ags_audio_add_synth_generator(audio,
				  (GObject *) synth_generator);
  }
  break;
  case PROP_SF2_SYNTH_GENERATOR:
  {
    AgsSF2SynthGenerator *sf2_synth_generator;

    sf2_synth_generator = (AgsSF2SynthGenerator *) g_value_get_pointer(value);

    ags_audio_add_sf2_synth_generator(audio,
				      (GObject *) sf2_synth_generator);
  }
  break;
  case PROP_SFZ_SYNTH_GENERATOR:
  {
    AgsSFZSynthGenerator *sfz_synth_generator;

    sfz_synth_generator = (AgsSFZSynthGenerator *) g_value_get_pointer(value);

    ags_audio_add_sfz_synth_generator(audio,
				      (GObject *) sfz_synth_generator);
  }
  break;
  case PROP_PLAYBACK_DOMAIN:
  {
    AgsPlaybackDomain *playback_domain;

    playback_domain = (AgsPlaybackDomain *) g_value_get_object(value);

    g_rec_mutex_lock(audio_mutex);

    if(audio->playback_domain == (GObject *) playback_domain){
      g_rec_mutex_unlock(audio_mutex);
	
      return;
    }

    if(audio->playback_domain != NULL){
      g_object_unref(audio->playback_domain);
    }

    if(playback_domain != NULL){
      g_object_ref(playback_domain);
    }

    audio->playback_domain = (GObject *) playback_domain;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_CURSOR:
  {
    GObject *cursor;

    cursor = (GObject *) g_value_get_pointer(value);

    ags_audio_add_cursor(audio,
			 (GObject *) cursor);
  }
  break;
  case PROP_NOTATION:
  {
    AgsNotation *notation;

    notation = (AgsNotation *) g_value_get_pointer(value);

    ags_audio_add_notation(audio,
			   (GObject *) notation);
  }
  break;
  case PROP_AUTOMATION:
  {
    AgsAutomation *automation;

    automation = (AgsAutomation *) g_value_get_pointer(value);

    ags_audio_add_automation(audio,
			     (GObject *) automation);
  }
  break;
  case PROP_WAVE:
  {
    AgsWave *wave;

    wave = (AgsWave *) g_value_get_pointer(value);

    ags_audio_add_wave(audio,
		       (GObject *) wave);
  }
  break;
  case PROP_OUTPUT_AUDIO_FILE:
  {
    AgsAudioFile *output_audio_file;

    output_audio_file = (AgsAudioFile *) g_value_get_object(value);

    g_rec_mutex_lock(audio_mutex);

    if(audio->output_audio_file == (GObject *) output_audio_file){
      g_rec_mutex_unlock(audio_mutex);
	
      return;
    }

    if(audio->output_audio_file != NULL){
      g_object_unref(audio->output_audio_file);
    }

    if(output_audio_file != NULL){
      g_object_ref(output_audio_file);
    }

    audio->output_audio_file = (GObject *) output_audio_file;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_AUDIO_FILE:
  {
    AgsAudioFile *input_audio_file;

    input_audio_file = (AgsAudioFile *) g_value_get_object(value);

    g_rec_mutex_lock(audio_mutex);

    if(audio->input_audio_file == (GObject *) input_audio_file){
      g_rec_mutex_unlock(audio_mutex);
	
      return;
    }

    if(audio->input_audio_file != NULL){
      g_object_unref(audio->input_audio_file);
    }

    if(input_audio_file != NULL){
      g_object_ref(input_audio_file);
    }

    audio->input_audio_file = (GObject *) input_audio_file;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI:
  {
    AgsMidi *midi;

    midi = (AgsMidi *) g_value_get_pointer(value);

    ags_audio_add_midi(audio,
		       (GObject *) midi);
  }
  break;
  case PROP_OUTPUT_MIDI_FILE:
  {
    AgsMidiFile *output_midi_file;

    output_midi_file = g_value_get_object(value);

    g_rec_mutex_lock(audio_mutex);

    if((AgsMidiFile *) audio->output_midi_file == output_midi_file){
      g_rec_mutex_unlock(audio_mutex);
	
      return;
    }

    if(audio->output_midi_file != NULL){
      g_object_unref(audio->output_midi_file);
    }

    if(output_midi_file != NULL) {
      g_object_ref(output_midi_file);
    }

    audio->output_midi_file = (GObject *) output_midi_file;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_MIDI_FILE:
  {
    AgsMidiFile *input_midi_file;

    input_midi_file = g_value_get_object(value);

    g_rec_mutex_lock(audio_mutex);

    if((AgsMidiFile *) audio->input_midi_file == input_midi_file){
      g_rec_mutex_unlock(audio_mutex);
	
      return;
    }

    if(audio->input_midi_file != NULL){
      g_object_unref(audio->input_midi_file);
    }

    if(input_midi_file != NULL) {
      g_object_ref(input_midi_file);
    }

    audio->input_midi_file = (GObject *) input_midi_file;

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_RECALL_ID:
  {
    AgsRecallID *recall_id;

    recall_id = (AgsRecallID *) g_value_get_pointer(value);

    ags_audio_add_recall_id(audio,
			    (GObject *) recall_id);
  }
  break;
  case PROP_RECYCLING_CONTEXT:
  {
    AgsRecyclingContext *recycling_context;

    recycling_context = (AgsRecyclingContext *) g_value_get_pointer(value);

    ags_audio_add_recycling_context(audio,
				    (GObject *) recycling_context);
  }
  break;
  case PROP_RECALL_CONTAINER:
  {
    AgsRecallContainer *recall_container;

    recall_container = (AgsRecallContainer *) g_value_get_pointer(value);

    ags_audio_add_recall_container(audio,
				   (GObject *) recall_container);
  }
  break;
  case PROP_PLAY:
  {
    AgsRecall *recall;

    /*  */
    recall = (AgsRecall *) g_value_get_pointer(value);

    ags_audio_add_recall(audio,
			 (GObject *) recall,
			 TRUE);
  }
  break;
  case PROP_RECALL:
  {
    AgsRecall *recall;

    /*  */
    recall = (AgsRecall *) g_value_get_pointer(value);

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

  GRecMutex *audio_mutex;

  audio = AGS_AUDIO(gobject);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  switch(prop_id){
  case PROP_AUDIO_NAME:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_string(value,
		       audio->audio_name);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->output_soundcard);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->input_soundcard);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_SEQUENCER:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->output_sequencer);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_SEQUENCER:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->input_sequencer);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->samplerate);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->buffer_size);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->format);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_double(value,
		       audio->bpm);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIN_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->min_audio_channels);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MAX_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->max_audio_channels);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIN_OUTPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->min_output_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MAX_OUTPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->max_output_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIN_INPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->min_input_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MAX_INPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->max_input_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->audio_channels);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->output_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_LINES:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->output_lines);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_PADS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->input_pads);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_LINES:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->input_lines);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_AUDIO_START_MAPPING:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->audio_start_mapping);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_AUDIO_END_MAPPING:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->audio_end_mapping);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_START_MAPPING:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->midi_start_mapping);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_END_MAPPING:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->midi_end_mapping);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_MIDI_CHANNEL:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->midi_channel);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_NUMERATOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->numerator);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_DENOMINATOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->denominator);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_TIME_SIGNATURE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_string(value,
		       audio->time_signature);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_IS_MINOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_boolean(value,
			audio->is_minor);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SHARP_FLATS:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->sharp_flats);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OCTAVE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_int(value,
		    audio->octave);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_KEY:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint(value,
		     audio->key);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_ABSOLUTE_KEY:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_int(value,
		    audio->absolute_key);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint64(value,
		       audio->loop_start);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint64(value,
		       audio->loop_end);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OFFSET:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_uint64(value,
		       audio->offset);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->output);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->input);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_PRESET:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->preset,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SYNTH_GENERATOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->synth_generator,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SF2_SYNTH_GENERATOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->sf2_synth_generator,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_SFZ_SYNTH_GENERATOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->sfz_synth_generator,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_PLAYBACK_DOMAIN:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->playback_domain);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_CURSOR:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->cursor,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_NOTATION:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->notation,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_AUTOMATION:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->automation,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_WAVE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->wave,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_AUDIO_FILE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->output_audio_file);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_AUDIO_FILE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->input_audio_file);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_OUTPUT_MIDI_FILE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->output_midi_file);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_INPUT_MIDI_FILE:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_object(value,
		       audio->input_midi_file);

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_RECALL_ID:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->recall_id,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_RECYCLING_CONTEXT:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->recycling_context,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_RECALL_CONTAINER:
  {
    g_rec_mutex_lock(audio_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->recall_container,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_mutex);
  }
  break;
  case PROP_PLAY:
  {
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /*  */
    g_rec_mutex_lock(play_mutex);
      
    g_value_set_pointer(value,
			g_list_copy_deep(audio->play,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(play_mutex);
  }
  break;
  case PROP_RECALL:
  {
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /*  */
    g_rec_mutex_lock(recall_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio->recall,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(recall_mutex);
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

  GList *start_list, *list, *list_next;

  GRecMutex *play_mutex;
  GRecMutex *recall_mutex;

  audio = AGS_AUDIO(gobject);

  ags_connectable_disconnect(AGS_CONNECTABLE(audio));

  /* soundcard */
  if(audio->output_soundcard != NULL){    
    gpointer tmp;

    tmp = audio->output_soundcard;

    audio->output_soundcard = NULL;

    g_object_unref(tmp);
  }

  if(audio->input_soundcard != NULL){    
    gpointer tmp;

    tmp = audio->input_soundcard;

    audio->input_soundcard = NULL;

    g_object_unref(tmp);
  }

  /* sequencer */
  if(audio->output_sequencer != NULL){
    gpointer tmp;

    tmp = audio->output_sequencer;
    
    audio->output_sequencer = NULL;

    g_object_unref(tmp);
  }

  if(audio->input_sequencer != NULL){
    gpointer tmp;

    tmp = audio->input_sequencer;

    audio->input_sequencer = NULL;

    g_object_unref(tmp);
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
    list =
      start_list = audio->preset;

    audio->preset = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* synth generator */
  if(audio->synth_generator != NULL){
    list =
      start_list = audio->synth_generator;

    audio->synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* sf2 synth generator */
  if(audio->sf2_synth_generator != NULL){
    list =
      start_list = audio->sf2_synth_generator;

    audio->sf2_synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* sfz synth generator */
  if(audio->sfz_synth_generator != NULL){
    list =
      start_list = audio->sfz_synth_generator;

    audio->sfz_synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* playback domain */
  if(audio->playback_domain != NULL){
    AgsPlaybackDomain *playback_domain;

    playback_domain = audio->playback_domain;

    audio->playback_domain = NULL;
    
    g_object_run_dispose(playback_domain);
  }
  
  /* notation */
  if(audio->notation != NULL){
    list =
      start_list = audio->notation;

    audio->notation = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* automation */
  if(audio->automation != NULL){
    list =
      start_list = audio->automation;

    audio->automation = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* wave */
  if(audio->wave != NULL){
    list =
      start_list = audio->wave;

    audio->wave = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* output audio file */
  if(audio->output_audio_file != NULL){
    gpointer tmp;

    tmp = audio->output_audio_file;

    audio->output_audio_file = NULL;
    
    g_object_unref(tmp);
  }
  
  /* input audio file */
  if(audio->input_audio_file != NULL){
    gpointer tmp;

    tmp = audio->input_audio_file;

    audio->input_audio_file = NULL;

    g_object_unref(tmp);
  }

  /* midi */
  if(audio->midi != NULL){
    list =
      start_list = audio->midi;

    audio->midi = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* output midi file */
  if(audio->output_midi_file != NULL){
    gpointer tmp;

    tmp = audio->output_midi_file;

    audio->output_midi_file = NULL;
    
    g_object_unref(tmp);
  }
  
  /* input midi file */
  if(audio->input_midi_file != NULL){
    gpointer tmp;

    tmp = audio->input_midi_file;

    audio->input_midi_file = NULL;

    g_object_unref(tmp);
  }

  /* recall id */
  if(audio->recall_id != NULL){
    list =
      start_list = audio->recall_id;

    audio->recall_id = NULL;
    
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
      
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* recycling context */
  if(audio->recycling_context != NULL){
    list =
      start_list = audio->recycling_context;

    audio->recycling_context = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* recall container */
  if(audio->recall_container != NULL){
    list =
      start_list = audio->recall_container;

    audio->recall_container = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* play */
  if(audio->play != NULL){
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* run dispose and unref */
    g_rec_mutex_lock(play_mutex);

    list =
      start_list = audio->play;

    audio->play = NULL;

    g_rec_mutex_unlock(play_mutex);

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* recall */
  if(audio->recall != NULL){
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* run dispose and unref */
    g_rec_mutex_lock(recall_mutex);
    
    list =
      start_list = audio->recall;

    audio->recall = NULL;
  
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);

    g_rec_mutex_unlock(recall_mutex);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->dispose(gobject);
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;

  GList *start_list, *list, *list_next;

  GRecMutex *play_mutex;
  GRecMutex *recall_mutex;

  audio = AGS_AUDIO(gobject);

  ags_uuid_free(audio->uuid);
  
  g_free(audio->audio_name);  

  /* soundcard */
  if(audio->output_soundcard != NULL){    
    gpointer tmp;

    tmp = audio->output_soundcard;

    audio->output_soundcard = NULL;

    g_object_unref(tmp);
  }

  if(audio->input_soundcard != NULL){    
    gpointer tmp;

    tmp = audio->input_soundcard;

    audio->input_soundcard = NULL;

    g_object_unref(tmp);
  }

  /* sequencer */
  if(audio->output_sequencer != NULL){
    gpointer tmp;

    tmp = audio->output_sequencer;
    
    audio->output_sequencer = NULL;

    g_object_unref(tmp);
  }

  if(audio->input_sequencer != NULL){
    gpointer tmp;

    tmp = audio->input_sequencer;

    audio->input_sequencer = NULL;

    g_object_unref(tmp);
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
    list =
      start_list = audio->preset;

    audio->preset = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* synth generator */
  if(audio->synth_generator != NULL){
    list =
      start_list = audio->synth_generator;

    audio->synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* sf2 synth generator */
  if(audio->sf2_synth_generator != NULL){
    list =
      start_list = audio->sf2_synth_generator;

    audio->sf2_synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* sfz synth generator */
  if(audio->sfz_synth_generator != NULL){
    list =
      start_list = audio->sfz_synth_generator;

    audio->sfz_synth_generator = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* playback domain */
  if(audio->playback_domain != NULL){
    AgsPlaybackDomain *playback_domain;

    playback_domain = audio->playback_domain;

    audio->playback_domain = NULL;
    
    g_object_run_dispose(playback_domain);
  }
  
  /* notation */
  if(audio->notation != NULL){
    list =
      start_list = audio->notation;

    audio->notation = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* automation */
  if(audio->automation != NULL){
    list =
      start_list = audio->automation;

    audio->automation = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* wave */
  if(audio->wave != NULL){
    list =
      start_list = audio->wave;

    audio->wave = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* output audio file */
  if(audio->output_audio_file != NULL){
    gpointer tmp;

    tmp = audio->output_audio_file;

    audio->output_audio_file = NULL;
    
    g_object_unref(tmp);
  }
  
  /* input audio file */
  if(audio->input_audio_file != NULL){
    gpointer tmp;

    tmp = audio->input_audio_file;

    audio->input_audio_file = NULL;

    g_object_unref(tmp);
  }

  /* midi */
  if(audio->midi != NULL){
    list =
      start_list = audio->midi;

    audio->midi = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* output midi file */
  if(audio->output_midi_file != NULL){
    gpointer tmp;

    tmp = audio->output_midi_file;

    audio->output_midi_file = NULL;
    
    g_object_unref(tmp);
  }
  
  /* input midi file */
  if(audio->input_midi_file != NULL){
    gpointer tmp;

    tmp = audio->input_midi_file;

    audio->input_midi_file = NULL;

    g_object_unref(tmp);
  }

  /* recall id */
  if(audio->recall_id != NULL){
    list =
      start_list = audio->recall_id;

    audio->recall_id = NULL;
    
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
      
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* recycling context */
  if(audio->recycling_context != NULL){
    list =
      start_list = audio->recycling_context;

    audio->recycling_context = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }
  
    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* recall container */
  if(audio->recall_container != NULL){
    list =
      start_list = audio->recall_container;

    audio->recall_container = NULL;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);

      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* play */
  if(audio->play != NULL){
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* run dispose and unref */
    g_rec_mutex_lock(play_mutex);

    list =
      start_list = audio->play;

    audio->play = NULL;

    g_rec_mutex_unlock(play_mutex);

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* recall */
  if(audio->recall != NULL){
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* run dispose and unref */
    g_rec_mutex_lock(recall_mutex);
    
    list =
      start_list = audio->recall;

    audio->recall = NULL;
  
    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(list->data);
    
      list = list_next;
    }

    g_list_free_full(start_list,
		     g_object_unref);

    g_rec_mutex_unlock(recall_mutex);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_get_uuid(AgsConnectable *connectable)
{
  AgsAudio *audio;
  
  AgsUUID *ptr;

  GRecMutex *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get UUID */
  g_rec_mutex_lock(audio_mutex);

  ptr = audio->uuid;

  g_rec_mutex_unlock(audio_mutex);
  
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

  GRecMutex *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check is ready */
  g_rec_mutex_lock(audio_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (audio->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_mutex);
  
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
  
  GRecMutex *audio_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(audio_mutex);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

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
  AgsAudio *audio;

  GRecMutex *audio_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(audio_mutex);

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

  GRecMutex *audio_mutex;

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check is connected */
  g_rec_mutex_lock(audio_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (audio->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_mutex);
  
  return(is_connected);
}

void
ags_audio_connect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel, *next_channel;

  GList *start_list, *list;

  GRecMutex *audio_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(audio_mutex);

#ifdef AGS_DEBUG
  g_message("connecting audio");
#endif

  /* connect channels - output */
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(channel);
  
    while(channel != NULL){
      /* connect */
      ags_connectable_connect(AGS_CONNECTABLE(channel));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* connect channels - input */
  g_object_get(audio,
	       "input", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(channel);

    while(channel != NULL){
      /* connect */
      ags_connectable_connect(AGS_CONNECTABLE(channel));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* connect recall container */
  g_object_get(audio,
	       "recall-container", &start_list,
	       NULL);
  
  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* connect recall - play context */
  g_object_get(audio,
	       "play", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* connect recall - recall context */
  g_object_get(audio,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_audio_disconnect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel, *next_channel;

  GList *start_list, *list;

  GRecMutex *audio_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio = AGS_AUDIO(connectable);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(audio_mutex);

#ifdef AGS_DEBUG
  g_message("disconnecting audio");
#endif

  /* disconnect channels - output */
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(channel);
  
    while(channel != NULL){
      /* disconnect */
      ags_connectable_disconnect(AGS_CONNECTABLE(channel));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* disconnect channels - input */
  g_object_get(audio,
	       "input", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(channel);

    while(channel != NULL){
      /* disconnect */
      ags_connectable_disconnect(AGS_CONNECTABLE(channel));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* disconnect recall container */
  g_object_get(audio,
	       "recall-container", &start_list,
	       NULL);
  
  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* disconnect recall - play context */
  g_object_get(audio,
	       "play", &start_list,
	       NULL);

  list = start_list;
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* disconnect recall - recall context */
  g_object_get(audio,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_audio_get_obj_mutex:
 * @audio: the #AgsAudio
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @audio
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_audio_get_obj_mutex(AgsAudio *audio)
{
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  return(AGS_AUDIO_GET_OBJ_MUTEX(audio));
}

/**
 * ags_audio_get_play_mutex:
 * @audio: the #AgsAudio
 * 
 * Get play mutex.
 * 
 * Returns: the #GRecMutex to lock @audio's play property
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_audio_get_play_mutex(AgsAudio *audio)
{
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  return(AGS_AUDIO_GET_PLAY_MUTEX(audio));
}

/**
 * ags_audio_get_recall_mutex:
 * @audio: the #AgsAudio
 * 
 * Get recall mutex.
 * 
 * Returns: the #GRecMutex to lock @audio's recall property
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_audio_get_recall_mutex(AgsAudio *audio)
{
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  return(AGS_AUDIO_GET_RECALL_MUTEX(audio));
}

/**
 * ags_audio_test_flags:
 * @audio: the #AgsAudio
 * @flags: the flags
 *
 * Test @flags to be set on @audio.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_test_flags(AgsAudio *audio, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  /* test */
  g_rec_mutex_lock(audio_mutex);

  retval = (flags & (audio->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_mutex);

  return(retval);
}

/**
 * ags_audio_set_flags:
 * @audio: the #AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Enable a feature of #AgsAudio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_flags(AgsAudio *audio, guint flags)
{
  guint audio_flags;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check flags */
  g_rec_mutex_lock(audio_mutex);
  
  audio_flags = audio->flags;
  
  g_rec_mutex_unlock(audio_mutex);

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
    AgsChannel *start_channel, *channel, *next_channel;

    g_object_get(audio,
		 "output", &start_channel,
		 NULL);

    if(start_channel != NULL){
      channel = start_channel;
      g_object_ref(channel);
      
      while(channel != NULL){
	GObject *output_soundcard;
	
	AgsRecycling *first_recycling, *last_recycling;
	
	/* get some fields */
	g_object_get(channel,
		     "output-soundcard", &output_soundcard,
		     NULL);
      
	/* add recycling */
	first_recycling =
	  last_recycling = ags_recycling_new(output_soundcard);
	g_object_ref(first_recycling);
	g_object_set(first_recycling,
		     "channel", channel,
		     NULL);
	  
	ags_channel_reset_recycling(channel,
				    first_recycling, last_recycling);

	g_object_unref(output_soundcard);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      g_object_unref(start_channel);
    }
  }

  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (flags)) != 0 &&
     (AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) == 0){
    AgsChannel *start_channel, *channel, *next_channel;
    
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);

    if(start_channel != NULL){
      channel = start_channel;
      g_object_ref(channel);
      
      while(channel != NULL){
	AgsRecycling *recycling;
	AgsRecycling *first_recycling, *last_recycling;

	GObject *output_soundcard;
	
	/* get some fields */
	g_object_get(channel,
		     "output-soundcard", &output_soundcard,
		     "first-recycling", &recycling,
		     NULL);
      
	/* add recycling */
	if(recycling == NULL){
	  first_recycling =
	    last_recycling = ags_recycling_new(output_soundcard);
	  g_object_ref(first_recycling);
	  g_object_set(first_recycling,
		       "channel", channel,
		       NULL);
	  
	  ags_channel_reset_recycling(channel,
				      first_recycling, last_recycling);
	}else{
	  g_object_unref(recycling);
	}

	g_object_unref(output_soundcard);
      
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }
      
      g_object_unref(start_channel);
    }
  }

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_mutex);

  audio->flags |= flags;
  
  g_rec_mutex_unlock(audio_mutex);
}
    
/**
 * ags_audio_unset_flags:
 * @audio: the #AgsAudio
 * @flags: see enum AgsAudioFlags
 *
 * Disable a feature of AgsAudio.
 *
 * Since: 3.0.0
 */
void
ags_audio_unset_flags(AgsAudio *audio, guint flags)
{
  guint audio_flags;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check flags */
  g_rec_mutex_lock(audio_mutex);
  
  audio_flags = audio->flags;
  
  g_rec_mutex_unlock(audio_mutex);

  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0 &&
     (AGS_AUDIO_OUTPUT_HAS_RECYCLING & (flags)) == 0){
    AgsChannel *start_channel, *channel, *next_channel;

    GRecMutex *channel_mutex;
    
    g_object_get(audio,
		 "output", &start_channel,
		 NULL);

    if(start_channel != NULL){
      channel = start_channel;
      g_object_ref(channel);
      
      while(channel != NULL){
	AgsRecycling *first_recycling;
	
	/* get channel mutex */
	channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

	/* unset recycling */
	g_rec_mutex_lock(channel_mutex);

	first_recycling = channel->first_recycling;

	channel->first_recycling =
	  channel->last_recycling = NULL;
      
	g_rec_mutex_unlock(channel_mutex);
      
	/* remove recycling */
	if(first_recycling != NULL){
	  g_object_run_dispose((GObject *) first_recycling);
	  g_object_unref(first_recycling);
	}
      
	ags_channel_reset_recycling(channel,
				    NULL, NULL);
      
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      g_object_unref(start_channel);
    }
  }
  
  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0 &&
     (AGS_AUDIO_INPUT_HAS_RECYCLING & (flags)) == 0){
    AgsChannel *start_channel, *channel, *next_channel;
    AgsChannel *link;
    
    GRecMutex *channel_mutex;
    
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);

    if(start_channel != NULL){
      channel = start_channel;
      g_object_ref(channel);

      while(channel != NULL){
	AgsRecycling *first_recycling;
      
	/* get channel mutex */
	channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

	/* get some fields */
	g_rec_mutex_lock(channel_mutex);

	link = channel->link;
      
	g_rec_mutex_unlock(channel_mutex);
      
	/* unset recycling */
	if(link == NULL){
	  g_rec_mutex_lock(channel_mutex);

	  first_recycling = channel->first_recycling;

	  channel->first_recycling =
	    channel->last_recycling = NULL;
      
	  g_rec_mutex_unlock(channel_mutex);
      
	  /* remove recycling */
	  if(first_recycling != NULL){
	    g_object_run_dispose((GObject *) first_recycling);
	    g_object_unref(first_recycling);
	  }
	
	  ags_channel_reset_recycling(channel,
				      NULL, NULL);
	}
      
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }
      
      g_object_unref(start_channel);
    }
  }

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_mutex);

  audio->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_test_ability_flags:
 * @audio: the #AgsAudio
 * @ability_flags: the ability flags
 *
 * Test @ability_flags to be set on @audio.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_test_ability_flags(AgsAudio *audio, guint ability_flags)
{
  gboolean retval;  
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test */
  g_rec_mutex_lock(audio_mutex);

  retval = (ability_flags & (audio->ability_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_mutex);

  return(retval);
}

void
ags_audio_set_ability_flags_channel(AgsChannel *start_channel, guint ability_flags)
{
  AgsChannel *channel, *next_channel;

  if(!AGS_IS_CHANNEL(start_channel)){
    return;
  }
  
  channel = start_channel;
  g_object_ref(channel);
      
  while(channel != NULL){
    /* set ability flags */
    ags_channel_set_ability_flags(channel, ability_flags);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

/**
 * ags_audio_set_ability_flags:
 * @audio: the #AgsAudio
 * @ability_flags: see enum AgsSoundAbilityFlags
 *
 * Enable an ability of AgsAudio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_ability_flags(AgsAudio *audio, guint ability_flags)
{
  AgsChannel *start_output, *start_input;  
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *main_loop;
  AgsAudioLoop *audio_loop;
  
  AgsApplicationContext *application_context;
  
  GObject *output_soundcard;
  
  guint samplerate, buffer_size;
  guint audio_ability_flags;
  gboolean super_threaded_audio;
  
  GRecMutex *audio_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check flags */
  g_rec_mutex_lock(audio_mutex);

  audio_ability_flags = audio->ability_flags;
  
  samplerate = audio->samplerate;
  buffer_size = audio->buffer_size;
  
  g_rec_mutex_unlock(audio_mutex);

  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "output", &start_output,
	       "input", &start_input,
	       "playback-domain", &playback_domain,
	       NULL);

  /* get super-threaded flags */
  super_threaded_audio = ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO);
  
  /* notation ability */  
  if(super_threaded_audio){
    /* find audio loop */
    audio_loop = (AgsAudioLoop *) ags_thread_find_type(main_loop,
						       AGS_TYPE_AUDIO_LOOP);
    
    /* playback ability */
    if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_PLAYBACK & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(output_soundcard,
					  (GObject *) audio);
      ags_audio_thread_set_sound_scope(audio_thread,
				       AGS_SOUND_SCOPE_PLAYBACK);
      
      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   (AgsThread *) audio_thread,
					   AGS_SOUND_SCOPE_PLAYBACK);    

      /* set thread child */
      ags_thread_add_child_extended((AgsThread *) audio_loop,
				    (AgsThread *) audio_thread,
				    TRUE, TRUE);
    }

    /* sequencer ability */
    if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_SEQUENCER & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(output_soundcard,
					  (GObject *) audio);
      ags_audio_thread_set_sound_scope(audio_thread,
				       AGS_SOUND_SCOPE_SEQUENCER);

      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   (AgsThread *) audio_thread,
					   AGS_SOUND_SCOPE_SEQUENCER);

      /* set thread child */
      ags_thread_add_child_extended((AgsThread *) audio_loop,
				    (AgsThread *) audio_thread,
				    TRUE, TRUE);
    }

    /* notation ability */
    if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_NOTATION & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(output_soundcard,
					  (GObject *) audio);
      ags_audio_thread_set_sound_scope(audio_thread,
				       AGS_SOUND_SCOPE_NOTATION);

      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   (AgsThread *) audio_thread,
					   AGS_SOUND_SCOPE_NOTATION);    

      /* set thread child */
      ags_thread_add_child_extended((AgsThread *) audio_loop,
				    (AgsThread *) audio_thread,
				    TRUE, TRUE);
    }

    /* wave ability */
    if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_WAVE & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(output_soundcard,
					  (GObject *) audio);
      ags_audio_thread_set_sound_scope(audio_thread,
				       AGS_SOUND_SCOPE_WAVE);

      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   (AgsThread *) audio_thread,
					   AGS_SOUND_SCOPE_WAVE);

      /* set thread child */
      ags_thread_add_child_extended((AgsThread *) audio_loop,
				    (AgsThread *) audio_thread,
				    TRUE, TRUE);
    }

    /* midi ability */
    if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0 &&
       (AGS_SOUND_ABILITY_MIDI & (audio_ability_flags)) == 0){
      AgsAudioThread *audio_thread;

      audio_thread = ags_audio_thread_new(output_soundcard,
					  (GObject *) audio);
      ags_audio_thread_set_sound_scope(audio_thread,
				       AGS_SOUND_SCOPE_MIDI);

      g_object_set(audio_thread,
		   "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		   NULL);
    
      ags_playback_domain_set_audio_thread(playback_domain,
					   (AgsThread *) audio_thread,
					   AGS_SOUND_SCOPE_MIDI);

      /* set thread child */
      ags_thread_add_child_extended((AgsThread *) audio_loop,
				    (AgsThread *) audio_thread,
				    TRUE, TRUE);
    }
  }
  
  /* channel */
  ags_audio_set_ability_flags_channel(start_output, ability_flags);  
  ags_audio_set_ability_flags_channel(start_input, ability_flags);
  
  /* set flags */
  g_rec_mutex_lock(audio_mutex);

  audio->ability_flags |= ability_flags;
  
  g_rec_mutex_unlock(audio_mutex);

  /* unref */
  if(main_loop != NULL){
    g_object_unref(main_loop);
  }

  if(output_soundcard != NULL) {
    g_object_unref(output_soundcard);
  }
  
  g_object_unref(playback_domain);

  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_audio_unset_ability_flags_channel(AgsChannel *start_channel, guint ability_flags)
{
  AgsChannel *channel, *next_channel;

  if(!AGS_IS_CHANNEL(start_channel)){
    return;
  }
  
  channel = start_channel;
  g_object_ref(channel);

  while(channel != NULL){
    /* set ability flags */
    ags_channel_unset_ability_flags(channel, ability_flags);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

/**
 * ags_audio_unset_ability_flags:
 * @audio: the #AgsAudio
 * @ability_flags: see enum AgsSoundAbilityFlags
 *
 * Disable an ability of AgsAudio.
 *
 * Since: 3.0.0
 */
void
ags_audio_unset_ability_flags(AgsAudio *audio, guint ability_flags)
{
  AgsChannel *start_output, *start_input;  
  AgsPlaybackDomain *playback_domain;

  AgsThread *main_loop;
  AgsThread *audio_loop;

  AgsApplicationContext *application_context;
  
  guint audio_ability_flags;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* get main loop */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* check flags */
  g_rec_mutex_lock(audio_mutex);
  
  audio_ability_flags = audio->ability_flags;

  g_rec_mutex_unlock(audio_mutex);

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "playback-domain", &playback_domain,
	       NULL);

  /* find audio loop */
  audio_loop = ags_thread_find_type(main_loop,
				    AGS_TYPE_AUDIO_LOOP);

  /* playback ability */
  if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_PLAYBACK & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;

    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_PLAYBACK);
    ags_thread_remove_child(audio_loop,
			    (AgsThread *) audio_thread);
    
    ags_playback_domain_set_audio_thread(playback_domain,
					 NULL,
					 AGS_SOUND_SCOPE_PLAYBACK);
  }

  /* notation ability */
  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_NOTATION & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_NOTATION);
    ags_thread_remove_child(audio_loop,
			    (AgsThread *) audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 NULL,
					 AGS_SOUND_SCOPE_NOTATION);
  }

  /* sequencer ability */
  if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_SEQUENCER & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_SEQUENCER);
    ags_thread_remove_child(audio_loop,
			    (AgsThread *) audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 NULL,
					 AGS_SOUND_SCOPE_SEQUENCER);
  }

  /* wave ability */
  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_WAVE & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_WAVE);
    ags_thread_remove_child(audio_loop,
			    (AgsThread *) audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 NULL,
					 AGS_SOUND_SCOPE_WAVE);
  }

  /* midi ability */
  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) == 0 &&
     (AGS_SOUND_ABILITY_MIDI & (audio_ability_flags)) != 0){
    AgsAudioThread *audio_thread;
    
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   AGS_SOUND_SCOPE_MIDI);
    ags_thread_remove_child(audio_loop,
			    (AgsThread *) audio_thread);    

    ags_playback_domain_set_audio_thread(playback_domain,
					 NULL,
					 AGS_SOUND_SCOPE_MIDI);
  }

  /* channel */
  ags_audio_unset_ability_flags_channel(start_output, ability_flags);  
  ags_audio_unset_ability_flags_channel(start_input, ability_flags);
  
  /* unset flags */
  g_rec_mutex_lock(audio_mutex);

  audio->ability_flags &= (~ability_flags);
  
  g_rec_mutex_unlock(audio_mutex);

  /* unref */
  g_object_unref(main_loop);

  g_object_unref(playback_domain);

  g_object_unref(start_output);
  g_object_unref(start_input);
}

/**
 * ags_audio_test_behaviour_flags:
 * @audio: the #AgsAudio
 * @behaviour_flags: the behaviour flags
 *
 * Test @behaviour_flags to be set on @audio.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_test_behaviour_flags(AgsAudio *audio, guint behaviour_flags)
{
  gboolean retval;  
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test */
  g_rec_mutex_lock(audio_mutex);

  retval = (behaviour_flags & (audio->behaviour_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_mutex);

  return(retval);
}

/**
 * ags_audio_set_behaviour_flags:
 * @audio: the #AgsAudio
 * @behaviour_flags: the behaviour flags
 * 
 * Set behaviour flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_set_behaviour_flags(AgsAudio *audio, guint behaviour_flags)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set flags */
  g_rec_mutex_lock(audio_mutex);

  audio->behaviour_flags |= behaviour_flags;

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_behaviour_flags:
 * @audio: the #AgsAudio
 * @behaviour_flags: the behaviour flags
 * 
 * Unset behaviour flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_unset_behaviour_flags(AgsAudio *audio, guint behaviour_flags)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* unset flags */
  g_rec_mutex_lock(audio_mutex);

  audio->behaviour_flags &= (~behaviour_flags);

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_test_staging_flags:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to test
 * @staging_flags: the staging flags
 *
 * Test @staging_flags to be set on @audio.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_test_staging_flags(AgsAudio *audio, gint sound_scope,
			     guint staging_flags)
{
  gboolean retval;  
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test */
  g_rec_mutex_lock(audio_mutex);

  retval = (staging_flags & (audio->staging_flags[sound_scope])) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_mutex);

  return(retval);
}

/**
 * ags_audio_set_staging_flags:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Set staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_set_staging_flags(AgsAudio *audio, gint sound_scope,
			    guint staging_flags)
{
  guint i;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set flags */
  g_rec_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_flags[i] |= staging_flags;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_flags[sound_scope] |= staging_flags;
  }

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_staging_flags:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * @staging_flags: the staging flags
 * 
 * Unset staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_audio_unset_staging_flags(AgsAudio *audio, gint sound_scope,
			      guint staging_flags)
{
  guint i;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* unset flags */
  g_rec_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_flags[i] &= (~staging_flags);
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_flags[sound_scope] &= (~staging_flags);
  }

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_test_staging_completed:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to test
 *
 * Test @sound_scope to be completed on @audio.
 * 
 * Returns: %TRUE if completed, otherwise %FALSE
 *
 * Since: 3.3.0
 */
gboolean
ags_audio_test_staging_completed(AgsAudio *audio, gint sound_scope)
{
  guint i;
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test staging completed */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      if(!audio->staging_completed[i]){
	break;
      }
    }

    if(i == AGS_SOUND_SCOPE_LAST){
      success = TRUE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    success = audio->staging_completed[sound_scope];
  }

  g_rec_mutex_unlock(audio_mutex);

  return(success);
}

/**
 * ags_audio_set_staging_completed:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * 
 * Set @sound_scope to be completed.
 * 
 * Since: 3.3.0
 */
void
ags_audio_set_staging_completed(AgsAudio *audio, gint sound_scope)
{
  guint i;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test staging completed */  
  g_rec_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_completed[i] = TRUE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_completed[sound_scope] = TRUE;
  }

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_unset_staging_completed:
 * @audio: the #AgsAudio
 * @sound_scope: the #AgsSoundScope to apply, or -1 to apply to all
 * 
 * Unset @sound_scope to be completed.
 * 
 * Since: 3.3.0
 */
void
ags_audio_unset_staging_completed(AgsAudio *audio, gint sound_scope)
{
  guint i;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* test staging completed */  
  g_rec_mutex_lock(audio_mutex);

  if(sound_scope < 0){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio->staging_completed[i] = FALSE;
    }
  }else if(sound_scope < AGS_SOUND_SCOPE_LAST){
    audio->staging_completed[sound_scope] = FALSE;
  }

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_audio_name:
 * @audio: the #AgsAudio
 * 
 * Get audio name of @audio.
 * 
 * Returns: the audio name
 * 
 * Since: 3.1.0
 */
gchar*
ags_audio_get_audio_name(AgsAudio *audio)
{
  gchar *audio_name;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  audio_name = NULL;
  
  g_object_get(audio,
	       "audio-name", &audio_name,
	       NULL);

  return(audio_name);
}

/**
 * ags_audio_set_audio_name:
 * @audio: the #AgsAudio
 * @audio_name: the audio name
 * 
 * Set audio name of @audio.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_audio_name(AgsAudio *audio, gchar *audio_name)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "audio-name", audio_name,
	       NULL);
}

/**
 * ags_audio_find_name:
 * @audio: (element-type AgsAudio.Audio) (transfer none): the #GList-struct containing #AgsAudio
 * @audio_name: the audio name to find
 * 
 * Find @audio_name in @audio.
 *
 * Returns: (element-type AgsAudio.Audio) (transfer none): the next matching #GList-struct containing #AgsAudio
 * 
 * Since: 3.0.0
 */
GList*
ags_audio_find_name(GList *audio,
		    gchar *audio_name)
{
  if(audio == NULL ||
     audio_name == NULL){
    return(NULL);
  }

  while(audio != NULL){
    gboolean success;
    
    GRecMutex *audio_mutex;

    /* get audio mutex */
    audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio->data);

    /* match */
    g_rec_mutex_lock(audio_mutex);

    success = (AGS_AUDIO(audio->data)->audio_name != NULL &&
	       !g_strcmp0(AGS_AUDIO(audio->data)->audio_name,
			  audio_name)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(audio_mutex);
    
    if(success){
      return(audio);
    }
    
    audio = audio->next;
  }
  
  return(NULL);
}

/**
 * ags_audio_get_max_audio_channels:
 * @audio: the #AgsAudio
 * 
 * Get maximum audio channels.
 * 
 * Returns: the maximum audio channels
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_max_audio_channels(AgsAudio *audio)
{
  guint max_audio_channels;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "max-audio-channels", &max_audio_channels,
	       NULL);

  return(max_audio_channels);
}

/**
 * ags_audio_set_max_audio_channels:
 * @audio: the #AgsAudio
 * @max_audio_channels: maximum of audio channels
 * 
 * Set maximum audio channels.
 * 
 * Since: 3.0.0
 */
void
ags_audio_set_max_audio_channels(AgsAudio *audio,
				 guint max_audio_channels)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set max audio channels */
  g_rec_mutex_lock(audio_mutex);

  audio->max_audio_channels = max_audio_channels;

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_max_output_pads:
 * @audio: the #AgsAudio
 * 
 * Get maximum output pads.
 * 
 * Returns: the maximum output pads
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_max_output_pads(AgsAudio *audio)
{
  guint max_output_pads;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "max-output-pads", &max_output_pads,
	       NULL);

  return(max_output_pads);
}

/**
 * ags_audio_set_max_output_pads:
 * @audio: the #AgsAudio
 * @max_output_pads: maximum of output pads
 * 
 * Set maximum output pads.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_max_output_pads(AgsAudio *audio,
			      guint max_output_pads)
{
  ags_audio_set_max_pads(audio,
			 AGS_TYPE_OUTPUT,
			 max_output_pads);
}

/**
 * ags_audio_get_max_input_pads:
 * @audio: the #AgsAudio
 * 
 * Get maximum input pads.
 * 
 * Returns: the maximum input pads
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_max_input_pads(AgsAudio *audio)
{
  guint max_input_pads;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "max-input-pads", &max_input_pads,
	       NULL);

  return(max_input_pads);
}

/**
 * ags_audio_set_max_input_pads:
 * @audio: the #AgsAudio
 * @max_input_pads: maximum of input pads
 * 
 * Set maximum input pads.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_max_input_pads(AgsAudio *audio,
			     guint max_input_pads)
{
  ags_audio_set_max_pads(audio,
			 AGS_TYPE_INPUT,
			 max_input_pads);
}

/**
 * ags_audio_set_max_pads:
 * @audio: the #AgsAudio
 * @channel_type: the #GType of channel, either AGS_TYPE_OUTPUT or AGS_TYPE_INPUT
 * @max_pads: maximum of pads
 * 
 * Set maximum pads of @channel_type.
 * 
 * Since: 3.0.0
 */
void
ags_audio_set_max_pads(AgsAudio *audio,
		       GType channel_type,
		       guint max_pads)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set max pads */
  g_rec_mutex_lock(audio_mutex);

  if(g_type_is_a(channel_type,
		 AGS_TYPE_OUTPUT)){
    audio->max_output_pads = max_pads;
  }else if(g_type_is_a(channel_type,
		       AGS_TYPE_INPUT)){
    audio->max_input_pads = max_pads;
  }

  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_audio_channels:
 * @audio: the #AgsAudio
 * 
 * Get audio channels.
 * 
 * Returns: the audio channels
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_audio_channels(AgsAudio *audio)
{
  guint audio_channels;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);

  return(audio_channels);
}

void
ags_audio_set_audio_channels_grow(AgsAudio *audio,
				  GType channel_type,
				  guint audio_channels, guint audio_channels_old,
				  guint bank_dim_0, guint bank_dim_1, guint bank_dim_2,
				  gboolean add_recycling, gboolean add_pattern, gboolean add_synth_generator,
				  gboolean link_recycling,
				  gboolean set_sync_link, gboolean set_async_link)
{
  AgsChannel *start;
  AgsChannel *channel, *current, *pad_next, *nth_channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsPlaybackDomain *playback_domain;
  
  GObject *output_soundcard, *input_soundcard;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint pads;
  guint i, j;

  GRecMutex *audio_mutex;
  GRecMutex *prev_mutex, *prev_pad_mutex;
  GRecMutex *next_pad_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* get some fields */  
  g_object_get(audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "playback-domain", &playback_domain,
	       "output-soundcard", &output_soundcard,
	       "input-soundcard", &input_soundcard,
	       NULL);
  
  if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(audio,
		 "output", &start,
		 "output-pads", &pads,
		 NULL);
  }else{
    g_object_get(audio,
		 "input", &start,
		 "input-pads", &pads,
		 NULL);
  }

  /* grow */
  for(j = 0; j < pads; j++){
    for(i = audio_channels_old; i < audio_channels; i++){
      AgsPlayback *playback;
	
      channel = (AgsChannel *) g_object_new(channel_type,
					    "audio", (GObject *) audio,
					    "output-soundcard", output_soundcard,
					    "output-soundcard-channel", i,
					    "input-soundcard", input_soundcard,
					    "input-soundcard-channel", i,
					    "samplerate", samplerate,
					    "buffer-size", buffer_size,
					    "format", format,
					    NULL);
      g_object_ref(channel);
      
      /* set indices */
      channel->pad = j;
      channel->audio_channel = i;
      channel->line = j * audio_channels + i;

      playback = (AgsPlayback *) channel->playback;
      ags_playback_domain_insert_playback(playback_domain,
					  (GObject *) playback, channel_type,
					  channel->line);

      if(add_pattern){
	AgsPattern *pattern;

	pattern = ags_pattern_new();
	g_object_set(pattern,
		     "channel", channel,
		     NULL);
	
	g_object_set(channel,
		     "pattern", pattern,
		     NULL);
	  
	ags_pattern_set_dim(pattern,
			    bank_dim_0, bank_dim_1, bank_dim_2);
      }

      if(add_synth_generator){
	AgsSynthGenerator *synth_generator;

	synth_generator = ags_synth_generator_new();
	ags_input_add_synth_generator((AgsInput *) channel,
				      (GObject *) synth_generator);
      }
	
      if(start == NULL){
	g_rec_mutex_lock(audio_mutex);
	  
	start = channel;

	if(channel_type == AGS_TYPE_OUTPUT){
	  audio->output = channel;
	}else{
	  audio->input = channel;
	}

	g_rec_mutex_unlock(audio_mutex);
      }

      if(j * audio_channels + i != 0){
	AgsChannel *prev;

	prev = ags_channel_nth(start, j * audio_channels + i - 1);
	
	/* set prev next */
	if(prev != NULL){
	  prev_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev);
	  
	  g_rec_mutex_lock(prev_mutex);
	  
	  prev->next = channel;
	  
	  g_rec_mutex_unlock(prev_mutex);
	}
	
	/* set prev */
	channel->prev = prev;

	if(prev != NULL){
	  g_object_unref(prev);
	}

	/* set next */
	if(audio_channels_old != 0 &&
	   i == audio_channels - 1){
	  AgsChannel *next_pad;

	  pad_next = ags_channel_pad_nth(start, j + 1);
	  
	  channel->next = pad_next;
	  
	  if(pad_next != NULL){
	    next_pad_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(pad_next);
	    
	    g_rec_mutex_lock(next_pad_mutex);
	  
	    pad_next->prev = channel;

	    g_rec_mutex_unlock(next_pad_mutex);	  
	    
	    g_object_unref(pad_next);
	  }
	}
      }

      if(j != 0){
	AgsChannel *prev_pad;
	
	nth_channel = ags_channel_nth(start, i);

	/* get prev pad */
	prev_pad = ags_channel_pad_nth(nth_channel, j - 1);

	prev_pad_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(prev_pad);

	/* set prev pad next pad */
	g_rec_mutex_lock(prev_pad_mutex);
	  
	prev_pad->next_pad = channel;

	g_rec_mutex_unlock(prev_pad_mutex);

	/* set prev pad */	  
	channel->prev_pad = prev_pad;

	g_object_unref(nth_channel);
	g_object_unref(prev_pad);
      }

      /* reset nested AgsRecycling tree */
      if(add_recycling){
	first_recycling =
	  last_recycling = ags_recycling_new(output_soundcard);
	g_object_ref(first_recycling);
	g_object_set(first_recycling,
		     "channel", channel,
		     NULL);
	  
	ags_channel_reset_recycling(channel,
				    first_recycling, last_recycling);
      }else if(set_sync_link){
	AgsChannel *input;

	g_rec_mutex_lock(audio_mutex);

	input = audio->input;
	  
	g_rec_mutex_unlock(audio_mutex);

	input = ags_channel_nth(input,
				channel->line);
	g_object_unref(input);
	  
	/* set sync link */
	if(input != NULL){
	  first_recycling = input->first_recycling;
	  last_recycling = input->last_recycling;
	    
	  ags_channel_reset_recycling(channel,
				      first_recycling, last_recycling);
	}
      }else if(set_async_link){
	AgsChannel *start_input;
	AgsChannel *input, *input_pad_last, *nth_input;

	g_rec_mutex_lock(audio_mutex);

	start_input = audio->input;

	if(start_input != NULL){
	  g_object_ref(start_input);
	}
	  
	g_rec_mutex_unlock(audio_mutex);

	nth_input = ags_channel_nth(start_input,
				    i);
	g_object_unref(nth_input);
	  
	input = nth_input;
	  
	/* set async link */
	if(input != NULL){
	  input_pad_last = ags_channel_pad_last(input);

	  first_recycling = input->first_recycling;
	  last_recycling = input_pad_last->last_recycling;
	    
	  ags_channel_reset_recycling(channel,
				      first_recycling, last_recycling);

	  g_object_unref(input_pad_last);
	  g_object_unref(input);
	}
	  
	/* unref */
	if(start_input != NULL){
	  g_object_unref(start_input);
	}
      }
    }
  }

  /* unref */
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(input_soundcard != NULL){
    g_object_unref(input_soundcard);
  }

  if(start != NULL){
    g_object_unref(start);
  }

  g_object_unref(playback_domain);
}

void
ags_audio_set_audio_channels_shrink_zero(AgsAudio *audio)
{
  AgsChannel *channel, *start, *channel_next;
    
  gboolean first_run;
    
  GError *error;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);
	  
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

      g_error_free(error);
    }
      
    channel = channel->next;
  }

  channel = start;

  while(channel != NULL){
    channel_next = channel->next;

    g_object_run_dispose((GObject *) channel);

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

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_set_audio_channels_shrink(AgsAudio *audio,
				    guint audio_channels, guint audio_channels_old)
{
  AgsChannel *start;
  AgsChannel *channel, *nth_channel;
  AgsChannel *channel0, *channel1;
  AgsRecycling *recycling;

  guint pads, i, j;
  gboolean first_run;
    
  GError *error;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     audio_channels == audio_channels_old){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  start =
    channel = audio->output;
  pads = audio->output_pads;

  g_rec_mutex_unlock(audio_mutex);

  first_run = TRUE;

ags_audio_set_audio_channel_shrink0:

  for(i = 0; i < pads; i++){
    channel = ags_channel_nth(channel, audio_channels);
    g_object_unref(channel);
      
    for(j = audio_channels; j < audio_channels_old; j++){
      error = NULL;
      ags_channel_set_link(channel, NULL,
			   &error);
	
      if(error != NULL){
	g_error("%s", error->message);

	g_error_free(error);
      }
	
      channel = channel->next;
    }
  }

  channel = start;

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

    j++;
      
    for(; j < audio_channels_old; j++){
      channel1 = channel0->next;
      
      g_object_run_dispose((GObject *) channel0);

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

  if(first_run){
    first_run = FALSE;

    g_rec_mutex_lock(audio_mutex);

    start =
      channel = audio->input;
    pads = audio->input_pads;

    g_rec_mutex_unlock(audio_mutex);

    goto ags_audio_set_audio_channel_shrink0;
  }
}

void
ags_audio_set_audio_channels_shrink_notation(AgsAudio *audio,
					     guint audio_channels, guint audio_channels_old)
{
  GList *list_start, *list;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  list = 
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

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_set_audio_channels_shrink_automation(AgsAudio *audio,
					       guint audio_channels, guint audio_channels_old)
{
  GList *list_start, *list;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  list = 
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

  g_rec_mutex_unlock(audio_mutex);
}
  
void
ags_audio_set_audio_channels_shrink_wave(AgsAudio *audio,
					 guint audio_channels, guint audio_channels_old)
{
  GList *list_start, *list;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  list = 
    list_start = g_list_copy(audio->wave);

  while(list != NULL){
    if(AGS_WAVE(list->data)->line % audio_channels_old >= audio_channels){
      ags_audio_remove_wave(audio,
			    list->data);

      g_object_run_dispose((GObject *) list->data);
      g_object_unref((GObject *) list->data);
    }
      
    list = list->next;
  }

  g_list_free(list_start);

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_set_audio_channels_shrink_midi(AgsAudio *audio,
					 guint audio_channels, guint audio_channels_old)
{
  GList *list_start, *list;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  list = 
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

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsChannel *start_channel, *channel, *nth_channel, *next_channel;

  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;

  guint bank_dim[3];

  guint audio_flags;
  guint ability_flags;
  guint output_pads, input_pads;
  gboolean add_recycling;
  gboolean add_pattern;
  gboolean add_synth_generator;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput
  
  GRecMutex *audio_mutex;
    
  if(audio_channels == audio_channels_old){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* grow / shrink */
  g_rec_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  ability_flags = audio->ability_flags;
  
  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  g_rec_mutex_unlock(audio_mutex);

  /* get some fields */
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);
  
  if(audio_channels > audio_channels_old){
    guint i, j;
    
    /* grow input channels */
    if(input_pads > 0 &&
       (AGS_AUDIO_NO_INPUT & (audio_flags)) == 0){
      add_recycling = FALSE;
      add_pattern = FALSE;
      add_synth_generator = FALSE;

      link_recycling = FALSE;

      set_sync_link = FALSE;
      set_async_link = FALSE;
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	add_recycling = TRUE;
      }

      bank_dim[0] = 0;
      bank_dim[1] = 0;
      bank_dim[2] = 0;
      
      if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0){
	add_pattern = TRUE;

	g_rec_mutex_lock(audio_mutex);
	
	bank_dim[0] = audio->bank_dim[0];
	bank_dim[1] = audio->bank_dim[1];
	bank_dim[2] = audio->bank_dim[2];

	g_rec_mutex_unlock(audio_mutex);

      }
      
      if((AGS_AUDIO_INPUT_HAS_SYNTH & (audio_flags)) != 0){
	add_synth_generator = TRUE;
      }

      if((AGS_AUDIO_ASYNC & (audio_flags)) != 0 && add_recycling){
	link_recycling = TRUE;
      }

      ags_audio_set_audio_channels_grow(audio,
					AGS_TYPE_INPUT,
					audio_channels, audio_channels_old,
					bank_dim[0], bank_dim[1], bank_dim[2],
					add_recycling, add_pattern, add_synth_generator,
					link_recycling,
					set_sync_link, set_async_link);
    }

    /* apply new sizes */
    g_rec_mutex_lock(audio_mutex);
    
    audio->input_lines = audio_channels * audio->input_pads;

    g_rec_mutex_unlock(audio_mutex);

    /* grow output channels */
    if(audio->output_pads > 0 &&
       (AGS_AUDIO_NO_OUTPUT & (audio_flags)) == 0){
      add_recycling = FALSE;
      add_pattern = FALSE;
      add_synth_generator = FALSE;

      link_recycling = FALSE;

      set_sync_link = FALSE;
      set_async_link = FALSE;

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0){
	add_recycling = TRUE;
      }else{
	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	  if((AGS_AUDIO_SYNC & (audio_flags)) != 0 && (AGS_AUDIO_ASYNC & (audio_flags)) == 0){
	    set_sync_link = FALSE;
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	    set_async_link = TRUE;
	    set_sync_link = FALSE;
	  }else{
#ifdef AGS_DEBUG
	    g_message("ags_audio_set_audio_channels - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC aren't defined");
#endif
	    set_sync_link = FALSE;
	    set_async_link = FALSE;
	  }
	}
      }
      
      ags_audio_set_audio_channels_grow(audio,
					AGS_TYPE_OUTPUT,
					audio_channels, audio_channels_old,
					0, 0, 0,
					add_recycling, add_pattern, add_synth_generator,
					link_recycling,
					set_sync_link, set_async_link);
    }

    /* apply new sizes */
    g_rec_mutex_lock(audio_mutex);
    
    audio->output_lines = audio_channels * audio->output_pads;

    audio->audio_channels = audio_channels;

    g_rec_mutex_unlock(audio_mutex);

    /* set ability flags */
    g_object_get(audio,
		 "output", &start_channel,
		 NULL);

    if(start_channel != NULL){
      for(i = 0; i < output_pads; i++){
	nth_channel = ags_channel_nth(start_channel,
				      (i * audio_channels) + audio_channels_old);

	channel = nth_channel;

	if(channel != NULL){
	  g_object_ref(channel);
	}

	next_channel = NULL;
	
	for(j = 0; j < audio_channels - audio_channels_old; j++){
	  ags_channel_set_ability_flags(channel, ability_flags);
	  
	  /* iterate */
	  next_channel = ags_channel_next(channel);

	  if(channel != NULL){
	    g_object_unref(channel);
	  }
	  
	  channel = next_channel;
	}

	if(next_channel != NULL){
	  g_object_unref(next_channel);
	}
	
	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }

      g_object_unref(start_channel);
    }
    
    /* set ability flags */
    g_object_get(audio,
		 "input", &start_channel,
		 NULL);

    if(start_channel != NULL){
      for(i = 0; i < input_pads; i++){
	nth_channel = ags_channel_nth(start_channel,
				      (i * audio_channels) + audio_channels_old);

	channel = nth_channel;

	if(channel != NULL){
	  g_object_ref(channel);
	}
	
	next_channel = NULL;
	
	for(j = 0; j < audio_channels - audio_channels_old; j++){
	  ags_channel_set_ability_flags(channel, ability_flags);
	  
	  /* iterate */
	  next_channel = ags_channel_next(channel);

	  if(channel != NULL){
	    g_object_unref(channel);
	  }
	  
	  channel = next_channel;
	}

	if(next_channel != NULL){
	  g_object_unref(next_channel);
	}
	
	if(nth_channel != NULL){
	  g_object_unref(nth_channel);
	}
      }

      g_object_unref(start_channel);
    }
  }else if(audio_channels < audio_channels_old){
    GList *start_list, *list;
    
    guint i, j;
    
    GRecMutex *playback_domain_mutex;

    /* shrink audio channels */
    ags_audio_set_audio_channels_shrink_automation(audio,
						   audio_channels, audio_channels_old);
    
    if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_notation(audio,
						   audio_channels, audio_channels_old);
    }

    if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_wave(audio,
					       audio_channels, audio_channels_old);
    }

    if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0){
      ags_audio_set_audio_channels_shrink_midi(audio,
					       audio_channels, audio_channels_old);
    }

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero(audio);
    }else{
      ags_audio_set_audio_channels_shrink(audio,
					  audio_channels, audio_channels_old);
    }

    /* apply new sizes */
    g_rec_mutex_lock(audio_mutex);

    audio->audio_channels = audio_channels;
    
    audio->input_lines = audio_channels * audio->input_pads;
    audio->output_lines = audio_channels * audio->output_pads;

    g_rec_mutex_unlock(audio_mutex);
  }

  /* unref */
  g_object_unref(playback_domain);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
    message = ags_message_envelope_new((GObject *) audio,
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
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_set_audio_channels:
 * @audio: the #AgsAudio
 * @audio_channels: new audio channels
 * @audio_channels_old: old audio channels
 *
 * Resize audio channels AgsInput will be allocated first.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old)
{
  GRecMutex *audio_mutex;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get audio channels old */
  g_rec_mutex_lock(audio_mutex);

  audio_channels_old = audio->audio_channels;

  g_rec_mutex_unlock(audio_mutex);

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) audio);
}

/**
 * ags_audio_get_output_pads:
 * @audio: the #AgsAudio
 * 
 * Get output pads.
 * 
 * Returns: the output pads
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_output_pads(AgsAudio *audio)
{
  guint output_pads;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);

  return(output_pads);
}

/**
 * ags_audio_set_output_pads:
 * @audio: the #AgsAudio
 * @output_pads: output pads
 * 
 * Set output pads.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_output_pads(AgsAudio *audio,
			  guint output_pads)
{
  ags_audio_set_pads(audio,
		     AGS_TYPE_OUTPUT,
		     output_pads, 0);
}

/**
 * ags_audio_get_input_pads:
 * @audio: the #AgsAudio
 * 
 * Get input pads.
 * 
 * Returns: the input pads
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_input_pads(AgsAudio *audio)
{
  guint input_pads;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "input-pads", &input_pads,
	       NULL);

  return(input_pads);
}

/**
 * ags_audio_set_input_pads:
 * @audio: the #AgsAudio
 * @input_pads: input pads
 * 
 * Set input pads.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_input_pads(AgsAudio *audio,
			  guint input_pads)
{
  ags_audio_set_pads(audio,
		     AGS_TYPE_INPUT,
		     input_pads, 0);
}

void
ags_audio_set_pads_grow(AgsAudio *audio,
			GType channel_type,
			guint pads, guint pads_old,
			guint bank_dim_0, guint bank_dim_1, guint bank_dim_2,
			gboolean add_recycling, gboolean add_pattern, gboolean add_synth_generator,
			gboolean link_recycling,
			gboolean set_sync_link, gboolean set_async_link)
{
  AgsChannel *start;
  AgsChannel *channel, *nth_channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsPlaybackDomain *playback_domain;

  GObject *output_soundcard;
  GObject *input_soundcard;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint audio_channels;
  guint i, j;

  GRecMutex *audio_mutex;
  GRecMutex *prev_mutex, *prev_pad_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  g_object_get(audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "audio-channels", &audio_channels,
	       "output-soundcard", &output_soundcard,
	       "input-soundcard", &input_soundcard,
	       "playback-domain", &playback_domain,
	       NULL);
  
  if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    g_object_get(audio,
		 "output", &start,
		 NULL);
  }else{
    g_object_get(audio,
		 "input", &start,
		 NULL);
  }

  for(j = pads_old; j < pads; j++){
    AgsPlayback *playback;
      
    for(i = 0; i < audio_channels; i++){
      channel = (AgsChannel *) g_object_new(channel_type,
					    "audio", (GObject *) audio,
					    "output-soundcard", output_soundcard,
					    "output-soundcard-channel", i,
					    "input-soundcard", input_soundcard,
					    "input-soundcard-channel", i,
					    "samplerate", samplerate,
					    "buffer-size", buffer_size,
					    "format", format,
					    NULL);
      g_object_ref(channel);

      /* set indices */
      channel->pad = j;
      channel->audio_channel = i;
      channel->line = j * audio_channels + i;

      playback = (AgsPlayback *) channel->playback;
      ags_playback_domain_insert_playback(playback_domain,
					  (GObject *) playback, channel_type,
					  channel->line);

      if(add_pattern){
	AgsPattern *pattern;

	pattern = ags_pattern_new();
	g_object_set(pattern,
		     "channel", channel,
		     NULL);

	g_object_set(channel,
		     "pattern", pattern,
		     NULL);
	  
	ags_pattern_set_dim(pattern,
			    bank_dim_0, bank_dim_1, bank_dim_2);
      }

      if(add_synth_generator){
	AgsSynthGenerator *synth_generator;

	synth_generator = ags_synth_generator_new();
	ags_input_add_synth_generator((AgsInput *) channel,
				      (GObject *) synth_generator);
      }
	
      if(start == NULL){
	/* set first channel in AgsAudio */
	g_rec_mutex_lock(audio_mutex);
	  
	if(channel_type == AGS_TYPE_OUTPUT){
	  start = 
	    audio->output = channel;
	}else{
	  start = 
	    audio->input = channel;
	}

	g_rec_mutex_unlock(audio_mutex);
      }

      if(j * audio_channels + i != 0){
	/* set prev */
	channel->prev = ags_channel_nth(start, j * audio_channels + i - 1);
	g_object_unref(channel->prev);
	  
	/* get prev mutex */
	prev_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel->prev);

	/* set prev->next */
	g_rec_mutex_lock(prev_mutex);
	  
	channel->prev->next = channel;

	g_rec_mutex_unlock(prev_mutex);
      }
	
      if(j != 0){
	/* set prev pad */
	nth_channel = ags_channel_nth(start, i);
	  
	channel->prev_pad = ags_channel_pad_nth(nth_channel, j - 1);

	g_object_unref(nth_channel);
	g_object_unref(channel->prev_pad);
	  
	/* get prev pad mutex */
	prev_pad_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel->prev_pad);

	/* set prev_pad->next_pad */
	g_rec_mutex_lock(prev_pad_mutex);
	  
	channel->prev_pad->next_pad = channel;
	  
	g_rec_mutex_unlock(prev_pad_mutex);
      }

      /* reset nested AgsRecycling tree */
      if(add_recycling){
	first_recycling =
	  last_recycling = ags_recycling_new(output_soundcard);
	g_object_ref(first_recycling);
	g_object_set(first_recycling,
		     "channel", channel,
		     NULL);
	  
	ags_channel_reset_recycling(channel,
				    first_recycling, last_recycling);
      }else if(set_sync_link){
	AgsChannel *input;

	g_rec_mutex_lock(audio_mutex);

	input = audio->input;
	  
	g_rec_mutex_unlock(audio_mutex);

	input = ags_channel_nth(input,
				channel->line);
	g_object_unref(input);
	  
	/* set sync link */
	if(input != NULL){
	  first_recycling = input->first_recycling;
	  last_recycling = input->last_recycling;
	    
	  ags_channel_reset_recycling(channel,
				      first_recycling, last_recycling);
	}
      }else if(set_async_link){
	AgsChannel *start_input;
	AgsChannel *input, *input_pad_last, *nth_input;

	g_rec_mutex_lock(audio_mutex);

	start_input = audio->input;

	if(start_input != NULL){
	  g_object_ref(start_input);
	}
	  
	g_rec_mutex_unlock(audio_mutex);

	nth_input = ags_channel_nth(start_input,
				    i);
	g_object_unref(nth_input);
	  
	input = nth_input;

	/* set async link */
	if(input != NULL){
	  input_pad_last = ags_channel_pad_last(input);

	  first_recycling = input->first_recycling;
	  last_recycling = input_pad_last->last_recycling;
	    
	  ags_channel_reset_recycling(channel,
				      first_recycling, last_recycling);

	  g_object_unref(input_pad_last);
	  g_object_unref(input);
	}

	/* unref */
	if(start_input != NULL){
	  g_object_unref(start_input);
	}
      }
    }
  }

  /* unref */
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(input_soundcard != NULL){
    g_object_unref(input_soundcard);
  }


  if(start != NULL){
    g_object_unref(start);
  }

  g_object_unref(playback_domain);
}

void
ags_audio_set_pads_unlink(AgsAudio *audio,
			  GType channel_type,
			  guint pads)
{
  AgsChannel *channel, *next_channel;
  
  GError *error;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }  

  if(channel_type == AGS_TYPE_OUTPUT){
    channel = ags_channel_pad_nth(audio->output,
				  pads);
  }else{
    channel = ags_channel_pad_nth(audio->input,
				  pads);
  }  
  
  while(channel != NULL){
    error = NULL;
    ags_channel_set_link(channel, NULL,
			 &error);

    if(error != NULL){
      g_error("%s", error->message);

      g_error_free(error);
    }

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);
    
    channel = next_channel;
  }
}
  
void
ags_audio_set_pads_shrink_zero(AgsAudio *audio,
			       GType channel_type,
			       guint pads)
{
  AgsChannel *channel, *channel_next;

  GError *error;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  if(channel_type == AGS_TYPE_OUTPUT){
    channel = ags_channel_pad_nth(audio->output,
				  pads);
  }else{
    channel = ags_channel_pad_nth(audio->input,
				  pads);
  }
    
  while(channel != NULL){
    channel_next = channel->next;

#if 0
    error = NULL;
    ags_channel_set_link(channel, NULL,
			 &error);

    if(error != NULL){
      g_error("%s", error->message);

      g_error_free(error);
    }
#endif
      
    g_object_run_dispose((GObject *) channel);

    channel = channel_next;
  }
}

void
ags_audio_set_pads_shrink(AgsAudio *audio,
			  GType channel_type,
			  guint pads)
{
  AgsChannel *channel, *current;

  guint audio_channels;
  
  guint i;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  if(channel_type == AGS_TYPE_OUTPUT){
    channel = ags_channel_pad_nth(audio->output,
				  pads);
  }else{
    channel = ags_channel_pad_nth(audio->input,
				  pads);
  }
  
  g_object_unref(channel);
  current = channel;
    
  if(channel != NULL &&
     channel->prev_pad != NULL){
    channel = channel->prev_pad;
  }else{
    channel = NULL;
  }
    
  ags_audio_set_pads_shrink_zero(audio,
				 channel_type,
				 pads);

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

    if(current != NULL){
      g_object_unref(current);
      
      current->next = NULL;
    }
  }
}
  
void
ags_audio_set_pads_remove_notes(AgsAudio *audio,
				GType channel_type,
				guint pads)
{
  GList *notation;
  GList *note_start, *note;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

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

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_set_pads_shrink_automation(AgsAudio *audio,
				     GType channel_type,
				     guint pads)
{
  GList *list_start, *list;

  guint audio_channels;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  audio_channels = audio->audio_channels;
  
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

  g_rec_mutex_unlock(audio_mutex);
}

void
ags_audio_set_pads_shrink_wave(AgsAudio *audio,
			       GType channel_type,
			       guint pads)
{
  GList *list_start, *list;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

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

  g_rec_mutex_unlock(audio_mutex);
}
  
void
ags_audio_set_pads_shrink_midi(AgsAudio *audio,
			       GType channel_type,
			       guint pads)
{
  GList *list_start, *list;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);
    
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

  g_rec_mutex_unlock(audio_mutex);
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
  AgsChannel *start_channel, *channel, *next_channel;
  AgsPlaybackDomain *playback_domain;
  
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  
  guint bank_dim[3];

  guint audio_flags;
  guint ability_flags;
  guint behaviour_flags;
  guint audio_channels;
  guint output_pads, input_pads;
  gboolean add_recycling;
  gboolean add_pattern;
  gboolean add_synth_generator;
  gboolean link_recycling;
  gboolean set_sync_link, set_async_link;
  
  GRecMutex *audio_mutex;
  
  if(!(g_type_is_a(channel_type,
		   AGS_TYPE_OUTPUT) ||
       g_type_is_a(channel_type,
		   AGS_TYPE_INPUT))){
    g_warning("unknown channel type");

    return;
  }
  
  if(pads_old == pads){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get some fields */
  g_rec_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  ability_flags = audio->ability_flags;
  behaviour_flags = audio->behaviour_flags;
  
  audio_channels = audio->audio_channels;
  
  output_pads = audio->output_pads;
  input_pads = audio->input_pads;
  
  g_rec_mutex_unlock(audio_mutex);
  
  /* get some fields */
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  add_recycling = FALSE;
  add_pattern = FALSE;
  add_synth_generator = FALSE;

  link_recycling = FALSE;
  set_sync_link = FALSE;
  set_async_link = FALSE;
      
  if(g_type_is_a(channel_type,
		 AGS_TYPE_OUTPUT) &&
     (AGS_AUDIO_NO_OUTPUT & (audio_flags)) == 0){
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio_flags)) != 0){
      add_recycling = TRUE;
    }else{
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
	if((AGS_AUDIO_SYNC & (audio_flags)) != 0 && (AGS_AUDIO_ASYNC & (audio_flags)) == 0){
	  set_async_link = TRUE;
	}else if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	  set_async_link = TRUE;
	}else{
#ifdef AGS_DEBUG
	  g_message("ags_audio_set_pads - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC aren't defined");
#endif
	}
      }
    }

    /* output */
    if(audio_channels != 0){
      /* grow or shrink */
      if(pads > output_pads){
	guint i, j;

	/* grow channels */
	ags_audio_set_pads_grow(audio,
				AGS_TYPE_OUTPUT,
				pads, pads_old,
				0, 0, 0,
				add_recycling, add_pattern, add_synth_generator,
				link_recycling,
				set_sync_link, set_async_link);

	/* get some fields */
	g_object_get(audio,
		     "output", &start_channel,
		     NULL);

	/* set ability flags */
	channel = ags_channel_pad_nth(start_channel,
				      pads_old);
	
	for(j = pads_old; j < pads; j++){
	  for(i = 0; i < audio_channels; i++){
	    ags_channel_set_ability_flags(channel, ability_flags);
      
	    /* iterate */
	    next_channel = ags_channel_next(channel);

	    if(channel != NULL){
	      g_object_unref(channel);
	    }
	    
	    channel = next_channel;
	  }
	}

	if(start_channel != NULL){
	  g_object_unref(start_channel);
	}
      }else if(pads == 0){	
	GList *start_list, *list;
      
	ags_audio_set_pads_shrink_automation(audio,
					     AGS_TYPE_OUTPUT,
					     pads);

	if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (behaviour_flags)) != 0){
	  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0){
	    ags_audio_set_pads_remove_notes(audio,
					    AGS_TYPE_OUTPUT,
					    pads);
	  }

	  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0){
	    ags_audio_set_pads_shrink_wave(audio,
					   AGS_TYPE_OUTPUT,
					   pads);
	  }

	  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0){
	    ags_audio_set_pads_shrink_midi(audio,
					   AGS_TYPE_OUTPUT,
					   pads);
	  }
	}
	      
	/* unlink and remove */
	ags_audio_set_pads_unlink(audio,
				  AGS_TYPE_OUTPUT,
				  0);
	ags_audio_set_pads_shrink_zero(audio,
				       AGS_TYPE_OUTPUT,
				       0);
      
	g_rec_mutex_lock(audio_mutex);
	
	audio->output = NULL;

	g_rec_mutex_unlock(audio_mutex);
      }else if(pads < output_pads){
	GList *start_list, *list;

	/* get some fields */
	ags_audio_set_pads_shrink_automation(audio,
					     AGS_TYPE_OUTPUT,
					     pads);
	ags_audio_set_pads_remove_notes(audio,
					AGS_TYPE_OUTPUT,
					pads);
	
	ags_audio_set_pads_unlink(audio,
				  AGS_TYPE_OUTPUT,
				  pads);
	ags_audio_set_pads_shrink(audio,
				  AGS_TYPE_OUTPUT,
				  pads);
      }
    }
    
    /* apply new size */
    g_rec_mutex_lock(audio_mutex);
    
    audio->output_pads = pads;
    audio->output_lines = pads * audio_channels;

    g_rec_mutex_unlock(audio_mutex);

    //    if((AGS_AUDIO_SYNC & audio_flags) != 0 && (AGS_AUDIO_ASYNC & audio_flags) == 0){
    //TODO:JK: fix me
    //      input_pads = pads;
    //      audio->input_lines = pads * audio_channels;
    //    }
  }else if(g_type_is_a(channel_type,
		       AGS_TYPE_INPUT) &&
	   (AGS_AUDIO_NO_INPUT & (audio_flags)) == 0){
    if((AGS_AUDIO_INPUT_HAS_SYNTH & (audio_flags)) != 0){
      add_synth_generator = TRUE;
    }

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio_flags)) != 0){
      add_recycling = TRUE;
    }
      
    if((AGS_AUDIO_ASYNC & (audio_flags)) != 0 && add_recycling){
      link_recycling = TRUE;
    }

    bank_dim[0] = 0;
    bank_dim[1] = 0;
    bank_dim[2] = 0;

    g_rec_mutex_lock(audio_mutex);
      
    if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0){
      add_pattern = TRUE;

      bank_dim[0] = audio->bank_dim[0];
      bank_dim[1] = audio->bank_dim[1];
      bank_dim[2] = audio->bank_dim[2];
    }      

    g_rec_mutex_unlock(audio_mutex);

    /* input */
    if(audio_channels != 0){
      /* grow or shrink */
      if(pads > pads_old){
	AgsChannel *current;

	guint i, j;

	/* grow channels */
	ags_audio_set_pads_grow(audio,
				AGS_TYPE_INPUT,
				pads, pads_old,
				bank_dim[0], bank_dim[1], bank_dim[2],
				add_recycling, add_pattern, add_synth_generator,
				link_recycling,
				set_sync_link, set_async_link);

	/* get some fields */
	g_object_get(audio,
		     "input", &start_channel,
		     NULL);

	/* set ability flags */
	channel = ags_channel_pad_nth(start_channel,
				      pads_old);
	
	for(j = pads_old; j < pads; j++){
	  for(i = 0; i < audio_channels; i++){	    
	    ags_channel_set_ability_flags(channel, ability_flags);
      
	    /* iterate */
	    next_channel = ags_channel_next(channel);

	    if(channel != NULL){
	      g_object_unref(channel);
	    }
	    
	    channel = next_channel;
	  }
	}

	if(start_channel != NULL){
	  g_object_unref(start_channel);
	}
      }else if(pads == 0){
	GList *start_list, *list;
	
	ags_audio_set_pads_shrink_automation(audio,
					     AGS_TYPE_INPUT,
					     pads);

	if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (behaviour_flags)) != 0){
	  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0){
	    ags_audio_set_pads_remove_notes(audio,
					    AGS_TYPE_INPUT,
					    pads);
	  }

	  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0){
	    ags_audio_set_pads_shrink_wave(audio,
					   AGS_TYPE_INPUT,
					   pads);
	  }

	  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0){
	    ags_audio_set_pads_shrink_midi(audio,
					   AGS_TYPE_INPUT,
					   pads);
	  }
	}

	/* shrink channels */
	ags_audio_set_pads_unlink(audio,
				  AGS_TYPE_INPUT,
				  pads);
	ags_audio_set_pads_shrink_zero(audio,
				       AGS_TYPE_INPUT,
				       pads);
	
	g_rec_mutex_lock(audio_mutex);

	audio->input = NULL;  

	g_rec_mutex_unlock(audio_mutex);
      }else if(pads < pads_old){
	GList *start_list, *list;

	/* get some fields */
	ags_audio_set_pads_shrink_automation(audio,
					     AGS_TYPE_INPUT,
					     pads);
	
	ags_audio_set_pads_unlink(audio,
				  AGS_TYPE_INPUT,
				  pads);
	ags_audio_set_pads_shrink(audio,
				  AGS_TYPE_INPUT,
				  pads);
      }
    }

    /* apply new allocation */
    g_rec_mutex_lock(audio_mutex);
    
    audio->input_pads = pads;
    audio->input_lines = pads * audio_channels;
    
    g_rec_mutex_unlock(audio_mutex);    
  }

  /* unref */
  g_object_unref(playback_domain);

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
    message = ags_message_envelope_new((GObject *) audio,
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
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[0]),
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
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_set_pads:
 * @audio: the #AgsAudio
 * @channel_type: AGS_TYPE_INPUT or AGS_TYPE_OUTPUT
 * @pads: new pad count
 * @pads_old: old pad count
 *
 * Set pad count for the apropriate @channel_type
 *
 * Since: 3.0.0
 */
void
ags_audio_set_pads(AgsAudio *audio,
		   GType channel_type,
		   guint pads, guint pads_old)
{
  GRecMutex *audio_mutex;
  
  g_return_if_fail(AGS_IS_AUDIO(audio));

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* get output/input pads old */
  g_rec_mutex_lock(audio_mutex);

  pads_old = ((g_type_is_a(channel_type, AGS_TYPE_OUTPUT)) ? audio->output_pads: audio->input_pads);

  g_rec_mutex_unlock(audio_mutex);

  /* emit */
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		channel_type,
		pads, pads_old);
  g_object_unref((GObject *) audio);  
}

/**
 * ags_audio_get_output_lines:
 * @audio: the #AgsAudio
 * 
 * Get output lines.
 * 
 * Returns: the output lines
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_output_lines(AgsAudio *audio)
{
  guint output_lines;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "output-lines", &output_lines,
	       NULL);

  return(output_lines);
}

/**
 * ags_audio_get_input_lines:
 * @audio: the #AgsAudio
 * 
 * Get input lines.
 * 
 * Returns: the input lines
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_input_lines(AgsAudio *audio)
{
  guint input_lines;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "input-lines", &input_lines,
	       NULL);

  return(input_lines);
}

/**
 * ags_audio_get_output:
 * @audio: the #AgsAudio
 *
 * Get the output object of @audio.
 *
 * Returns: (transfer full): the output
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_audio_get_output(AgsAudio *audio)
{
  AgsChannel *output;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "output", &output,
	       NULL);

  return(output);
}

/**
 * ags_audio_get_input:
 * @audio: the #AgsAudio
 *
 * Get the input object of @audio.
 *
 * Returns: (transfer full): the input
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_audio_get_input(AgsAudio *audio)
{
  AgsChannel *input;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "input", &input,
	       NULL);

  return(input);
}

/**
 * ags_audio_get_output_soundcard:
 * @audio: the #AgsAudio
 *
 * Get the output soundcard object of @audio.
 *
 * Returns: (transfer full): the output soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_output_soundcard(AgsAudio *audio)
{
  GObject *output_soundcard;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  return(output_soundcard);
}

void
ags_audio_real_set_output_soundcard(AgsAudio *audio,
				    GObject *output_soundcard)
{
  AgsChannel *start_channel, *channel, *next_channel;
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *audio_thread;
  
  GObject *old_soundcard;
  
  GList *list;

  guint samplerate;
  guint buffer_size;
  guint format;
  guint i;
  
  GRecMutex *audio_mutex;
  GRecMutex *play_mutex, *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* old soundcard */
  g_rec_mutex_lock(audio_mutex);

  old_soundcard = audio->output_soundcard;

  g_rec_mutex_unlock(audio_mutex);
  
  if(old_soundcard == output_soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(output_soundcard != NULL){
    g_object_ref(output_soundcard);
  }
  
  g_rec_mutex_lock(audio_mutex);
  
  audio->output_soundcard = (GObject *) output_soundcard;

  g_rec_mutex_unlock(audio_mutex);

  if(output_soundcard != NULL){
    /* get presets */    
    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
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
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(start_channel);
  
    while(channel != NULL){
      /* reset */
      g_object_set(G_OBJECT(channel),
		   "output-soundcard", output_soundcard,
		   NULL);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* input */
  g_object_get(audio,
	       "input", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(start_channel);

    while(channel != NULL){
      /* reset */
      g_object_set(G_OBJECT(channel),
		   "output-soundcard", output_soundcard,
		   NULL);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }
  
    g_object_unref(start_channel);
  }
  
  /* playback domain - audio thread */
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  /* audio thread - output soundcard */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							i);
    
    if(audio_thread != NULL){
      /* set output soundcard */
      g_object_set(audio_thread,
		   "default-output-soundcard", output_soundcard,
		   NULL);

      g_object_unref(audio_thread);
    }
  }

  g_object_unref(playback_domain);
  
  /* get play mutex */
  play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

  /* play context */
  g_rec_mutex_lock(play_mutex);
  
  list = audio->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "output-soundcard", output_soundcard,
		 NULL);
    
    list = list->next;
  }
  
  g_rec_mutex_unlock(play_mutex);

  /* get recall mutex */  
  recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

  /* recall context */
  g_rec_mutex_lock(recall_mutex);

  list = audio->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "output-soundcard", output_soundcard,
		 NULL);
    
    list = list->next;
  } 

  g_rec_mutex_unlock(recall_mutex);

  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_audio_set_output_soundcard:
 * @audio: the #AgsAudio
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the output soundcard object of @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_output_soundcard(AgsAudio *audio, GObject *output_soundcard)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "output-soundcard", output_soundcard,
	       NULL);
}

/**
 * ags_audio_get_input_soundcard:
 * @audio: the #AgsAudio
 *
 * Get the input soundcard object of @audio.
 *
 * Returns: (transfer full): the input soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_input_soundcard(AgsAudio *audio)
{
  GObject *input_soundcard;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "input-soundcard", &input_soundcard,
	       NULL);

  return(input_soundcard);
}

void
ags_audio_real_set_input_soundcard(AgsAudio *audio,
				   GObject *input_soundcard)
{
  AgsChannel *start_channel, *channel, *next_channel;

  GObject *old_soundcard;
  
  GList *list;

  GRecMutex *audio_mutex;
  GRecMutex *play_mutex, *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* old soundcard */
  g_rec_mutex_lock(audio_mutex);

  old_soundcard = audio->input_soundcard;

  g_rec_mutex_unlock(audio_mutex);
  
  if(old_soundcard == input_soundcard){
    return;
  }

  /* ref and set new soundcard */
  if(input_soundcard != NULL){
    g_object_ref(input_soundcard);
  }
  
  g_rec_mutex_lock(audio_mutex);
  
  audio->input_soundcard = (GObject *) input_soundcard;

  g_rec_mutex_unlock(audio_mutex);  

  /* output */
  g_object_get(audio,
	       "output", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(start_channel);

    while(channel != NULL){
      /* reset */
      g_object_set(G_OBJECT(channel),
		   "input-soundcard", input_soundcard,
		   NULL);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }
  
    g_object_unref(start_channel);
  }
  
  /* input */
  g_object_get(audio,
	       "input", &start_channel,
	       NULL);

  if(start_channel != NULL){
    channel = start_channel;
    g_object_ref(start_channel);

    while(channel != NULL){
      /* reset */
      g_object_set(G_OBJECT(channel),
		   "input-soundcard", input_soundcard,
		   NULL);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    g_object_unref(start_channel);
  }
  
  /* get play mutex */
  play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

  /* play context */
  g_rec_mutex_lock(play_mutex);
  
  list = audio->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "input-soundcard", input_soundcard,
		 NULL);
    
    list = list->next;
  }
  
  g_rec_mutex_unlock(play_mutex);

  /* get recall mutex */  
  recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

  /* recall context */
  g_rec_mutex_lock(recall_mutex);

  list = audio->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "input-soundcard", input_soundcard,
		 NULL);
    
    list = list->next;
  } 

  g_rec_mutex_unlock(recall_mutex);

  /* unref old soundcard */
  if(old_soundcard != NULL){
    g_object_unref(old_soundcard);
  }
}

/**
 * ags_audio_set_input_soundcard:
 * @audio: an #AgsAudio
 * @input_soundcard: the #GObject implementing #AgsSoundcard
 *
 * Set the input soundcard object of @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_input_soundcard(AgsAudio *audio, GObject *input_soundcard)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "input-soundcard", input_soundcard,
	       NULL);
}

/**
 * ags_audio_get_output_sequencer:
 * @audio: the #AgsAudio
 *
 * Get the output sequencer object of @audio.
 *
 * Returns: (transfer full): the output sequencer
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_output_sequencer(AgsAudio *audio)
{
  GObject *output_sequencer;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "output-sequencer", &output_sequencer,
	       NULL);

  return(output_sequencer);
}

void
ags_audio_real_set_output_sequencer(AgsAudio *audio,
				    GObject *sequencer)
{
  GObject *old_sequencer;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* old sequencer */
  g_rec_mutex_lock(audio_mutex);

  old_sequencer = audio->output_sequencer;

  g_rec_mutex_unlock(audio_mutex);
  
  if(old_sequencer == sequencer){
    return;
  }

  /* ref and set new sequencer */
  if(sequencer != NULL){
    g_object_ref(sequencer);
  }
  
  g_rec_mutex_lock(audio_mutex);
  
  audio->output_sequencer = (GObject *) sequencer;

  g_rec_mutex_unlock(audio_mutex);  

  /* unref old sequencer */
  if(old_sequencer != NULL){
    g_object_unref(old_sequencer);
  }
}

/**
 * ags_audio_set_output_sequencer:
 * @audio: an #AgsAudio
 * @output_sequencer: the #GObject implementing #AgsSequencer
 *
 * Set the output sequencer object of @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_output_sequencer(AgsAudio *audio, GObject *output_sequencer)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "output-sequencer", output_sequencer,
	       NULL);
}

/**
 * ags_audio_get_input_sequencer:
 * @audio: the #AgsAudio
 *
 * Get the input sequencer object of @audio.
 *
 * Returns: (transfer full): the input sequencer
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_input_sequencer(AgsAudio *audio)
{
  GObject *input_sequencer;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "input-sequencer", &input_sequencer,
	       NULL);

  return(input_sequencer);
}

void
ags_audio_real_set_input_sequencer(AgsAudio *audio,
				   GObject *sequencer)
{
  GObject *old_sequencer;

  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */  
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* old sequencer */
  g_rec_mutex_lock(audio_mutex);

  old_sequencer = audio->input_sequencer;

  g_rec_mutex_unlock(audio_mutex);
  
  if(old_sequencer == sequencer){
    return;
  }

  /* ref and set new sequencer */
  if(sequencer != NULL){
    g_object_ref(sequencer);
  }
  
  g_rec_mutex_lock(audio_mutex);
  
  audio->input_sequencer = (GObject *) sequencer;

  g_rec_mutex_unlock(audio_mutex);  

  /* unref old sequencer */
  if(old_sequencer != NULL){
    g_object_unref(old_sequencer);
  }
}

/**
 * ags_audio_set_input_sequencer:
 * @audio: an #AgsAudio
 * @input_sequencer: the #GObject implementing #AgsSequencer
 *
 * Set the input sequencer object of @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_input_sequencer(AgsAudio *audio, GObject *input_sequencer)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "input-sequencer", input_sequencer,
	       NULL);
}

/**
 * ags_audio_get_samplerate:
 * @audio: the #AgsAudio
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_samplerate(AgsAudio *audio)
{
  guint samplerate;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

void
ags_audio_set_samplerate_channel(AgsChannel *start_channel, guint samplerate)
{
  AgsChannel *channel, *next_channel;

  if(!AGS_IS_CHANNEL(start_channel)){
    return;
  }

  channel = start_channel;
  g_object_ref(channel);
  
  while(channel != NULL){
    /* set samplerate */
    g_object_set(channel,
		 "samplerate", samplerate,
		 NULL);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_audio_real_set_samplerate(AgsAudio *audio, guint samplerate)
{
  AgsChannel *start_output, *start_input;
  AgsPlaybackDomain *playback_domain;
  
  AgsThread *audio_thread;
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  GList *start_list, *list;

  gdouble frequency;
  guint old_samplerate;
  guint i;
  
  GRecMutex *audio_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set samplerate */
  g_rec_mutex_lock(audio_mutex);

  old_samplerate = audio->samplerate;
  
  audio->samplerate = samplerate;

  frequency = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_rec_mutex_unlock(audio_mutex);

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "playback-domain", &playback_domain,
	       "synth-generator", &start_list,
	       NULL);
  
  /* audio thread - frequency */
  if(playback_domain != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);
      
      if(audio_thread != NULL){
	/* apply new frequency */
	g_object_set(audio_thread,
		     "frequency", frequency,
		     NULL);

	g_object_unref(audio_thread);
      }
    }
  }
  
  /* set samplerate output/input */
  ags_audio_set_samplerate_channel(start_output, samplerate);
  ags_audio_set_samplerate_channel(start_input, samplerate);
  
  /* set samplerate synth generator */
  list = start_list;
  
  while(list != NULL){
    g_object_set(list->data,
		 "samplerate", samplerate,
		 NULL);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
	       "AgsAudio::set-samplerate");

    /* add message */
    message = ags_message_envelope_new((GObject *) audio,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* samplerate */
    message->parameter_name[0] = "samplerate";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     samplerate);

    /* old samplerate */
    message->parameter_name[1] = "old-samplerate";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_samplerate);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_set_samplerate:
 * @audio: the #AgsAudio
 * @samplerate: the samplerate
 *
 * Sets samplerate.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_samplerate(AgsAudio *audio, guint samplerate)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "samplerate", samplerate,
	       NULL);
}

/**
 * ags_audio_get_buffer_size:
 * @audio: the #AgsAudio
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_buffer_size(AgsAudio *audio)
{
  guint buffer_size;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

void
ags_audio_set_buffer_size_channel(AgsChannel *start_channel, guint buffer_size)
{
  AgsChannel *channel, *next_channel;

  if(!AGS_IS_CHANNEL(start_channel)){
    return;
  }

  channel = start_channel;
  g_object_ref(channel);

  while(channel != NULL){
    /* set buffer size */
    g_object_set(channel,
		 "buffer-size", buffer_size,
		 NULL);
      
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_audio_real_set_buffer_size(AgsAudio *audio, guint buffer_size)
{
  AgsChannel *start_output, *start_input;
  AgsPlaybackDomain *playback_domain;

  AgsThread *audio_thread;
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  GList *start_list, *list;
  
  gdouble frequency;
  guint old_buffer_size;
  guint i;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set buffer size */
  g_rec_mutex_lock(audio_mutex);

  old_buffer_size = audio->buffer_size;
  
  audio->buffer_size = buffer_size;
  
  frequency = ceil((gdouble) audio->samplerate / (gdouble) audio->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  
  g_rec_mutex_unlock(audio_mutex);
  
  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "playback-domain", &playback_domain,
	       "synth-generator", &start_list,
	       NULL);

  /* audio thread - frequency */
  if(playback_domain != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);
      
      if(audio_thread != NULL){
	/* apply new frequency */
	g_object_set(playback_domain->audio_thread[i],
		     "frequency", frequency,
		     NULL);

	g_object_unref(audio_thread);
      }
    }  
  }
  
  /* set buffer size output/input */
  ags_audio_set_buffer_size_channel(start_output, buffer_size);
  ags_audio_set_buffer_size_channel(start_input, buffer_size);

  /* set buffer size synth generator */
  list = start_list;
  
  while(list != NULL){
    g_object_set(list->data,
		 "buffer-size", buffer_size,
		 NULL);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
	       "AgsAudio::set-buffer-size");

    /* add message */
    message = ags_message_envelope_new((GObject *) audio,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* buffer_size */
    message->parameter_name[0] = "buffer-size";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     buffer_size);

    /* old buffer_size */
    message->parameter_name[1] = "old-buffer-size";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_buffer_size);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_set_buffer_size:
 * @audio: the #AgsAudio
 * @buffer_size: the buffer length
 *
 * Sets buffer length.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_buffer_size(AgsAudio *audio, guint buffer_size)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "buffer-size", buffer_size,
	       NULL);
}

/**
 * ags_audio_get_format:
 * @audio: the #AgsAudio
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_format(AgsAudio *audio)
{
  guint format;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "format", &format,
	       NULL);

  return(format);
}

void
ags_audio_set_format_channel(AgsChannel *start_channel, guint format)
{
  AgsChannel *channel, *next_channel;

  if(!AGS_IS_CHANNEL(start_channel)){
    return;
  }

  channel = start_channel;
  g_object_ref(channel);

  while(channel != NULL){
    /* set format */
    g_object_set(channel,
		 "format", format,
		 NULL);
      
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}  

void
ags_audio_real_set_format(AgsAudio *audio, guint format)
{
  AgsChannel *start_output, *start_input;
  
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  GList *start_list, *list;

  guint old_format;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* set format */
  g_rec_mutex_lock(audio_mutex);

  old_format = audio->format;
  
  audio->format = format;

  g_rec_mutex_unlock(audio_mutex);
    
  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       "synth-generator", &start_list,
	       NULL);

  /* set format output/input */
  ags_audio_set_format_channel(start_output, format);
  ags_audio_set_format_channel(start_input, format);

  /* set format synth generator */
  list = start_list;
  
  while(list != NULL){
    g_object_set(list->data,
		 "format", format,
		 NULL);
    
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
	       "AgsAudio::set-format");

    /* add message */
    message = ags_message_envelope_new((GObject *) audio,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* format */
    message->parameter_name[0] = "format";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[0]),
		     format);

    /* old format */
    message->parameter_name[1] = "old-format";
    
    g_value_init(&(message->value[1]),
		 G_TYPE_UINT);
    g_value_set_uint(&(message->value[1]),
		     old_format);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_set_format:
 * @audio: the #AgsAudio
 * @format: the format
 *
 * Sets buffer length.
 *
 * Since: 3.0.0
 */
void
ags_audio_set_format(AgsAudio *audio, guint format)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "format", format,
	       NULL);
}

/**
 * ags_audio_get_bpm:
 * @audio: the #AgsAudio
 *
 * Gets bpm.
 * 
 * Returns: the bpm
 * 
 * Since: 3.1.0
 */
gdouble
ags_audio_get_bpm(AgsAudio *audio)
{
  gdouble bpm;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0.0);
  }

  g_object_get(audio,
	       "bpm", &bpm,
	       NULL);

  return(bpm);
}

/**
 * ags_audio_set_bpm:
 * @audio: the #AgsAudio
 * @bpm: the bpm
 *
 * Sets bpm.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_bpm(AgsAudio *audio, gdouble bpm)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "bpm", bpm,
	       NULL);
}

/**
 * ags_audio_get_audio_start_mapping:
 * @audio: the #AgsAudio
 *
 * Gets audio start mapping.
 * 
 * Returns: the audio start mapping
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_audio_start_mapping(AgsAudio *audio)
{
  guint audio_start_mapping;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       NULL);

  return(audio_start_mapping);
}

/**
 * ags_audio_set_audio_start_mapping:
 * @audio: the #AgsAudio
 * @audio_start_mapping: the audio start mapping
 *
 * Sets audio start mapping.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_audio_start_mapping(AgsAudio *audio, guint audio_start_mapping)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "audio-start-mapping", audio_start_mapping,
	       NULL);
}

/**
 * ags_audio_get_midi_start_mapping:
 * @audio: the #AgsAudio
 *
 * Gets midi start mapping.
 * 
 * Returns: the midi start mapping
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_midi_start_mapping(AgsAudio *audio)
{
  guint midi_start_mapping;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  return(midi_start_mapping);
}

/**
 * ags_audio_set_midi_start_mapping:
 * @audio: the #AgsAudio
 * @midi_start_mapping: the midi start mapping
 *
 * Sets midi start mapping.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_midi_start_mapping(AgsAudio *audio, guint midi_start_mapping)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "midi-start-mapping", midi_start_mapping,
	       NULL);
}

/**
 * ags_audio_get_midi_channel:
 * @audio: the #AgsAudio
 *
 * Gets midi channel.
 * 
 * Returns: the midi channel
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_midi_channel(AgsAudio *audio)
{
  guint midi_channel;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "midi-channel", &midi_channel,
	       NULL);

  return(midi_channel);
}

/**
 * ags_audio_set_midi_channel:
 * @audio: the #AgsAudio
 * @midi_channel: the midi channel
 *
 * Sets midi channel.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_midi_channel(AgsAudio *audio, guint midi_channel)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "midi-channel", midi_channel,
	       NULL);
}

/**
 * ags_audio_get_numerator:
 * @audio: the #AgsAudio
 *
 * Gets numerator.
 * 
 * Returns: the numerator
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_numerator(AgsAudio *audio)
{
  guint numerator;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "numerator", &numerator,
	       NULL);

  return(numerator);
}

/**
 * ags_audio_set_numerator:
 * @audio: the #AgsAudio
 * @numerator: the numerator
 *
 * Sets numerator.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_numerator(AgsAudio *audio, guint numerator)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "numerator", numerator,
	       NULL);
}

/**
 * ags_audio_get_denominator:
 * @audio: the #AgsAudio
 *
 * Gets denominator.
 * 
 * Returns: the denominator
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_denominator(AgsAudio *audio)
{
  guint denominator;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "denominator", &denominator,
	       NULL);

  return(denominator);
}

/**
 * ags_audio_set_denominator:
 * @audio: the #AgsAudio
 * @denominator: the denominator
 *
 * Sets denominator.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_denominator(AgsAudio *audio, guint denominator)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "denominator", denominator,
	       NULL);
}

/**
 * ags_audio_get_time_signature:
 * @audio: the #AgsAudio
 *
 * Gets time signature.
 * 
 * Returns: the time signature
 * 
 * Since: 3.1.0
 */
gchar*
ags_audio_get_time_signature(AgsAudio *audio)
{
  gchar *time_signature;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "time-signature", &time_signature,
	       NULL);

  return(time_signature);
}

/**
 * ags_audio_set_time_signature:
 * @audio: the #AgsAudio
 * @time_signature: the time signature
 *
 * Sets time signature.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_time_signature(AgsAudio *audio, gchar *time_signature)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "time-signature", time_signature,
	       NULL);
}

/**
 * ags_audio_get_is_minor:
 * @audio: the #AgsAudio
 *
 * Gets is minor.
 * 
 * Returns: is minor
 * 
 * Since: 3.1.0
 */
gboolean
ags_audio_get_is_minor(AgsAudio *audio)
{
  gboolean is_minor;
  
  if(!AGS_IS_AUDIO(audio)){
    return(FALSE);
  }

  g_object_get(audio,
	       "is-minor", &is_minor,
	       NULL);

  return(is_minor);
}

/**
 * ags_audio_set_is_minor:
 * @audio: the #AgsAudio
 * @is_minor: is minor
 *
 * Sets is minor.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_is_minor(AgsAudio *audio, gboolean is_minor)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "is-minor", is_minor,
	       NULL);
}

/**
 * ags_audio_get_sharp_flats:
 * @audio: the #AgsAudio
 *
 * Gets sharp flats.
 * 
 * Returns: the sharp flats
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_sharp_flats(AgsAudio *audio)
{
  guint sharp_flats;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "sharp-flats", &sharp_flats,
	       NULL);

  return(sharp_flats);
}

/**
 * ags_audio_set_sharp_flats:
 * @audio: the #AgsAudio
 * @sharp_flats: the sharp flats
 *
 * Sets sharp flats.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_sharp_flats(AgsAudio *audio, guint sharp_flats)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "sharp-flats", sharp_flats,
	       NULL);
}

/**
 * ags_audio_get_octave:
 * @audio: the #AgsAudio
 *
 * Gets octave.
 * 
 * Returns: the octave
 * 
 * Since: 3.1.0
 */
gint
ags_audio_get_octave(AgsAudio *audio)
{
  gint octave;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "octave", &octave,
	       NULL);

  return(octave);
}

/**
 * ags_audio_set_octave:
 * @audio: the #AgsAudio
 * @octave: the octave
 *
 * Sets octave.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_octave(AgsAudio *audio, gint octave)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "octave", octave,
	       NULL);
}

/**
 * ags_audio_get_key:
 * @audio: the #AgsAudio
 *
 * Gets key.
 * 
 * Returns: the key
 * 
 * Since: 3.1.0
 */
guint
ags_audio_get_key(AgsAudio *audio)
{
  guint key;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "key", &key,
	       NULL);

  return(key);
}

/**
 * ags_audio_set_key:
 * @audio: the #AgsAudio
 * @key: the key
 *
 * Sets key.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_key(AgsAudio *audio, guint key)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "key", key,
	       NULL);
}

/**
 * ags_audio_get_absolute_key:
 * @audio: the #AgsAudio
 *
 * Gets absolute key.
 * 
 * Returns: the absolute key
 * 
 * Since: 3.1.0
 */
gint
ags_audio_get_absolute_key(AgsAudio *audio)
{
  gint absolute_key;
  
  if(!AGS_IS_AUDIO(audio)){
    return(0);
  }

  g_object_get(audio,
	       "absolute-key", &absolute_key,
	       NULL);

  return(absolute_key);
}

/**
 * ags_audio_set_absolute_key:
 * @audio: the #AgsAudio
 * @absolute_key: the absolute key
 *
 * Sets absolute key.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_absolute_key(AgsAudio *audio, gint absolute_key)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "absolute-key", absolute_key,
	       NULL);
}

/**
 * ags_audio_get_preset:
 * @audio: the #AgsAudio
 * 
 * Get preset.
 * 
 * Returns: (element-type AgsAudio.Preset) (transfer full): the #GList-struct containig #AgsPreset
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_preset(AgsAudio *audio)
{
  GList *preset;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "preset", &preset,
	       NULL);

  return(preset);
}

/**
 * ags_audio_set_preset:
 * @audio: the #AgsAudio
 * @preset: (element-type AgsAudio.Preset) (transfer full): the #GList-struct containing #AgsPreset
 * 
 * Set preset by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_preset(AgsAudio *audio, GList *preset)
{
  GList *start_preset;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_preset = audio->preset;
  audio->preset = preset;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_preset,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Adds a preset.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_preset(AgsAudio *audio,
		     GObject *preset)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_PRESET(preset)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add preset */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->preset,
		 preset) == NULL){
    success = TRUE;
    
    g_object_ref(preset);
    audio->preset = g_list_prepend(audio->preset,
				   preset);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(preset,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_preset:
 * @audio: the #AgsAudio
 * @preset: an #AgsPreset
 *
 * Removes a preset.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_preset(AgsAudio *audio,
			GObject *preset)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_PRESET(preset)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove preset */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->preset,
		 preset) != NULL){
    success = TRUE;
    
    audio->preset = g_list_remove(audio->preset,
				  preset);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(preset,
		 "audio", NULL,
		 NULL);

    g_object_unref(preset);
  }
}

/**
 * ags_audio_get_playback_domain:
 * @audio: the #AgsAudio
 * 
 * Get playback domain.
 * 
 * Returns: (transfer full): the #AgsPlaybackDomain
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_playback_domain(AgsAudio *audio)
{
  GObject *playback_domain;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  return(playback_domain);
}

/**
 * ags_audio_set_playback_domain:
 * @audio: the #AgsAudio
 * @playback_domain: the #AgsPlaybackDomain
 * 
 * Set playback domain.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_playback_domain(AgsAudio *audio, GObject *playback_domain)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "playback-domain", playback_domain,
	       NULL);
}

/**
 * ags_audio_get_synth_generator:
 * @audio: the #AgsAudio
 * 
 * Get synth generator.
 * 
 * Returns: (element-type AgsAudio.SynthGenerator) (transfer full): the #GList-struct containing #AgsSynthGenerator
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_synth_generator(AgsAudio *audio)
{
  GList *synth_generator;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "synth-generator", &synth_generator,
	       NULL);

  return(synth_generator);
}

/**
 * ags_audio_set_synth_generator:
 * @audio: the #AgsAudio
 * @synth_generator: (element-type AgsAudio.SynthGenerator) (transfer full): the #GList-struct containing #AgsSynthGenerator
 * 
 * Set synth generator by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_synth_generator(AgsAudio *audio, GList *synth_generator)
{
  GList *start_synth_generator;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_synth_generator = audio->synth_generator;
  audio->synth_generator = synth_generator;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_synth_generator,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_synth_generator:
 * @audio: the #AgsAudio
 * @synth_generator: an #AgsSynthGenerator
 *
 * Adds a synth generator.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_synth_generator(AgsAudio *audio,
			      GObject *synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->synth_generator,
		 synth_generator) == NULL){
    g_object_ref(synth_generator);
    audio->synth_generator = g_list_prepend(audio->synth_generator,
					    synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_synth_generator:
 * @audio: the #AgsAudio
 * @synth_generator: an #AgsSynthGenerator
 *
 * Removes a synth generator.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_synth_generator(AgsAudio *audio,
				 GObject *synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->synth_generator,
		 synth_generator) != NULL){
    audio->synth_generator = g_list_remove(audio->synth_generator,
					   synth_generator);
    g_object_unref(synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_sf2_synth_generator:
 * @audio: the #AgsAudio
 * 
 * Get SF2 synth generator.
 * 
 * Returns: (element-type AgsAudio.SF2SynthGenerator) (transfer full): the #GList-struct containing #AgsSF2SynthGenerator
 * 
 * Since: 3.4.0
 */
GList*
ags_audio_get_sf2_synth_generator(AgsAudio *audio)
{
  GList *sf2_synth_generator;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "sf2-synth-generator", &sf2_synth_generator,
	       NULL);

  return(sf2_synth_generator);
}

/**
 * ags_audio_set_sf2_synth_generator:
 * @audio: the #AgsAudio
 * @sf2_synth_generator: (element-type AgsAudio.SF2SynthGenerator) (transfer full): the #GList-struct containing #AgsSF2SynthGenerator
 * 
 * Set SF2 synth generator by replacing existing.
 * 
 * Since: 3.4.0
 */
void
ags_audio_set_sf2_synth_generator(AgsAudio *audio, GList *sf2_synth_generator)
{
  GList *start_sf2_synth_generator;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_sf2_synth_generator = audio->sf2_synth_generator;
  audio->sf2_synth_generator = sf2_synth_generator;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_sf2_synth_generator,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_sf2_synth_generator:
 * @audio: the #AgsAudio
 * @sf2_synth_generator: an #AgsSF2SynthGenerator
 *
 * Adds a SF2 synth generator.
 *
 * Since: 3.4.0
 */
void
ags_audio_add_sf2_synth_generator(AgsAudio *audio,
				  GObject *sf2_synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add sf2_synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->sf2_synth_generator,
		 sf2_synth_generator) == NULL){
    g_object_ref(sf2_synth_generator);
    audio->sf2_synth_generator = g_list_prepend(audio->sf2_synth_generator,
						sf2_synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_sf2_synth_generator:
 * @audio: the #AgsAudio
 * @sf2_synth_generator: an #AgsSF2SynthGenerator
 *
 * Removes a SF2 synth generator.
 *
 * Since: 3.4.0
 */
void
ags_audio_remove_sf2_synth_generator(AgsAudio *audio,
				     GObject *sf2_synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove sf2_synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->sf2_synth_generator,
		 sf2_synth_generator) != NULL){
    audio->sf2_synth_generator = g_list_remove(audio->sf2_synth_generator,
					       sf2_synth_generator);
    g_object_unref(sf2_synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_sfz_synth_generator:
 * @audio: the #AgsAudio
 * 
 * Get SFZ synth generator.
 * 
 * Returns: (element-type AgsAudio.SFZSynthGenerator) (transfer full): the #GList-struct containing #AgsSFZSynthGenerator
 * 
 * Since: 3.4.0
 */
GList*
ags_audio_get_sfz_synth_generator(AgsAudio *audio)
{
  GList *sfz_synth_generator;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "sfz-synth-generator", &sfz_synth_generator,
	       NULL);

  return(sfz_synth_generator);
}

/**
 * ags_audio_set_sfz_synth_generator:
 * @audio: the #AgsAudio
 * @sfz_synth_generator: (element-type AgsAudio.SFZSynthGenerator) (transfer full): the #GList-struct containing #AgsSFZSynthGenerator
 * 
 * Set SFZ synth generator by replacing existing.
 * 
 * Since: 3.4.0
 */
void
ags_audio_set_sfz_synth_generator(AgsAudio *audio, GList *sfz_synth_generator)
{
  GList *start_sfz_synth_generator;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_sfz_synth_generator = audio->sfz_synth_generator;
  audio->sfz_synth_generator = sfz_synth_generator;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_sfz_synth_generator,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_sfz_synth_generator:
 * @audio: the #AgsAudio
 * @sfz_synth_generator: an #AgsSFZSynthGenerator
 *
 * Adds a SFZ synth generator.
 *
 * Since: 3.4.0
 */
void
ags_audio_add_sfz_synth_generator(AgsAudio *audio,
				  GObject *sfz_synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add sfz_synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->sfz_synth_generator,
		 sfz_synth_generator) == NULL){
    g_object_ref(sfz_synth_generator);
    audio->sfz_synth_generator = g_list_prepend(audio->sfz_synth_generator,
						sfz_synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_sfz_synth_generator:
 * @audio: the #AgsAudio
 * @sfz_synth_generator: an #AgsSFZSynthGenerator
 *
 * Removes a SFZ synth generator.
 *
 * Since: 3.4.0
 */
void
ags_audio_remove_sfz_synth_generator(AgsAudio *audio,
				     GObject *sfz_synth_generator)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove sfz_synth_generator */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->sfz_synth_generator,
		 sfz_synth_generator) != NULL){
    audio->sfz_synth_generator = g_list_remove(audio->sfz_synth_generator,
					       sfz_synth_generator);
    g_object_unref(sfz_synth_generator);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_cursor:
 * @audio: the #AgsAudio
 * 
 * Get cursor.
 * 
 * Returns: (element-type GObject) (transfer full): the #GList-struct containig #GObject implementing #AgsCursor
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_cursor(AgsAudio *audio)
{
  GList *cursor;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "cursor", &cursor,
	       NULL);

  return(cursor);
}

/**
 * ags_audio_set_cursor:
 * @audio: the #AgsAudio
 * @cursor: (element-type GObject) (transfer full): the #GList-struct containing #AgsCursor
 * 
 * Set cursor by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_cursor(AgsAudio *audio, GList *cursor)
{
  GList *start_cursor;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_cursor = audio->cursor;
  audio->cursor = cursor;
  
  g_rec_mutex_unlock(audio_mutex);
  
  g_list_free_full(start_cursor,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_cursor:
 * @audio: the #AgsAudio
 * @cursor: the #GObject implementing #AgsCursor
 *
 * Adds a cursor.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_cursor(AgsAudio *audio, GObject *cursor)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_CURSOR(cursor)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add cursor */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->cursor,
		 cursor) == NULL){
    success = TRUE;
    
    g_object_ref(cursor);
    audio->cursor = g_list_prepend(audio->cursor,
				   (GObject *) cursor);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(cursor,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_cursor:
 * @audio: the #AgsAudio
 * @cursor: the #GObject implementing #AgsCursor
 *
 * Removes a cursor.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_cursor(AgsAudio *audio, GObject *cursor)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_CURSOR(cursor)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove cursor */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->cursor,
		 cursor) != NULL){
    success = TRUE;
    
    audio->cursor = g_list_remove(audio->cursor,
				  cursor);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(cursor,
		 "audio", NULL,
		 NULL);
    
    g_object_unref(cursor);
  }
}

/**
 * ags_audio_get_notation:
 * @audio: the #AgsAudio
 * 
 * Get notation.
 * 
 * Returns: (element-type AgsAudio.Notation) (transfer full): the #GList-struct containig #AgsNotation
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_notation(AgsAudio *audio)
{
  GList *notation;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "notation", &notation,
	       NULL);

  return(notation);
}

/**
 * ags_audio_set_notation:
 * @audio: the #AgsAudio
 * @notation: (element-type AgsAudio.Notation) (transfer full): the #GList-struct containing #AgsNotation
 * 
 * Set notation by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_notation(AgsAudio *audio, GList *notation)
{
  GList *start_notation;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_notation = audio->notation;
  audio->notation = notation;
  
  g_rec_mutex_unlock(audio_mutex);
  
  g_list_free_full(start_notation,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 *
 * Adds a notation.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_notation(AgsAudio *audio, GObject *notation)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_NOTATION(notation)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add notation */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->notation,
		 notation) == NULL){
    success = TRUE;
    
    g_object_ref(notation);
    audio->notation = ags_notation_add(audio->notation,
				       (AgsNotation *) notation);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(notation,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_notation:
 * @audio: the #AgsAudio
 * @notation: the #AgsNotation
 *
 * Removes a notation.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_notation(AgsAudio *audio, GObject *notation)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_NOTATION(notation)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove notation */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->notation,
		 notation) != NULL){
    success = TRUE;
    
    audio->notation = g_list_remove(audio->notation,
				    notation);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(notation,
		 "audio", NULL,
		 NULL);
    
    g_object_unref(notation);
  }
}

/**
 * ags_audio_get_automation_port:
 * @audio: the #AgsAudio
 *
 * Get automation port.
 *
 * Returns: (transfer full): the string vector containing automation ports
 * 
 * Since: 3.1.0
 */
gchar**
ags_audio_get_automation_port(AgsAudio *audio)
{
  gchar **automation_port;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  automation_port = g_strdupv(audio->automation_port);
  
  g_rec_mutex_unlock(audio_mutex);

  return(automation_port);
}

/**
 * ags_audio_set_automation_port:
 * @audio: the #AgsAudio
 * @automation_port: (transfer full): the string vector containing automation ports
 * 
 *
 * Get automation port.
 *
 * Since: 3.1.0
 */
void
ags_audio_set_automation_port(AgsAudio *audio,
			      gchar **automation_port)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  g_strfreev(audio->automation_port);
  
  audio->automation_port = automation_port;
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_add_automation_port:
 * @audio: the #AgsAudio
 * @control_name: the control name
 *
 * Adds an automation port.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_automation_port(AgsAudio *audio, gchar *control_name)
{
  guint length;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     control_name == NULL){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  if(audio->automation_port != NULL &&
     g_strv_contains(audio->automation_port, control_name)){
    g_rec_mutex_unlock(audio_mutex);

    return;
  }

  if(audio->automation_port == NULL){
    audio->automation_port = (gchar **) malloc(2 * sizeof(gchar *));

    audio->automation_port[0] = g_strdup(control_name);
    audio->automation_port[1] = NULL;
  }else{
    length = g_strv_length(audio->automation_port);

    audio->automation_port = (gchar **) realloc(audio->automation_port,
						(length + 2) * sizeof(gchar *));

    audio->automation_port[length] = g_strdup(control_name);
    audio->automation_port[length + 1] = NULL;
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_automation_port:
 * @audio: the #AgsAudio
 * @control_name: the control name
 *
 * Removes an automation port.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_automation_port(AgsAudio *audio, gchar *control_name)
{
  gchar **automation_port;

  guint length;
  guint i, j;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     control_name == NULL){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  g_rec_mutex_lock(audio_mutex);

  if(!g_strv_contains(audio->automation_port, control_name)){
    g_rec_mutex_unlock(audio_mutex);

    return;
  }

  length = g_strv_length(audio->automation_port);

  if(length == 1){
    g_strfreev(audio->automation_port);

    audio->automation_port = NULL;
  }else{
    automation_port = (gchar **) malloc((length) * sizeof(gchar *));
    
    for(i = 0, j = 0; i < length; i++){
      if(!g_strcmp0(audio->automation_port[i], control_name)){
	g_free(audio->automation_port[i]);
      }else{
	automation_port[j] = audio->automation_port[i];
	
	j++;
      }
    }

    automation_port[j] = NULL;

    g_strfreev(audio->automation_port);

    audio->automation_port = automation_port;
  }
  
  g_rec_mutex_unlock(audio_mutex);
}


/**
 * ags_audio_get_automation:
 * @audio: the #AgsAudio
 * 
 * Get automation.
 * 
 * Returns: (element-type AgsAudio.Automation) (transfer full): the #GList-struct containig #AgsAutomation
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_automation(AgsAudio *audio)
{
  GList *automation;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "automation", &automation,
	       NULL);

  return(automation);
}

/**
 * ags_audio_set_automation:
 * @audio: the #AgsAudio
 * @automation: (element-type AgsAudio.Automation) (transfer full): the #GList-struct containing #AgsAutomation
 * 
 * Set automation by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_automation(AgsAudio *audio, GList *automation)
{
  GList *start_automation;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_automation = audio->automation;
  audio->automation = automation;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_automation,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Adds an automation.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_automation(AgsAudio *audio, GObject *automation)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add automation */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->automation,
		 automation) == NULL){
    success = TRUE;
    
    g_object_ref(automation);
    audio->automation = ags_automation_add(audio->automation,
					   (AgsAutomation *) automation);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(automation,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_automation:
 * @audio: the #AgsAudio
 * @automation: the #AgsAutomation
 *
 * Removes an automation.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_automation(AgsAudio *audio, GObject *automation)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_AUTOMATION(automation)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove automation */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->automation,
		 automation) != NULL){
    success = TRUE;
    
    audio->automation = g_list_remove(audio->automation,
				      automation);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(automation,
		 "audio", NULL,
		 NULL);

    g_object_unref(automation);
  }
}

/**
 * ags_audio_get_wave:
 * @audio: the #AgsAudio
 * 
 * Get wave.
 * 
 * Returns: (element-type AgsAudio.Wave) (transfer full): the #GList-struct containig #AgsWave
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_wave(AgsAudio *audio)
{
  GList *wave;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "wave", &wave,
	       NULL);

  return(wave);
}

/**
 * ags_audio_set_wave:
 * @audio: the #AgsAudio
 * @wave: (element-type AgsAudio.Wave) (transfer full): the #GList-struct containing #AgsWave
 * 
 * Set wave by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_wave(AgsAudio *audio, GList *wave)
{
  GList *start_wave;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_wave = audio->wave;
  audio->wave = wave;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_wave,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_wave:
 * @audio: the #AgsAudio
 * @wave: the #AgsWave
 *
 * Adds a wave.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_wave(AgsAudio *audio, GObject *wave)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add wave */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->wave,
		 wave) == NULL){
    success = TRUE;
    
    g_object_ref(wave);
    audio->wave = ags_wave_add(audio->wave,
			       (AgsWave *) wave);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(wave,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_wave:
 * @audio: the #AgsAudio
 * @wave: the #AgsWave
 *
 * Removes a wave.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_wave(AgsAudio *audio, GObject *wave)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove wave */
  success = FALSE;
  
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->wave,
		 wave) != NULL){
    success = TRUE;
    
    audio->wave = g_list_remove(audio->wave,
				wave);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(wave,
		 "audio", NULL,
		 NULL);

    g_object_unref(wave);
  }
}

/**
 * ags_audio_get_output_audio_file:
 * @audio: the #AgsAudio
 * 
 * Get output audio file.
 * 
 * Returns: (transfer full): the #GObject
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_output_audio_file(AgsAudio *audio)
{
  GObject *output_audio_file;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "output-audio-file", &output_audio_file,
	       NULL);

  return(output_audio_file);
}

/**
 * ags_audio_set_output_audio_file:
 * @audio: the #AgsAudio
 * @output_audio_file: the #GObject
 * 
 * Set output audio file.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_output_audio_file(AgsAudio *audio, GObject *output_audio_file)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "output-audio-file", output_audio_file,
	       NULL);
}

/**
 * ags_audio_get_input_audio_file:
 * @audio: the #AgsAudio
 * 
 * Get input audio file.
 * 
 * Returns: (transfer full): the #GObject
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_input_audio_file(AgsAudio *audio)
{
  GObject *input_audio_file;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "input-audio-file", &input_audio_file,
	       NULL);

  return(input_audio_file);
}

/**
 * ags_audio_set_input_audio_file:
 * @audio: the #AgsAudio
 * @input_audio_file: the #GObject
 * 
 * Set input audio file.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_input_audio_file(AgsAudio *audio, GObject *input_audio_file)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "input-audio-file", input_audio_file,
	       NULL);
}

/**
 * ags_audio_get_instrument_name:
 * @audio: the #AgsAudio
 *
 * Gets instrument name.
 * 
 * Returns: the instrument name
 * 
 * Since: 3.1.0
 */
gchar*
ags_audio_get_instrument_name(AgsAudio *audio)
{
  gchar *instrument_name;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "instrument-name", &instrument_name,
	       NULL);

  return(instrument_name);
}

/**
 * ags_audio_set_instrument_name:
 * @audio: the #AgsAudio
 * @instrument_name: the instrument name
 *
 * Sets instrument name.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_instrument_name(AgsAudio *audio, gchar *instrument_name)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "instrument-name", instrument_name,
	       NULL);
}

/**
 * ags_audio_get_track_name:
 * @audio: the #AgsAudio
 *
 * Gets track name.
 * 
 * Returns: the track name
 * 
 * Since: 3.1.0
 */
gchar*
ags_audio_get_track_name(AgsAudio *audio)
{
  gchar *track_name;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "track-name", &track_name,
	       NULL);

  return(track_name);
}

/**
 * ags_audio_set_track_name:
 * @audio: the #AgsAudio
 * @track_name: the track name
 *
 * Sets track name.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_track_name(AgsAudio *audio, gchar *track_name)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "track-name", track_name,
	       NULL);
}

/**
 * ags_audio_get_midi:
 * @audio: the #AgsAudio
 * 
 * Get midi.
 * 
 * Returns: (element-type AgsAudio.Midi) (transfer full): the #GList-struct containig #AgsMidi
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_midi(AgsAudio *audio)
{
  GList *midi;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "midi", &midi,
	       NULL);

  return(midi);
}

/**
 * ags_audio_set_midi:
 * @audio: the #AgsAudio
 * @midi: (element-type AgsAudio.Midi) (transfer full): the #GList-struct containing #AgsMidi
 * 
 * Set midi by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_midi(AgsAudio *audio, GList *midi)
{
  GList *start_midi;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_midi = audio->midi;
  audio->midi = midi;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_midi,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_midi:
 * @audio: the #AgsAudio
 * @midi: the #AgsMidi
 *
 * Adds a midi.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_midi(AgsAudio *audio, GObject *midi)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_MIDI(midi)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add midi */
  success = FALSE;

  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->midi,
		 midi) == NULL){
    success = TRUE;
    
    g_object_ref(midi);
    audio->midi = ags_midi_add(audio->midi,
			       (AgsMidi *) midi);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(midi,
		 "audio", audio,
		 NULL);
  }
}

/**
 * ags_audio_remove_midi:
 * @audio: the #AgsAudio
 * @midi: the #AgsMidi
 *
 * Removes a midi.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_midi(AgsAudio *audio, GObject *midi)
{
  gboolean success;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_MIDI(midi)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove midi */
  success = FALSE;

  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->midi,
		 midi) != NULL){
    success = TRUE;
    
    audio->midi = g_list_remove(audio->midi,
				midi);
  }
  
  g_rec_mutex_unlock(audio_mutex);

  if(success){
    g_object_set(midi,
		 "audio", NULL,
		 NULL);

    g_object_unref(midi);
  }
}

/**
 * ags_audio_get_output_midi_file:
 * @audio: the #AgsAudio
 * 
 * Get output MIDI file.
 * 
 * Returns: (transfer full): the #GObject
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_output_midi_file(AgsAudio *audio)
{
  GObject *output_midi_file;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "output-midi-file", &output_midi_file,
	       NULL);

  return(output_midi_file);
}

/**
 * ags_audio_set_output_midi_file:
 * @audio: the #AgsAudio
 * @output_midi_file: the #GObject
 * 
 * Set output MIDI file.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_output_midi_file(AgsAudio *audio, GObject *output_midi_file)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "output-midi-file", output_midi_file,
	       NULL);
}

/**
 * ags_audio_get_input_midi_file:
 * @audio: the #AgsAudio
 * 
 * Get input MIDI file.
 * 
 * Returns: (transfer full): the #GObject
 * 
 * Since: 3.1.0
 */
GObject*
ags_audio_get_input_midi_file(AgsAudio *audio)
{
  GObject *input_midi_file;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "input-midi-file", &input_midi_file,
	       NULL);

  return(input_midi_file);
}

/**
 * ags_audio_set_input_midi_file:
 * @audio: the #AgsAudio
 * @input_midi_file: the #GObject
 * 
 * Set input MIDI file.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_input_midi_file(AgsAudio *audio, GObject *input_midi_file)
{
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_set(audio,
	       "input-midi-file", input_midi_file,
	       NULL);
}

/**
 * ags_audio_get_recall_id:
 * @audio: the #AgsAudio
 * 
 * Get recall id.
 * 
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containig #AgsRecallID
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_recall_id(AgsAudio *audio)
{
  GList *recall_id;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "recall_id", &recall_id,
	       NULL);

  return(recall_id);
}

/**
 * ags_audio_set_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID
 * 
 * Set recall id by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_recall_id(AgsAudio *audio, GList *recall_id)
{
  GList *start_recall_id;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_recall_id = audio->recall_id;
  audio->recall_id = recall_id;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_recall_id,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Adds a recall id.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add recall id */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_id,
		 recall_id) == NULL){
    g_object_ref(recall_id);
    audio->recall_id = g_list_prepend(audio->recall_id,
				      recall_id);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recall_id:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID
 *
 * Removes a recall id.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove recall id */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_id,
		 recall_id) != NULL){
    audio->recall_id = g_list_remove(audio->recall_id,
				     recall_id);
    g_object_unref(recall_id);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_recycling_context:
 * @audio: the #AgsAudio
 * 
 * Get recycling_context.
 * 
 * Returns: (element-type AgsAudio.RecyclingContext) (transfer full): the #GList-struct containig #AgsRecyclingContext
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_recycling_context(AgsAudio *audio)
{
  GList *recycling_context;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "recycling_context", &recycling_context,
	       NULL);

  return(recycling_context);
}

/**
 * ags_audio_set_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: (element-type AgsAudio.RecyclingContext) (transfer full): the #GList-struct containing #AgsRecyclingContext
 * 
 * Set recycling_context by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_recycling_context(AgsAudio *audio, GList *recycling_context)
{
  GList *start_recycling_context;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_recycling_context = audio->recycling_context;
  audio->recycling_context = recycling_context;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_recycling_context,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Adds a recycling context.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add recycling context */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->recycling_context,
		 recycling_context) == NULL){
    g_object_ref(recycling_context);
    audio->recycling_context = g_list_prepend(audio->recycling_context,
					      recycling_context);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recycling_context:
 * @audio: the #AgsAudio
 * @recycling_context: the #AgsRecyclingContext
 *
 * Removes a recycling context.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_recycling_context(AgsAudio *audio, GObject *recycling_context)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove recycling container */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->recycling_context,
		 recycling_context) != NULL){
    audio->recycling_context = g_list_remove(audio->recycling_context,
					     recycling_context);
    g_object_unref(recycling_context);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_recall_container:
 * @audio: the #AgsAudio
 * 
 * Get recall_container.
 * 
 * Returns: (element-type AgsAudio.RecallContainer) (transfer full): the #GList-struct containig #AgsRecallContainer
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_recall_container(AgsAudio *audio)
{
  GList *recall_container;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "recall-container", &recall_container,
	       NULL);

  return(recall_container);
}

/**
 * ags_audio_set_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: (element-type AgsAudio.RecallContainer) (transfer full): the #GList-struct containing #AgsRecallContainer
 * 
 * Set recall_container by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_recall_container(AgsAudio *audio, GList *recall_container)
{
  GList *start_recall_container;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_recall_container = audio->recall_container;
  audio->recall_container = recall_container;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_recall_container,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Adds a recall container.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* add recall container */
  g_rec_mutex_lock(audio_mutex);
  
  if(g_list_find(audio->recall_container,
		 recall_container) == NULL){
    g_object_ref(recall_container);
    audio->recall_container = g_list_prepend(audio->recall_container,
					     recall_container);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_remove_recall_container:
 * @audio: the #AgsAudio
 * @recall_container: the #AgsRecallContainer
 *
 * Removes a recall container.
 *
 * Since: 3.0.0
 */
void
ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container)
{
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* remove recall container */
  g_rec_mutex_lock(audio_mutex);

  if(g_list_find(audio->recall_container,
		 recall_container) != NULL){
    audio->recall_container = g_list_remove(audio->recall_container,
					    recall_container);
    g_object_unref(recall_container);
  }
  
  g_rec_mutex_unlock(audio_mutex);
}

/**
 * ags_audio_get_play:
 * @audio: the #AgsAudio
 * 
 * Get play.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containig #AgsRecall
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_play(AgsAudio *audio)
{
  GList *play;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "play", &play,
	       NULL);

  return(play);
}

/**
 * ags_audio_set_play:
 * @audio: the #AgsAudio
 * @play: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Set play by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_play(AgsAudio *audio, GList *play)
{
  GList *start_play;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_play = audio->play;
  audio->play = play;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_play,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_get_recall:
 * @audio: the #AgsAudio
 * 
 * Get recall.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containig #AgsRecall
 * 
 * Since: 3.1.0
 */
GList*
ags_audio_get_recall(AgsAudio *audio)
{
  GList *recall;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  g_object_get(audio,
	       "recall", &recall,
	       NULL);

  return(recall);
}

/**
 * ags_audio_set_recall:
 * @audio: the #AgsAudio
 * @recall: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Set recall by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_audio_set_recall(AgsAudio *audio, GList *recall)
{
  GList *start_recall;
  
  GRecMutex *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
    
  g_rec_mutex_lock(audio_mutex);

  start_recall = audio->recall;
  audio->recall = recall;
  
  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_audio_add_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play_context: if %TRUE play context, else if %FALSE recall context
 *
 * Adds a recall to @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_add_recall(AgsAudio *audio, GObject *recall,
		     gboolean play_context)
{
  gboolean success;
  
  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  success = FALSE;
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(audio->play, recall) == NULL){
      success = TRUE;

      g_object_ref(G_OBJECT(recall));
    
      audio->play = g_list_prepend(audio->play,
				   recall);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(audio->recall, recall) == NULL){
      success = TRUE;
      
      g_object_ref(G_OBJECT(recall));
    
      audio->recall = g_list_prepend(audio->recall,
				     recall);
    }

    g_rec_mutex_unlock(recall_mutex);
  }

  if(success){
    if(AGS_IS_RECALL_AUDIO(recall) ||
       AGS_IS_RECALL_AUDIO_RUN(recall)){
      g_object_set(recall,
		   "audio", audio,
		   NULL);
    }
  }
}

/**
 * ags_audio_insert_recall:
 * @audio: the #AgsAudio
 * @recall: the #AgsRecall
 * @play_context: if %TRUE play context, else if %FALSE recall context
 * @position: the position
 *
 * Insert @recall at @position in @audio's @play_context.
 *
 * Since: 3.3.0
 */
void
ags_audio_insert_recall(AgsAudio *audio, GObject *recall,
			gboolean play_context,
			gint position)
{
  gboolean success;
  
  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  success = FALSE;
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(audio->play, recall) == NULL){
      success = TRUE;

      g_object_ref(G_OBJECT(recall));
    
      audio->play = g_list_insert(audio->play,
				  recall,
				  position);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(audio->recall, recall) == NULL){
      success = TRUE;
      
      g_object_ref(G_OBJECT(recall));
    
      audio->recall = g_list_insert(audio->recall,
				    recall,
				    position);
    }

    g_rec_mutex_unlock(recall_mutex);
  }

  if(success){
    if(AGS_IS_RECALL_AUDIO(recall) ||
       AGS_IS_RECALL_AUDIO_RUN(recall)){
      g_object_set(recall,
		   "audio", audio,
		   NULL);
    }
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
 * Since: 3.0.0
 */
void
ags_audio_remove_recall(AgsAudio *audio, GObject *recall,
			gboolean play_context)
{
  gboolean success;
  
  if(!AGS_IS_AUDIO(audio) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  success = FALSE;
  
  if(play_context){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(play_mutex);
    
    if(g_list_find(audio->play, recall) != NULL){
      audio->play = g_list_remove(audio->play,
				  recall);
    }

    g_rec_mutex_unlock(play_mutex);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* add recall */
    g_rec_mutex_lock(recall_mutex);

    if(g_list_find(audio->recall, recall) != NULL){
      audio->recall = g_list_remove(audio->recall,
				    recall);
    }

    g_rec_mutex_unlock(recall_mutex);
  }

#if 0
  if(success){
    if(AGS_IS_RECALL_AUDIO(recall) ||
       AGS_IS_RECALL_AUDIO_RUN(recall)){
      g_object_set(recall,
		   "audio", NULL,
		   NULL);
    }

    g_object_unref(G_OBJECT(recall));
  }
#endif
}

void
ags_audio_real_duplicate_recall(AgsAudio *audio,
				AgsRecallID *recall_id,
				guint pad, guint audio_channel,
				guint line)
{
  AgsRecall *recall, *copy_recall;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *list_start, *list;

  guint sound_scope;
  guint current_staging_flags;
  gboolean play_context;
  
  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only duplicate for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate list */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list_start = g_list_copy_deep(audio->play,
				  (GCopyFunc) g_object_ref,
				  NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    play_context = FALSE;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list_start = g_list_copy_deep(audio->recall,
				  (GCopyFunc) g_object_ref,
				  NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* notify run */  
  //  ags_recall_notify_dependency(AGS_RECALL(list->data), AGS_RECALL_NOTIFY_RUN, 1);
  
  /* return if already played */
  g_rec_mutex_lock(recall_id_mutex);

  if(ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_WAITING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_ACTIVE) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_PROCESSING) ||
     ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_TERMINATING)){
    g_list_free_full(list_start,
		     g_object_unref);

    if(parent_recycling_context != NULL){
      g_object_unref(parent_recycling_context);
    }
  
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    g_rec_mutex_unlock(recall_id_mutex);
    
    return;
  }

//  ags_recall_id_set_state_flags(recall_id,
//				AGS_SOUND_STATE_IS_WAITING);

  g_rec_mutex_unlock(recall_id_mutex);
  
  /* duplicate */
  while(list != NULL){
    GRecMutex *current_recall_mutex;
    
    recall = AGS_RECALL(list->data);

    /* get current recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

    /* some checks */
    g_rec_mutex_lock(current_recall_mutex);
    
    if(AGS_IS_RECALL_AUDIO(recall) ||
       (AGS_RECALL_TEMPLATE & (recall->flags)) == 0 ||
       recall->recall_id != NULL ||
       !ags_recall_match_ability_flags_to_scope(recall,
						sound_scope)){
      list = list->next;

      g_rec_mutex_unlock(current_recall_mutex);
      
      continue;
    }  

    g_rec_mutex_unlock(current_recall_mutex);

    /* duplicate the recall */
    copy_recall = ags_recall_duplicate(recall, recall_id,
				       NULL, NULL, NULL);
      
    if(copy_recall == NULL){
      /* iterate */    
      list = list->next;

      continue;
    }

    g_object_set(copy_recall,
		 "pad", pad,
		 "audio-channel", audio_channel,
		 "line", line,
		 NULL);
    
#ifdef AGS_DEBUG
    g_message("recall duplicated: %s %s", G_OBJECT_TYPE_NAME(audio), G_OBJECT_TYPE_NAME(copy_recall));
#endif

    /* set appropriate sound scope */
    ags_recall_set_sound_scope(copy_recall, sound_scope);
    
    /* append to AgsAudio */
    ags_audio_add_recall(audio,
			 (GObject *) copy_recall,
			 play_context);
    g_signal_connect(copy_recall, "done",
		     G_CALLBACK(ags_audio_recall_done_callback), audio);
    
    /* connect */
    ags_connectable_connect(AGS_CONNECTABLE(copy_recall));

    /* notify run */
    ags_recall_notify_dependency(copy_recall, AGS_RECALL_NOTIFY_RUN, 1);

    /* iterate */
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}


/**
 * ags_audio_duplicate_recall:
 * @audio: the #AgsAudio
 * @recall_id: an #AgsRecallID
 * @pad: the pad
 * @audio_channel: the audio channel
 * @line: the line
 * 
 * Duplicate all #AgsRecall templates of @audio.
 *
 * Since: 3.0.0
 */
void
ags_audio_duplicate_recall(AgsAudio *audio,
			   AgsRecallID *recall_id,
			   guint pad, guint audio_channel,
			   guint line)
{  
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[DUPLICATE_RECALL], 0,
		recall_id,
		pad, audio_channel,
		line);
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
  
  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }

  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only resolve for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* resolve */
  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    ags_recall_resolve_dependency(recall);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_audio_resolve_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to use
 *
 * Performs resolving of recalls.
 *
 * Since: 3.0.0
 */
void
ags_audio_resolve_recall(AgsAudio *audio,
			 AgsRecallID *recall_id)
{  
  g_return_if_fail(AGS_IS_AUDIO(audio));

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

  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  if(sound_scope == -1){
    g_critical("can only init for specific sound scope");
    
    return;
  }

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) != 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

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
  
  g_list_free_full(list_start,
		   g_object_unref);

#if 0
  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);
#endif

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_audio_init_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to use or #NULL
 * @staging_flags: staging flags, see #AgsSoundStagingFlags-enum
 *
 * Initializes the recalls of @audio
 *
 * Since: 3.0.0
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

  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;
  
  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  if(ags_recall_id_check_state_flags(recall_id, AGS_SOUND_STATE_IS_TERMINATING)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

#if 0
  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    return;
  }
#endif
  
  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list_start = g_list_copy_deep(audio->play,
				  (GCopyFunc) g_object_ref,
				  NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list_start = g_list_copy_deep(audio->recall,
				  (GCopyFunc) g_object_ref,
				  NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* automate and play  */
  staging_flags = staging_flags & staging_mask;
  
  if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0){
    while(list != NULL){
      recall = AGS_RECALL(list->data);
      
      /* play stages */
      if(AGS_IS_RECALL_AUDIO(recall)){
	ags_recall_set_staging_flags(recall,
				     AGS_SOUND_STAGING_AUTOMATE);
	ags_recall_unset_staging_flags(recall,
				       AGS_SOUND_STAGING_AUTOMATE);
      }
      
      list = list->next;
    }
  }

  staging_flags = staging_flags & (~AGS_SOUND_STAGING_AUTOMATE);
  
  list = list_start;

  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* play stages */
    ags_recall_set_staging_flags(recall,
				 staging_flags);
    ags_recall_unset_staging_flags(recall,
				   staging_flags);

    list = list->next;
  }
  
  g_list_free_full(list_start,
		   g_object_unref);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }

  //FIXME:JK: uncomment
  //  ags_audio_set_staging_flags(audio, sound_scope,
  //			      staging_flags);
}

/**
 * ags_audio_play_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 * @staging_flags: staging flags, see #AgsSoundStagingFlags-enum
 *
 * Performs play for the specified @staging_flags.
 *
 * Since: 3.0.0
 */
void
ags_audio_play_recall(AgsAudio *audio,
		      AgsRecallID *recall_id, guint staging_flags)
{
  g_return_if_fail(AGS_IS_AUDIO(audio) && AGS_IS_RECALL_ID(recall_id));

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

  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);
  
  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

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
  
  g_list_free_full(list_start,
		   g_object_unref);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_audio_done_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Done processing audio data.
 *
 * Since: 3.0.0
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

  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  if(!AGS_IS_RECYCLING_CONTEXT(recycling_context) ||
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (current_staging_flags)) == 0 ||
     (AGS_SOUND_STAGING_RUN_INIT_POST & (current_staging_flags)) == 0){
    if(recycling_context != NULL){
      g_object_unref(recycling_context);
    }

    return;
  }

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

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

  g_list_free_full(list_start,
		   g_object_unref);

  ags_audio_set_staging_flags(audio, sound_scope,
			      staging_flags);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

/**
 * ags_audio_cancel_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Cancel processing audio data.
 *
 * Since: 3.0.0
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
  GList *match_start, *match;

  gint sound_scope;
  guint current_staging_flags;
  gboolean play_context;
  gboolean remove_recycling_context;
  
  static const guint staging_mask = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				     AGS_SOUND_STAGING_RUN_INIT_PRE |
				     AGS_SOUND_STAGING_RUN_INIT_INTER |
				     AGS_SOUND_STAGING_RUN_INIT_POST);

  GRecMutex *audio_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *recycling_context_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return;
  }
  
  /* get recall id mutex */
  recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

  /* get some fields */
  g_rec_mutex_lock(recall_id_mutex);

  sound_scope = recall_id->sound_scope;

  g_rec_mutex_unlock(recall_id_mutex);

  recycling_context = NULL;

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get staging flags */
  g_rec_mutex_lock(audio_mutex);

  current_staging_flags = audio->staging_flags[sound_scope];
  
  g_rec_mutex_unlock(audio_mutex);

  /* get recycling context mutex */
  recycling_context_mutex = AGS_RECYCLING_CONTEXT_GET_OBJ_MUTEX(recycling_context);

  /* get parent recycling context */
  parent_recycling_context = NULL;
  
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_rec_mutex_lock(recycling_context_mutex);

  remove_recycling_context = FALSE;
  
  if((AgsRecallID *) recycling_context->recall_id == recall_id){
    remove_recycling_context = TRUE;
  }
  
  g_rec_mutex_unlock(recycling_context_mutex);

  /* get the appropriate lists */
  if(parent_recycling_context == NULL){
    GRecMutex *play_mutex;

    play_context = TRUE;
    
    /* get play mutex */
    play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* copy play context */
    g_rec_mutex_lock(play_mutex);

    list = 
      list_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(play_mutex);

    /* reverse play context */
    list =
      list_start = g_list_reverse(list_start);
  }else{
    GRecMutex *recall_mutex;

    play_context = FALSE;
    
    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* copy recall context */
    g_rec_mutex_lock(recall_mutex);

    list = 
      list_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);
    
    g_rec_mutex_unlock(recall_mutex);

    /* reverse recall context */
    list =
      list_start = g_list_reverse(list_start);
  }

  /* cleanup  */
  match_start = NULL;

  while((list = ags_recall_find_recycling_context(list,
						  (GObject *) recycling_context)) != NULL){
    recall = AGS_RECALL(list->data);
    
    /* remove recall */
    ags_audio_remove_recall(audio, (GObject *) recall, play_context);
    match_start = g_list_prepend(match_start,
				 recall);

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* destroy  */
  match = match_start;
  
  while(match != NULL){
    recall = AGS_RECALL(match->data);

    /* destroy */
    ags_connectable_disconnect(AGS_CONNECTABLE(recall));
    g_object_run_dispose((GObject *) recall);
    
    match = match->next;
  }

  g_list_free_full(match_start,
		   g_object_unref);
}

/**
 * ags_audio_cleanup_recall:
 * @audio: the #AgsAudio
 * @recall_id: the #AgsRecallID to apply to
 *
 * Cleanup processing audio data.
 *
 * Since: 3.0.0
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

void
ags_audio_recall_done_callback(AgsRecall *recall,
			       AgsAudio *audio)
{
  AgsCancelAudio *cancel_audio;
  
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;

  gint sound_scope;
  
  if(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING) &&
     (AGS_IS_FX_PATTERN_AUDIO_PROCESSOR(recall) ||
      AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(recall) ||
      AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(recall))){
    sound_scope = ags_recall_get_sound_scope(recall);

    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    cancel_audio = ags_cancel_audio_new(audio,
					sound_scope);

    ags_task_launcher_add_task(task_launcher,
			       cancel_audio);

    g_object_unref(cancel_audio);
  }
}

GList*
ags_audio_real_start(AgsAudio *audio,
		     gint sound_scope)
{
  AgsChannel *channel;
  AgsRecycling *first_recycling;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsRecallID *audio_recall_id;
  AgsRecallID *channel_recall_id;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsThread *audio_loop;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  AgsApplicationContext *application_context;  

  GList *start_message_queue;
  GList *start_output_playback, *output_playback;
  GList *start_recall_id;
  
  guint audio_channels;
  guint output_pads;
  gint i;

  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);
  
  if(sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* add audio to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   (GObject *) audio);

  ags_audio_loop_set_flags(audio_loop, AGS_AUDIO_LOOP_PLAY_AUDIO);

  /* get some fields */
  playback_domain = NULL;
  start_output_playback = NULL;
  
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);
    
  g_object_get(playback_domain,
	       "output-playback", &start_output_playback,
	       NULL);

  /* initialize channel */
  start_recall_id = NULL;
  
  if(sound_scope >= 0){
    output_playback = start_output_playback;

    while(output_playback != NULL){
      playback = AGS_PLAYBACK(output_playback->data);

      current_recall_id = ags_playback_get_recall_id(playback,
						     sound_scope);

      if(current_recall_id == NULL){
	/* get some fields */
	channel = NULL;
	
	g_object_get(playback,
		     "channel", &channel,
		     NULL);

	g_object_get(channel,
		     "first-recycling", &first_recycling,
		     NULL);

	/* recycling context */
	recycling_context = ags_recycling_context_new(1);
	ags_audio_add_recycling_context(audio,
					(GObject *) recycling_context);
	ags_recycling_context_replace(recycling_context,
				      first_recycling,
				      0);

	/* create audio recall id */
	audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
				       "recycling-context", recycling_context,
				       NULL);
	ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
	ags_audio_add_recall_id(audio,
				(GObject *) audio_recall_id);

	g_object_set(recycling_context,
		     "recall-id", audio_recall_id,
		     NULL);
      
	/* create channel recall id */
	channel_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling-context", recycling_context,
					 NULL);
	ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
	ags_channel_add_recall_id(channel,
				  (GObject *) channel_recall_id);

	/* prepend recall id */
	start_recall_id = g_list_prepend(start_recall_id,
					 channel_recall_id);

	/* set playback's recall id */
	//NOTE:JK: we use audio recall id, although on AgsPlayback
	ags_playback_set_recall_id(playback,
				   audio_recall_id,
				   sound_scope);

	/* unref */
	g_object_unref(channel);
      
	g_object_unref(first_recycling);
      }else{
	start_recall_id = g_list_prepend(start_recall_id,
					 current_recall_id);
      }
      
      /* iterate */
      output_playback = output_playback->next;
    }

    /* play init */
    ags_audio_recursive_run_stage(audio,
				  sound_scope, staging_flags);

    output_playback = start_output_playback;

    /* add to start queue */
    if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  sound_scope);

      if(audio_thread != NULL){
	ags_audio_thread_set_processing(audio_thread,
					TRUE);

	g_object_unref(audio_thread);
      }
    }
    
    while(output_playback != NULL){
      playback = AGS_PLAYBACK(output_playback->data);

      if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	channel_thread = ags_playback_get_channel_thread(playback,
							 sound_scope);
      
	/* add to start queue */
	if(channel_thread != NULL){
	  ags_channel_thread_set_processing(channel_thread,
					    TRUE);

	  g_object_unref(channel_thread);
	}
      }      
      
      /* iterate */
      output_playback = output_playback->next;
    }    
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      output_playback = start_output_playback;

      while(output_playback != NULL){
	playback = AGS_PLAYBACK(output_playback->data);

	current_recall_id = ags_playback_get_recall_id(playback,
						       i);

	if(current_recall_id == NULL){
	  /* get some fields */
	  channel = NULL;
	  
	  g_object_get(playback,
		       "channel", &channel,
		       NULL);

	  g_object_get(channel,
		       "first-recycling", &first_recycling,
		       NULL);

	  /* recycling context */
	  recycling_context = ags_recycling_context_new(1);
	  ags_audio_add_recycling_context(audio,
					  (GObject *) recycling_context);

	  /* set recycling */
	  ags_recycling_context_replace(recycling_context,
					first_recycling,
					0);

	  /* create audio recall id */
	  audio_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					 "recycling-context", recycling_context,
					 NULL);
	  ags_recall_id_set_sound_scope(audio_recall_id, i);
	  ags_audio_add_recall_id(audio,
				  (GObject *) audio_recall_id);

	  g_object_set(recycling_context,
		       "recall-id", audio_recall_id,
		       NULL);
      
	  /* create channel recall id */
	  channel_recall_id = g_object_new(AGS_TYPE_RECALL_ID,
					   "recycling-context", recycling_context,
					   NULL);
	  ags_recall_id_set_sound_scope(channel_recall_id, i);
	  ags_channel_add_recall_id(channel,
				    (GObject *) channel_recall_id);

	  /* prepend recall id */
	  start_recall_id = g_list_prepend(start_recall_id,
					   channel_recall_id);

	  /* set playback's recall id */
	  //NOTE:JK: we use audio recall id, although on AgsPlayback
	  ags_playback_set_recall_id(playback,
				     audio_recall_id,
				     i);

	  /* unref */
	  g_object_unref(channel);
      
	  g_object_unref(first_recycling);
	}else{
	  start_recall_id = g_list_prepend(start_recall_id,
					   current_recall_id);
	}

	/* iterate */
	output_playback = output_playback->next;
      }

      /* play init */
      ags_audio_recursive_run_stage(audio,
				    i, staging_flags);

      output_playback = start_output_playback;

      /* add to start queue */
      if(ags_playback_domain_test_flags(playback_domain, AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO)){
	audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							    i);

	if(audio_thread != NULL){
	  ags_audio_thread_set_processing(audio_thread,
					  TRUE);

	  g_object_unref(audio_thread);
	}
      }
      
      while(output_playback != NULL){
	playback = AGS_PLAYBACK(output_playback->data);

	if(ags_playback_test_flags(playback, AGS_PLAYBACK_SUPER_THREADED_CHANNEL)){
	  channel_thread = ags_playback_get_channel_thread(playback,
							   i);

	  if(channel_thread != NULL){
	    ags_channel_thread_set_processing(channel_thread,
					      TRUE);

	    g_object_unref(channel_thread);
	  }
	}
	
	/* iterate */
	output_playback = output_playback->next;
      }
    }
  }

  if(audio_loop != NULL){
    g_object_unref(audio_loop);
  }
  
  g_object_unref(playback_domain);
  
  g_list_free_full(start_output_playback,
		   g_object_unref);

  start_recall_id = g_list_reverse(start_recall_id);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
    message = ags_message_envelope_new((GObject *) audio,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* sound scope */
    message->parameter_name[0] = "sound-scope";
    g_value_init(&(message->value[0]),
		 G_TYPE_INT);
    g_value_set_int(&(message->value[0]),
		    sound_scope);

    /* recall id */
    message->parameter_name[1] = "recall-id";
    g_value_init(&(message->value[1]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[1]),
			g_list_copy_deep(start_recall_id,
					 (GCopyFunc) g_object_ref,
					 NULL));

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }

  return(start_recall_id);
}

/**
 * ags_audio_start:
 * @audio: the #AgsAudio
 * @sound_scope: the scope
 *
 * Is emitted as audio is started.
 *
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the #GList-struct containing #AgsRecallID
 * 
 * Since: 3.0.0
 */
GList*
ags_audio_start(AgsAudio *audio,
		gint sound_scope)
{
  GList *recall_id;
  
  g_return_val_if_fail(AGS_IS_AUDIO(audio), NULL);
  
  /* emit */
  recall_id = NULL;
  
  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[START], 0,
		sound_scope,
		&recall_id);
  g_object_unref((GObject *) audio);

  return(recall_id);
}

void
ags_audio_real_stop(AgsAudio *audio,
		    GList *recall_id, gint sound_scope)
{
  AgsChannel *channel;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsRecallID *sequencer_recall_id, *notation_recall_id, *wave_recall_id, *midi_recall_id;
  
  AgsThread *audio_loop;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  AgsMessageDelivery *message_delivery;

  AgsApplicationContext *application_context;

  GList *list;
  GList *start_message_queue;
  GList *start_output_playback, *output_playback;
  GList *sequencer, *notation, *wave, *midi;
  
  gint i;
  
  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL |
				      AGS_SOUND_STAGING_REMOVE);
  
  if(recall_id == NULL ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return;
  }

  list = recall_id;

  while(list != NULL){
    ags_recall_id_set_state_flags(list->data, AGS_SOUND_STATE_IS_TERMINATING);
    
    list = list->next;
  }

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get some fields */
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  g_object_get(playback_domain,
	       "output-playback", &start_output_playback,
	       NULL);

  if(sound_scope >= 0){
    /* stop thread */
    audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							sound_scope);

    if(audio_thread != NULL){
      ags_audio_thread_set_processing(audio_thread,
				      FALSE);

      g_object_unref(audio_thread);
    }

    output_playback = start_output_playback;
    
    while(output_playback != NULL){
      playback = AGS_PLAYBACK(output_playback->data);

      channel = NULL;
      
      g_object_get(playback,
		   "channel", &channel,
		   NULL);

      channel_thread = ags_playback_get_channel_thread(playback,
						       sound_scope);

      if(channel_thread != NULL){
	ags_channel_thread_set_processing(channel_thread,
					  FALSE);

	g_object_unref(channel_thread);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      /* iterate */
      output_playback = output_playback->next;
    }

    /* cancel */
    ags_audio_recursive_run_stage(audio,
				  sound_scope, staging_flags);

    /* clean - fini */
    ags_audio_recursive_run_stage(audio,
				  sound_scope, AGS_SOUND_STAGING_FINI);

    output_playback = start_output_playback;
    
    while(output_playback != NULL){
      playback = AGS_PLAYBACK(output_playback->data);

      ags_playback_set_recall_id(playback,
				 NULL,
				 sound_scope);
      
      /* iterate */
      output_playback = output_playback->next;
    }
  }else{
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      /* stop thread */
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);

      if(audio_thread != NULL){
	ags_audio_thread_set_processing(audio_thread,
					FALSE);

	g_object_unref(audio_thread);
      }

      output_playback = start_output_playback;
    
      while(output_playback != NULL){
	playback = AGS_PLAYBACK(output_playback->data);

	channel = NULL;
	
	g_object_get(playback,
		     "channel", &channel,
		     NULL);

	channel_thread = ags_playback_get_channel_thread(playback,
							 i);

	if(channel_thread != NULL){
	  ags_channel_thread_set_processing(channel_thread,
					    FALSE);

	  g_object_unref(channel_thread);
	}

	if(channel != NULL){
	  g_object_unref(channel);
	}
	
	/* iterate */
	output_playback = output_playback->next;
      }
    }
    
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      /* cancel */
      ags_audio_recursive_run_stage(audio,
				    i, staging_flags);

      /* clean - fini */
      ags_audio_recursive_run_stage(audio,
				    i, AGS_SOUND_STAGING_FINI);

      output_playback = start_output_playback;
    
      while(output_playback != NULL){
	playback = AGS_PLAYBACK(output_playback->data);

	ags_playback_set_recall_id(playback,
				   NULL,
				   i);
      
	/* iterate */
	output_playback = output_playback->next;
      }
    }
  }

  list = recall_id;

  while(list != NULL){
    ags_audio_remove_recall_id(audio,
			       (GObject *) list->data);
    
    list = list->next;
  }

  /* remove audio from AgsAudioLoop */
  sequencer = ags_audio_check_scope(audio,
				    (AGS_SOUND_SCOPE_SEQUENCER));
  sequencer_recall_id = ags_recall_id_find_parent_recycling_context(sequencer,
								    NULL);

  notation = ags_audio_check_scope(audio,
				   (AGS_SOUND_SCOPE_NOTATION));
  notation_recall_id = ags_recall_id_find_parent_recycling_context(notation,
								   NULL);

  wave = ags_audio_check_scope(audio,
			       (AGS_SOUND_SCOPE_WAVE));
  wave_recall_id = ags_recall_id_find_parent_recycling_context(wave,
							       NULL);

  midi = ags_audio_check_scope(audio,
			       (AGS_SOUND_SCOPE_MIDI));
  midi_recall_id = ags_recall_id_find_parent_recycling_context(midi,
							       NULL);

  if(sequencer_recall_id == NULL &&
     notation_recall_id == NULL &&
     wave_recall_id == NULL &&
     midi_recall_id == NULL){
    ags_audio_loop_remove_audio(audio_loop,
				(GObject *) audio);
  }

  g_list_free_full(sequencer,
		   g_object_unref);
  g_list_free_full(notation,
		   g_object_unref);
  g_list_free_full(wave,
		   g_object_unref);
  g_list_free_full(midi,
		   g_object_unref);
  
  g_object_unref(playback_domain);
  
  g_list_free_full(start_output_playback,
		   g_object_unref);
  
  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
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
    message = ags_message_envelope_new((GObject *) audio,
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
		 G_TYPE_INT);
    g_value_set_int(&(message->value[1]),
		    sound_scope);

    /* terminate string vector */
    message->parameter_name[2] = NULL;

    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

/**
 * ags_audio_stop:
 * @audio: the #AgsAudio
 * @recall_id: (element-type AgsAudio.RecallID) (transfer none): the #GList-struct containing #AgsRecallID
 * @sound_scope: the scope
 *
 * Is emitted as playing audio is stopped.
 *
 * Since: 3.0.0
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

  /* get recall id */
  g_object_get(audio,
	       "recall-id", &list_start,
	       NULL);

  /* iterate recall id */
  list = list_start;

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

  /* reverse recall id */
  recall_id = g_list_reverse(recall_id);
  g_list_foreach(recall_id,
		 (GFunc) g_object_ref,
		 NULL);

  /* unref */
  g_list_free_full(list_start,
		   g_object_unref);
  
  return(recall_id);
}

/**
 * ags_audio_check_scope:
 * @audio: the #AgsAudio
 * @sound_scope: the scope
 *
 * Check scope's recall id.
 * 
 * Returns: (element-type AgsAudio.RecallID) (transfer full): the scope's recall id of @audio
 * 
 * Since: 3.0.0
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
 * Returns: (element-type AgsAudio.Port) (transfer full): a new #GList containing #AgsPort
 *
 * Since: 3.0.0
 */
GList*
ags_audio_collect_all_audio_ports(AgsAudio *audio)
{
  GList *recall_start, *recall;
  GList *list;

  GRecMutex *recall_mutex, *play_mutex;
  GRecMutex *mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  list = NULL;
 
  /* get play mutex */  
  play_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

  /* collect port of playing recall */
  g_rec_mutex_lock(play_mutex);

  recall =
    recall_start = g_list_copy_deep(audio->play,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(play_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* concat port */
    g_rec_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy_deep(current->port,
				(GCopyFunc) g_object_ref,
				NULL);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy_deep(current->port,
						(GCopyFunc) g_object_ref,
						NULL));
	}
      }
    }

    g_rec_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(recall_start,
		   g_object_unref);
  
  /* get recall mutex */  
  recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

  /* the same for true recall */
  g_rec_mutex_lock(recall_mutex);

  recall =
    recall_start = g_list_copy_deep(audio->recall,
				    (GCopyFunc) g_object_ref,
				    NULL);

  g_rec_mutex_unlock(recall_mutex);
   
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* concat port */
    g_rec_mutex_lock(mutex);
    
    if(current->port != NULL){
      if(list == NULL){
	list = g_list_copy_deep(current->port,
				(GCopyFunc) g_object_ref,
				NULL);
      }else{
	if(current->port != NULL){
	  list = g_list_concat(list,
			       g_list_copy_deep(current->port,
						(GCopyFunc) g_object_ref,
						NULL));
	}
      }
    }

    g_rec_mutex_unlock(mutex);

    /* iterate */
    recall = recall->next;
  }
   
  g_list_free_full(recall_start,
		   g_object_unref);

  /* reverse result */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_audio_collect_all_audio_ports_by_specifier_and_context:
 * @audio: an #AgsAudio
 * @specifier: the port's name
 * @play_context: either %TRUE for play or %FALSE for recall
 *
 * Retrieve specified port of #AgsAudio
 *
 * Returns: (element-type AgsAudio.Port) (transfer full): a #GList-struct of #AgsPort if found, otherwise %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_audio_collect_all_audio_ports_by_specifier_and_context(AgsAudio *audio,
							   gchar *specifier,
							   gboolean play_context)
{
  GList *recall_start, *recall;
  GList *port_start, *port;
  GList *list;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }
  
  if(play_context){
    /* get play mutex */
    recall_mutex = AGS_AUDIO_GET_PLAY_MUTEX(audio);

    /* get recall */
    g_rec_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy_deep(audio->play,
				      (GCopyFunc) g_object_ref,
				      NULL);
    
    g_rec_mutex_unlock(recall_mutex);
  }else{
    /* get recall mutex */
    recall_mutex = AGS_AUDIO_GET_RECALL_MUTEX(audio);

    /* get recall */
    g_rec_mutex_lock(recall_mutex);

    recall =
      recall_start = g_list_copy_deep(audio->recall,
				      (GCopyFunc) g_object_ref,
				      NULL);
    
    g_rec_mutex_unlock(recall_mutex);
  }
  
  /* collect port of playing recall */
  list = NULL;

  while(recall != NULL){
    AgsRecall *current;

    GRecMutex *mutex;

    current = AGS_RECALL(recall->data);
    
    /* get mutex */  
    mutex = AGS_RECALL_GET_OBJ_MUTEX(current);

    /* get port */
    g_rec_mutex_lock(mutex);
    
    port =
      port_start = g_list_copy_deep(current->port,
				    (GCopyFunc) g_object_ref,
				    NULL);

    g_rec_mutex_unlock(mutex);

    /* check specifier */
    while((port = ags_port_find_specifier(port, specifier)) != NULL){
      AgsPort *current;

      current = AGS_PORT(port->data);

      g_object_ref(current);
      list = g_list_prepend(list,
			    current);

      /* iterate - port */
      port = port->next;
    }

    g_list_free_full(port_start,
		     g_object_unref);

    /* iterate - recall */
    recall = recall->next;
  }

  g_list_free_full(recall_start,
		   g_object_unref);

  /* reverse result */
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_audio_open_audio_file_as_channel:
 * @audio: the #AgsAudio
 * @filename: (element-type utf8) (transfer none): the files to open
 * @overwrite_channels: if existing channels should be assigned
 * @create_channels: if new channels should be created as not fitting if combined with @overwrite_channels
 *
 * Open some files.
 *
 * Since: 3.0.0
 */
void
ags_audio_open_audio_file_as_channel(AgsAudio *audio,
				     GSList *filename,
				     gboolean overwrite_channels,
				     gboolean create_channels)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  AgsAudioFile *audio_file;

  GObject *soundcard;
  
  GList *audio_signal;

  guint input_pads;
  guint audio_channels;
  guint i, j;
  guint list_length;

  GError *error;

  GRecMutex *audio_mutex;
  GRecMutex *recycling_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     filename == NULL ||
     (!overwrite_channels &&
      !create_channels)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get audio fields */
  soundcard = NULL;
  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &soundcard,
	       "input", &start_input,
	       NULL);

  g_rec_mutex_lock(audio_mutex);
  
  input_pads = audio->input_pads;
  audio_channels = audio->audio_channels;
  
  g_rec_mutex_unlock(audio_mutex);
  
  /* overwriting existing channels */
  if(overwrite_channels){
    channel = start_input;

    g_object_ref(channel);
    
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
	  AgsChannel *next;
	  AgsRecycling *recycling;

	  AgsFileLink *file_link;
	  
	  /* reset link */
	  error = NULL;

	  ags_channel_set_link(channel, NULL,
			       &error);

	  if(error != NULL){
	    g_warning("%s", error->message);


	    g_error_free(error);
	  }

	  /* get recycling */
	  recycling = NULL;

	  file_link = NULL;
	
	  g_object_get(channel,
		       "first-recycling", &recycling,
		       "file-link", &file_link,
		       NULL);

	  /* set filename and channel */
	  if(file_link == NULL){
	    file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
				     NULL);
      
	    g_object_set(channel,
			 "file-link", file_link,
			 NULL);
	  }
	  
	  g_object_set(file_link,
		       "filename", filename->data,
		       "audio-channel", j,
		       NULL);
	  	
	  g_object_unref(file_link);
	  
	  /* get recycling mutex */
	  recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

	  /* replace template audio signal */
	  AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  g_object_set(AGS_AUDIO_SIGNAL(audio_signal->data),
		       "recycling", recycling,
		       NULL);

	  ags_recycling_add_audio_signal(recycling,
					 audio_signal->data);

	  /* iterate */
	  audio_signal = audio_signal->next;

	  next = ags_channel_next(channel);
	
	  if(channel != NULL){
	    g_object_unref(channel);
	  }

	  channel = next;
	}

	if(audio_file->file_audio_channels < audio_channels){
	  AgsChannel *nth_channel;
	  
	  nth_channel = ags_channel_nth(channel,
					audio_channels - audio_file->file_audio_channels);

	  if(channel != NULL){
	    g_object_unref(channel);
	  }
	  
	  channel = nth_channel;
	}

	g_object_run_dispose(audio_file);
	g_object_unref(audio_file);
	
	filename = filename->next;
      }
    }
  }

  if(soundcard != NULL){
    g_object_unref(soundcard);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  /* appending to channels */
  if(create_channels && filename != NULL){
    list_length = g_slist_length(filename);
    
    ags_audio_set_pads((AgsAudio *) audio, AGS_TYPE_INPUT,
		       input_pads + list_length, 0);

    g_object_get(audio,
		 "input-pads", &input_pads,
		 "input", &start_input,
		 NULL);
    
    channel = ags_channel_nth(start_input,
			      (input_pads - list_length) * AGS_AUDIO(audio)->audio_channels);
    
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
	AgsChannel *next;
	AgsRecycling *recycling;
	
	AgsFileLink *file_link;

	/* get recycling */
	recycling = NULL;

	file_link = NULL;
	
	g_object_get(channel,
		     "first-recycling", &recycling,
		     "file-link", &file_link,
		     NULL);

	/* set filename and channel */
	if(file_link == NULL){
	  file_link = g_object_new(AGS_TYPE_AUDIO_FILE_LINK,
				   NULL);
      
	  g_object_set(channel,
		       "file-link", file_link,
		       NULL);
	}
	
	g_object_set(file_link,
		     "filename", filename->data,
		     "audio-channel", j,
		     NULL);
	
	g_object_unref(file_link);

	/* get recycling mutex */
	recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

	/* replace template audio signal */
	g_rec_mutex_lock(recycling_mutex);
	  
	AGS_AUDIO_SIGNAL(audio_signal->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal->data)->recycling = (GObject *) recycling;
	
	ags_recycling_add_audio_signal(recycling,
				       audio_signal->data);
	
	g_rec_mutex_unlock(recycling_mutex);

	/* iterate */
	audio_signal = audio_signal->next;

	next = ags_channel_next(channel);
	
	if(channel != NULL){
	  g_object_unref(channel);
	}

	channel = next;
      }
      
      if(audio_channels > audio_file->file_audio_channels){
	AgsChannel *nth_channel;
	
	nth_channel = ags_channel_nth(channel,
				      audio_channels - audio_file->file_audio_channels);

	if(channel != NULL){
	  g_object_unref(channel);
	}

	channel = nth_channel;
      }

      g_object_run_dispose(audio_file);
      g_object_unref(audio_file);
      
      filename = filename->next;
    }

    if(start_input != NULL){
      g_object_unref(start_input);
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

  GRecMutex *audio_mutex;
  GRecMutex *channel_mutex;

  if(!AGS_IS_AUDIO(audio) ||
     filename == NULL ||
     (!overwrite_channels &&
      !create_channels)){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get audio fields */
  g_rec_mutex_lock(audio_mutex);

  audio_channels = audio->audio_channels;

  soundcard = audio->output_soundcard;

  g_rec_mutex_unlock(audio_mutex);

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

void
ags_audio_set_property_all(AgsAudio *audio,
			   gint n_params,
			   const gchar *parameter_name[], const GValue value[])
{
  guint i;

  for(i = 0; i < n_params; i++){
    g_object_set_property(G_OBJECT(audio),
			  parameter_name[i], &(value[i]));
  }
}
  
void
ags_audio_recursive_set_property_down(AgsChannel *channel,
				      gint n_params,
				      const gchar *parameter_name[], const GValue value[])
{
  if(channel == NULL){
    return;
  }

  ags_audio_set_property_all(AGS_AUDIO(channel->audio),
			     n_params,
			     parameter_name, value);
    
  ags_audio_recursive_set_property_down_input(channel,
					      n_params,
					      parameter_name, value);
}
    
void
ags_audio_recursive_set_property_down_input(AgsChannel *channel,
					    gint n_params,
					    const gchar *parameter_name[], const GValue value[])
{
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
  g_object_unref(input);
    
  while(input != NULL){      
    ags_audio_recursive_set_property_down(input->link,
					  n_params,
					  parameter_name, value);

    input = input->next;
  }
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
 * Since: 3.0.0
 */
void
ags_audio_recursive_set_property(AgsAudio *audio,
				 gint n_params,
				 const gchar *parameter_name[], const GValue value[])
{
  AgsChannel *channel;
  
  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  ags_audio_set_property_all(audio,
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

  GRecMutex *audio_mutex;
  GRecMutex *channel_mutex;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get channel */
  g_rec_mutex_lock(audio_mutex);

  channel = audio->output;

  g_rec_mutex_unlock(audio_mutex);

  /* initialize return value */
  while(channel != NULL){
    /* reset recursive channel stage */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, staging_flags);

    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* iterate */
    g_rec_mutex_lock(channel_mutex);

    channel = channel->next;

    g_rec_mutex_unlock(channel_mutex);
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
 * Since: 3.0.0
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
 * Creates an #AgsAudio, with defaults of @output_soundcard.
 *
 * Returns: a new #AgsAudio
 *
 * Since: 3.0.0
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
