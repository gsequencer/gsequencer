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

#include <ags/audio/file/ags_sndfile.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <string.h>

#include <sndfile.h>

#include <ags/i18n.h>

void ags_sndfile_class_init(AgsSndfileClass *sndfile);
void ags_sndfile_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_sndfile_init(AgsSndfile *sndfile);
void ags_sndfile_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_sndfile_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_sndfile_dispose(GObject *gobject);
void ags_sndfile_finalize(GObject *gobject);

gboolean ags_sndfile_open(AgsSoundResource *sound_resource,
			  gchar *filename);
gboolean ags_sndfile_rw_open(AgsSoundResource *sound_resource,
			     gchar *filename,
			     gboolean create);
gboolean ags_sndfile_info(AgsSoundResource *sound_resource,
			  guint *frame_count,
			  guint *loop_start, guint *loop_end);
void ags_sndfile_set_presets(AgsSoundResource *sound_resource,
			     guint channels,
			     guint samplerate,
			     guint buffer_size,
			     guint format);
void ags_sndfile_get_presets(AgsSoundResource *sound_resource,
			     guint *channels,
			     guint *samplerate,
			     guint *buffer_size,
			     guint *format);
guint ags_sndfile_read(AgsSoundResource *sound_resource,
		       void *dbuffer,
		       guint audio_channel,
		       guint frame_count, guint format);
void ags_sndfile_write(AgsSoundResource *sound_resource,
		       void *sbuffer,
		       guint audio_channel,
		       guint frame_count, guint format);
void ags_sndfile_flush(AgsSoundResource *sound_resource);
void ags_sndfile_seek(AgsSoundResource *sound_resource,
		      gint64 frame_count, gint whence);
void ags_sndfile_close(AgsSoundResource *sound_resource);

sf_count_t ags_sndfile_vio_get_filelen(void *user_data);
sf_count_t ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data);
sf_count_t ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data);
sf_count_t ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data);
sf_count_t ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data);

