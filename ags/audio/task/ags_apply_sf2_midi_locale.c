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

#include <ags/audio/task/ags_apply_sf2_midi_locale.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_sf2_midi_locale_class_init(AgsApplySF2MidiLocaleClass *apply_sf2_midi_locale);
void ags_apply_sf2_midi_locale_init(AgsApplySF2MidiLocale *apply_sf2_midi_locale);
void ags_apply_sf2_midi_locale_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_apply_sf2_midi_locale_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_apply_sf2_midi_locale_dispose(GObject *gobject);
void ags_apply_sf2_midi_locale_finalize(GObject *gobject);

void ags_apply_sf2_midi_locale_launch(AgsTask *task);

/**
 * SECTION:ags_apply_sf2_midi_locale
 * @short_description: apply Soundfont2 synth to channel
 * @title: AgsApplySF2MidiLocale
 * @section_id:
 * @include: ags/audio/task/ags_apply_sf2_midi_locale.h
 *
 * The #AgsApplySF2MidiLocale task apply the specified Soundfont2 synth to channel.
 */

static gpointer ags_apply_sf2_midi_locale_parent_class = NULL;

enum{
  PROP_0,
  PROP_TEMPLATE,
  PROP_SYNTH,
};

GType
ags_apply_sf2_midi_locale_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_sf2_midi_locale = 0;

    static const GTypeInfo ags_apply_sf2_midi_locale_info = {
      sizeof(AgsApplySF2MidiLocaleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_sf2_midi_locale_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySF2MidiLocale),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_sf2_midi_locale_init,
    };

    ags_type_apply_sf2_midi_locale = g_type_register_static(AGS_TYPE_TASK,
							    "AgsApplySF2MidiLocale",
							    &ags_apply_sf2_midi_locale_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_sf2_midi_locale);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_sf2_midi_locale_class_init(AgsApplySF2MidiLocaleClass *apply_sf2_midi_locale)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_apply_sf2_midi_locale_parent_class = g_type_class_peek_parent(apply_sf2_midi_locale);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_sf2_midi_locale;

  gobject->set_property = ags_apply_sf2_midi_locale_set_property;
  gobject->get_property = ags_apply_sf2_midi_locale_get_property;

  gobject->dispose = ags_apply_sf2_midi_locale_dispose;
  gobject->finalize = ags_apply_sf2_midi_locale_finalize;

  /* properties */
  /**
   * AgsApplySF2MidiLocale:template:
   *
   * The template to apply.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_pointer("template",
				    i18n_pspec("template"),
				    i18n_pspec("The template to apply"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPLATE,
				  param_spec);

  /**
   * AgsApplySF2MidiLocale:synth:
   *
   * The synth to apply.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_pointer("synth",
				    i18n_pspec("synth"),
				    i18n_pspec("The synth to apply"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_sf2_midi_locale;

  task->launch = ags_apply_sf2_midi_locale_launch;
}

void
ags_apply_sf2_midi_locale_init(AgsApplySF2MidiLocale *apply_sf2_midi_locale)
{
  apply_sf2_midi_locale->template = NULL;
  apply_sf2_midi_locale->synth = NULL;
}

void
ags_apply_sf2_midi_locale_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;

  apply_sf2_midi_locale = AGS_APPLY_SF2_MIDI_LOCALE(gobject);

  switch(prop_id){
  case PROP_TEMPLATE:
  {
    apply_sf2_midi_locale->template = g_value_get_pointer(value);
  }
  break;
  case PROP_SYNTH:
  {
    apply_sf2_midi_locale->synth = g_value_get_pointer(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sf2_midi_locale_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;

  apply_sf2_midi_locale = AGS_APPLY_SF2_MIDI_LOCALE(gobject);

  switch(prop_id){
  case PROP_TEMPLATE:
  {
    g_value_set_pointer(value, apply_sf2_midi_locale->template);
  }
  break;
  case PROP_SYNTH:
  {
    g_value_set_pointer(value, apply_sf2_midi_locale->synth);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sf2_midi_locale_dispose(GObject *gobject)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;

  apply_sf2_midi_locale = AGS_APPLY_SF2_MIDI_LOCALE(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sf2_midi_locale_parent_class)->dispose(gobject);
}

void
ags_apply_sf2_midi_locale_finalize(GObject *gobject)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;

  apply_sf2_midi_locale = AGS_APPLY_SF2_MIDI_LOCALE(gobject);

  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sf2_midi_locale_parent_class)->finalize(gobject);
}

void
ags_apply_sf2_midi_locale_launch(AgsTask *task)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;
  
  AgsSF2SynthUtil *template;
  AgsSF2SynthUtil *synth;
  AgsResampleUtil *template_resample_util;
  AgsResampleUtil *synth_resample_util;
  AgsHQPitchUtil *template_hq_pitch_util;
  AgsHQPitchUtil *synth_hq_pitch_util;
  AgsLinearInterpolateUtil *template_hq_pitch_linear_interpolate_util;
  AgsLinearInterpolateUtil *synth_hq_pitch_linear_interpolate_util;

  apply_sf2_midi_locale = AGS_APPLY_SF2_MIDI_LOCALE(task);

  g_return_if_fail(apply_sf2_midi_locale->template != NULL);
  g_return_if_fail(apply_sf2_midi_locale->synth != NULL);

  template = apply_sf2_midi_locale->template;
  synth = apply_sf2_midi_locale->synth;

  /* synth */
  synth->flags = template->flags;

  synth->sf2_file = template->sf2_file;

  synth->sf2_sample_count = template->sf2_sample_count;
  synth->sf2_sample = template->sf2_sample;
  synth->sf2_note_range = template->sf2_note_range;
  
  synth->sf2_orig_buffer_length = template->sf2_orig_buffer_length;
  synth->sf2_orig_buffer = template->sf2_orig_buffer;

  synth->sf2_resampled_buffer_length = template->sf2_resampled_buffer_length;
  synth->sf2_resampled_buffer = template->sf2_resampled_buffer;

  synth->ipatch_sample = template->ipatch_sample;

  synth->source = template->source;
  synth->source_stride = template->source_stride;

  synth->sample_buffer = template->sample_buffer;
  synth->im_buffer = template->im_buffer;
  
  synth->buffer_length = template->buffer_length;
  synth->format = template->format;
  synth->samplerate = template->samplerate;

  synth->preset = template->preset;
  synth->instrument = template->instrument;
  synth->sample = template->sample;

  synth->bank = template->bank;
  synth->program = template->program;

  synth->midi_key = template->midi_key;

  synth->note = template->note;

  synth->volume = template->volume;

  synth->frame_count = template->frame_count;
  synth->offset = template->offset;

  synth->loop_mode = template->loop_mode;

  synth->loop_start = template->loop_start;
  synth->loop_end = template->loop_end;

  /* resample util */
  template_resample_util = template->resample_util;
  synth_resample_util = synth->resample_util;

  synth_resample_util->secret_rabbit.src_ratio = template_resample_util->secret_rabbit.src_ratio;

  synth_resample_util->secret_rabbit.input_frames = template_resample_util->secret_rabbit.input_frames;
  synth_resample_util->secret_rabbit.data_in = template_resample_util->secret_rabbit.data_in;

  synth_resample_util->secret_rabbit.output_frames = template_resample_util->secret_rabbit.output_frames;
  synth_resample_util->secret_rabbit.data_out = template_resample_util->secret_rabbit.data_out;

  synth_resample_util->source = template_resample_util->source;
  synth_resample_util->source_stride = template_resample_util->source_stride;

  synth_resample_util->destination = template_resample_util->destination;
  synth_resample_util->destination_stride = template_resample_util->destination_stride;

  synth_resample_util->buffer_length = template_resample_util->buffer_length;
  synth_resample_util->format = template_resample_util->format;
  synth_resample_util->samplerate = template_resample_util->samplerate;

  synth_resample_util->audio_buffer_util_format = template_resample_util->audio_buffer_util_format;
  synth_resample_util->target_samplerate = template_resample_util->target_samplerate;

  /* HQ pitch util */
  template_hq_pitch_util = template->hq_pitch_util;
  synth_hq_pitch_util = synth->hq_pitch_util;

  synth_hq_pitch_util->source = template_hq_pitch_util->source;
  synth_hq_pitch_util->source_stride = template_hq_pitch_util->source_stride;

  synth_hq_pitch_util->destination = template_hq_pitch_util->destination;
  synth_hq_pitch_util->destination_stride = template_hq_pitch_util->destination_stride;
  
  synth_hq_pitch_util->low_mix_buffer = template_hq_pitch_util->low_mix_buffer;
  synth_hq_pitch_util->new_mix_buffer = template_hq_pitch_util->new_mix_buffer;

  synth_hq_pitch_util->buffer_length = template_hq_pitch_util->buffer_length;
  synth_hq_pitch_util->format = template_hq_pitch_util->format;
  synth_hq_pitch_util->samplerate = template_hq_pitch_util->samplerate;

  synth_hq_pitch_util->base_key = template_hq_pitch_util->base_key;
  synth_hq_pitch_util->tuning = template_hq_pitch_util->tuning;

  /*  */
  template_hq_pitch_linear_interpolate_util = template_hq_pitch_util->linear_interpolate_util;
  synth_hq_pitch_linear_interpolate_util = synth_hq_pitch_util->linear_interpolate_util;

  synth_hq_pitch_linear_interpolate_util->source = template_hq_pitch_linear_interpolate_util->source;
  synth_hq_pitch_linear_interpolate_util->source_stride = template_hq_pitch_linear_interpolate_util->source_stride;

  synth_hq_pitch_linear_interpolate_util->destination = template_hq_pitch_linear_interpolate_util->destination;
  synth_hq_pitch_linear_interpolate_util->destination_stride = template_hq_pitch_linear_interpolate_util->destination_stride;
  
  synth_hq_pitch_linear_interpolate_util->buffer_length = template_hq_pitch_linear_interpolate_util->buffer_length;
  synth_hq_pitch_linear_interpolate_util->format = template_hq_pitch_linear_interpolate_util->format;
  synth_hq_pitch_linear_interpolate_util->samplerate = template_hq_pitch_linear_interpolate_util->samplerate;

  synth_hq_pitch_linear_interpolate_util->factor = template_hq_pitch_linear_interpolate_util->factor;
}

/**
 * ags_apply_sf2_midi_locale_new:
 * @template: the template
 * @synth: the synth
 *
 * Creates an #AgsApplySF2MidiLocale.
 *
 * Returns: an new #AgsApplySF2MidiLocale.
 *
 * Since: 3.16.0
 */
AgsApplySF2MidiLocale*
ags_apply_sf2_midi_locale_new(AgsSF2SynthUtil *template,
			      AgsSF2SynthUtil *synth)
{
  AgsApplySF2MidiLocale *apply_sf2_midi_locale;

  apply_sf2_midi_locale = (AgsApplySF2MidiLocale *) g_object_new(AGS_TYPE_APPLY_SF2_MIDI_LOCALE,
								 "template", template,
								 "synth", synth,
								 NULL);


  return(apply_sf2_midi_locale);
}
