/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/file/ags_wav_file.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <stdlib.h>
#include <string.h>

void ags_wav_file_class_init(AgsWavFileClass *wav_file);
void ags_wav_file_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_wav_file_init(AgsWavFile *wav_file);
void ags_wav_file_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_wav_file_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_wav_file_finalize(GObject *object);

gboolean ags_wav_file_open(AgsSoundResource *sound_resource,
			   gchar *filename);
gboolean ags_wav_file_rw_open(AgsSoundResource *sound_resource,
			      gchar *filename,
			      gboolean create);
void ags_wav_file_load(AgsSoundResource *sound_resource);
void ags_wav_file_info(AgsSoundResource *sound_resource,
		       guint *frame_count,
		       guint *loop_start, guint *loop_end);  
void ags_wav_file_set_presets(AgsSoundResource *sound_resource,
			      guint channels,
			      guint samplerate,
			      guint buffer_size,
			      guint format);
void ags_wav_file_get_presets(AgsSoundResource *sound_resource,
			      guint *channels,
			      guint *samplerate,
			      guint *buffer_size,
			      guint *format);
guint ags_wav_file_read(AgsSoundResource *sound_resource,
			void *dbuffer,
			guint audio_channel,
			guint frame_count, guint format);
void ags_wav_file_write(AgsSoundResource *sound_resource,
			void *sbuffer,
			guint audio_channel,
			guint frame_count, guint format);
void ags_wav_file_flush(AgsSound_Resource *sound_resource);
void ags_wav_file_seek(AgsSoundResource *sound_resource,
		       guint frames, gint whence);
void ags_wav_file_close(AgsSoundResource *sound_resource);

void ags_wav_file_write_header(AgsWavFile *wav_file);
void ags_wav_file_write_format_chunk(AgsWavFile *wav_file,
				     AgsWavFileFormatChunk *format_chunk);
void ags_wav_file_write_fact_chunk(AgsWavFile *wav_file,
				   AgsWavFileFactChunk *fact_chunk);
void ags_wav_file_write_data_chunk(AgsWavFile *wav_file,
				   AgsWavFileDataChunk *data_chunk);

enum{
  PROP_0,
  PROP_FILENAME,
};

/**
 * SECTION:ags_wav_file
 * @short_description: Wav file input/output
 * @title: AgsWavFile
 * @section_id:
 * @include: ags/audio/file/ags_wav_file.h
 *
 * #AgsWavFile is the base object to read/write wav data.
 */

static gpointer ags_wav_file_parent_class = NULL;

