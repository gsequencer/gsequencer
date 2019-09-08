/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/file/ags_sound_resource.h>

#include <ags/libags.h>

#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

void ags_sound_resource_base_init(AgsSoundResourceInterface *interface);

/**
 * SECTION:ags_sound_resource
 * @short_description: read/write audio
 * @title: AgsSoundResource
 * @section_id:
 * @include: ags/audio/file/ags_sound_resource.h
 *
 * The #AgsSoundResource interface gives you a unique access to file related
 * IO operations.
 */

GType
ags_sound_resource_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sound_resource = 0;

    static const GTypeInfo ags_sound_resource_info = {
      sizeof(AgsSoundResourceInterface),
      (GBaseInitFunc) ags_sound_resource_base_init,
      NULL, /* base_finalize */
    };

    ags_type_sound_resource = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsSoundResource", &ags_sound_resource_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sound_resource);
  }

  return g_define_type_id__volatile;
}


void
ags_sound_resource_base_init(AgsSoundResourceInterface *interface)
{
  /* empty */
}

/**
 * ags_sound_resource_open:
 * @sound_resource: the #AgsSoundResource
 * @filename: the filename as string
 * 
 * Open @sound_resource for reading and assign filename.
 * 
 * Returns: %TRUE if operation was successful, otherwise %FALSE.
 * 
 * Since: 2.0.0
 */
gboolean
ags_sound_resource_open(AgsSoundResource *sound_resource,
			gchar *filename)
{
  AgsSoundResourceInterface *sound_resource_interface;

  gboolean retval;

  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), FALSE);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->open, FALSE);

  retval = sound_resource_interface->open(sound_resource,
					   filename);

  return(retval);
}

/**
 * ags_sound_resource_rw_open:
 * @sound_resource: the #AgsSoundResource
 * @filename: the filename as string
 * @create: create
 * 
 * Open @sound_resource for reading/writing and assign filename. Setting
 * @create to %TRUE causes to create the file if it doesn't exist.
 * 
 * Returns: %TRUE if operation was successful, otherwise %FALSE.
 * 
 * Since: 2.0.0
 */
gboolean
ags_sound_resource_rw_open(AgsSoundResource *sound_resource,
			   gchar *filename,
			   guint audio_channels, guint samplerate,
			   gboolean create)
{
  AgsSoundResourceInterface *sound_resource_interface;

  gboolean retval;

  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), FALSE);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->open, FALSE);

  retval = sound_resource_interface->rw_open(sound_resource,
					     filename,
					     audio_channels, samplerate,
					     create);
  
  return(retval);
}

/**
 * ags_sound_resource_load:
 * @sound_resource: the #AgsSoundResource
 * 
 * Load audio data of @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_load(AgsSoundResource *sound_resource)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->load);
  sound_resource_interface->load(sound_resource);
}

/**
 * ags_sound_resource_info:
 * @sound_resource: the #AgsSoundResource
 * @frame_count: return location of frame count
 * @loop_start: return location of loop-start
 * @loop_end: return location of loop-end
 * 
 * Get information about @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_info(AgsSoundResource *sound_resource,
			guint *frame_count,
			guint *loop_start, guint *loop_end)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->info);
  sound_resource_interface->info(sound_resource,
				 frame_count,
				 loop_start, loop_end);
}

/**
 * ags_sound_resource_set_presets:
 * @sound_resource: the #AgsSoundResource
 * @channels: channels to set
 * @samplerate: samplerate to set
 * @buffer_size: buffer-size to set
 * @format: format to set
 * 
 * Set presets of @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_set_presets(AgsSoundResource *sound_resource,
			       guint channels,
			       guint samplerate,
			       guint buffer_size,
			       guint format)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->set_presets);
  sound_resource_interface->set_presets(sound_resource,
					channels,
					samplerate,
					buffer_size,
					format);
}

/**
 * ags_sound_resource_get_presets:
 * @sound_resource: the #AgsSoundResource
 * @channels: return location of channels
 * @samplerate: return location of samplerate
 * @buffer_size: return location of buffer-size
 * @format: return location of format
 * 
 * Get presets of @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_get_presets(AgsSoundResource *sound_resource,
			       guint *channels,
			       guint *samplerate,
			       guint *buffer_size,
			       guint *format)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->get_presets);
  sound_resource_interface->get_presets(sound_resource,
					channels,
					samplerate,
					buffer_size,
					format);
}

/**
 * ags_sound_resource_read:
 * @sound_resource: the #AgsSoundResource
 * @dbuffer: the destination buffer
 * @daudio_channels: destination buffer audio channel count
 * @audio_channel: the audio channel to read
 * @frame_count: the frame count to read
 * @format: the format to read
 * 
 * Read @frame_count number of frames from @sound_resource and copy the data
 * to @dbuffer using @format by skipping @daudio_channels.
 * 
 * Returns: the count of frames actually read
 * 
 * Since: 2.0.0
 */
