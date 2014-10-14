/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/object/ags_playable.h>

#include <ags-lib/object/ags_connectable.h>

#include <math.h>

void ags_playable_base_init(AgsPlayableInterface *interface);

/**
 * SECTION:ags_playable
 * @short_description: read/write audio
 * @title: AgsPlayable
 * @section_id:
 * @include: ags/object/ags_playable.h
 *
 * The #AgsPlayable interface gives you a unique access to file related
 * IO operations.
 */

GType
ags_playable_get_type()
{
  static GType ags_type_playable = 0;

  if(!ags_type_playable){
    static const GTypeInfo ags_playable_info = {
      sizeof(AgsPlayableInterface),
      (GBaseInitFunc) ags_playable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_playable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsPlayable\0", &ags_playable_info,
					       0);
  }

  return(ags_type_playable);
}


GQuark
ags_playable_error_quark()
{
  return(g_quark_from_static_string("ags-playable-error-quark\0"));
}

void
ags_playable_base_init(AgsPlayableInterface *interface)
{
  /* empty */
}

/**
 * ags_playable_rw_open:
 * @playable: the #AgsPlayable
 * @name: the filename 
 * @create: if %TRUE file is created
 * @samplerate: the samplerate of the file
 * @channels: the count of audio channels
 * @frames: the count of frames
 * @format: the audio file's format
 * 
 * Opens a file in read/write mode.
 *
 * Returns: %TRUE on success.
 */
gboolean
ags_playable_rw_open(AgsPlayable *playable, gchar *name,
		     gboolean create,
		     guint samplerate, guint channels,
		     guint frames,
		     guint format)
{
  AgsPlayableInterface *playable_interface;
  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), FALSE);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->open, FALSE);
  ret_val = playable_interface->rw_open(playable, name,
					create,
					samplerate, channels,
					frames,
					format);

  return(ret_val);
}

/**
 * ags_playable_open:
 * @playable: the #AgsPlayable
 * @name: the filename 
 * 
 * Opens a file in read-only mode.
 *
 * Returns: %TRUE on success
 */
gboolean
ags_playable_open(AgsPlayable *playable, gchar *name)
{
  AgsPlayableInterface *playable_interface;
  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), FALSE);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->open, FALSE);
  ret_val = playable_interface->open(playable, name);

  return(ret_val);
}

/**
 * ags_playable_level_count:
 * @playable: the #AgsPlayable
 * 
 * Retrieve the count of levels.
 *
 * Returns: level count
 */
guint
ags_playable_level_count(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;
  guint ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), 0);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->level_count, 0);
  ret_val = playable_interface->level_count(playable);

  return(ret_val);
}

/**
 * ags_playable_level_count:
 * @playable: the #AgsPlayable
 * 
 * Retrieve the selected level.
 *
 * Returns: nth level
 */
guint
ags_playable_nth_level(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;
  guint ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), 0);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->nth_level, 0);
  ret_val = playable_interface->nth_level(playable);

  return(ret_val);
}

/**
 * ags_playable_selected_level:
 * @playable: the #AgsPlayable
 * 
 * Retrieve the selected level's name.
 *
 * Returns: nth level name
 */
gchar*
ags_playable_selected_level(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;
  gchar *ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), 0);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->selected_level, 0);
  ret_val = playable_interface->selected_level(playable);

  return(ret_val);

}

/**
 * ags_playable_sublevel_names:
 * @playable: the #AgsPlayable
 * 
 * Retrieve the all sub-level's name.
 *
 * Returns: sub-level names
 */
gchar**
ags_playable_sublevel_names(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;
  gchar **ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), NULL);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->sublevel_names, NULL);
  ret_val = playable_interface->sublevel_names(playable);

  return(ret_val);
}

/**
 * ags_playable_level_select:
 * @playable: an #AgsPlayable
 * @nth_level: of type guint
 * @sublevel_name: a gchar pointer
 * @error: an error that may occure
 *
 * Select a level in an monolythic file where @nth_level and @sublevel_name are equivalent.
 * If @sublevel_name is NULL @nth_level will be chosen.
 */
void
ags_playable_level_select(AgsPlayable *playable,
			  guint nth_level, gchar *sublevel_name,
			  GError **error)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->level_select);
  playable_interface->level_select(playable, nth_level, sublevel_name, error);
}

/**
 * ags_playable_level_up:
 * @playable: an #AgsPlayable
 * @levels: n-levels up
 * @error: returned error
 *
 * Move up in hierarchy.
 */
void
ags_playable_level_up(AgsPlayable *playable,
		      guint levels,
		      GError **error)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->level_up);
  playable_interface->level_up(playable, levels, error);
}

/**
 * ags_playable_iter_start:
 * @playable: an #AgsPlayable
 *
 * Start iterating current level.
 */
void
ags_playable_iter_start(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->iter_start);
  playable_interface->iter_start(playable);
}

/**
 * ags_playable_iter_next:
 * @playable: an #AgsPlayable
 *
 * Iterating next on current level.
 */
gboolean
ags_playable_iter_next(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;
  gboolean ret_val;

  g_return_val_if_fail(AGS_IS_PLAYABLE(playable), FALSE);
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_val_if_fail(playable_interface->iter_next, FALSE);
  ret_val = playable_interface->iter_next(playable);

  return(ret_val);
}

