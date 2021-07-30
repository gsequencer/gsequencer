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

#include <ags/audio/ags_sf2_synth_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fluid_pitch_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

#include <math.h>
#include <complex.h>

gpointer ags_sf2_synth_util_strct_copy(gpointer ptr);
void ags_sf2_synth_util_strct_free(gpointer ptr);

/**
 * SECTION:ags_sf2_synth_util
 * @short_description: SF2 synth util
 * @title: AgsSF2SynthUtil
 * @section_id:
 * @include: ags/audio/ags_sf2_synth_util.h
 *
 * Utility functions to compute SF2 synths.
 */

GType
ags_sf2_synth_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_synth_util = 0;

    ags_type_sf2_synth_util =
      g_boxed_type_register_static("AgsSF2SynthUtil",
				   (GBoxedCopyFunc) ags_sf2_synth_util_strct_copy,
				   (GBoxedFreeFunc) ags_sf2_synth_util_strct_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_synth_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_sf2_synth_util_strct_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsSF2SynthUtil));
 
  return(retval);
}

void
ags_sf2_synth_util_strct_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_sf2_synth_util_midi_locale_find_sample_near_midi_key:
 * @ipatch: the #AgsIpatch
 * @bank: the bank
 * @program: the program
 * @midi_key: the MIDI key
 * @preset: (out): the preset
 * @instrument: (out): the instrument
 * @sample: (out): the sample
 * 
 * Find sample near MIDI key.
 * 
 * Since: 3.4.0
 */
AgsIpatchSample*
ags_sf2_synth_util_midi_locale_find_sample_near_midi_key(AgsIpatch *ipatch,
							 gint bank,
							 gint program,
							 gint midi_key,
							 gchar **preset,
							 gchar **instrument,
							 gchar **sample)
{
  AgsIpatchSample *ipatch_sample;
  
  IpatchSF2 *sf2;
  IpatchSF2Preset *sf2_preset;
  IpatchSF2Inst *sf2_instrument;
  IpatchSF2Sample *sf2_sample, *first_sf2_sample;
  IpatchSF2Sample *matching_sf2_sample;
  IpatchItem *pzone;
  IpatchItem *izone;

  IpatchList *pzone_list;
  IpatchList *izone_list;

  IpatchIter pzone_iter;
  IpatchIter izone_iter;

  gchar *matching_preset;
  gchar *matching_instrument;
  gchar *matching_sample;
  
  GError *error;

  if(!AGS_IS_IPATCH(ipatch)){
    return(NULL);
  }

  ipatch_sample = NULL;
  matching_sf2_sample = NULL;

  matching_preset = NULL;
  matching_instrument = NULL;
  matching_sample = NULL;
  
  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }
  
  sf2_preset = ipatch_sf2_find_preset(sf2,
				      NULL,
				      bank,
				      program,
				      NULL);

  if(sf2_preset != NULL){
    if(preset != NULL){
      matching_preset = ipatch_sf2_preset_get_name(sf2_preset);
    }
    
    pzone_list = ipatch_sf2_preset_get_zones(sf2_preset);

    ipatch_list_init_iter(pzone_list, &pzone_iter);
    
    if(ipatch_iter_first(&pzone_iter) != NULL){
      do{
	IpatchRange *note_range;
	
	pzone = ipatch_iter_get(&pzone_iter);

	g_object_get(pzone,
		     "note-range", &note_range,
		     NULL);

	first_sf2_sample = NULL;
	
	if(note_range->low <= midi_key &&
	   note_range->high >= midi_key){
	  sf2_instrument = (IpatchItem *) ipatch_sf2_pzone_get_inst(ipatch_iter_get(&pzone_iter));

	  if(instrument != NULL){
	    matching_instrument = ipatch_sf2_inst_get_name(sf2_instrument);
	  }
	  
	  izone_list = ipatch_sf2_inst_get_zones(sf2_instrument);

	  if(izone_list != NULL){
	    ipatch_list_init_iter(izone_list, &izone_iter);

	    if(ipatch_iter_first(&izone_iter) != NULL){      
	      izone = ipatch_iter_get(&izone_iter);

	      first_sf2_sample = ipatch_sf2_izone_get_sample(izone);
	      
	      do{
		gint root_note;
		
		izone = ipatch_iter_get(&izone_iter);

		sf2_sample = ipatch_sf2_izone_get_sample(izone);
 
		g_object_get(sf2_sample,
			     "root-note", &root_note,
			     NULL);
		
		if(root_note == 60){
		  matching_sf2_sample = sf2_sample;

		  break;
		}
	      }while(ipatch_iter_next(&izone_iter) != NULL);
	    }
	  }

	  if(matching_sf2_sample == NULL){
	    matching_sf2_sample = first_sf2_sample;
	  }
	  
	  break;
	}
      }while(ipatch_iter_next(&pzone_iter) != NULL);
    }
  }

  if(matching_sf2_sample != NULL){
    GList *start_list, *list;

    if(sample != NULL){
      matching_sample = ipatch_sf2_sample_get_name(matching_sf2_sample);
    }
    
    list =
      start_list = ags_sound_container_get_resource_all(AGS_SOUND_CONTAINER(ipatch));

    while(list != NULL){
      if(AGS_IPATCH_SAMPLE(list->data)->sample == matching_sf2_sample){
	ipatch_sample = list->data;

	break;
      }
      
      list = list->next;
    }
  }

  if(preset != NULL){
    preset[0] = matching_preset;
  }

  if(instrument != NULL){
    instrument[0] = matching_instrument;
  }

  if(sample != NULL){
    sample[0] = matching_sample;
  }
  
  return(ipatch_sample);
}

