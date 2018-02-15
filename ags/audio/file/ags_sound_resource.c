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

#include <ags/audio/file/ags_sound_resource.h>

#include <ags/libags.h>

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
  static GType ags_type_sound_resource = 0;

  if(!ags_type_sound_resource){
    static const GTypeInfo ags_sound_resource_info = {
      sizeof(AgsSoundResourceInterface),
      (GBaseInitFunc) ags_sound_resource_base_init,
      NULL, /* base_finalize */
    };

    ags_type_sound_resource = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsSoundResource", &ags_sound_resource_info,
						     0);
  }

  return(ags_type_sound_resource);
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

  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), FALSE);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->open, FALSE);
  ret_val = sound_resource_interface->open(sound_resource,
					   filename);

  return(ret_val);
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
			   gboolean create)
{
  AgsSoundResourceInterface *sound_resource_interface;

  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), FALSE);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->open, FALSE);
  ret_val = sound_resource_interface->rw_open(sound_resource,
					      filename,
					      create,);

  return(ret_val);
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
 * @audio_channel: the audio channel of resource
 * @frame_count: the frame count to read
 * @format: the format to read
 * 
 * Read @frame_count number of frames from @sound_resource's @audio_channel and copy the data
 * to @dbuffer using @format.
 * 
 * Returns: the count of frames actually read
 * 
 * Since: 2.0.0
 */
guint
ags_sound_resource_read(AgsSoundResource *sound_resource,
			void *dbuffer,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsSound_ResourceInterface *sound_resource_interface;

  guint retval;
  
  g_return_val_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource), 0);
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_val_if_fail(sound_resource_interface->read, 0);

  retval = sound_resource_interface->read(sound_resource,
					  dbuffer,
					  audio_channel,
					  frame_count, format);
  
  return(retval);
}

/**
 * ags_sound_resource_write:
 * @sound_resource: the #AgsSoundResource
 * @sbuffer: the source buffer
 * @audio_channel: the audio channel of resource
 * @frame_count: the frame count to write
 * @format: the format to write
 * 
 * Write @sbuffer to @sound_resource's @audio_channel @frame_count number of 
 * frames having @format.
 * 
 * Since: 2.0.0
 */
void
ags_sound_resource_write(AgsSoundResource *sound_resource,
			 void *sbuffer,
			 guint audio_channel,
			 guint frame_count, guint format)
{
  AgsSound_ResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->write);
  sound_resource_interface->write(sound_resource,
				  sbuffer,
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
  AgsSound_ResourceInterface *sound_resource_interface;

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
			guint frame_count, gint whence)
{
  AgsSound_ResourceInterface *sound_resource_interface;

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
  AgsSound_ResourceInterface *sound_resource_interface;

  g_return_if_fail(AGS_IS_SOUND_RESOURCE(sound_resource));
  sound_resource_interface = AGS_SOUND_RESOURCE_GET_INTERFACE(sound_resource);
  g_return_if_fail(sound_resource_interface->close);  
  sound_resource_interface->close(sound_resource);
}