guint
ags_sound_resource_read(AgsSoundResource *sound_resource,
			void *dbuffer, guint daudio_channels,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsSoundResourceInterface *sound_resource_interface;

  guint retval;
  
  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), 0);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->read, 0);

  retval = sound_resource_interface->read(sound_resource,
					  dbuffer, daudio_channels,
					  audio_channel,
					  frame_count, format);
  
  return(retval);
}

/**
 * ags_sound_resource_write:
 * @sound_resource: the #AgsSoundResource
 * @sbuffer: the source buffer
 * @saudio_channels: source buffer audio channel count
 * @audio_channel: the audio channel
 * @frame_count: the frame count to write
 * @format: the format to write
 * 
 * Write @sbuffer to @sound_resource @frame_count number of 
 * frames having @format by skipping @saudio_channels.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_write(AgsSoundResource *sound_resource,
			 void *sbuffer, guint saudio_channels,
			 guint audio_channel,
			 guint frame_count, guint format)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->write);
  sound_resource_interface->write(sound_resource,
				  sbuffer, saudio_channels,
				  audio_channel,
				  frame_count, format);
}

/**
 * ags_sound_resource_flush:
 * @sound_resource: the #AgsSoundResource
 * 
 * Flush @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_flush(AgsSoundResource *sound_resource)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->flush);  
  sound_resource_interface->flush(sound_resource);
}

/**
 * ags_sound_resource_seek:
 * @sound_resource: the #AgsSoundResource
 * @frame_count: the frame count
 * @whence: SEEK_SET, SEEK_CUR or SEEK_END
 * 
 * Seek the @sound_resource @frame_count from @whence.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_seek(AgsSoundResource *sound_resource,
			gint64 frame_count, gint whence)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->seek);  
  sound_resource_interface->seek(sound_resource,
				 frame_count, whence);
}

/**
 * ags_sound_resource_close:
 * @sound_resource: the #AgsSoundResource
 * 
 * Close @sound_resource.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_close(AgsSoundResource *sound_resource)
{
  AgsSoundResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->close);  
  sound_resource_interface->close(sound_resource);
}

/**
 * ags_sound_resource_read_audio_signal:
 * @sound_resource: the #AgsSoundResource
 * @soundcard: the #AgsSoundcard
 * @audio_channel: the audio channel or -1 for all
 * 
 * Read audio signal from @sound_resource.
 * 
 * Returns: a #GList-struct containing #AgsAudioSignal
 * 
 * Since: 2.0.0
 */
