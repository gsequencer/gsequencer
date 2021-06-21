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

#include <ags/audio/ags_sfz_util.h>

#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_sfz_synth_util.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_generic_pitch_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_sfz_util
 * @short_description: SFZ synth util
 * @title: AgsSFZUtil
 * @section_id:
 * @include: ags/audio/ags_sfz_util.h
 *
 * Utility functions to compute SFZ synths.
 */

/**
 * ags_sfz_get_note:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get note.
 * 
 * Returns: the note
 * 
 * Since: 3.9.1
 */
gdouble
ags_sfz_get_note(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0.0);
  }
  
  return(sfz->note);
}

/**
 * ags_sfz_set_note:
 * @sfz: the #AgsSFZ-struct
 * @note: the note
 * 
 * Set note.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_note(AgsSFZ *sfz,
		 gdouble note)
{
  if(sfz == NULL){
    return;
  }

  sfz->note = note;
}

/**
 * ags_sfz_get_volume:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get volume.
 * 
 * Returns: the volume
 * 
 * Since: 3.9.1
 */
gdouble
ags_sfz_get_volume(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0.0);
  }

  return(sfz->volume);
}

/**
 * ags_sfz_set_volume:
 * @sfz: the #AgsSFZ-struct
 * @volume: the volume
 * 
 * Set volume.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_volume(AgsSFZ *sfz,
		   gdouble volume)
{
  if(sfz == NULL){
    return;
  }

  sfz->volume = volume;
}

/**
 * ags_sfz_get_samplerate:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_samplerate(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->samplerate);
}

/**
 * ags_sfz_set_samplerate:
 * @sfz: the #AgsSFZ-struct
 * @samplerate: the samplerate
 * 
 * Set samplerate.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_samplerate(AgsSFZ *sfz,
		       guint samplerate)
{
  if(sfz == NULL){
    return;
  }

  sfz->samplerate = samplerate;
}

/**
 * ags_sfz_get_offset:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get offset.
 * 
 * Returns: the offset
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_offset(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->offset);
}

/**
 * ags_sfz_set_offset:
 * @sfz: the #AgsSFZ-struct
 * @offset: the offset
 * 
 * Set offset.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_offset(AgsSFZ *sfz,
		   guint offset)
{
  if(sfz == NULL){
    return;
  }

  sfz->offset = 0;
}

/**
 * ags_sfz_get_n_frames:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get n-frames.
 * 
 * Returns: the n-frames
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_n_frames(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->n_frames);
}

/**
 * ags_sfz_set_n_frames:
 * @sfz: the #AgsSFZ-struct
 * @n_frames: the n-frames
 * 
 * Set n-frames.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_n_frames(AgsSFZ *sfz,
		     guint n_frames)
{
  if(sfz == NULL){
    return;
  }

  sfz->n_frames = n_frames;
}

/**
 * ags_sfz_get_loop_mode:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get loop mode.
 * 
 * Returns: the loop mode
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_loop_mode(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->loop_mode);
}

/**
 * ags_sfz_set_loop_mode:
 * @sfz: the #AgsSFZ-struct
 * @loop_mode: the loop mode
 * 
 * Set loop mode.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_loop_mode(AgsSFZ *sfz,
		      guint loop_mode)
{
  if(sfz == NULL){
    return;
  }

  sfz->loop_mode = loop_mode;
}

/**
 * ags_sfz_get_loop_start:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get loop start.
 * 
 * Returns: the loop start
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_loop_start(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->loop_start);
}

/**
 * ags_sfz_set_loop_start:
 * @sfz: the #AgsSFZ-struct
 * @loop_start: the loop start
 * 
 * Set loop start.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_loop_start(AgsSFZ *sfz,
		       guint loop_start)
{
  if(sfz == NULL){
    return;
  }

  sfz->loop_start = loop_start;
}

/**
 * ags_sfz_get_loop_end:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get loop end.
 * 
 * Returns: the loop end
 * 
 * Since: 3.9.1
 */
guint
ags_sfz_get_loop_end(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(0);
  }

  return(sfz->loop_end);
}

/**
 * ags_sfz_set_loop_end:
 * @sfz: the #AgsSFZ-struct
 * @loop_end: the loop end
 * 
 * Set loop end.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_loop_end(AgsSFZ *sfz,
		     guint loop_end)
{
  if(sfz == NULL){
    return;
  }

  sfz->loop_end = loop_end;
}

/**
 * ags_sfz_get_generic_pitch:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get generic pitch.
 * 
 * Returns: the generic pitch
 * 
 * Since: 3.9.1
 */
gpointer
ags_sfz_get_generic_pitch(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(NULL);
  }

  return(sfz->generic_pitch);
}