GType
ags_wav_file_get_type()
{
  static GType ags_type_wav_file = 0;

  if(!ags_type_wav_file){
    static const GTypeInfo ags_wav_file_info = {
      sizeof (AgsWavFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wav_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWavFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wav_file_init,
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_wav_file_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wav_file = g_type_register_static(G_TYPE_OBJECT,
					       "AgsWavFile",
					       &ags_wav_file_info,
					       0);

    g_type_add_interface_static(ags_type_wav_file,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);
  }

  return (ags_type_wav_file);
}

void
ags_wav_file_class_init(AgsWavFileClass *wav_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_wav_file_parent_class = g_type_class_peek_parent(wav_file);

  gobject = (GObjectClass *) wav_file;

  gobject->set_property = ags_wav_file_set_property;
  gobject->get_property = ags_wav_file_get_property;

  gobject->finalize = ags_wav_file_finalize;

  /* properties */
  /**
   * AgsWavFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of wav file"),
				   i18n_pspec("The filename of wav file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);
}

void
ags_wav_file_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  sound_resource->open = ags_wav_file_open;
  sound_resource->rw_open = ags_wav_file_rw_open;

  sound_resource->load = ags_wav_file_load;

  sound_resource->info = ags_wav_file_info;

  sound_resource->set_presets = ags_wav_file_set_presets;
  sound_resource->get_presets = ags_wav_file_get_presets;

  sound_resource->read = ags_wav_file_read;

  sound_resource->write = ags_wav_file_write;
  sound_resource->flush = ags_wav_file_flush;

  sound_resource->seek = ags_wav_file_seek;

  sound_resource->close = ags_wav_file_close;
}

void
ags_wav_file_init(AgsWavFile *wav_file)
{
  wav_file->flags = AGS_WAV_FILE_READ_ONLY;
  
  wav_file->filename = 0;

  wav_file->stream = NULL;

  wav_file->chunk_id = AGS_WAV_FILE_RIFF;
  wav_file->chunk_size = 4;

  wav_file->wave_id = AGS_WAV_FILE_WAVE;

  wav_file->current_format_chunk = NULL;
  wav_file->current_fact_chunk = NULL;
  wav_file->current_data_chunk = NULL;

  wav_file->offset = 0;

  wav_file->format_chunk = NULL;
  wav_file->fact_chunk = NULL;
  wav_file->data_chunk = NULL;

  wav_file->all_chunk = NULL;
}

void
ags_wav_file_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(wav_file->filename == filename){
	return;
      }
      
      if(wav_file->filename != NULL){
	g_free(wav_file->filename);
      }

      wav_file->filename = g_strdup(filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wav_file_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, wav_file->filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wav_file_finalize(GObject *gobject)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(gobject);

  /* filename */
  g_free(wav_file->filename);

  /* chunks */
  g_list_free_full(wav_file->format_chunk,
		   ags_wav_file_format_chunk_free);
  
  g_list_free_full(wav_file->fact_chunk,
		   ags_wav_file_fact_chunk_free);

  g_list_free_full(wav_file->data_chunk,
		   ags_wav_file_data_chunk_free);

  g_list_free(wav_file->all_chunk);
  
  /* call parent */
  G_OBJECT_CLASS(ags_wav_file_parent_class)->finalize(gobject);
}

gboolean
ags_wav_file_open(AgsSoundResource *sound_resource,
		  gchar *filename)
{
  AgsWavFile *wav_file;
  
  unsigned char buffer[16];

  size_t retval;
  
  if(filename == NULL){
    return(FALSE);
  }

  wav_file = AGS_WAV_FILE(sound_resource);  
  g_object_set(wav_file,
	       "filename", filename,
	       NULL);
  
  wav_file->stream = fopen(filename,
			   "r");
  retval = fread(buffer, sizeof(unsigned char), 12, wav_file->stream);

  /* compare number of bytes read */
  if(retval != 12){
    return(FALSE);
  }
  
  /* check RIFF header */
  if(!(!g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RIFF,
			    4) ||
       !g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RIFX,
			    4) ||
       !g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RF64,
			    4))){
    return(FALSE);
  }

  /* check WAVE id */
  if(!(!g_ascii_strncasecmp(buffer + 8,
			    AGS_WAV_FILE_WAVE,
			    4))){
    return(FALSE);
  }

  /* chunk id */
  memcpy(wav_file->chunk_id, buffer, 4 * sizeof(unsigned char));

  /* chunk size */
  if(!g_ascii_strncasecmp(buffer,
			  AGS_WAV_FILE_RIFF,
			  4) ||
     !g_ascii_strncasecmp(buffer,
			  AGS_WAV_FILE_RF64,
			  4)){
    if(ags_endian_host_is_le()){
      wav_file->chunk_size = ags_buffer_util_char_buffer_read_s32(buffer + 4,
								  FALSE);
    }else{
      wav_file->chunk_size = ags_buffer_util_char_buffer_read_s32(buffer + 4,
								  TRUE);
    }
  }else if(!g_ascii_strncasecmp(buffer,
				AGS_WAV_FILE_RIFX,
				4)){
    if(ags_endian_host_is_le()){
      wav_file->chunk_size = ags_buffer_util_char_buffer_read_s32(buffer + 4,
								  TRUE);
    }else{
      wav_file->chunk_size = ags_buffer_util_char_buffer_read_s32(buffer + 4,
								  FALSE);
    }
  }

  /* WAVE id */
  memcpy(wav_file->wave_id, buffer + 8, 4 * sizeof(unsigned char));

  /* set read-only */
  wav_file->flags |= AGS_WAV_FILE_READ_ONLY;

  return(TRUE);
}