/**
 * SECTION:ags_sndfile
 * @short_description: Libsndfile wrapper
 * @title: AgsSndfile
 * @section_id:
 * @include: ags/audio/file/ags_sndfile.h
 *
 * #AgsSndfile is the base object to ineract with libsndfile.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNELS,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_FILE,
};

static gpointer ags_sndfile_parent_class = NULL;
static AgsSoundResourceInterface *ags_sndfile_parent_sound_resource_interface;

static SF_VIRTUAL_IO *ags_sndfile_virtual_io = NULL;

GType
ags_sndfile_get_type()
{
  static GType ags_type_sndfile = 0;

  if(!ags_type_sndfile){
    static const GTypeInfo ags_sndfile_info = {
      sizeof (AgsSndfileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sndfile_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSndfile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sndfile_init,
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sndfile = g_type_register_static(G_TYPE_OBJECT,
					      "AgsSndfile",
					      &ags_sndfile_info,
					      0);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);
  }
  
  return (ags_type_sndfile);
}

void
ags_sndfile_class_init(AgsSndfileClass *sndfile)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sndfile_parent_class = g_type_class_peek_parent(sndfile);

  gobject = (GObjectClass *) sndfile;

  gobject->set_property = ags_sndfile_set_property;
  gobject->get_property = ags_sndfile_get_property;

  gobject->dispose = ags_sndfile_dispose;
  gobject->finalize = ags_sndfile_finalize;

  /* sndfile callbacks */
  if(ags_sndfile_virtual_io == NULL){
    ags_sndfile_virtual_io = (SF_VIRTUAL_IO *) malloc(sizeof(SF_VIRTUAL_IO));

    ags_sndfile_virtual_io->get_filelen = ags_sndfile_vio_get_filelen;
    ags_sndfile_virtual_io->seek = ags_sndfile_vio_seek;
    ags_sndfile_virtual_io->read = ags_sndfile_vio_read;
    ags_sndfile_virtual_io->write = ags_sndfile_vio_write;
    ags_sndfile_virtual_io->tell = ags_sndfile_vio_tell;
  }

  /* properties */
  /**
   * AgsSndfile:audio-channels:
   *
   * The audio channels to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("using audio channels"),
				 i18n_pspec("The audio channels to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsSndfile:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);
  
  /**
   * AgsSndfile:format:
   *
   * The format to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSndfile:file:
   *
   * The assigned output #SNDFILE-struct.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("file",
				    i18n_pspec("assigned file"),
				    i18n_pspec("The assigned file"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);
}

void
ags_sndfile_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_sndfile_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = ags_sndfile_open;
  sound_resource->rw_open = ags_sndfile_rw_open;

  sound_resource->load = NULL;

  sound_resource->info = ags_sndfile_info;

  sound_resource->set_presets = ags_sndfile_set_presets;
  sound_resource->get_presets = ags_sndfile_get_presets;
  
  sound_resource->read = ags_sndfile_read;

  sound_resource->write = ags_sndfile_write;
  sound_resource->flush = ags_sndfile_flush;
  
  sound_resource->seek = ags_sndfile_seek;

  sound_resource->close = ags_sndfile_close;
}

void
ags_sndfile_init(AgsSndfile *sndfile)
{
  AgsConfig *config;

  sndfile->flags = AGS_SNDFILE_FILL_CACHE;

  config = ags_config_get_instance();

  sndfile->audio_channels = 1;
  sndfile->audio_channel_written = (guint64 *) malloc(1 * sizeof(guint64));

  sndfile->audio_channel_written[0] = -1;
  
  sndfile->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  sndfile->format = AGS_SOUNDCARD_DOUBLE;

  sndfile->offset = 0;
  sndfile->buffer_offset = 0;
  
  sndfile->buffer = ags_stream_alloc(sndfile->buffer_size,
				     sndfile->format);

  sndfile->pointer = NULL;
  sndfile->current = NULL;
  sndfile->length = 0;

  sndfile->info = NULL;
  sndfile->file = NULL;
}

void
ags_sndfile_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      guint audio_channels;
      guint i;
      
      audio_channels = g_value_get_uint(value);

      if(audio_channels == sndfile->audio_channels){
	return;	
      }
      
      ags_stream_free(sndfile->buffer);

      if(audio_channels > 0){
	sndfile->audio_channel_written = (guint64 *) realloc(sndfile->audio_channel_written,
							     audio_channels * sizeof(guint64));
	
	for(i = sndfile->audio_channels; i < audio_channels; i++){
	  sndfile->audio_channel_written[i] = -1;
	}
      }else{
	free(sndfile->audio_channel_written);
      }

      sndfile->audio_channels = audio_channels;
      
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);
    }
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == sndfile->buffer_size){
	return;	
      }
      
      ags_stream_free(sndfile->buffer);

      sndfile->buffer_size = buffer_size;
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == sndfile->format){
	return;	
      }

      ags_stream_free(sndfile->buffer);

      sndfile->format = format;
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);
    }
    break;
  case PROP_FILE:
    {
      SNDFILE *file;

      file = g_value_get_pointer(value);

      if(sndfile->file == file){
	return;
      }
      
      sndfile->file = file;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sndfile_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsSndfile *sndfile;

  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, sndfile->audio_channels);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, sndfile->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, sndfile->format);
    }
    break;
  case PROP_FILE:
    {
      g_value_set_pointer(value, sndfile->file);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sndfile_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sndfile_parent_class)->dispose(gobject);
}

void
ags_sndfile_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sndfile_parent_class)->finalize(gobject);
}

gboolean
ags_sndfile_open(AgsSoundResource *sound_resource,
		 gchar *filename)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(sound_resource);

  if(sndfile->info != NULL){
    return(FALSE);
  }

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  sndfile->info->format = 0;
  sndfile->info->channels = 0;
  sndfile->info->samplerate = 0;

  if((AGS_SNDFILE_VIRTUAL & (sndfile->flags)) == 0){
    if(filename != NULL){
      sndfile->file = (SNDFILE *) sf_open(filename, SFM_READ, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_READ, sndfile->info, sndfile);
  }
  
  if(sndfile->file == NULL){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_sndfile_open(): channels %d frames %d", sndfile->info->channels, sndfile->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sndfile_rw_open(AgsSoundResource *sound_resource,
		    gchar *filename,
		    gboolean create)
{
  AgsSndfile *sndfile;
  
  AgsConfig *config;

  sndfile = AGS_SNDFILE(sound_resource);

  if(sndfile->info != NULL){
    return(FALSE);
  }

  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
    return(FALSE);
  }
  
  config = ags_config_get_instance();

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));

  sndfile->info->samplerate = (int) ags_soundcard_helper_config_get_samplerate(config);
  sndfile->info->channels = (int) sndfile->audio_channels;
  sndfile->info->format = (int) SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  sndfile->info->frames = 0;
  sndfile->info->seekable = 0;
  sndfile->info->sections = 0;

  if(!sf_format_check(sndfile->info)){
    g_warning("invalid format");
  }

  if((AGS_SNDFILE_VIRTUAL & (sndfile->flags)) == 0){
    if(filename != NULL){
      sndfile->file = (SNDFILE *) sf_open(filename, SFM_RDWR, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_RDWR, sndfile->info, sndfile);
  }

  if(sndfile->file == NULL){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_sndfile_rw_open(): channels %d frames %d", sndfile->info->channels, sndfile->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sndfile_info(AgsSoundResource *sound_resource,
		 guint *frame_count,
		 guint *loop_start, guint *loop_end)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(sound_resource);

  if(loop_start != NULL){
    *loop_start = 0;
  }

  if(loop_end != NULL){
    *loop_end = 0;
  }
  
  if(sndfile->info == NULL){
    if(frame_count != NULL){
      *frame_count = 0;
    }

    return(FALSE);
  }
  
  if(frame_count != NULL){
    *frame_count = sndfile->info->frames;
  }

  return(TRUE);
}

void
ags_sndfile_set_presets(AgsSoundResource *sound_resource,
			guint channels,
			guint samplerate,
			guint buffer_size,
			guint format)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(sound_resource);

  g_object_set(sndfile,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
  if(sndfile->info == NULL){
    return;
  }

  sndfile->info->channels = channels;
  sndfile->info->samplerate = samplerate;
  sndfile->info->format &= (~(SF_FORMAT_PCM_S8 |
			      SF_FORMAT_PCM_16 |
			      SF_FORMAT_PCM_24 |
			      SF_FORMAT_PCM_32 |
			      SF_FORMAT_FLOAT |
			      SF_FORMAT_DOUBLE));
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_24;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_32;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      sndfile->info->format |= SF_FORMAT_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      sndfile->info->format |= SF_FORMAT_DOUBLE;
    }
    break;
  }
}

void
ags_sndfile_get_presets(AgsSoundResource *sound_resource,
			guint *channels,
			guint *samplerate,
			guint *buffer_size,
			guint *format)
{
  AgsSndfile *sndfile;
   
  sndfile = AGS_SNDFILE(sound_resource);

  if(sndfile->info == NULL){
    if(channels != NULL){
      *channels = 0;
    }

    if(samplerate != NULL){
      *samplerate = 0;
    }
    
    if(buffer_size != NULL){
      *buffer_size = 0;
    }

    if(format != NULL){
      *format = 0;
    }

    return;
  }
  
  if(channels != NULL){
    *channels = sndfile->info->channels;
  }

  if(samplerate != NULL){
    *samplerate = sndfile->info->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = sndfile->buffer_size;
  }

  if(format != NULL){
    switch(((SF_FORMAT_PCM_S8 |
	     SF_FORMAT_PCM_16 |
	     SF_FORMAT_PCM_24 |
	     SF_FORMAT_PCM_32 |
	     SF_FORMAT_FLOAT |
	     SF_FORMAT_DOUBLE) & sndfile->info->format)){
    case SF_FORMAT_PCM_S8:
      {
	*format = AGS_SOUNDCARD_SIGNED_8_BIT;
      }
      break;
    case SF_FORMAT_PCM_16:
      {
	*format = AGS_SOUNDCARD_SIGNED_16_BIT;
      }
      break;
    case SF_FORMAT_PCM_24:
      {
	*format = AGS_SOUNDCARD_SIGNED_24_BIT;
      }
      break;
    case SF_FORMAT_PCM_32:
      {
	*format = AGS_SOUNDCARD_SIGNED_32_BIT;
      }
      break;
    case SF_FORMAT_FLOAT:
      {
	*format = AGS_SOUNDCARD_FLOAT;
      }
      break;
    case SF_FORMAT_DOUBLE:
      {
	*format = AGS_SOUNDCARD_DOUBLE;
      }
      break;
    }
  }
}

guint
ags_sndfile_read(AgsSoundResource *sound_resource,
		 void *dbuffer,
		 guint audio_channel,
		 guint frame_count, guint format)
{
  AgsSndfile *sndfile;

  sf_count_t multi_frames;
  guint total_frame_count;
  guint read_count;
  guint copy_mode;
  gboolean use_cache;
  guint i;

  sndfile = AGS_SNDFILE(sound_resource);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  if(sndfile->offset >= total_frame_count){
    return(0);
  }

  if(sndfile->offset + frame_count >= total_frame_count){
    frame_count = total_frame_count - sndfile->offset;
  }

  use_cache = FALSE;

  if(sndfile->buffer_offset == sndfile->offset &&
     frame_count <= sndfile->buffer_size){
    use_cache = TRUE;
  }
  
  sndfile->buffer_offset = sndfile->offset;
  
  read_count = sndfile->buffer_size;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sndfile->format));
  
  for(i = 0; i < frame_count; ){
    if(i + sndfile->buffer_size > frame_count){
      read_count = frame_count - i;
    }

    multi_frames = read_count * sndfile->info->channels;

    if(!use_cache){
      switch(sndfile->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  //TODO:JK: implement me
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  sf_read_short(sndfile->file, sndfile->buffer, multi_frames);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  //TODO:JK: implement me
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  //TODO:JK: implement me
	}
	break;
      case AGS_SOUNDCARD_FLOAT:
	{
	  sf_read_float(sndfile->file, sndfile->buffer, multi_frames);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  sf_read_double(sndfile->file, sndfile->buffer, multi_frames);
	}
	break;
      }
    }
    
    ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, 1, i,
						sndfile->buffer, audio_channel, 0,
						read_count, copy_mode);
    
    i += read_count;
  }

  sndfile->offset += frame_count;
  
  return(frame_count);
}

void
ags_sndfile_write(AgsSoundResource *sound_resource,
		  void *sbuffer,
		  guint audio_channel,
		  guint frame_count, guint format)
{
  AgsSndfile *sndfile;

  gint64 current_offset;
  guint copy_mode;
  sf_count_t multi_frames;
  gboolean write_cache;
  guint i;
  
  sndfile = AGS_SNDFILE(sound_resource);

  sndfile->audio_channel_written[audio_channel] = sndfile->offset;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(sndfile->format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  /* check write */
  write_cache = TRUE;

  if((AGS_SNDFILE_FILL_CACHE & (sndfile->flags)) != 0){
    ags_audio_buffer_util_copy_buffer_to_buffer(sndfile->buffer, audio_channel, 0,
						sbuffer, 1, i,
						frame_count, copy_mode);
    current_offset = sndfile->audio_channel_written[0];
    
    for(i = 0; i < sndfile->audio_channels; i++){
      if(sndfile->audio_channel_written[i] != current_offset){
	write_cache = FALSE;

	break;
      }
    } 
  }
  
  if(write_cache){
    multi_frames = frame_count * sndfile->audio_channels;

    switch(format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	sf_write_short(sndfile->file, sndfile->buffer, multi_frames);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	sf_write_float(sndfile->file, sndfile->buffer, multi_frames);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	sf_write_double(sndfile->file, sndfile->buffer, multi_frames);
      }
      break;
    }

    sndfile->buffer_offset = sndfile->offset;
    
    sndfile->offset += frame_count;
  }
}