/**
 * ags_sf2_synth_util_copy_s8:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s8(gint8 *buffer,
			   guint buffer_size,
			   AgsIpatchSample *ipatch_sample,
			   gdouble note,
			   gdouble volume,
			   guint samplerate,
			   guint offset, guint n_frames,
			   guint loop_mode,
			   gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint8 *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S8,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s8(im_buffer,
				  frame_count,
				  samplerate,
				  base_key,
				  tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s8_to_s8(buffer + i2, 1,
					    im_buffer + i1, 1,
					    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s8(buffer + i2, 1,
				      im_buffer + i1, 1,
				      copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_s16:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s16(gint16 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint16 *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;
  
  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_16_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);
    
    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);

  ags_fluid_pitch_util_compute_s16(im_buffer,
				   frame_count,
				   samplerate,
				   base_key,
				   tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s16_to_s16(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s16(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_s24:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s24(gint32 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_24_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s24(im_buffer,
				   frame_count,
				   samplerate,
				   base_key,
				   tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s24_to_s24(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s24(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_s32:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s32(gint32 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint32 *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_32_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s32(im_buffer,
				   frame_count,
				   samplerate,
				   base_key,
				   tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s32_to_s32(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s32(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_s64:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_s64(gint64 *buffer,
			    guint buffer_size,
			    AgsIpatchSample *ipatch_sample,
			    gdouble note,
			    gdouble volume,
			    guint samplerate,
			    guint offset, guint n_frames,
			    guint loop_mode,
			    gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gint64 *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_64_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_s64(im_buffer,
				   frame_count,
				   samplerate,
				   base_key,
				   tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s64_to_s64(buffer + i2, 1,
					      im_buffer + i1, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s64(buffer + i2, 1,
				       im_buffer + i1, 1,
				       copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_float:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_float(gfloat *buffer,
			      guint buffer_size,
			      AgsIpatchSample *ipatch_sample,
			      gdouble note,
			      gdouble volume,
			      guint samplerate,
			      guint offset, guint n_frames,
			      guint loop_mode,
			      gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gfloat *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_FLOAT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_float(im_buffer,
				     frame_count,
				     samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_float_to_float(buffer + i2, 1,
						  im_buffer + i1, 1,
						  copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_float(buffer + i2, 1,
					 im_buffer + i1, 1,
					 copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_double:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_double(gdouble *buffer,
			       guint buffer_size,
			       AgsIpatchSample *ipatch_sample,
			       gdouble note,
			       gdouble volume,
			       guint samplerate,
			       guint offset, guint n_frames,
			       guint loop_mode,
			       gint loop_start, gint loop_end)
{
  void *sample_buffer;

  gdouble *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_DOUBLE);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;
  
  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_double(im_buffer,
				      frame_count,
				      samplerate,
				      base_key,
				      tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_double_to_double(buffer + i2, 1,
						    im_buffer + i1, 1,
						    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_double(buffer + i2, 1,
					  im_buffer + i1, 1,
					  copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }
  
  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy_complex:
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: loop start
 * @loop_end: loop end
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy_complex(AgsComplex *buffer,
				guint buffer_size,
				AgsIpatchSample *ipatch_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint loop_mode,
				gint loop_start, gint loop_end)
{
  void *sample_buffer;

  AgsComplex *im_buffer;

  gint midi_key;
  guint frame_count;
  gdouble base_key;
  gdouble tuning;
  guint source_frame_count;
  guint source_samplerate;
  guint source_buffer_size;
  guint source_format;
  guint copy_mode;

  guint i0, i1, i2;
  gboolean success;
  gboolean pong_copy;

  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }
  
  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  

  ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(ipatch_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(ipatch_sample->buffer);
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * source_frame_count,
					   ipatch_sample->format);
  
  ipatch_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(ipatch_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);

  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_COMPLEX);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);

  /* pitch */
  midi_key = 60;

  if(ipatch_sample->sample != NULL){
    gint tmp_midi_key;
    
    g_object_get(ipatch_sample->sample,
		 "root-note", &tmp_midi_key,
		 NULL);

    if(tmp_midi_key >= 0 &&
       tmp_midi_key < 128){
      midi_key = tmp_midi_key;
    }
  }
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * ((note + 48.0) - base_key);
  
  ags_fluid_pitch_util_compute_complex(im_buffer,
				       frame_count,
				       samplerate,
				       base_key,
				       tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > n_frames){
      copy_n_frames = n_frames - i0;
    }
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= loop_end){
	  copy_n_frames = loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= loop_end){
	    copy_n_frames = loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= loop_start){
	    copy_n_frames = i1 - loop_start;
	    
	    set_loop_start = TRUE;
	  }
	}
      }
    }else{
      /* can't loop */
      if(i1 + copy_n_frames > frame_count){
	copy_n_frames = frame_count - i1;

	success = TRUE;
      }
    }

    start_frame = 0;
      
    if(i0 + copy_n_frames > offset){
      do_copy = TRUE;
      
      if(i0 < offset){
	start_frame = (i0 + copy_n_frames) - offset;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_complex_to_complex(buffer + i2, 1,
						      im_buffer + i1, 1,
						      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_complex(buffer + i2, 1,
					   im_buffer + i1, 1,
					   copy_n_frames - start_frame);
      }
    }

    i0 += copy_n_frames;
    
    if((loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       loop_start >= 0 &&
       loop_end >= 0 &&
       loop_start < loop_end){
      /* can loop */
      if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = loop_start;
	    
	    pong_copy = FALSE;
	  }else{
	    i1 -= copy_n_frames;
	  }
	}
      }
    }else{
      /* can't loop */
      i1 += copy_n_frames;
    }    

    if(do_copy){
      i2 += (copy_n_frames - start_frame);
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}

