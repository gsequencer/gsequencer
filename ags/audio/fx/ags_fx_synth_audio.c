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

#include <ags/audio/fx/ags_fx_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_synth_audio_class_init(AgsFxSynthAudioClass *fx_synth_audio);
void ags_fx_synth_audio_init(AgsFxSynthAudio *fx_synth_audio);
void ags_fx_synth_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_synth_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_synth_audio_dispose(GObject *gobject);
void ags_fx_synth_audio_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_synth_audio_get_synth_0_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_octave_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_key_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_phase_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_volume_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_synth_1_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_octave_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_key_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_phase_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_volume_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_low_pass_enabled__plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_low_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_low_pass_filter_gain_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_high_pass_enabled__plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_high_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_high_pass_filter_gain_plugin_port();

/**
 * SECTION:ags_fx_synth_audio
 * @short_description: fx synth audio
 * @title: AgsFxSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_synth_audio.h
 *
 * The #AgsFxSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_synth_audio_parent_class = NULL;

const gchar *ags_fx_synth_audio_plugin_name = "ags-fx-synth";

const gchar* ags_fx_synth_audio_specifier[] = {
  "./synth-0-oscillator-mode[0]",
  "./synth-0-octave[0]",
  "./synth-0-key[0]",
  "./synth-0-phase[0]",
  "./synth-0-volume[0]",
  "./synth-1-oscillator-mode[0]",
  "./synth-1-octave[0]",
  "./synth-1-key[0]",
  "./synth-1-phase[0]",
  "./synth-1-volume[0]",
  "./pitch-tuning[0]",
  "./chorus-enabled[0]",
  "./chorus-input-volume[0]",
  "./chorus-output-volume[0]",
  "./chorus-lfo-oscillator[0]",
  "./chorus-lfo-frequency[0]",
  "./chorus-depth[0]",
  "./chorus-mix[0]",
  "./chorus-delay[0]",
  "./low-pass-enabled[0]",
  "./low-pass-q-lin[0]",
  "./low-pass-filter-gain[0]",
  "./high-pass-enabled[0]",
  "./high-pass-q-lin[0]",
  "./high-pass-filter-gain[0]",
  NULL,
};

const gchar* ags_fx_synth_audio_control_port[] = {
  "1/25",
  "2/25",
  "3/25",
  "4/25",
  "5/25",
  "6/25",
  "7/25",
  "8/25",
  "9/25",
  "10/25",
  "11/25",
  "12/25",
  "13/25",
  "14/25",
  "15/25",
  "16/25",
  "17/25",
  "18/25",
  "19/25",
  "20/25",
  "21/25",
  "22/25",
  "23/25",
  "24/25",
  "25/25",
  NULL,
};

GType
ags_fx_synth_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_synth_audio = 0;

    static const GTypeInfo ags_fx_synth_audio_info = {
      sizeof (AgsFxSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_synth_audio_init,
    };

    ags_type_fx_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
						     "AgsFxSynthAudio",
						     &ags_fx_synth_audio_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_synth_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_synth_audio_class_init(AgsFxSynthAudioClass *fx_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_synth_audio_parent_class = g_type_class_peek_parent(fx_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_synth_audio;

  gobject->dispose = ags_fx_synth_audio_dispose;
  gobject->finalize = ags_fx_synth_audio_finalize;

  /* properties */
}

void
ags_fx_synth_audio_init(AgsFxSynthAudio *fx_synth_audio)
{
  AGS_RECALL(fx_synth_audio)->name = "ags-fx-synth";
  AGS_RECALL(fx_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_synth_audio)->xml_type = "ags-fx-synth-audio";
}

void
ags_fx_synth_audio_dispose(GObject *gobject)
{
  AgsFxSynthAudio *fx_synth_audio;
  
  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_synth_audio_finalize(GObject *gobject)
{
  AgsFxSynthAudio *fx_synth_audio;
  
  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxSynthAudio
 *
 * Returns: the new #AgsFxSynthAudio
 *
 * Since: 3.14.0
 */
AgsFxSynthAudio*
ags_fx_synth_audio_new(AgsAudio *audio)
{
  AgsFxSynthAudio *fx_synth_audio;

  fx_synth_audio = (AgsFxSynthAudio *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO,
						    "audio", audio,
						    NULL);

  return(fx_synth_audio);
}