gboolean
ags_wav_file_rw_open(AgsSoundResource *sound_resource,
		     gchar *filename,
		     gboolean create)
{
  AgsWavFile *wav_file;

  if(filename == NULL){
    return(FALSE);
  }

  wav_file = AGS_WAV_FILE(sound_resource);  
  g_object_set(wav_file,
	       "filename", filename,
	       NULL);

  if(!g_file_test(wav_file->filename,
		  G_FILE_TEST_EXISTS)){
    if(create){
      wav_file->stream = fopen(wav_file->filename,
			       "w");

      ags_wav_file_write_header(wav_file);
      
      fflush(wav_file->stream);
      fclose(wav_file->stream);

      wav_file->stream = NULL;
    }else{
      return(FALSE);
    }
  }
  
  if(!ags_sound_resource_open(sound_resource)){
    return(FALSE);
  }

  wav_file->flags &= (~AGS_WAV_FILE_READ_ONLY);
  
  return(TRUE);
}

void
ags_wav_file_load(AgsSoundResource *sound_resource)
{
  AgsWavFile *wav_file;

  AgsWavFileFormatChunk *format_chunk;
  AgsWavFileFactChunk *fact_chunk;
  AgsWavFileDataChunk *data_chunk;

  gchar chunk_id[4];
  gchar fmt_chunk[48] = "fmt ";
  gchar fact_chunk[12] = "fact";
  gchar data_chunk[8] = "data";
  gchar *iter;
  
  guint data_length;
  guint offset;
  gboolean file_is_le;
  gboolean swap_bytes;
  
  wav_file = AGS_WAV_FILE(sound_resource);

  if((AGS_WAV_FILE_LOADED & (wav_file->flags)) != 0){
    return;
  }

  wav_file->flags |= AGS_WAV_FILE_LOADED;

  /* get file LE/BE */
  file_is_le = TRUE;

  if(!g_ascii_strncasecmp(wav_file->chunk_id,
			  AGS_WAV_FILE_RIFX,
			  4)){
    file_is_le = FALSE;
  }

  /* initial position */
  fseek(wav_file->stream, 12 * sizeof(gchar), SEEK_SET);
  
  /* read all chunks */
  data_length = wav_file->chunk_size - 4;
  offset = 0; 

  swap_bytes = ((file_is_le &&
		 ags_endian_host_is_be()) ||
		(!file_is_le &&
		 ags_endian_host_is_le())) ? TRUE: FALSE;
  
  for(; offset < data_length;){
    fread(chunk_id, sizeof(gchar), 4, wav_file->stream);
    
    if(!g_ascii_strncasecmp(chunk_id,
			    "fmt ",
			    4)){
      format_chunk = ags_wav_file_format_chunk_alloc();
      ags_wav_file_add_format_chunk(wav_file,
				    format_chunk);

      if(wav_file->current_format_chunk == NULL){
	wav_file->current_format_chunk = format_chunk;
      }
      
      fread(fmt_chunk + 4, sizeof(gchar), 44, wav_file->stream);
      iter = fmt_chunk + 4;

      /* read fields */
      AGS_WAV_FILE_CHUNK(format_chunk)->chunk_size = ags_buffer_util_char_buffer_read_s32(iter,
											  swap_bytes);
      iter += 4;
      
      format_chunk->format = ags_buffer_util_char_buffer_read_s32(iter,
								  swap_bytes);
      iter += 4;
      
      format_chunk->channels = ags_buffer_util_char_buffer_read_s16(iter,
								    swap_bytes);
      iter += 2;
      
      format_chunk->samplerate = ags_buffer_util_char_buffer_read_s32(iter,
								      swap_bytes);
      iter += 4;
      
      format_chunk->bps = ags_buffer_util_char_buffer_read_s32(iter,
							       swap_bytes);
      iter += 4;
      
      format_chunk->block_align = ags_buffer_util_char_buffer_read_s16(iter,
								       swap_bytes);
      iter += 2;
      
      format_chunk->bits_per_sample = ags_buffer_util_char_buffer_read_s16(iter,
									   swap_bytes);
      iter += 2;

      /* variable length chunk */
      if(AGS_WAV_FILE_CHUNK(format_chunk)->chunk_size == 16){
	fseek(wav_file->stream, -24 * sizeof(gchar), SEEK_CUR);

	offset += 24;
      }else if(AGS_WAV_FILE_CHUNK(format_chunk)->chunk_size == 18){
	format_chunk->cb_size = ags_buffer_util_char_buffer_read_s16(iter,
								     swap_bytes);
	iter += 2;
	
	fseek(wav_file->stream, -22 * sizeof(gchar), SEEK_CUR);

	offset += 26;
      }else if(AGS_WAV_FILE_CHUNK(format_chunk)->chunk_size == 40){
	format_chunk->cb_size = ags_buffer_util_char_buffer_read_s16(iter,
								     swap_bytes);
	iter += 2;

	format_chunk->valid_bits_per_sample = ags_buffer_util_char_buffer_read_s16(iter,
										   swap_bytes);
	iter += 2;
	
	format_chunk->channels_mask = ags_buffer_util_char_buffer_read_s32(iter,
									   swap_bytes);
	iter += 4;

	memcpy(format_chunk->guid, iter, 16 * sizeof(gchar));

	offset += 48;
      }
    }else if(!g_ascii_strncasecmp(chunk_id,
				  "fact",
				  4)){
      fact_chunk = ags_wav_file_fact_chunk_alloc();
      ags_wav_file_add_fact_chunk(wav_file,
				  fact_chunk);

      if(wav_file->current_fact_chunk == NULL){
	wav_file->current_fact_chunk = fact_chunk;
      }

      fread(fact_chunk + 4, sizeof(gchar), 8, wav_file->stream);
      iter = fact_chunk + 4;

      /* read fields */
      AGS_WAV_FILE_CHUNK(fact_chunk)->chunk_size = ags_buffer_util_char_buffer_read_s32(iter,
											swap_bytes);
      iter += 4;
      
      fact_chunk->samples_per_channel = ags_buffer_util_char_buffer_read_s32(iter,
									     swap_bytes);
      iter += 4;
      
      offset += 12;
    }else if(!g_ascii_strncasecmp(chunk_id,
				  "data",
				  4)){
      guint frame_count;
      guint i;
      
      data_chunk = ags_wav_file_data_chunk_alloc();
      ags_wav_file_add_data_chunk(wav_file,
				  data_chunk);

      if(wav_file->current_data_chunk == NULL){
	wav_file->current_data_chunk = data_chunk;
      }

      fread(data_chunk + 4, sizeof(gchar), 4, wav_file->stream);
      iter = data_chunk + 4;

      /* read fields */
      AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size = ags_buffer_util_char_buffer_read_s32(iter,
											swap_bytes);
      iter += 4;

      data_chunk->data = (void *) malloc(AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size * sizeof(gchar));
      frame_count = fread(data_chunk->data, sizeof(gchar), AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size, wav_file->stream);

      if(swap_bytes){
	if(wav_file->current_format_chunk){
	  ags_buffer_util_char_buffer_swap_bytes(data_chunk->data, wav_file->current_format_chunk->valid_bits_per_sample / 8,
						 AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size);
	}else{
	  g_warning("no format chunk - can't swap bytes");
	}
      }
      
      offset += (8 + AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size);
    }else{
      /* no chunk id */
      fseek(wav_file->stream, -3 * sizeof(gchar), SEEK_CUR);
      offset++;
    }
  }  
}

