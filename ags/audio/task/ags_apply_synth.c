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

#include <ags/audio/task/ags_apply_synth.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_config.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>

#include <math.h>

void ags_apply_synth_class_init(AgsApplySynthClass *apply_synth);
void ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_connect(AgsConnectable *connectable);
void ags_apply_synth_disconnect(AgsConnectable *connectable);
void ags_apply_synth_finalize(GObject *gobject);

void ags_apply_synth_launch(AgsTask *task);

/**
 * SECTION:ags_apply_synth
 * @short_description: apply synth to channel
 * @title: AgsApplySynth
 * @section_id:
 * @include: ags/audio/task/ags_apply_synth.h
 *
 * The #AgsApplySynth task apply the specified synth to channel.
 */

static gpointer ags_apply_synth_parent_class = NULL;
static AgsConnectableInterface *ags_apply_synth_parent_connectable_interface;

GType
ags_apply_synth_get_type()
{
  static GType ags_type_apply_synth = 0;

  if(!ags_type_apply_synth){
    static const GTypeInfo ags_apply_synth_info = {
      sizeof (AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth\0",
						  &ags_apply_synth_info,
						  0);

    g_type_add_interface_static(ags_type_apply_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_synth);
}

void
ags_apply_synth_class_init(AgsApplySynthClass *apply_synth)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_apply_synth_parent_class = g_type_class_peek_parent(apply_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_synth;

  gobject->finalize = ags_apply_synth_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_synth;

  task->launch = ags_apply_synth_launch;
}

void
ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_synth_connect;
  connectable->disconnect = ags_apply_synth_disconnect;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
  apply_synth->start_channel = NULL;
  apply_synth->count = 0;

  apply_synth->wave = AGS_APPLY_SYNTH_INVALID;
  apply_synth->attack = 0;
  apply_synth->frame_count = 0;
  apply_synth->phase = 0;
  apply_synth->start_frequency = 0;
  apply_synth->volume = 1.0;

  apply_synth->loop_start = 0;
  apply_synth->loop_end = 0;
}

void
ags_apply_synth_connect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_synth_disconnect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_synth_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_apply_synth_launch(AgsTask *task)
{
  AgsApplySynth *apply_synth;

  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  AgsSoundcard *soundcard;

  GList *stream;

  gint wave;
  guint attack, frame_count, stop, phase, frequency;
  double volume;
  guint current_attack, current_frame_count, current_stop, current_phase[2], current_frequency;
  guint stream_start, last_frame_count;
  guint i, j;
  double factor;
  guint buffer_size;
  guint samplerate;
  guint audio_buffer_util_format;
  gchar *str;

  auto double ags_apply_synth_calculate_factor(guint base_frequency, guint wished_frequency, guint wave);
  auto void ags_apply_synth_launch_write(GList *stream,
					 guint frequency, guint phase, gdouble volume,
					 guint samplerate, guint audio_buffer_util_format,
					 guint offset, guint frame_count);
  
  double ags_apply_synth_calculate_factor(guint base_frequency, guint wished_frequency, guint wave){
    double factor;

    factor = ((1.0 / base_frequency) * (wished_frequency));

    return(factor);
  }
  
  void ags_apply_synth_launch_write(GList *stream,
				    guint frequency, guint phase, gdouble volume,
				    guint samplerate, guint audio_buffer_util_format,
				    guint offset, guint frame_count){
    switch(wave){
    case AGS_APPLY_SYNTH_SIN:
      ags_synth_util_sin(stream->data,
			 frequency, phase, volume,
			 samplerate, audio_buffer_util_format,
			 offset, frame_count);
      break;
    case AGS_APPLY_SYNTH_SAW:
      ags_synth_util_sawtooth(stream->data,
			      frequency, phase, volume,
			      samplerate, audio_buffer_util_format,
			      offset, frame_count);
      break;
    case AGS_APPLY_SYNTH_TRIANGLE:
      ags_synth_util_triangle(stream->data,
			      frequency, phase, volume,
			      samplerate, audio_buffer_util_format,
			      offset, frame_count);
      break;
    case AGS_APPLY_SYNTH_SQUARE:
      ags_synth_util_square(stream->data,
			    frequency, phase, volume,
			    samplerate, audio_buffer_util_format,
			    offset, frame_count);
      break;
    default:
      g_warning("ags_apply_synth_launch_write: warning no wave selected\n\0");
    }
  }

  apply_synth = AGS_APPLY_SYNTH(task);
  channel = apply_synth->start_channel;
  soundcard = AGS_SOUNDCARD(channel->soundcard);
  
  wave = (gint) apply_synth->wave;

#ifdef AGS_DEBUG
  g_message("wave = %d\n\0", wave);
#endif
  
  /* some settings */
  frequency = apply_synth->frequency;

  volume = (double) apply_synth->volume;

  /* settings which needs to be initialized for factorizing */
  attack = apply_synth->attack;
  frame_count = apply_synth->frame_count;
  stop = (guint) ceil((double)(attack + frame_count) / (double) buffer_size);
  phase = apply_synth->phase;

  //TODO:JK: 
  //  attack = attack % (guint) buffer_size;
  
  current_phase[0] = (phase + (buffer_size - attack) + i * buffer_size) % frequency;

  factor = 1.0;

  for(i = 0; channel != NULL && i < apply_synth->count; i++){
    if(AGS_IS_INPUT(channel)){
      if(AGS_INPUT(channel)->synth_generator == NULL){
	AGS_INPUT(channel)->synth_generator = (GObject *) ags_synth_generator_new();
      }
      
      g_object_set(AGS_INPUT(channel)->synth_generator,
		   "n-frames\0", frame_count,
		   "frequency\0", frequency,
		   "phase\0", phase,
		   "volume\0", volume,
		   NULL);
    }
    
    /* calculate wished frequncy and phase */
    current_frequency = (guint) ((double) frequency * exp2((double)((apply_synth->start_frequency * -1.0) + (double)i) / 12.0));
    current_phase[0] = (guint) ((double) phase * ((double) frequency / (double) current_frequency));

    /* settings which needs to be factorized */
    factor = ags_apply_synth_calculate_factor(frequency, current_frequency, wave);

    current_attack = (guint) (factor * attack) % (guint) buffer_size;
    current_frame_count = frame_count * factor;
    current_stop = (guint) ceil((double)(current_attack + current_frame_count) / (double)buffer_size);
    current_phase[0] = ((guint)(factor * phase) + (buffer_size - current_attack) + i * buffer_size) % current_frequency;

    stream_start = (guint) floor((double)current_attack / (double) buffer_size);
    last_frame_count = (current_frame_count - buffer_size - current_attack) % buffer_size;

    /* create AgsAudioSignal */
    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    
    if(audio_signal->length < current_stop)
      ags_audio_signal_stream_resize(audio_signal, stop);

    audio_signal->loop_start = (guint) ((double) apply_synth->loop_start) * factor;
    audio_signal->loop_end = (guint) ((double) apply_synth->loop_end) * factor;
    

    /* fill in the stream */
    stream = g_list_nth(audio_signal->stream_beginning, stream_start);
    
    if(stream->next != NULL){
#ifdef AGS_DEBUG
      g_message("freq = %u, phase = %u\n\0", current_frequency, current_phase[0]);
#endif
      ags_apply_synth_launch_write(stream,
				   current_frequency, current_phase[0], volume,
				   audio_signal->samplerate, ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
				   attack, audio_signal->buffer_size - attack);
    }else{
      ags_apply_synth_launch_write(stream,
				   current_frequency, current_phase[0], volume,
				   audio_signal->samplerate, ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
				   attack, audio_signal->buffer_size - attack);
      
      channel = channel->next;
      continue;
    }
    
    stream = stream->next;
    
    
    for(j = 1; stream->next != NULL; j++){
      current_phase[1] = (j * buffer_size + current_phase[0]) % (samplerate / current_frequency);
#ifdef AGS_DEBUG
      g_message("freq = %u, phase = %u\n\0", current_frequency, current_phase[1]);
#endif
      
      ags_apply_synth_launch_write(stream,
				   current_frequency, current_phase[0], volume,
				   audio_signal->samplerate, ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
				   0, audio_signal->buffer_size);
      
      stream = stream->next;
    }
    
    current_phase[1] = (current_phase[0] + (buffer_size - attack) + j * buffer_size) % current_frequency;
    ags_apply_synth_launch_write(stream,
				 current_frequency, current_phase[0], volume,
				 audio_signal->samplerate, ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
				 0, audio_signal->buffer_size);
    
    
    channel = channel->next;
  }
}

/**
 * ags_apply_synth_new:
 * @start_channel: the start channel
 * @count: the count of lines
 * @wave: the selected wave
 * @attack: the attack
 * @frame_count: frame count
 * @frequency: frequency
 * @phase: the phase
 * @start_frequency: base frequency
 * @volume: volume
 * @loop_start: loop start
 * @loop_end: loop end
 *
 * Creates an #AgsApplySynth.
 *
 * Returns: an new #AgsApplySynth.
 *
 * Since: 0.4
 */
AgsApplySynth*
ags_apply_synth_new(AgsChannel *start_channel, guint count,
		    guint wave,
		    guint attack, guint frame_count,
		    gdouble frequency, gdouble phase, gdouble start_frequency,
		    gdouble volume,
		    guint loop_start, guint loop_end)
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       NULL);

  apply_synth->start_channel = start_channel;
  apply_synth->count = count;
  apply_synth->wave = wave;
  apply_synth->attack = attack;
  apply_synth->frame_count = frame_count;
  apply_synth->frequency = frequency;
  apply_synth->phase = phase;
  apply_synth->start_frequency = start_frequency;
  apply_synth->volume = volume;
  apply_synth->loop_start = loop_start;
  apply_synth->loop_end = loop_end;

  return(apply_synth);
}