GList*
ags_sound_resource_read_audio_signal(AgsSoundResource *sound_resource,
				     GObject *soundcard,
				     gint audio_channel)
{
  GList *start_list;

  void *target_data, *data;

  guint frame_count;
  guint loop_start, loop_end;
  guint audio_channels;
  guint target_samplerate, samplerate;
  guint target_buffer_size, buffer_size;
  guint target_format, format;
  guint copy_mode;
  guint i, i_start, i_stop;

  if(!AGS_SOUND_RESOURCE(sound_resource)){
    return(NULL);
  }

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sound_resource),
			  &frame_count,
			  &loop_start, &loop_end);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sound_resource),
				 &audio_channels,
				 &samplerate,
				 &buffer_size,
				 NULL);
  
  g_object_get(G_OBJECT(sound_resource),
	       "format", &format,
	       NULL);

  if(soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      NULL,
			      &target_samplerate,
			      &target_buffer_size,
			      &target_format);
  }else{
    AgsConfig *config;

    config = ags_config_get_instance();

    target_samplerate = ags_soundcard_helper_config_get_samplerate(config);
    target_buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
    target_format = ags_soundcard_helper_config_get_format(config);
  }
  
  start_list = NULL;

  if(audio_channel == -1){
    i_start = 0;
    i_stop = audio_channels;
  }else{
    i_start = audio_channel;
    i_stop = i_start + 1;
  }
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
						  ags_audio_buffer_util_format_from_soundcard(format));

  data = NULL;

  if(samplerate != target_samplerate){
    data = ags_stream_alloc(buffer_size,
			    format);
  }
    
  for(i = i_start; i < i_stop; i++){
    AgsAudioSignal *audio_signal;

    GList *stream;

    ags_sound_resource_seek(AGS_SOUND_RESOURCE(sound_resource),
    			    0, G_SEEK_SET);
    
    audio_signal = ags_audio_signal_new(soundcard,
					NULL,
					NULL);
    g_object_set(audio_signal,
		 "samplerate", target_samplerate,
		 "buffer-size", target_buffer_size,
		 "format", target_format,
		 "loop-start", target_samplerate * (loop_start / samplerate),
		 "loop-end", target_samplerate * (loop_end / samplerate),
		 NULL);
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / target_buffer_size) + 1);
    audio_signal->stream_current = audio_signal->stream;
    
    start_list = g_list_prepend(start_list,
				audio_signal);

    stream = audio_signal->stream;

    g_object_set(audio_signal,
		 "last-frame", frame_count,
		 NULL);
    
    while(stream != NULL){
      if(samplerate != target_samplerate){
	if(format == AGS_SOUNDCARD_DOUBLE){
	  ags_audio_buffer_util_clear_double(data, 1,
					     buffer_size);
	}else if(format == AGS_SOUNDCARD_FLOAT){
	  ags_audio_buffer_util_clear_float(data, 1,
					    buffer_size);
	}else{
	  ags_audio_buffer_util_clear_buffer(data, 1,
					     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
	}
	
	ags_sound_resource_read(AGS_SOUND_RESOURCE(sound_resource),
				data, 1,
				i,
				buffer_size, format);

	target_data = ags_audio_buffer_util_resample(data, 1,
						     ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						     buffer_size,
						     target_samplerate);

	ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						    target_data, 1, 0,
						    target_buffer_size, copy_mode);

	free(target_data);
      }else{
	ags_sound_resource_read(AGS_SOUND_RESOURCE(sound_resource),
				stream->data, 1,
				i,
				target_buffer_size, target_format);
      }
      
      /* iterate */
      stream = stream->next;
    }
  }

  if(data != NULL){
    free(data);
  }

  start_list = g_list_reverse(start_list);

  g_list_foreach(start_list,
		 (GFunc) g_object_ref,
		 NULL);
  
  return(start_list);
}

/**
 * ags_sound_resource_read_wave:
 * @sound_resource: the #AgsSoundResource
 * @soundcard: the #AgsSoundcard
 * @audio_channel: the audio channel or -1 for all
 * @x_offset: the x offset
 * @delay: the delay
 * @attack: the attack
 * 
 * Read wave from @sound_resource.
 * 
 * Returns: a #GList-struct containing #AgsWave
 * 
 * Since: 2.0.0
 */
