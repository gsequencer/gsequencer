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

#include <ags/audio/fx/ags_fx_envelope_audio_signal.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_volume_util.h>

#include <ags/audio/fx/ags_fx_envelope_channel.h>
#include <ags/audio/fx/ags_fx_envelope_channel_processor.h>
#include <ags/audio/fx/ags_fx_envelope_recycling.h>
#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_audio_processor.h>

#include <complex.h>

#include <ags/i18n.h>

void ags_fx_envelope_audio_signal_class_init(AgsFxEnvelopeAudioSignalClass *fx_envelope_audio_signal);
void ags_fx_envelope_audio_signal_init(AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal);
void ags_fx_envelope_audio_signal_dispose(GObject *gobject);
void ags_fx_envelope_audio_signal_finalize(GObject *gobject);

gdouble ags_fx_envelope_audio_signal_get_ratio(guint x0, gdouble y0,
					       guint x1, gdouble y1);
gdouble ags_fx_envelope_audio_signal_get_volume(gdouble volume, gdouble ratio,
						guint start_x, guint current_x,
						guint length);

void ags_fx_envelope_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_envelope_audio_signal
 * @short_description: fx envelope audio signal
 * @title: AgsFxEnvelopeAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_envelope_audio_signal.h
 *
 * The #AgsFxEnvelopeAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_envelope_audio_signal_parent_class = NULL;

const gchar *ags_fx_envelope_audio_signal_plugin_name = "ags-fx-envelope";