void
ags_wav_file_info(AgsSoundResource *sound_resource,
		  guint *frame_count,
		  guint *loop_start, guint *loop_end)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  if(frame_count != NULL){
    if(wav_file->current_format_chunk != NULL &&
       wav_file->current_data_chunk != NULL){
      frame_count[0] = ((AGS_WAV_FILE_CHUNK(wav_file->current_format_chunk)->chunk_size /
			 wav_file->current_format_chunk->channels) /
			(wav_file->current_format_chunk->bits_per_sample / 8.0));
    }else{
      frame_count[0] = 0;
    }
  }
  
  if(loop_start != NULL){
    loop_start[0] = 0;
  }

  if(loop_end != NULL){
    loop_end[0] = 0;
  }
}

void
ags_wav_file_set_presets(AgsSoundResource *sound_resource,
			 guint channels,
			 guint samplerate,
			 guint buffer_size,
			 guint format)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  if(wav_file->current_format_chunk != NULL){
    wav_file->current_format_chunk->channels = channels;
    wav_file->current_format_chunk->samplerate = samplerate;

    switch(format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	wav_file->current_format_chunk->format = AGS_WAV_FILE_FORMAT_PCM;
	
	wav_file->current_format_chunk->bits_per_sample = 8;
	wav_file->current_format_chunk->valid_bits_per_sample = 8;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	wav_file->current_format_chunk->format = AGS_WAV_FILE_FORMAT_PCM;
	
	wav_file->current_format_chunk->bits_per_sample = 16;
	wav_file->current_format_chunk->valid_bits_per_sample = 16;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	wav_file->current_format_chunk->format = AGS_WAV_FILE_FORMAT_PCM;
	
	wav_file->current_format_chunk->bits_per_sample = 32;
	wav_file->current_format_chunk->valid_bits_per_sample = 24;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	wav_file->current_format_chunk->format = AGS_WAV_FILE_FORMAT_PCM;
	
	wav_file->current_format_chunk->bits_per_sample = 32;
	wav_file->current_format_chunk->valid_bits_per_sample = 32;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	wav_file->current_format_chunk->format = AGS_WAV_FILE_FORMAT_PCM;
	
	wav_file->current_format_chunk->bits_per_sample = 64;
	wav_file->current_format_chunk->valid_bits_per_sample = 64;
      }
      break;
    }
  }
  
  if(wav_file->current_data_chunk != NULL){
    switch(format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size = channels * buffer_size * sizeof(signed char);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size = channels * buffer_size * sizeof(signed short);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size = channels * buffer_size * sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size = channels * buffer_size * sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size = channels * buffer_size * sizeof(signed long long);
      }
      break;
    }
  }
}