/**
 * ags_playable_info:
 * @playable: an #AgsPlayable
 * @channels: channels
 * @frames: frames
 * @loop_start: loop start
 * @loop_end: loop end
 * @error: returned error
 *
 * Retrieve information about selected audio data.
 */
void
ags_playable_info(AgsPlayable *playable,
		  guint *channels, guint *frames,
		  guint *loop_start, guint *loop_end,
		  GError **error)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->info);
  playable_interface->info(playable, channels, frames, loop_start, loop_end, error);
}

/**
 * ags_playable_read:
 * @playable: an #AgsPlayable
 * @channel: nth channel
 * @error: returned error
 *
 * Read audio buffer of playable audio data.
 * 
 * Returns: audio buffer
 */
short*
ags_playable_read(AgsPlayable *playable,
		  guint channel,
		  GError **error)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->read);
  playable_interface->read(playable, channel, error);
}

/**
 * ags_playable_write:
 * @playable: an #AgsPlayable
 * @buffer: audio data
 * @buffer_length: frame count
 *
 * Write @buffer_length of @buffer audio data.
 */
void
ags_playable_write(AgsPlayable *playable,
		   signed short *buffer, guint buffer_length)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->write);
  playable_interface->write(playable, buffer, buffer_length);
}

/**
 * ags_playable_flush:
 * @playable: an #AgsPlayable
 *
 * Flush internal audio buffer.
 */
void
ags_playable_flush(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->flush);
  playable_interface->flush(playable);
}

/**
 * ags_playable_seek:
 * @playable: an #AgsPlayable
 * @frames: n-frames to seek
 * @whence: SEEK_SET, SEEK_CUR, or SEEK_END
 *
 * Seek @playable to address.
 */
void
ags_playable_seek(AgsPlayable *playable,
		  guint frames, gint whence)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->seek);
  playable_interface->seek(playable, frames, whence);
}

/**
 * ags_playable_flush:
 * @playable: an #AgsPlayable
 *
 * Close audio file.
 */
void
ags_playable_close(AgsPlayable *playable)
{
  AgsPlayableInterface *playable_interface;

  g_return_if_fail(AGS_IS_PLAYABLE(playable));
  playable_interface = AGS_PLAYABLE_GET_INTERFACE(playable);
  g_return_if_fail(playable_interface->close);
  playable_interface->close(playable);
}

/**
 * ags_playable_read_audio_signal:
 * @playable: an #AgsPlayable
 * @devout: the #AgsDevout defaulting to
 * @start_channel: read from channel
 * @channels_to_read: n-times
 *
 * Read the audio signal of @AgsPlayable.
 *
 * Returns: a #GList of #AgsAudioSignal
 */
GList*
ags_playable_read_audio_signal(AgsPlayable *playable,
			       AgsDevout *devout,
			       guint start_channel, guint channels_to_read)
{
  AgsAudioSignal *audio_signal;
  GList *stream, *list, *list_beginning;
  short *buffer;
  guint channels;
  guint frames;
  guint loop_start;
  guint loop_end;
  guint length;
  guint i, j, k, i_stop, j_stop;
  GError *error;

  ags_playable_info(playable,
		    &channels, &frames,
		    &loop_start, &loop_end,
		    &error);

  length = (guint) ceil((double)(frames) / (double)(devout->buffer_size));

#ifdef AGS_DEBUG
  g_message("ags_playable_read_audio_signal:\n  frames = %u\n  devout->buffer_size = %u\n  length = %u\n\0", frames, devout->buffer_size, length);
#endif

  list = NULL;
  i = start_channel;
  i_stop = start_channel + channels_to_read;

  for(; i < i_stop; i++){
    audio_signal = ags_audio_signal_new((GObject *) devout,
					NULL,
					NULL);
    audio_signal->samplerate = devout->frequency;
    audio_signal->buffer_size = devout->buffer_size;

    list = g_list_prepend(list, audio_signal);

    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

    list->data = (gpointer) audio_signal;
    audio_signal->devout = (GObject *) devout;
  }

  list_beginning = list;

  j_stop = (guint) floor((double)(frames) / (double)(devout->buffer_size));

  for(i = start_channel; list != NULL; i++){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);
    ags_audio_signal_stream_resize(audio_signal, length);
    audio_signal->loop_start = loop_start;
    audio_signal->loop_end = loop_end;

    error = NULL;
    buffer = ags_playable_read(playable,
			       i,
			       &error);

    if(error != NULL){
      g_error("%s\0", error->message);
    }

    stream = audio_signal->stream_beginning;
    
    for(j = 0; j < j_stop; j++){
      for(k = 0; k < devout->buffer_size; k++)
	((short *) stream->data)[k] = buffer[j * devout->buffer_size + k];
      
      stream = stream->next;
    }
    
    if(frames % devout->buffer_size != 0){
      for(k = 0; k < frames % devout->buffer_size; k++)
	((short *) stream->data)[k] = buffer[j * devout->buffer_size + k];
    }

    free(buffer);
    list = list->next;
  }

  return(list_beginning);
}