GList*
ags_sound_resource_read_wave(AgsSoundResource *sound_resource,
			     GObject *soundcard,
			     gint audio_channel,
			     guint64 x_offset,
			     gdouble delay, guint attack)
{
  GList *start_list;

  void *target_data, *data;

  guint copy_mode;
  guint64 relative_offset;
  guint64 x_point_offset;
  guint frame_count;
  guint audio_channels;
  guint target_samplerate, samplerate;
  guint target_buffer_size, buffer_size;
  guint target_format, format;
  guint i, i_start, i_stop;
  
  if(!AGS_SOUND_RESOURCE(sound_resource)){
    return(NULL);
  }

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sound_resource),
			  &frame_count,
			  NULL, NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sound_resource),
				 &audio_channels,
				 &samplerate,
				 &buffer_size,
				 NULL);

  g_object_get(G_OBJECT(sound_resource),
	       "format", &format,
	       NULL);
  
  if(soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      NULL,
			      &target_samplerate,
			      &target_buffer_size,
			      &target_format);
  }else{
    AgsConfig *config;

    config = ags_config_get_instance();

    target_samplerate = ags_soundcard_helper_config_get_samplerate(config);
    target_buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
    target_format = ags_soundcard_helper_config_get_format(config);
  }
  
  start_list = NULL;

  if(audio_channel == -1){
    i_start = 0;
    i_stop = audio_channels;

    //    g_message("do %d", audio_channels);
  }else{
    i_start = audio_channel;
    i_stop = i_start + 1;
  }

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  data = NULL;

  if(samplerate != target_samplerate){
    data = ags_stream_alloc(buffer_size,
			    format);
  }
  
  for(i = i_start; i < i_stop; i++){
    AgsWave *wave;

    gboolean success;

    ags_sound_resource_seek(AGS_SOUND_RESOURCE(sound_resource),
			    0, G_SEEK_SET);
    
    wave = ags_wave_new(NULL,
			i);
    g_object_set(wave,
		 "samplerate", target_samplerate,
		 "buffer-size", target_buffer_size,
		 "format", target_format,
		 NULL);

    start_list = ags_wave_add(start_list,
			      wave);
    
    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * target_samplerate;
        
    frame_count = target_buffer_size;

    x_point_offset = x_offset;
      
    success = TRUE;
    
    while(success){
      AgsBuffer *buffer;
      
      guint num_read;
      gboolean create_wave;
      
      create_wave = FALSE;
      
      if(x_point_offset + frame_count > relative_offset * floor(x_point_offset / relative_offset) + relative_offset){
	frame_count = relative_offset * floor((x_point_offset + frame_count) / relative_offset) - x_point_offset;

	create_wave = TRUE;
      }else if(x_point_offset + frame_count == relative_offset * floor(x_point_offset / relative_offset) + relative_offset){
	create_wave = TRUE;
      }
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "x", x_point_offset,
		   "samplerate", target_samplerate,
		   "buffer-size", target_buffer_size,
		   "format", target_format,
		   NULL);

      if(samplerate != target_samplerate){
	if(format == AGS_SOUNDCARD_DOUBLE){
	  ags_audio_buffer_util_clear_double(data, 1,
					     buffer_size);
	}else if(format == AGS_SOUNDCARD_FLOAT){
	  ags_audio_buffer_util_clear_float(data, 1,
					    buffer_size);
	}else{
	  ags_audio_buffer_util_clear_buffer(data, 1,
					     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
	}
	
	num_read = ags_sound_resource_read(AGS_SOUND_RESOURCE(sound_resource),
					   data, 1,
					   i,
					   buffer_size, format);

	target_data = ags_audio_buffer_util_resample(data, 1,
						     ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						     buffer_size,
						     target_samplerate);

	ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
						    target_data, 1, 0,
						    frame_count, copy_mode);

	free(target_data);

	num_read = (guint) ceil(num_read / buffer_size * frame_count);
      }else{
	num_read = ags_sound_resource_read(AGS_SOUND_RESOURCE(sound_resource),
					   buffer->data, 1,
					   i,
					   frame_count, target_format);
      }
      //      g_message("read %d[%d-%d]: %d", frame_count, i, i_stop, num_read);

      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);
      
      if(create_wave){
	AgsTimestamp *timestamp;

	wave = ags_wave_new(NULL,
			    i);
	g_object_set(wave,
		     "samplerate", target_samplerate,
		     "buffer-size", target_buffer_size,
		     "format", target_format,
		     NULL);

	g_object_get(wave,
		     "timestamp", &timestamp,
		     NULL);
	ags_timestamp_set_ags_offset(timestamp,
				     (guint64) relative_offset * floor((x_point_offset + frame_count) / relative_offset));
	
	g_object_unref(timestamp);

	start_list = ags_wave_add(start_list,
				  wave);
      }
      
      /* iterate */
      x_point_offset += frame_count;

      if(num_read < frame_count){
	success = FALSE;
      }

      if(frame_count != target_buffer_size){
	frame_count = target_buffer_size;
      }
    }
  }
  
  if(data != NULL){
    free(data);
  }

  g_list_foreach(start_list,
		 (GFunc) g_object_ref,
		 NULL);
  
  return(start_list);
}