void
ags_wav_file_get_presets(AgsSoundResource *sound_resource,
			 guint *channels,
			 guint *samplerate,
			 guint *buffer_size,
			 guint *format)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  if(wav_file->current_format_chunk != NULL){
    if(channels != NULL){
      channels[0] = wav_file->current_format_chunk->channels;
    }

    if(samplerate != NULL){
      samplerate[0] = wav_file->current_format_chunk->samplerate;
    }

    if(format != NULL){
      switch(wav_file->current_format_chunk->bits_per_sample){
      case 8:
	{
	  format[0] = AGS_SOUNDCARD_SIGNED_8_BIT;
	}
	break;
      case 16:
	{
	  format[0] = AGS_SOUNDCARD_SIGNED_16_BIT;
	}
	break;
      case 24:
	{
	  format[0] = AGS_SOUNDCARD_SIGNED_24_BIT;
	}
	break;
      case 32:
	{
	  format[0] = AGS_SOUNDCARD_SIGNED_32_BIT;
	}
	break;
      case 64:
	{
	  format[0] = AGS_SOUNDCARD_SIGNED_64_BIT;
	}
	break;
      }
    }
  }

  if(wav_file->current_format_chunk != NULL &&
     wav_file->current_data_chunk != NULL){
    if(buffer_size != NULL){
      buffer_size[0] = (AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size /
			wav_file->current_format_chunk->channels /
			(wav_file->current_format_chunk->bits_per_sample / 8.0));
    }
  }
}