/**
 * ags_sfz_set_generic_pitch:
 * @sfz: the #AgsSFZ-struct
 * @generic_pitch: the generic pitch
 * 
 * Set generic pitch.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_generic_pitch(AgsSFZ *sfz,
			  gpointer generic_pitch)
{
  if(sfz == NULL){
    return;
  }

  g_free(sfz->generic_pitch);
  
  sfz->generic_pitch = generic_pitch;
}

/**
 * ags_sfz_get_user_data:
 * @sfz: the #AgsSFZ-struct
 * 
 * Get user data.
 * 
 * Returns: the user data
 * 
 * Since: 3.9.1
 */
gpointer
ags_sfz_get_user_data(AgsSFZ *sfz)
{
  if(sfz == NULL){
    return(NULL);
  }

  return(sfz->user_data);
}

/**
 * ags_sfz_set_user_data:
 * @sfz: the #AgsSFZ-struct
 * @user_data: the user data
 * 
 * Set user data.
 * 
 * Since: 3.9.1
 */
void
ags_sfz_set_user_data(AgsSFZ *sfz,
		      gpointer user_data)
{
  if(sfz == NULL){
    return;
  }

  sfz->user_data = user_data;
}

/**
 * ags_sfz_util_copy_s8:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_s8(AgsSFZ *sfz,
		     gint8 *buffer,
		     guint buffer_size,
		     AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gint8 *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
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
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_s8(generic_pitch,
				    im_buffer,
				    frame_count,
				    sfz->samplerate,
				    base_key,
				    tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_s16:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_s16(AgsSFZ *sfz,
		      gint16 *buffer,
		      guint buffer_size,
		      AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gint16 *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_s16(generic_pitch,
				     im_buffer,
				     frame_count,
				     sfz->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_s24:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_s24(AgsSFZ *sfz,
		      gint32 *buffer,
		      guint buffer_size,
		      AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gint32 *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S24,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_s24(generic_pitch,
				     im_buffer,
				     frame_count,
				     sfz->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_s32:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_s32(AgsSFZ *sfz,
		      gint32 *buffer,
		      guint buffer_size,
		      AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gint32 *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S32,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_s32(generic_pitch,
				     im_buffer,
				     frame_count,
				     sfz->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_s64:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_s64(AgsSFZ *sfz,
		      gint64 *buffer,
		      guint buffer_size,
		      AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gint64 *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S64,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_s64(generic_pitch,
				     im_buffer,
				     frame_count,
				     sfz->samplerate,
				     base_key,
				     tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_float:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_float(AgsSFZ *sfz,
			gfloat *buffer,
			guint buffer_size,
			AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gfloat *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_float(generic_pitch,
				       im_buffer,
				       frame_count,
				       sfz->samplerate,
				       base_key,
				       tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_double:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_double(AgsSFZ *sfz,
			 gdouble *buffer,
			 guint buffer_size,
			 AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  gdouble *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_double(generic_pitch,
					im_buffer,
					frame_count,
					sfz->samplerate,
					base_key,
					tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy_complex:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy_complex(AgsSFZ *sfz,
			  AgsComplex *buffer,
			  guint buffer_size,
			  AgsSFZSample *sfz_sample)
{
  AgsGenericPitch *generic_pitch;
  
  void *sample_buffer;

  AgsComplex *im_buffer;

  gint key;
  gint pitch_keycenter;
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

  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }
  
  generic_pitch = AGS_GENERIC_PITCH(sfz->generic_pitch);

  source_frame_count = 0;

  source_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  source_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;  
  
  ags_sound_resource_info(AGS_SOUND_RESOURCE(sfz_sample),
			  &source_frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sfz_sample),
				 NULL,
				 &source_samplerate,
				 &source_buffer_size,
				 &source_format);

  sample_buffer = ags_stream_alloc(source_frame_count,
				   AGS_SOUNDCARD_DOUBLE);

  ags_stream_free(sfz_sample->buffer);
  
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * source_frame_count,
					sfz_sample->format);
  
  sfz_sample->offset = 0;

  ags_sound_resource_read(AGS_SOUND_RESOURCE(sfz_sample),
			  sample_buffer, 1,
			  0,
			  source_frame_count, AGS_SOUNDCARD_DOUBLE);
  
  /* resample if needed */
  frame_count = source_frame_count;
  
  if(source_samplerate != sfz->samplerate){
    void *tmp_sample_buffer;

    guint tmp_frame_count;

    tmp_frame_count = (sfz->samplerate / source_samplerate) * source_frame_count;

    tmp_sample_buffer = ags_stream_alloc(tmp_frame_count,
					 AGS_SOUNDCARD_DOUBLE);

    ags_audio_buffer_util_resample_with_buffer(sample_buffer, 1,
					       AGS_AUDIO_BUFFER_UTIL_DOUBLE, source_samplerate,
					       source_frame_count,
					       sfz->samplerate,
					       tmp_frame_count,
					       tmp_sample_buffer);
    
    ags_stream_free(sample_buffer);

    sample_buffer = tmp_sample_buffer;
    
    frame_count = tmp_frame_count;
  }

  /* format */
  im_buffer = ags_stream_alloc(frame_count,
			       AGS_SOUNDCARD_SIGNED_8_BIT);

  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						  AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  ags_audio_buffer_util_copy_buffer_to_buffer(im_buffer, 1, 0,
					      sample_buffer, 1, 0,
					      frame_count, copy_mode);
  
  /* pitch */
  key = ags_sfz_sample_get_key(sfz_sample);
  pitch_keycenter = ags_sfz_sample_get_pitch_keycenter(sfz_sample);

  if(key >= 0){
    base_key = (gdouble) key - 21.0;
  }else if(pitch_keycenter >= 0){
    base_key = (gdouble) pitch_keycenter - 21.0;
  }else{
    base_key = 48.0;
  }
  
  tuning = 100.0 * ((sfz->note + 48.0) - base_key);
  
  ags_generic_pitch_util_compute_complex(generic_pitch,
					 im_buffer,
					 frame_count,
					 sfz->samplerate,
					 base_key,
					 tuning);

  success = FALSE;
  pong_copy = FALSE;

  for(i0 = 0, i1 = 0, i2 = 0; i0 < sfz->n_frames && !success && i2 < buffer_size; ){
    guint copy_n_frames;
    guint start_frame;

    gboolean set_loop_start;
    gboolean set_loop_end;
    gboolean do_copy;
    
    copy_n_frames = buffer_size;

    set_loop_start = FALSE;
    set_loop_end = FALSE;

    do_copy = FALSE;
    
    if(i0 + copy_n_frames > sfz->n_frames){
      copy_n_frames = sfz->n_frames - i0;
    }
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(i1 + copy_n_frames >= sfz->loop_end){
	  copy_n_frames = sfz->loop_end - i1;
	  
	  set_loop_start = TRUE;
	}
      }else{
	if(!pong_copy){
	  if(i1 + copy_n_frames >= sfz->loop_end){
	    copy_n_frames = sfz->loop_end - i1;

	    set_loop_end = TRUE;
	  }
	}else{
	  if(i1 - copy_n_frames <= sfz->loop_start){
	    copy_n_frames = i1 - sfz->loop_start;
	    
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
      
    if(i0 + copy_n_frames > sfz->offset){
      do_copy = TRUE;
      
      if(i0 < sfz->offset){
	start_frame = (i0 + copy_n_frames) - sfz->offset;
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
    
    if((sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE ||
	sfz->loop_mode == AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG) &&
       sfz->loop_start >= 0 &&
       sfz->loop_end >= 0 &&
       sfz->loop_start < sfz->loop_end){
      /* can loop */
      if(sfz->loop_mode != AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG){
	if(set_loop_start){
	  i1 = sfz->loop_start;
	}else{
	  i1 += copy_n_frames;
	}
      }else{
	if(!pong_copy){
	  if(set_loop_end){
	    i1 = sfz->loop_end; 
	    
	    pong_copy = TRUE;
	  }else{
	    i1 += copy_n_frames;
	  }
	}else{
	  if(set_loop_start){
	    i1 = sfz->loop_start;
	    
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
 * ags_sfz_util_copy:
 * @sfz: the #AgsSFZ-struct
 * @buffer: the audio buffer
 * @buffer_size: the audio buffer length
 * @sfz_sample: the #AgsSFZSample
 * @audio_buffer_util_format: the audio data format
 * 
 * Generate Soundfont2 wave.
 * 
 * Since: 3.9.0
 */
void
ags_sfz_util_copy(AgsSFZ *sfz,
		  void *buffer,
		  guint buffer_size,
		  AgsSFZSample *sfz_sample,
		  guint audio_buffer_util_format)
{
  if(sfz == NULL ||
     buffer == NULL ||
     sfz_sample == NULL ||
     !AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  switch(audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_sfz_util_copy_s8(sfz,
			 (gint8 *) buffer,
			 buffer_size,
			 sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_sfz_util_copy_s16(sfz,
			  (gint16 *) buffer,
			  buffer_size,
			  sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_sfz_util_copy_s24(sfz,
			  (gint32 *) buffer,
			  buffer_size,
			  sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_sfz_util_copy_s32(sfz,
			  (gint32 *) buffer,
			  buffer_size,
			  sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_sfz_util_copy_s64(sfz,
			  (gint64 *) buffer,
			  buffer_size,
			  sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_sfz_util_copy_float(sfz,
			    (float *) buffer,
			    buffer_size,
			    sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_sfz_util_copy_double(sfz,
			     (double *) buffer,
			     buffer_size,
			     sfz_sample);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_sfz_util_copy_complex(sfz,
			      (AgsComplex *) buffer,
			      buffer_size,
			      sfz_sample);
  }
  break;
  default:
  {
    g_warning("ags_sfz_util_copy() - unsupported format");
  }
  }
}
