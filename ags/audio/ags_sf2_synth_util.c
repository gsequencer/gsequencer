/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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
#include <ags/audio/ags_filter_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sf2_synth_util
 * @short_description: frequency modulation synth util
 * @title: AgsSF2SynthUtil
 * @section_id:
 * @include: ags/audio/ags_sf2_synth_util.h
 *
 * Utility functions to compute SF2 synths.
 */

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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s8(im_buffer,
			   frame_count,
			   samplerate,
			   base_key,
			   tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s8_to_s8(buffer + l, 1,
					    im_buffer + k, 1,
					    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s8(buffer + l, 1,
				      im_buffer + k, 1,
				      copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);

  ags_filter_util_pitch_s16(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s16_to_s16(buffer + l, 1,
					      im_buffer + k, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s16(buffer + l, 1,
				       im_buffer + k, 1,
				       copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s24(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s24_to_s24(buffer + l, 1,
					      im_buffer + k, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s24(buffer + l, 1,
				       im_buffer + k, 1,
				       copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s32(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s32_to_s32(buffer + l, 1,
					      im_buffer + k, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s32(buffer + l, 1,
				       im_buffer + k, 1,
				       copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s64(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_s64_to_s64(buffer + l, 1,
					      im_buffer + k, 1,
					      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_s64(buffer + l, 1,
				       im_buffer + k, 1,
				       copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_float(im_buffer,
			      frame_count,
			      samplerate,
			      base_key,
			      tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_float_to_float(buffer + l, 1,
						  im_buffer + k, 1,
						  copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_float(buffer + l, 1,
					 im_buffer + k, 1,
					 copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_double(im_buffer,
			       frame_count,
			       samplerate,
			       base_key,
			       tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_double_to_double(buffer + l, 1,
						    im_buffer + k, 1,
						    copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_double(buffer + l, 1,
					  im_buffer + k, 1,
					  copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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

  guint i;
  guint j;
  guint k;
  guint l;
  gboolean pong_copy;
  
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

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_complex(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  pong_copy = FALSE;
  
  for(i = 0, j = 0, k = 0, l = 0; i < offset + n_frames;){
    guint copy_n_frames;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean success;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    success = FALSE;
    
    if(offset + copy_n_frames > n_frames){
      copy_n_frames = n_frames - offset;
    }
    
    if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
       (loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	!pong_copy)){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    if(loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	      set_loop_start = TRUE;
	    }else{
	      set_loop_end = TRUE;
	    }
	  }else{
	    //nothing
	  }
	}
      }
    }else if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	     pong_copy){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k - copy_n_frames < loop_start){
	    copy_n_frames = copy_n_frames - (loop_start - (k - copy_n_frames));

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}	
      }
    }

    if(!pong_copy){
      if(l + copy_n_frames >= buffer_size){
	copy_n_frames = copy_n_frames - ((l + copy_n_frames) - buffer_size);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }else{
      if(l - copy_n_frames < 0){
	copy_n_frames = copy_n_frames + (l - copy_n_frames);
      
	if(i + copy_n_frames > offset){
	  success = TRUE;
	}
      }
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      if(i < offset){
	start_frame = (i + copy_n_frames) - offset;
      }else{
	start_frame = 0;
      }
      
      if(!pong_copy){	
	ags_audio_buffer_util_copy_complex_to_complex(buffer + l, 1,
						      im_buffer + k, 1,
						      copy_n_frames - start_frame);
      }else{
	ags_audio_buffer_util_pong_complex(buffer + l, 1,
					   im_buffer + k, 1,
					   copy_n_frames - start_frame);
      }
    }

    if(success){
      break;
    }

    i += copy_n_frames;

    if(j + copy_n_frames < buffer_size){
      j += copy_n_frames;
    }else{
      j = (j + copy_n_frames) % buffer_size;
    }

    if(!pong_copy){
      k += copy_n_frames;
    }else{
      k -= copy_n_frames;
    }

    l += copy_n_frames;

    if(l >= buffer_size){
      l = 0;
    }
    
    if(set_loop_start){
      k = loop_start;

      if(pong_copy){
	pong_copy = FALSE;
      }
    }
    
    if(set_loop_end){
      k = loop_end;

      if(loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG &&
	 k + copy_n_frames < n_frames - offset){
	pong_copy = TRUE;
      }
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