guint
ags_wav_file_read(AgsSoundResource *sound_resource,
		  void *dbuffer, guint daudio_channels,
		  guint frame_count, guint dformat)
{
  AgsWavFile *wav_file;

  guint64 buffer_size;
  guint sformat;
  guint copy_mode;
  
  wav_file = AGS_WAV_FILE(sound_resource);  

  if(dbuffer == NULL){
    return(0);
  }

  if(wav_file->current_format_chunk != NULL &&
     wav_file->current_data_chunk != NULL){
    buffer_size = (AGS_WAV_FILE_CHUNK(wav_file->current_data_chunk)->chunk_size /
		   wav_file->current_format_chunk->channels /
		   (wav_file->current_format_chunk->bits_per_sample / 8.0));
  }

  if(wav_file->current_format_chunk->format == AGS_WAV_FILE_FORMAT_PCM){
    switch(wav_file->current_format_chunk->bits_per_sample){
    case 8:
      {
	sformat = AGS_SOUNDCARD_SIGNED_8_BIT;
      }
      break;
    case 16:
      {
	sformat = AGS_SOUNDCARD_SIGNED_16_BIT;
      }
      break;
    case 24:
      {
	sformat = AGS_SOUNDCARD_SIGNED_24_BIT;
      }
      break;
    case 32:
      {
	sformat = AGS_SOUNDCARD_SIGNED_32_BIT;
      }
      break;
    case 64:
      {
	sformat = AGS_SOUNDCARD_SIGNED_64_BIT;
      }
      break;
    }

    copy_mode = ags_audio_buffer_util_get_copy_mode(dformat,
						    sformat);

    if(wav_file->current_data_chunk != NULL &&
       wav_file->current_format_chunk != NULL){
      if(offset + frame_count < buffer_size){
	ags_char_buffer_util_copy_buffer_to_buffer(dbuffer, audio_channels,
						   wav_file->current_data_chunk->data + wav_file->offset, wav_file->current_format_chunk->channels,
						   frame_count, copy_mode);

	return(frame_count);
      }else{
	ags_char_buffer_util_copy_buffer_to_buffer(dbuffer, audio_channels,
						   wav_file->current_data_chunk->data + wav_file->offset, wav_file->current_format_chunk->channels,
						   buffer_size - wav_file->offset, copy_mode);

	return(buffer_size - wav_file->offset);
      }
    }
  }else if(wav_file->current_format_chunk->format == AGS_WAV_FILE_FORMAT_IEEE_FLOAT){
    if(offset + frame_count < buffer_size){
      //TODO:JK: implement me
      
      return(frame_count);
    }else{
      //TODO:JK: implement me

      return(buffer_size - wav_file->offset);
    }
  }
  
  return(0);
}

void
ags_wav_file_write(AgsSoundResource *sound_resource,
		   void *sbuffer,
		   guint audio_channel,
		   guint frame_count, guint format)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  //TODO:JK: implement me
}

void
ags_wav_file_flush(AgsSound_Resource *sound_resource)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  //TODO:JK: implement me
}

void
ags_wav_file_seek(AgsSoundResource *sound_resource,
		  guint frames, gint whence)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  //TODO:JK: implement me
}

void
ags_wav_file_close(AgsSoundResource *sound_resource)
{
  AgsWavFile *wav_file;

  wav_file = AGS_WAV_FILE(sound_resource);  

  //TODO:JK: implement me
}

void
ags_wav_file_write_header(AgsWavFile *wav_file)
{
  unsigned char buffer[16];

  gboolean file_is_le;

  /* get file LE/BE */
  file_is_le = TRUE;

  if(!g_ascii_strncasecmp(wav_file->chunk_id,
			  AGS_WAV_FILE_RIFX,
			  4)){
    file_is_le = FALSE;
  }

  /* chunk id */
  memcpy(buffer, wav_file->chunk_id, 4 * sizeof(unsigned char));

  if((file_is_le &&
      ags_endian_host_is_le()) ||
     (!file_is_le &&
      ags_endian_host_is_be())){
    ags_buffer_util_char_buffer_write_s32(buffer,
					  wav_file->chunk_size,
					  FALSE);
  }else if((file_is_le &&
	    ags_endian_host_is_be()) ||
	   (!file_is_le &&
	    ags_endian_host_is_le())){
    ags_buffer_util_char_buffer_write_s32(buffer,
					  wav_file->chunk_size,
					  TRUE);
  }

  /* wave id */
  memcpy(buffer + 8, wav_file->wave_id, 4 * sizeof(unsigned char));

  /* write */
  fwrite(buffer, sizeof(unsigned char), 12, wav_file->stream);
}

