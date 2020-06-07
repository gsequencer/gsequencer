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

#include <ags/audio/file/ags_sound_resource.h>

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
  //TODO:JK: implement me

  return(NULL);
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s8(im_buffer,
			   frame_count,
			   samplerate,
			   base_key,
			   tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }

    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_s8_to_s8(buffer + (i % buffer_size), 1,
					  im_buffer + k, 1,
					  copy_n_frames - start_frame);
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

    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s16(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }

    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_s16_to_s16(buffer + (i % buffer_size), 1,
					    im_buffer + k, 1,
					    copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s24(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }

    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_s24_to_s24(buffer + (i % buffer_size), 1,
					    im_buffer + k, 1,
					    copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s32(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }
    
    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_s32_to_s32(buffer + (i % buffer_size), 1,
					    im_buffer + k, 1,
					    copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_s64(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }
    
    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_s64_to_s64(buffer + (i % buffer_size), 1,
					    im_buffer + k, 1,
					    copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_float(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }

    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_float_to_float(buffer + (i % buffer_size), 1,
						im_buffer + k, 1,
						copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_double(im_buffer,
			    frame_count,
			    samplerate,
			    base_key,
			    tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }
      
    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_double_to_double(buffer + (i % buffer_size), 1,
						  im_buffer + k, 1,
						  copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
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
  
  g_object_get(ipatch_sample->sample,
	       "root-note", &midi_key,
	       NULL);
  
  base_key = (gdouble) midi_key - 21.0;

  tuning = 100.0 * (note - base_key);
  
  ags_filter_util_pitch_complex(im_buffer,
				frame_count,
				samplerate,
				base_key,
				tuning);

  for(i = 0, j = 0, k = 0; i < offset + n_frames;){
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
       loop_mode == AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG){
      if(k >= loop_end){
	//nothing
      }else{
	if(k + copy_n_frames > n_frames - offset){
	  copy_n_frames = copy_n_frames - ((k + copy_n_frames) - (n_frames - offset));
	  
	  set_loop_end = TRUE;
	}else{
	  if(k + copy_n_frames > loop_end){
	    copy_n_frames = copy_n_frames - ((k + copy_n_frames) - loop_end);

	    set_loop_start = TRUE;
	  }else{
	    //nothing
	  }
	}
      }
    }
    
    if(i % buffer_size + copy_n_frames >= buffer_size){
      copy_n_frames = copy_n_frames - ((i % buffer_size + copy_n_frames) - buffer_size);
      
      success = TRUE;
    }
    
    if(i + copy_n_frames > offset){
      guint start_frame;

      start_frame = (i + copy_n_frames) - offset;

      ags_audio_buffer_util_copy_complex_to_complex(buffer + (i % buffer_size), 1,
						    im_buffer + k, 1,
						    copy_n_frames - start_frame);
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
    
    k += copy_n_frames;
    
    if(set_loop_start){
      k = loop_start;
    }
    
    if(set_loop_end){
      k = loop_end;
    }
  }

  ags_stream_free(sample_buffer);
  ags_stream_free(im_buffer);
}