/**
 * ags_sf2_synth_util_copy:
 * @buffer: the audio buffer
 * @buffer_size: the buffer size
 * @ipatch_sample: the #AgsIpatchSample
 * @note: the note
 * @volume: the volume of the sin wave
 * @samplerate: the samplerate
 * @audio_buffer_util_format: the audio data format
 * @offset: start frame
 * @n_frames: generate n frames
 * @loop_mode: the loop mode
 * @loop_start: the loop start
 * @loop_end: the loop end
 *
 * Generate Soundfont2 wave.
 *
 * Since: 3.4.0
 */
void
ags_sf2_synth_util_copy(void *buffer,
			guint buffer_size,
			AgsIpatchSample *ipatch_sample,
			gdouble note,
			gdouble volume,
			guint samplerate, guint audio_buffer_util_format,
			guint offset, guint n_frames,
			guint loop_mode,
			gint loop_start, gint loop_end)
{
  if(buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_sf2_synth_util_copy_s8((gint8 *) buffer,
			       buffer_size,
			       ipatch_sample,
			       note,
			       volume,
			       samplerate,
			       offset, n_frames,
			       loop_mode,
			       loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_sf2_synth_util_copy_s16((gint16 *) buffer,
				buffer_size,
				ipatch_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_sf2_synth_util_copy_s24((gint32 *) buffer,
				buffer_size,
				ipatch_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_sf2_synth_util_copy_s32((gint32 *) buffer,
				buffer_size,
				ipatch_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_sf2_synth_util_copy_s64((gint64 *) buffer,
				buffer_size,
				ipatch_sample,
				note,
				volume,
				samplerate,
				offset, n_frames,
				loop_mode,
				loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_sf2_synth_util_copy_float((float *) buffer,
				  buffer_size,
				  ipatch_sample,
				  note,
				  volume,
				  samplerate,
				  offset, n_frames,
				  loop_mode,
				  loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_sf2_synth_util_copy_double((double *) buffer,
				   buffer_size,
				   ipatch_sample,
				   note,
				   volume,
				   samplerate,
				   offset, n_frames,
				   loop_mode,
				   loop_start, loop_end);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_sf2_synth_util_copy_complex((AgsComplex *) buffer,
				    buffer_size,
				    ipatch_sample,
				    note,
				    volume,
				    samplerate,
				    offset, n_frames,
				    loop_mode,
				    loop_start, loop_end);
  }
  break;
  default:
  {
    g_warning("ags_sf2_synth_util_copy() - unsupported format");
  }
  }
}