void
ags_sndfile_flush(AgsSoundResource *sound_resource)
{
  AgsSndfile *sndfile;
   
  sndfile = AGS_SNDFILE(sound_resource);

  sf_write_sync(sndfile->file);
}

void
ags_sndfile_seek(AgsSoundResource *sound_resource,
		 gint64 frame_count, gint whence)
{
  AgsSndfile *sndfile;

  guint total_frame_count;
  
  sndfile = AGS_SNDFILE(sound_resource);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  if(whence == G_SEEK_CUR){
    if(frame_count >= 0){
      if(sndfile->offset + frame_count < total_frame_count){
	sndfile->offset += total_frame_count;
      }else{
	sndfile->offset = total_frame_count;
      }
    }else{
      if(sndfile->offset + frame_count >= 0){
	sndfile->offset += total_frame_count;
      }else{
	sndfile->offset = 0;
      }
    } 
  }else if(whence == G_SEEK_SET){
    if(frame_count >= 0){
      if(frame_count < total_frame_count){
	sndfile->offset = frame_count;
      }else{
	sndfile->offset = total_frame_count;
      }
    }else{
      sndfile->offset = 0;
    }
  }else if(whence == G_SEEK_END){
    if(frame_count > 0){
      sndfile->offset = total_frame_count;
    }else{
      if(total_frame_count + frame_count > 0){
	sndfile->offset = total_frame_count + total_frame_count;
      }else{
	sndfile->offset = 0;
      }
    }
  }

  sf_seek(sndfile->file, frame_count, whence);
}

