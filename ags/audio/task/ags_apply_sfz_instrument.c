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

#include <ags/audio/task/ags_apply_sfz_instrument.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_sfz_instrument_class_init(AgsApplySFZInstrumentClass *apply_sfz_instrument);
void ags_apply_sfz_instrument_init(AgsApplySFZInstrument *apply_sfz_instrument);
void ags_apply_sfz_instrument_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_apply_sfz_instrument_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_apply_sfz_instrument_dispose(GObject *gobject);
void ags_apply_sfz_instrument_finalize(GObject *gobject);

void ags_apply_sfz_instrument_launch(AgsTask *task);

/**
 * SECTION:ags_apply_sfz_instrument
 * @short_description: apply SFZ synth template
 * @title: AgsApplySFZInstrument
 * @section_id:
 * @include: ags/audio/task/ags_apply_sfz_instrument.h
 *
 * The #AgsApplySFZInstrument task apply the specified SFZ synth template.
 */

static gpointer ags_apply_sfz_instrument_parent_class = NULL;

enum{
  PROP_0,
  PROP_TEMPLATE,
  PROP_SYNTH,
};

GType
ags_apply_sfz_instrument_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_sfz_instrument = 0;

    static const GTypeInfo ags_apply_sfz_instrument_info = {
      sizeof(AgsApplySFZInstrumentClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_sfz_instrument_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySFZInstrument),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_sfz_instrument_init,
    };

    ags_type_apply_sfz_instrument = g_type_register_static(AGS_TYPE_TASK,
							   "AgsApplySFZInstrument",
							   &ags_apply_sfz_instrument_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_sfz_instrument);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_sfz_instrument_class_init(AgsApplySFZInstrumentClass *apply_sfz_instrument)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_apply_sfz_instrument_parent_class = g_type_class_peek_parent(apply_sfz_instrument);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_sfz_instrument;

  gobject->set_property = ags_apply_sfz_instrument_set_property;
  gobject->get_property = ags_apply_sfz_instrument_get_property;

  gobject->dispose = ags_apply_sfz_instrument_dispose;
  gobject->finalize = ags_apply_sfz_instrument_finalize;

  /* properties */
  /**
   * AgsApplySFZInstrument:template:
   *
   * The template to apply.
   * 
   * Since: 3.17.0
   */
  param_spec = g_param_spec_pointer("template",
				    i18n_pspec("template"),
				    i18n_pspec("The template to apply"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPLATE,
				  param_spec);

  /**
   * AgsApplySFZInstrument:synth:
   *
   * The synth to apply.
   * 
   * Since: 3.17.0
   */
  param_spec = g_param_spec_pointer("synth",
				    i18n_pspec("synth"),
				    i18n_pspec("The synth to apply"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_sfz_instrument;

  task->launch = ags_apply_sfz_instrument_launch;
}

void
ags_apply_sfz_instrument_init(AgsApplySFZInstrument *apply_sfz_instrument)
{
  apply_sfz_instrument->template = NULL;
  apply_sfz_instrument->synth = NULL;
}

void
ags_apply_sfz_instrument_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsApplySFZInstrument *apply_sfz_instrument;

  apply_sfz_instrument = AGS_APPLY_SFZ_INSTRUMENT(gobject);

  switch(prop_id){
  case PROP_TEMPLATE:
  {
    apply_sfz_instrument->template = g_value_get_pointer(value);
  }
  break;
  case PROP_SYNTH:
  {
    apply_sfz_instrument->synth = g_value_get_pointer(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sfz_instrument_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsApplySFZInstrument *apply_sfz_instrument;

  apply_sfz_instrument = AGS_APPLY_SFZ_INSTRUMENT(gobject);

  switch(prop_id){
  case PROP_TEMPLATE:
  {
    g_value_set_pointer(value, apply_sfz_instrument->template);
  }
  break;
  case PROP_SYNTH:
  {
    g_value_set_pointer(value, apply_sfz_instrument->synth);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sfz_instrument_dispose(GObject *gobject)
{
  AgsApplySFZInstrument *apply_sfz_instrument;

  apply_sfz_instrument = AGS_APPLY_SFZ_INSTRUMENT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sfz_instrument_parent_class)->dispose(gobject);
}

void
ags_apply_sfz_instrument_finalize(GObject *gobject)
{
  AgsApplySFZInstrument *apply_sfz_instrument;

  apply_sfz_instrument = AGS_APPLY_SFZ_INSTRUMENT(gobject);

  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sfz_instrument_parent_class)->finalize(gobject);
}

void
ags_apply_sfz_instrument_launch(AgsTask *task)
{
  AgsApplySFZInstrument *apply_sfz_instrument;
  
  AgsSFZSynthUtil *template;
  AgsSFZSynthUtil *synth;
  AgsResampleUtil *template_resample_util;
  AgsResampleUtil *synth_resample_util;
  AgsHQPitchUtil *template_hq_pitch_util;
  AgsHQPitchUtil *synth_hq_pitch_util;
  AgsLinearInterpolateUtil *template_hq_pitch_linear_interpolate_util;
  AgsLinearInterpolateUtil *synth_hq_pitch_linear_interpolate_util;

  guint i;
  
  apply_sfz_instrument = AGS_APPLY_SFZ_INSTRUMENT(task);

  g_return_if_fail(apply_sfz_instrument->template != NULL);
  g_return_if_fail(apply_sfz_instrument->synth != NULL);

  template = apply_sfz_instrument->template;
  synth = apply_sfz_instrument->synth;

  /* synth */
  synth->flags = template->flags;

  synth->sfz_file = template->sfz_file;
  
  if(synth->sfz_file != NULL){
    g_object_ref(synth->sfz_file);
  }
  
  synth->sfz_sample_count = template->sfz_sample_count;
  template->sfz_sample_count = 0;
  
  synth->sfz_sample_arr = template->sfz_sample_arr;
  template->sfz_sample_arr = NULL;

  g_free(synth->sfz_note_range);
  
  synth->sfz_note_range = template->sfz_note_range;
  template->sfz_note_range = NULL;

  if(synth->sfz_orig_buffer != NULL){
    for(i = 0; i < 128; i++){
      ags_stream_free(synth->sfz_orig_buffer[i]);
      ags_stream_free(synth->sfz_resampled_buffer[i]);

      synth->sfz_orig_buffer[i] = NULL;
      synth->sfz_resampled_buffer[i] = NULL;
    }
  }

  g_free(synth->sfz_orig_buffer);
  g_free(synth->sfz_resampled_buffer);
  
  synth->sfz_orig_buffer = template->sfz_orig_buffer;
  template->sfz_orig_buffer = NULL;
  
  synth->sfz_resampled_buffer = template->sfz_resampled_buffer;
  template->sfz_resampled_buffer = NULL;
  
  for(i = 0; i < 128; i++){
    synth->sfz_orig_buffer_length[i] = template->sfz_orig_buffer_length[i];
    
    synth->sfz_resampled_buffer_length[i] = template->sfz_resampled_buffer_length[i];

    synth->sfz_loop_mode[i] = template->sfz_loop_mode[i];

    synth->sfz_loop_start[i] = template->sfz_loop_start[i];
    synth->sfz_loop_end[i] = template->sfz_loop_end[i];
  }
  
  synth->sfz_sample = template->sfz_sample;
  
  if(synth->sfz_sample != NULL){
    g_object_ref(synth->sfz_sample);
  }

  ags_stream_free(synth->source);
  
  synth->source_stride = template->source_stride;
  synth->source = template->source;
  template->source = NULL;  

  ags_stream_free(synth->sample_buffer);

  synth->sample_buffer = template->sample_buffer;
  template->sample_buffer = NULL;
  
  ags_stream_free(synth->im_buffer);

  synth->im_buffer = template->im_buffer;
  template->im_buffer = NULL;
  
  synth->buffer_length = template->buffer_length;
  synth->format = template->format;
  synth->samplerate = template->samplerate;

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

  g_free(template_resample_util->secret_rabbit.data_in);
  g_free(template_resample_util->secret_rabbit.data_out);
  
  synth_resample_util->secret_rabbit.input_frames = template_resample_util->secret_rabbit.input_frames;
  synth_resample_util->secret_rabbit.data_in = template_resample_util->secret_rabbit.data_in;
  template_resample_util->secret_rabbit.data_in = NULL;
  
  synth_resample_util->secret_rabbit.output_frames = template_resample_util->secret_rabbit.output_frames;
  synth_resample_util->secret_rabbit.data_out = template_resample_util->secret_rabbit.data_out;
  template_resample_util->secret_rabbit.data_out = NULL;
  
  synth_resample_util->source_stride = template_resample_util->source_stride;
  synth_resample_util->source = template_resample_util->source;
  template_resample_util->source = NULL;
  
  synth_resample_util->destination_stride = template_resample_util->destination_stride;
  synth_resample_util->destination = template_resample_util->destination;
  template_resample_util->destination = NULL;
  
  synth_resample_util->buffer_length = template_resample_util->buffer_length;
  synth_resample_util->format = template_resample_util->format;
  synth_resample_util->samplerate = template_resample_util->samplerate;

  synth_resample_util->audio_buffer_util_format = template_resample_util->audio_buffer_util_format;
  synth_resample_util->target_samplerate = template_resample_util->target_samplerate;

  /* HQ pitch util */
  template_hq_pitch_util = template->hq_pitch_util;
  synth_hq_pitch_util = synth->hq_pitch_util;

  synth_hq_pitch_util->source_stride = template_hq_pitch_util->source_stride;
  synth_hq_pitch_util->source = template_hq_pitch_util->source;
  template_hq_pitch_util->source = NULL;
  
  synth_hq_pitch_util->destination_stride = template_hq_pitch_util->destination_stride;
  synth_hq_pitch_util->destination = template_hq_pitch_util->destination;
  template_hq_pitch_util->destination = NULL;
  
  synth_hq_pitch_util->low_mix_buffer = template_hq_pitch_util->low_mix_buffer;
  template_hq_pitch_util->low_mix_buffer = NULL;
  
  synth_hq_pitch_util->new_mix_buffer = template_hq_pitch_util->new_mix_buffer;
  template_hq_pitch_util->new_mix_buffer = NULL;
  
  synth_hq_pitch_util->buffer_length = template_hq_pitch_util->buffer_length;
  synth_hq_pitch_util->format = template_hq_pitch_util->format;
  synth_hq_pitch_util->samplerate = template_hq_pitch_util->samplerate;

  synth_hq_pitch_util->base_key = template_hq_pitch_util->base_key;
  synth_hq_pitch_util->tuning = template_hq_pitch_util->tuning;

  /*  */
  template_hq_pitch_linear_interpolate_util = template_hq_pitch_util->linear_interpolate_util;
  synth_hq_pitch_linear_interpolate_util = synth_hq_pitch_util->linear_interpolate_util;

  synth_hq_pitch_linear_interpolate_util->source_stride = template_hq_pitch_linear_interpolate_util->source_stride;
  synth_hq_pitch_linear_interpolate_util->source = template_hq_pitch_linear_interpolate_util->source;
  template_hq_pitch_linear_interpolate_util->source = NULL;
  
  synth_hq_pitch_linear_interpolate_util->destination_stride = template_hq_pitch_linear_interpolate_util->destination_stride;
  synth_hq_pitch_linear_interpolate_util->destination = template_hq_pitch_linear_interpolate_util->destination;
  template_hq_pitch_linear_interpolate_util->destination = NULL;
  
  synth_hq_pitch_linear_interpolate_util->buffer_length = template_hq_pitch_linear_interpolate_util->buffer_length;
  synth_hq_pitch_linear_interpolate_util->format = template_hq_pitch_linear_interpolate_util->format;
  synth_hq_pitch_linear_interpolate_util->samplerate = template_hq_pitch_linear_interpolate_util->samplerate;

  synth_hq_pitch_linear_interpolate_util->factor = template_hq_pitch_linear_interpolate_util->factor;
}

/**
 * ags_apply_sfz_instrument_new:
 * @template: the template
 * @synth: the synth
 *
 * Creates an #AgsApplySFZInstrument.
 *
 * Returns: an new #AgsApplySFZInstrument.
 *
 * Since: 3.17.0
 */
AgsApplySFZInstrument*
ags_apply_sfz_instrument_new(AgsSFZSynthUtil *template,
			     AgsSFZSynthUtil *synth)
{
  AgsApplySFZInstrument *apply_sfz_instrument;

  apply_sfz_instrument = (AgsApplySFZInstrument *) g_object_new(AGS_TYPE_APPLY_SFZ_INSTRUMENT,
								"template", template,
								"synth", synth,
								NULL);


  return(apply_sfz_instrument);
}