void
ags_wav_file_write_format_chunk(AgsWavFile *wav_file,
				AgsWavFileFormatChunk *format_chunk)
{
  //TODO:JK: implement me
}

void
ags_wav_file_write_fact_chunk(AgsWavFile *wav_file,
			      AgsWavFileFactChunk *fact_chunk)
{
  //TODO:JK: implement me
}

void
ags_wav_file_write_data_chunk(AgsWavFile *wav_file,
			      AgsWavFileDataChunk *data_chunk)
{
  //TODO:JK: implement me
}

/**
 * ags_wav_file_format_chunk_alloc:
 * 
 * Allocate WAV file format chunk. 
 *
 * Returns: the newly allocated #AgsWavFileFormatChunk-struct
 * 
 * Since: 2.0.0
 */
AgsWavFileFormatChunk*
ags_wav_file_format_chunk_alloc()
{
  AgsWavFileFormatChunk *format_chunk;

  format_chunk = (AgsWavFileFormatChunk *) malloc(sizeof(AgsWavFileFormatChunk));

  AGS_WAV_FILE_CHUNK(format_chunk)->chunk_id = "fmt ";
  AGS_WAV_FILE_CHUNK(format_chunk)->chunk_size = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_SIZE;

  format_chunk->format = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_FORMAT;
  format_chunk->channels = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_CHANNELS;
  format_chunk->samplerate = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_SAMPLERATE;
  format_chunk->bps = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BPS;
  format_chunk->block_align = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BLOCK_ALIGN;
  format_chunk->bits_per_sample = AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BITS_PER_SAMPLE;

  /* default skipped */
  format_chunk->cb_size = 0;
  format_chunk->valid_bits_per_sample = 0;
  format_chunk->channel_mask = 0;
  memset(format_chunk->guid, 0, sizeof(format_chunk->guid));
  
  return(format_chunk);
}

/**
 * ags_wav_file_format_chunk_free:
 * 
 * Free WAV file format chunk.
 * 
 * Since: 2.0.0
 */
void
ags_wav_file_format_chunk_free(AgsWavFileFormatChunk *format_chunk)
{
  g_free(format_chunk);
}

/**
 * ags_wav_file_fact_chunk_alloc:
 * 
 * Allocate WAV file fact chunk. 
 *
 * Returns: the newly allocated #AgsWavFileFactChunk-struct
 * 
 * Since: 2.0.0
 */
AgsWavFileFactChunk*
ags_wav_file_fact_chunk_alloc()
{
  AgsWavFileFactChunk *fact_chunk;

  fact_chunk = (AgsWavFileFactChunk *) malloc(sizeof(AgsWavFileFactChunk));

  AGS_WAV_FILE_CHUNK(fact_chunk)->chunk_id = "fact";
  AGS_WAV_FILE_CHUNK(fact_chunk)->chunk_size = AGS_WAV_FILE_DEFAULT_FACT_CHUNK_SIZE;

  fact_chunk->samples_per_channel = 0;

  return(fact_chunk);
}

/**
 * ags_wav_file_fact_chunk_free:
 * 
 * Free WAV file fact chunk.
 * 
 * Since: 2.0.0
 */
void
ags_wav_file_fact_chunk_free(AgsWavFileFactChunk *fact_chunk)
{
  g_free(fact_chunk);
}

/**
 * ags_wav_file_data_chunk_alloc:
 * 
 * Allocate WAV file data chunk. 
 *
 * Returns: the newly allocated #AgsWavFileDataChunk-struct
 * 
 * Since: 2.0.0
 */
AgsWavFileDataChunk*
ags_wav_file_data_chunk_alloc()
{
  AgsWavFileDataChunk *data_chunk;

  data_chunk = (AgsWavFileDataChunk *) malloc(sizeof(AgsWavFileDataChunk));

  AGS_WAV_FILE_CHUNK(data_chunk)->chunk_id = "data";
  AGS_WAV_FILE_CHUNK(data_chunk)->chunk_size = 0;

  data_chunk->data = NULL;
  
  return(data_chunk);
}

