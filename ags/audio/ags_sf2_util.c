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

#include <ags/audio/ags_sf2_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_sf2_synth_util.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_generic_pitch_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sf2_util
 * @short_description: SF2 synth util
 * @title: AgsSF2Util
 * @section_id:
 * @include: ags/audio/ags_sf2_util.h
 *
 * Utility functions to compute SF2 synths.
 */

/**
 * ags_sf2_util_copy_s8:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_s8(AgsSF2 *sf2,
		     gint8 *buffer,
		     guint buffer_size,
		     AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_s8(generic_pitch,
				    im_buffer,
				    frame_count,
				    sf2->samplerate,
				    base_key,
				    tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_s16:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_s16(AgsSF2 *sf2,
		      gint16 *buffer,
		      guint buffer_size,
		      AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_s16(generic_pitch,
				     im_buffer,
				     frame_count,
				     sf2->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_s24:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_s24(AgsSF2 *sf2,
		      gint32 *buffer,
		      guint buffer_size,
		      AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_s24(generic_pitch,
				     im_buffer,
				     frame_count,
				     sf2->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_s32:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_s32(AgsSF2 *sf2,
		      gint32 *buffer,
		      guint buffer_size,
		      AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_s32(generic_pitch,
				     im_buffer,
				     frame_count,
				     sf2->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_s64:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_s64(AgsSF2 *sf2,
		      gint64 *buffer,
		      guint buffer_size,
		      AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_s64(generic_pitch,
				     im_buffer,
				     frame_count,
				     sf2->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_float:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_float(AgsSF2 *sf2,
			gfloat *buffer,
			guint buffer_size,
			AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_float(generic_pitch,
				       im_buffer,
				       frame_count,
				       sf2->samplerate,
				       base_key,
				       tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_double:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_double(AgsSF2 *sf2,
			 gdouble *buffer,
			 guint buffer_size,
			 AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_double(generic_pitch,
					im_buffer,
					frame_count,
					sf2->samplerate,
					base_key,
					tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
 * ags_sf2_util_copy_complex:
 * @sf2: the #AgsSF2-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @ipatch_sample: the #AgsIpatchSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sf2_util_copy_complex(AgsSF2 *sf2,
			  AgsComplex *buffer,
			  guint buffer_size,
			  AgsIpatchSample *ipatch_sample)
{
  AgsGenericPitch *generic_pitch;
  
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

  if(sf2 == NULL ||
     buffer == NULL ||
     ipatch_sample == NULL ||
     !AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  generic_pitch = AGS_GENERIC_PITCH(sf2->generic_pitch);
  
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
  
  if(source_samplerate != sf2->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sf2->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sf2->samplerate,
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
  
  ags_generic_pitch_util_compute_complex(generic_pitch,
					 im_buffer,
					 frame_count,
					 sf2->samplerate,
					 base_key,
					 tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sf2->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sf2->n_frames){
      copy_n_frames = sf2->n_frames - i0;
    }
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sf2->loop_end){
	  copy_n_frames = sf2->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sf2->loop_end){
	    copy_n_frames = sf2->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sf2->loop_start){
	    copy_n_frames = i1 - sf2->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sf2->offset){
      do_copy = TRUE;
      
      if(i0 < sf2->offset){
	start_frame = (i0 + copy_n_frames) - sf2->offset;
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
    
    if((sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_STANDARD ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_RELEASE ||
	sf2->loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG) &&
       sf2->loop_start >= 0 &&
       sf2->loop_end >= 0 &&
       sf2->loop_start < sf2->loop_end){
      /* can loop */
      if(sf2->loop_mode != AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sf2->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sf2->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sf2->loop_start;
	    
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