GType
ags_fx_envelope_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_envelope_audio_signal = 0;

    static const GTypeInfo ags_fx_envelope_audio_signal_info = {
      sizeof (AgsFxEnvelopeAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_envelope_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxEnvelopeAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_envelope_audio_signal_init,
    };

    ags_type_fx_envelope_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							       "AgsFxEnvelopeAudioSignal",
							       &ags_fx_envelope_audio_signal_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_envelope_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_envelope_audio_signal_class_init(AgsFxEnvelopeAudioSignalClass *fx_envelope_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_envelope_audio_signal_parent_class = g_type_class_peek_parent(fx_envelope_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_envelope_audio_signal;

  gobject->dispose = ags_fx_envelope_audio_signal_dispose;
  gobject->finalize = ags_fx_envelope_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_envelope_audio_signal;
  
  recall->run_inter = ags_fx_envelope_audio_signal_real_run_inter;
}

void
ags_fx_envelope_audio_signal_init(AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal)
{
  AGS_RECALL(fx_envelope_audio_signal)->name = "ags-fx-envelope";
  AGS_RECALL(fx_envelope_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_envelope_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_envelope_audio_signal)->xml_type = "ags-fx-envelope-audio-signal";
}

void
ags_fx_envelope_audio_signal_dispose(GObject *gobject)
{
  AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal;
  
  fx_envelope_audio_signal = AGS_FX_ENVELOPE_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_envelope_audio_signal_finalize(GObject *gobject)
{
  AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal;
  
  fx_envelope_audio_signal = AGS_FX_ENVELOPE_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_envelope_audio_signal_parent_class)->finalize(gobject);
}

gdouble
ags_fx_envelope_audio_signal_get_ratio(guint x0, gdouble y0,
				       guint x1, gdouble y1)
{
  if(x1 - x0 == 0){
    return(0.0);
  }else{
    return((y1 - y0) / (x1 - x0));
  }
}

gdouble
ags_fx_envelope_audio_signal_get_volume(gdouble volume, gdouble ratio,
					guint start_x, guint current_x,
					guint length)
{
  gdouble current_volume;
    
  if(length == 0){
    return(volume);
  }else{
    current_volume = volume + (ratio * (current_x - start_x));

#if 0
    g_message("envelope get volume %f %f -> %f", volume, ratio, current_volume);
#endif
      
    return(current_volume);
  }
}

void
ags_fx_envelope_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *template, *source;
  AgsFxEnvelopeChannel *fx_envelope_channel;
  AgsFxEnvelopeChannelProcessor *fx_envelope_channel_processor;
  AgsFxEnvelopeRecycling *fx_envelope_recycling;
  AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *fixed_length;
  AgsPort *attack, *decay, *sustain, *release, *ratio;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  GObject *output_soundcard;

  GList *start_list, *list;
  GList *start_note, *note;

  AgsComplex cattack, cdecay, csustain, crelease, cratio;

  guint note_offset, delay_counter;
  gdouble delay;
  guint length;
  guint buffer_size;
  guint format;
  gboolean is_pattern;
  gboolean is_sequencer;
  
  GRecMutex *stream_mutex;
  GRecMutex *attack_mutex, *decay_mutex, *sustain_mutex, *release_mutex, *ratio_mutex;

  output_soundcard = NULL;

  audio = NULL;
  channel = NULL;
  
  template = NULL;
  source = NULL;

  recall_id = NULL;
  recycling_context = NULL;
  parent_recycling_context = NULL;
  
  fx_envelope_channel = NULL;
  fx_envelope_channel_processor = NULL;
  fx_envelope_recycling = NULL;
  fx_envelope_audio_signal = (AgsFxEnvelopeAudioSignal *) recall;

  fixed_length = NULL;

  attack = NULL;
  decay = NULL;
  sustain = NULL;
  release = NULL;
  ratio = NULL;
  
  start_note = NULL;
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(recall,
	       "parent", &fx_envelope_recycling,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  if(recycling_context != NULL){
    g_object_get(recycling_context,
		 "parent", &parent_recycling_context,
		 NULL);
  }
  
  g_object_get(fx_envelope_recycling,
	       "parent", &fx_envelope_channel_processor,
	       NULL);

  g_object_get(fx_envelope_channel_processor,
	       "recall-channel", &fx_envelope_channel,
	       NULL);

  g_object_get(fx_envelope_channel,
	       "source", &channel,
	       "fixed-length", &fixed_length,
	       "attack", &attack,
	       "decay", &decay,
	       "sustain", &sustain,
	       "release", &release,
	       "ratio", &ratio,
	       NULL);
  
  g_object_get(source,
	       "output-soundcard", &output_soundcard,
	       "template", &template,
	       "note", &start_note,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  if(channel != NULL){
    g_object_get(channel,
		 "audio", &audio,
		 NULL);
  }
  
  is_pattern = ags_audio_test_behaviour_flags(audio,
					      AGS_SOUND_BEHAVIOUR_PATTERN_MODE);
  
  is_sequencer = ags_recall_id_check_sound_scope(recall_id,
						 AGS_SOUND_SCOPE_SEQUENCER);

  fx_notation_audio_processor = NULL;
  fx_pattern_audio_processor = NULL;
    
  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
  
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));

  start_list = NULL;

  if(audio != NULL){
    g_object_get(audio,
		 "recall", &start_list,
		 NULL);
  }
  
  if(!is_sequencer){
    GRecMutex *mutex;

    list = ags_recall_find_type_with_recycling_context(start_list,
						       AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR, recycling_context);

    if(list != NULL){
      fx_notation_audio_processor = list->data;
    }

    if(fx_notation_audio_processor != NULL){
      mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

      g_rec_mutex_lock(mutex);

      note_offset = fx_notation_audio_processor->offset_counter;
      delay_counter = fx_notation_audio_processor->delay_counter;
      
      g_rec_mutex_unlock(mutex);
    }
  }else{
    GRecMutex *mutex;
    
    list = ags_recall_find_type_with_recycling_context(start_list,
						       AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR, recycling_context);

    if(list != NULL){
      fx_pattern_audio_processor = list->data;
    }

    if(fx_pattern_audio_processor != NULL){
      mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);

      g_rec_mutex_lock(mutex);

      note_offset = fx_pattern_audio_processor->offset_counter;
      delay_counter = fx_pattern_audio_processor->delay_counter;
      
      g_rec_mutex_unlock(mutex);
    }
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
  
  length = (guint) floor(delay);
  
  if(template != NULL){
    g_object_get(template,
		 "length", &length,
		 NULL);
  }
  
  if(source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    fx_envelope_audio_signal->envelope_util.destination_stride = 1;
    
    fx_envelope_audio_signal->envelope_util.source_stride = 1;
    
    fx_envelope_audio_signal->envelope_util.format = format;
    
    fx_envelope_audio_signal->envelope_util.audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);
    
    if(start_note != NULL){
      note = start_note;
  
      while(note != NULL){
	if(ags_note_test_flags(AGS_NOTE(note->data), AGS_NOTE_ENVELOPE)){
	  AgsNote *current_note;
	  
	  gdouble envelope_x0, envelope_y0;
	  gdouble envelope_x1, envelope_y1;
	  gdouble envelope_current_x;

	  guint envelope_start_frame, envelope_end_frame;      
	  
	  guint x0, x1;
	  guint frame_count, current_frame;
	  guint current_frame_count, trailing_frame_count;
	  guint offset;

	  gdouble current_volume, current_ratio;
	  
	  GRecMutex *note_mutex;

	  current_note = note->data;
	  
	  /* get note mutex */
	  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(current_note);
	  
	  g_object_get(current_note,
		       "x0", &x0,
		       "x1", &x1,
		       NULL);

	  /* get note envelope */
	  g_rec_mutex_lock(note_mutex);

	  cattack.real = current_note->attack.real;
	  cattack.imag = current_note->attack.imag;

	  cdecay.real = current_note->decay.real;
	  cdecay.imag = current_note->decay.imag;

	  csustain.real = current_note->sustain.real;
	  csustain.imag = current_note->sustain.imag;

	  crelease.real = current_note->release.real;
	  crelease.imag = current_note->release.imag;
      
	  cratio.real = current_note->ratio.real;
	  cratio.imag = current_note->ratio.imag;

	  g_rec_mutex_unlock(note_mutex);
	  
	  /* get frame count */
	  if(!is_pattern){
	    frame_count = (x1 - x0) * (delay * buffer_size);
	  }else{
	    frame_count = length * buffer_size;
	  }

#if 0
	  g_message("frame-count: %d", frame_count);
#endif
	  
	  offset = 0;

	  if(note_offset >= x0){
	    current_frame = (guint) floor(((gdouble) (note_offset - x0) * delay + delay_counter) * buffer_size);
	  }else{
	    current_frame = 0;
	  }
	  
#if 0
	  g_message("current-frame: %d", current_frame);
#endif
	  
	  /* special case release - #0 key offset bigger than note offset */
	  if(x1 < note_offset){
	    AgsVolumeUtil volume_util;
	    
	    envelope_current_x = cattack.real + cdecay.real;
	
	    envelope_x0 = csustain.real;
	    envelope_y0 = csustain.imag + cratio.imag;

	    envelope_x1 = crelease.real;
	    envelope_y1 = crelease.imag + cratio.imag;

	    envelope_start_frame = (envelope_current_x + envelope_x0) * frame_count;
	    envelope_end_frame = (envelope_current_x + envelope_x0 + envelope_x1) * frame_count;
    	
	    current_ratio = ags_fx_envelope_audio_signal_get_ratio(0, envelope_y0,
								   envelope_end_frame - envelope_start_frame, envelope_y1);
	    current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								     0, envelope_end_frame - envelope_start_frame,
								     envelope_end_frame - envelope_start_frame);
	    
	    g_rec_mutex_lock(stream_mutex);

	    volume_util.destination = source->stream_current->data;
	    volume_util.destination_stride = 1;
    
	    volume_util.source = source->stream_current->data;
	    volume_util.source_stride = 1;

	    volume_util.buffer_length = buffer_size;
	    volume_util.format = format;

	    volume_util.audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);
    
	    volume_util.volume = current_volume;

	    ags_volume_util_compute(&volume_util);

	    g_rec_mutex_unlock(stream_mutex);
	    
	    /* iterate */
	    note = note->next;

	    continue;
	  }
	  
	  //TODO:JK: implement me

	  /* special case not applicable - #0 key offset lower than note offset */
	  if(x0 > note_offset){
	    /* iterate */
	    note = note->next;

	    continue;
	  }

	  envelope_current_x = 0.0;
      
	  /* attack */
	  envelope_x0 = 0.0;
	  envelope_y0 = cratio.imag;

	  envelope_x1 = cattack.real;
	  envelope_y1 = cattack.imag + cratio.imag;

	  envelope_start_frame = (guint) floor((envelope_current_x + envelope_x0) * (double) frame_count);
	  envelope_end_frame = (guint) floor((envelope_current_x + envelope_x0 + envelope_x1) * (double) frame_count);

#if 0
	  g_message("attack - first-frame: %d -> %d::%d", offset, envelope_start_frame, envelope_end_frame);
#endif
      
	  if(envelope_start_frame <= current_frame &&
	     envelope_end_frame > current_frame){
	    if(envelope_end_frame - current_frame < buffer_size){
	      current_frame_count = envelope_end_frame - current_frame;
	    }else{
	      current_frame_count = buffer_size;
	    }
      
	    current_ratio = ags_fx_envelope_audio_signal_get_ratio(0, envelope_y0,
								   envelope_end_frame - envelope_start_frame, envelope_y1);
	    current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								     0, current_frame - envelope_start_frame,
								     envelope_end_frame - envelope_start_frame);

	    g_rec_mutex_lock(stream_mutex);
	    
	    fx_envelope_audio_signal->envelope_util.destination = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.source = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.volume = current_volume;

	    fx_envelope_audio_signal->envelope_util.amount = current_ratio;
	    
	    fx_envelope_audio_signal->envelope_util.buffer_length = current_frame_count;
	
	    ags_envelope_util_compute(&(fx_envelope_audio_signal->envelope_util));

	    g_rec_mutex_unlock(stream_mutex);

	    current_frame += current_frame_count;
	    offset += current_frame_count;
	  }    
      
	  if(offset >= buffer_size){
	    /* iterate */
	    note = note->next;

	    continue;
	  }

	  envelope_current_x = 0.0;

	  /* decay */
	  envelope_x0 = cattack.real;
	  envelope_y0 = cattack.imag + cratio.imag;

	  envelope_x1 = cdecay.real;
	  envelope_y1 = cdecay.imag + cratio.imag;

	  envelope_start_frame = (guint) floor((envelope_current_x + envelope_x0) * (double) frame_count);
	  envelope_end_frame = (guint) floor((envelope_current_x + envelope_x0 + envelope_x1) * (double) frame_count);

#if 0
	  g_message("decay - first-frame: %d -> %d::%d", offset, envelope_start_frame, envelope_end_frame);
#endif
      
	  if(envelope_start_frame <= current_frame &&
	     envelope_end_frame > current_frame){
	    if(envelope_end_frame - current_frame < buffer_size){
	      current_frame_count = envelope_end_frame - current_frame;
	    }else{
	      current_frame_count = buffer_size - offset;
	    }
      
	    current_ratio = ags_fx_envelope_audio_signal_get_ratio(0, envelope_y0,
								   envelope_end_frame - envelope_start_frame, envelope_y1);
	    current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								     0, current_frame - envelope_start_frame,
								     envelope_end_frame - envelope_start_frame);

	    g_rec_mutex_lock(stream_mutex);
	    
	    fx_envelope_audio_signal->envelope_util.destination = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.source = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.volume = current_volume;

	    fx_envelope_audio_signal->envelope_util.amount = current_ratio;
	    
	    fx_envelope_audio_signal->envelope_util.buffer_length = current_frame_count;
	
	    ags_envelope_util_compute(&(fx_envelope_audio_signal->envelope_util));
	    
	    g_rec_mutex_unlock(stream_mutex);

	    current_frame += current_frame_count;
	    offset += current_frame_count;
	  }    

	  if(offset >= buffer_size){
	    /* iterate */
	    note = note->next;

	    continue;
	  }

	  envelope_current_x = cattack.real;

	  /* sustain */
	  envelope_x0 = cdecay.real;
	  envelope_y0 = cdecay.imag + cratio.imag;

	  envelope_x1 = csustain.real;
	  envelope_y1 = csustain.imag + cratio.imag;

	  envelope_start_frame = (guint) floor((envelope_current_x + envelope_x0) * (double) frame_count);
	  envelope_end_frame = (guint) floor((envelope_current_x + envelope_x0 + envelope_x1) * (double) frame_count);

#if 0
	  g_message("sustain - first-frame: %d -> %d::%d", offset, envelope_start_frame, envelope_end_frame);
#endif
      
	  if(envelope_start_frame <= current_frame &&
	     envelope_end_frame > current_frame){
	    if(envelope_end_frame - current_frame < buffer_size){
	      current_frame_count = envelope_end_frame - current_frame;
	    }else{
	      current_frame_count = buffer_size - offset;
	    }
      
	    current_ratio = ags_fx_envelope_audio_signal_get_ratio(0, envelope_y0,
								   envelope_end_frame - envelope_start_frame, envelope_y1);
	    current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								     0, current_frame - envelope_start_frame,
								     envelope_end_frame - envelope_start_frame);

	    g_rec_mutex_lock(stream_mutex);
	    
	    fx_envelope_audio_signal->envelope_util.destination = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.source = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.volume = current_volume;

	    fx_envelope_audio_signal->envelope_util.amount = current_ratio;
	    
	    fx_envelope_audio_signal->envelope_util.buffer_length = current_frame_count;
	
	    ags_envelope_util_compute(&(fx_envelope_audio_signal->envelope_util));

	    g_rec_mutex_unlock(stream_mutex);

	    current_frame += current_frame_count;
	    offset += current_frame_count;
	  }    
    
	  if(offset >= buffer_size){
	    /* iterate */
	    note = note->next;

	    continue;
	  }

	  envelope_current_x = cdecay.real + csustain.real;

	  /* release */
	  envelope_x0 = csustain.real;
	  envelope_y0 = csustain.imag + cratio.imag;

	  envelope_x1 = crelease.real;
	  envelope_y1 = crelease.imag + cratio.imag;

	  envelope_start_frame = (guint) floor((envelope_current_x + envelope_x0) * (double) frame_count);
	  envelope_end_frame = (guint) floor((envelope_current_x + envelope_x0 + envelope_x1) * (double) frame_count);
    