/**
 * ags_wav_file_data_chunk_free:
 * 
 * Free WAV file data chunk.
 * 
 * Since: 2.0.0
 */
void
ags_wav_file_data_chunk_free(AgsWavFileDataChunk *data_chunk)
{
  if(data_chunk != NULL){
    g_free(data_chunk->data);
  }

  g_free(data_chunk);
}

void
ags_wav_file_add_format_chunk(AgsWavFile *wav_file,
			      AgsWavFileFormatChunk *format_chunk)
{
  if(!AGS_WAV_FILE(wav_file)){
    return;
  }

  wav_file->format_chunk = g_list_prepend(wav_file->format_chunk,
					  format_chunk);

  wav_file->all_chunk = g_list_prepend(wav_file->all_chunk,
				       format_chunk);
}

void
ags_wav_file_remove_format_chunk(AgsWavFile *wav_file,
				 AgsWavFileFormatChunk *format_chunk)
{
  if(!AGS_WAV_FILE(wav_file)){
    return;
  }

  wav_file->format_chunk = g_list_remove(wav_file->format_chunk,
					 format_chunk);
  
  wav_file->all_chunk = g_list_remove(wav_file->all_chunk,
				      format_chunk);
}

void
ags_wav_file_add_fact_chunk(AgsWavFile *wav_file,
			    AgsWavFileFactChunk *fact_chunk)
{
  //TODO:JK: implement me
}

void
ags_wav_file_remove_fact_chunk(AgsWavFile *wav_file,
			       AgsWavFileFactChunk *fact_chunk)
{
  //TODO:JK: implement me
}

void
ags_wav_file_add_data_chunk(AgsWavFile *wav_file,
			    AgsWavFileDataChunk *data_chunk)
{
  //TODO:JK: implement me
}

void
ags_wav_file_remove_data_chunk(AgsWavFile *wav_file,
			       AgsWavFileDataChunk *data_chunk)
{
  //TODO:JK: implement me
}

/**
 * ags_wav_file_get_valid_suffix:
 * 
 * Get valid suffix.
 * 
 * Returns: a %NULL terminated string vector containing valid suffixes
 * 
 * Since: 2.0.0
 */
gchar**
ags_wav_file_get_valid_suffix()
{
  static const gchar *suffix[] = {
    "wav",
    "wave",
    NULL,
  };

  return(suffix);
}

/**
 * ags_wav_file_check_open:
 * @filename: the filename as string
 * 
 * Check if you can open @filename.
 * 
 * Returns: %TRUE if valid RIFF WAV file, otherwise %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_wav_file_check_open(gchar *filename)
{
  FILE *stream;
  unsigned char buffer[16];

  size_t retval;
  
  stream = fopen(filename,
		 "r");
  retval = fread(buffer, sizeof(unsigned char), 12, stream);

  fclose(stream);
  
  /* compare number of bytes read */
  if(retval != 12){
    return(FALSE);
  }
  
  /* check RIFF header */
  if(!(!g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RIFF,
			    4) ||
       !g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RIFX,
			    4) ||
       !g_ascii_strncasecmp(buffer,
			    AGS_WAV_FILE_RF64,
			    4))){
    return(FALSE);
  }

  /* check WAVE id */
  if(!(!g_ascii_strncasecmp(buffer + 8,
			    AGS_WAV_FILE_WAVE,
			    4))){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_wav_file_new:
 * @filename: the filename as string
 *
 * Creates an #AgsWavFile.
 *
 * Returns: an empty #AgsWavFile.
 *
 * Since: 2.0.0
 */
AgsWavFile*
ags_wav_file_new(gchar *filename)
{
  AgsWavFile *wav_file;

  wav_file = (AgsWavFile *) g_object_new(AGS_TYPE_WAV_FILE,
					 "filename", filename,
					 NULL);

  return(wav_file);
}