void
ags_sndfile_close(AgsSoundResource *sound_resource)
{
  AgsSndfile *sndfile;
   
  sndfile = AGS_SNDFILE(sound_resource);

  sf_close(sndfile->file);
  free(sndfile->info);

  sndfile->file = NULL;
  sndfile->info = NULL;
}

sf_count_t
ags_sndfile_vio_get_filelen(void *user_data)
{
  return(AGS_SNDFILE(user_data)->length);
}

sf_count_t
ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data)
{
  switch(whence){
  case SEEK_CUR:
    AGS_SNDFILE(user_data)->current += offset;
    break;
  case SEEK_SET:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[offset]);
    break;
  case SEEK_END:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[AGS_SNDFILE(user_data)->length - offset]);
    break;
  }

  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(ptr, AGS_SNDFILE(user_data)->current, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(AGS_SNDFILE(user_data)->current, ptr, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data)
{
  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

/**
 * ags_sndfile_new:
 *
 * Creates a new instance of #AgsSndfile.
 *
 * Returns: the new #AgsSndfile.
 *
 * Since: 2.0.0
 */
AgsSndfile*
ags_sndfile_new()
{
  AgsSndfile *sndfile;

  sndfile = (AgsSndfile *) g_object_new(AGS_TYPE_SNDFILE,
					NULL);

  return(sndfile);
}