#if 0
	  g_message("release - first-frame: %d -> %d::%d", offset, envelope_start_frame, envelope_end_frame);
#endif
      
	  if(envelope_start_frame <= current_frame &&
	     envelope_end_frame > current_frame){
	    trailing_frame_count = 0;

	    if(envelope_end_frame - current_frame < buffer_size){
	      current_frame_count = envelope_end_frame - current_frame;

	      trailing_frame_count = buffer_size - current_frame_count;
	    }else{
	      current_frame_count = buffer_size - offset;
	    }
      
	    current_ratio = ags_fx_envelope_audio_signal_get_ratio(0, envelope_y0,
								   envelope_end_frame - envelope_start_frame, envelope_y1);
	    current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								     0, current_frame - envelope_start_frame,
								     envelope_end_frame - envelope_start_frame);

	    g_rec_mutex_lock(stream_mutex);
	    
	    fx_envelope_audio_signal->envelope_util.destination = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.source = source->stream_current->data + offset;
	    
	    fx_envelope_audio_signal->envelope_util.volume = current_volume;

	    fx_envelope_audio_signal->envelope_util.amount = current_ratio;
	    
	    fx_envelope_audio_signal->envelope_util.buffer_length = current_frame_count;
	
	    ags_envelope_util_compute(&(fx_envelope_audio_signal->envelope_util));
	    
	    g_rec_mutex_unlock(stream_mutex);

	    offset += current_frame_count;

	    if(trailing_frame_count != 0){
	      AgsVolumeUtil volume_util;
	      
	      current_volume = ags_fx_envelope_audio_signal_get_volume(envelope_y0, current_ratio,
								       0, envelope_end_frame - envelope_start_frame,
								       envelope_end_frame);

	      g_rec_mutex_lock(stream_mutex);
	      
	      volume_util.destination = source->stream_current->data;
	      volume_util.destination_stride = 1;
    
	      volume_util.source = source->stream_current->data;
	      volume_util.source_stride = 1;

	      volume_util.buffer_length = buffer_size;
	      volume_util.format = format;

	      volume_util.audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);
    
	      volume_util.volume = current_volume;

	      ags_volume_util_compute(&volume_util);

	      g_rec_mutex_unlock(stream_mutex);
	    }
	  }
	}
	
	note = note->next;
      }
    }
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }  

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(template != NULL){
    g_object_unref(template);
  }

  if(source != NULL){
    g_object_unref(source);
  }

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  if(fx_envelope_channel != NULL){
    g_object_unref(fx_envelope_channel);
  }

  if(fixed_length != NULL){
    g_object_unref(fixed_length);
  }

  if(attack != NULL){
    g_object_unref(attack);
  }

  if(decay != NULL){
    g_object_unref(decay);
  }

  if(sustain != NULL){
    g_object_unref(sustain);
  }

  if(release != NULL){
    g_object_unref(release);
  }

  if(ratio != NULL){
    g_object_unref(ratio);
  }
  
  if(fx_envelope_channel_processor != NULL){
    g_object_unref(fx_envelope_channel_processor);
  }

  if(fx_envelope_recycling != NULL){
    g_object_unref(fx_envelope_recycling);
  }
  
  g_list_free_full(start_note,
		   (GDestroyNotify) g_object_unref);
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_envelope_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_envelope_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxEnvelopeAudioSignal
 *
 * Returns: the new #AgsFxEnvelopeAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxEnvelopeAudioSignal*
ags_fx_envelope_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxEnvelopeAudioSignal *fx_envelope_audio_signal;

  fx_envelope_audio_signal = (AgsFxEnvelopeAudioSignal *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO_SIGNAL,
								       "source", audio_signal,
								       NULL);

  return(fx_envelope_audio_signal);
}
