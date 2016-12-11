/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_soundcard.h>

#include <math.h>

void ags_soundcard_class_init(AgsSoundcardInterface *interface);

/**
 * SECTION:ags_soundcard
 * @short_description: unique access to soundcards
 * @title: AgsSoundcard
 * @section_id: AgsSoundcard
 * @include: ags/object/ags_soundcard.h
 *
 * The #AgsSoundcard interface gives you a unique access to audio devices.
 */

enum {
  TIC,
  OFFSET_CHANGED,
  STOP,
  LAST_SIGNAL,
};

static guint soundcard_signals[LAST_SIGNAL];

GType
ags_soundcard_get_type()
{
  static GType ags_type_soundcard = 0;

  if(!ags_type_soundcard){
    ags_type_soundcard = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsSoundcard\0",
						       sizeof(AgsSoundcardInterface),
						       (GClassInitFunc) ags_soundcard_class_init,
						       0, NULL, 0);
  }

  return(ags_type_soundcard);
}

GQuark
ags_soundcard_error_quark()
{
  return(g_quark_from_static_string("ags-soundcard-error-quark\0"));
}

void
ags_soundcard_class_init(AgsSoundcardInterface *interface)
{
  /**
   * AgsSoundcard::tic:
   * @soundcard: the object
   *
   * The ::tic signal is emitted every tic of the soundcard. This notifies
   * about a newly played buffer.
   */
  soundcard_signals[TIC] =
    g_signal_new("tic\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSoundcard::offset-changed:
   * @soundcard: the object
   * @note_offset: new notation offset
   *
   * The ::offset-changed signal notifies about changed position within
   * notation.
   */
  soundcard_signals[OFFSET_CHANGED] =
    g_signal_new("offset-changed\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, offset_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsSoundcard::stop:
   * @soundcard: the object
   *
   * The ::stop signal is emitted every stop of the soundcard. This notifies
   * about a newly played buffer.
   */
  soundcard_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

/**
 * ags_soundcard_set_application_context:
 * @soundcard: an #AgsSoundcard
 * @application_context: the application context to set
 *
 * Set application context.
 *
 * Since: 0.4.3
 */
void
ags_soundcard_set_application_context(AgsSoundcard *soundcard,
				      AgsApplicationContext *application_context)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_application_context);
  soundcard_interface->set_application_context(soundcard,
					       application_context);
}

/**
 * ags_soundcard_get_application_context:
 * @soundcard: an #AgsSoundcard
 *
 * Get application context. 
 *
 * Returns: #AgsApplicationContext
 *
 * Since: 0.4.3
 */
AgsApplicationContext*
ags_soundcard_get_application_context(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_application_context, NULL);

  return(soundcard_interface->get_application_context(soundcard));
}

/**
 * ags_soundcard_set_application_mutex:
 * @soundcard: an #AgsSoundcard
 * @application_mutex: the application mutex to set
 *
 * Set application mutex.
 *
 * Since: 0.7.0
 */
void
ags_soundcard_set_application_mutex(AgsSoundcard *soundcard,
				    pthread_mutex_t *application_mutex)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_application_mutex);
  soundcard_interface->set_application_mutex(soundcard,
					       application_mutex);
}

/**
 * ags_soundcard_get_application_mutex:
 * @soundcard: an #AgsSoundcard
 *
 * Get application mutex. 
 *
 * Returns: #AgsApplicationMutex
 *
 * Since: 0.7.0
 */
pthread_mutex_t*
ags_soundcard_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_application_mutex, NULL);

  return(soundcard_interface->get_application_mutex(soundcard));
}

/**
 * ags_soundcard_set_device:
 * @soundcard: an #AgsSoundcard
 * @device_id: the device to set
 *
 * Set device.
 *
 * Since: 0.4.3
 */
void
ags_soundcard_set_device(AgsSoundcard *soundcard,
			 gchar *device_id)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_device);
  soundcard_interface->set_device(soundcard,
				  device_id);
}

/**
 * ags_soundcard_get_device:
 * @soundcard: an #AgsSoundcard
 *
 * Get device.
 *
 * Returns: the device's identifier
 *
 * Since: 0.4.3
 */
gchar*
ags_soundcard_get_device(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_device, NULL);

  return(soundcard_interface->get_device(soundcard));
}

/**
 * ags_soundcard_get_presets:
 * @soundcard: an #AgsSoundcard
 * @channels: the audio channels
 * @rate: the samplerate
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Get presets. 
 *
 * Since: 0.4.2
 */
void
ags_soundcard_get_presets(AgsSoundcard *soundcard,
			  guint *channels,
			  guint *rate,
			  guint *buffer_size,
			  guint *format)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->get_presets);
  soundcard_interface->get_presets(soundcard,
				   channels,
				   rate,
				   buffer_size,
				   format);
}

/**
 * ags_soundcard_set_presets:
 * @soundcard: an #AgsSoundcard
 * @channels: the audio channels
 * @rate: the samplerate
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Set presets. 
 *
 * Since: 0.4.2
 */
void
ags_soundcard_set_presets(AgsSoundcard *soundcard,
			  guint channels,
			  guint rate,
			  guint buffer_size,
			  guint format)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_presets);
  soundcard_interface->set_presets(soundcard,
				   channels,
				   rate,
				   buffer_size,
				   format);
}

/**
 * ags_soundcard_list_cards:
 * @soundcard: an #AgsSoundcard
 * @card_id: a list containing card ids
 * @card_name: a list containing card names
 *
 * Retrieve @card_id and @card_name as a list of strings.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_list_cards(AgsSoundcard *soundcard,
			 GList **card_id, GList **card_name)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->list_cards);
  soundcard_interface->list_cards(soundcard, card_id, card_name);
}

/**
 * ags_soundcard_pcm_info:
 * @soundcard: an #AgsSoundcard
 * @card_id: the selected soundcard by its string identifier
 * @channels_min: min channels supported
 * @channels_max: max channels supported
 * @rate_min: min samplerate supported
 * @rate_max: max samplerate supported
 * @buffer_size_min: min buffer size supported by soundcard
 * @buffer_size_max: max buffer size supported by soundcard
 * @error: an error that may occure
 *
 * Retrieve detailed information of @card_id soundcard.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
		       guint *channels_min, guint *channels_max,
		       guint *rate_min, guint *rate_max,
		       guint *buffer_size_min, guint *buffer_size_max,
		       GError **error)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->pcm_info);
  soundcard_interface->pcm_info(soundcard, card_id,
				channels_min, channels_max,
				rate_min, rate_max,
				buffer_size_min, buffer_size_max,
				error);
}

/**
 * ags_soundcard_get_poll_fd:
 * @soundcard: an #AgsSoundcard
 *
 * Get poll file descriptors.
 *
 * Returns: a #GList-struct containing poll file descriptors
 *
 * Since: 0.7.50
 */
GList*
ags_soundcard_get_poll_fd(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_poll_fd, NULL);

  return(soundcard_interface->get_poll_fd(soundcard));
}

/**
 * ags_soundcard_is_available:
 * @soundcard: an #AgsSoundcard
 *
 * Get available.
 *
 * Returns: %TRUE if available, else %FALSE
 *
 * Since: 0.7.50
 */
gboolean
ags_soundcard_is_available(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), FALSE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->is_available, FALSE);

  return(soundcard_interface->is_available(soundcard));
}

/**
 * ags_soundcard_is_starting:
 * @soundcard: an #AgsSoundcard
 *
 * Get starting.
 *
 * Returns: %TRUE if starting, else %FALSE
 *
 * Since: 0.4.3
 */
gboolean
ags_soundcard_is_starting(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), FALSE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->is_starting, FALSE);

  return(soundcard_interface->is_starting(soundcard));
}

/**
 * ags_soundcard_is_playing:
 * @soundcard: an #AgsSoundcard
 *
 * Get playing.
 *
 * Returns: %TRUE if playing, else %FALSE
 *
 * Since: 0.4.3
 */
gboolean
ags_soundcard_is_playing(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), FALSE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->is_playing, FALSE);

  return(soundcard_interface->is_playing(soundcard));
}

/**
 * ags_soundcard_is_recording:
 * @soundcard: an #AgsSoundcard
 *
 * Get recording.
 *
 * Returns: %TRUE if recording, else %FALSE
 *
 * Since: 0.7.0
 */
gboolean
ags_soundcard_is_recording(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), FALSE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->is_recording, FALSE);

  return(soundcard_interface->is_recording(soundcard));
}

/**
 * ags_soundcard_get_uptime:
 * @soundcard: an #AgsSoundcard
 *
 * Get playback time as string.
 *
 * Returns: playback time as string
 *
 * Since: 0.7.53
 */
gchar*
ags_soundcard_get_uptime(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_uptime, NULL);

  return(soundcard_interface->get_uptime(soundcard));
}

/**
 * ags_soundcard_play:
 * @soundcard: an #AgsSoundcard
 * @error: an error that may occure
 *
 * Initializes the soundcard for playback.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_play_init(AgsSoundcard *soundcard,
			GError **error)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->play_init);
  soundcard_interface->play_init(soundcard,
				 error);
}

/**
 * ags_soundcard_play:
 * @soundcard: an #AgsSoundcard
 * @error: an error that may occure
 *
 * Plays the current buffer of soundcard.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_play(AgsSoundcard *soundcard,
		   GError **error)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->play);
  soundcard_interface->play(soundcard,
			    error);
}

/**
 * ags_soundcard_record:
 * @soundcard: an #AgsSoundcard
 * @error: an error that may occure
 *
 * Initializes the soundcard for recordback.
 *
 * Since: 0.7.0
 */
void
ags_soundcard_record_init(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->record_init);
  soundcard_interface->record_init(soundcard,
				   error);
}

/**
 * ags_soundcard_record:
 * @soundcard: an #AgsSoundcard
 * @error: an error that may occure
 *
 * Records the current buffer of soundcard.
 *
 * Since: 0.7.0
 */
void
ags_soundcard_record(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->record);
  soundcard_interface->record(soundcard,
			      error);
}

/**
 * ags_soundcard_stop:
 * @soundcard: an #AgsSoundcard
 *
 * Stops the soundcard from playing to it.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_stop(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_signal_emit(soundcard, soundcard_signals[STOP], 0);
}

/**
 * ags_soundcard_tic:
 * @soundcard: an #AgsSoundcard
 *
 * Every call to play may generate a tic. 
 *
 * Since: 0.4.2
 */
void
ags_soundcard_tic(AgsSoundcard *soundcard)
{
  g_signal_emit(soundcard, soundcard_signals[TIC], 0);
}

/**
 * ags_soundcard_offset_changed:
 * @soundcard: an #AgsSoundcard
 * @note_offset: the current note offset
 *
 * Callback when counter expires minor note offset.
 *
 * Since: 0.4.2
 */
void
ags_soundcard_offset_changed(AgsSoundcard *soundcard,
			     guint note_offset)
{
  g_signal_emit(soundcard,
		soundcard_signals[OFFSET_CHANGED],
		0,
		note_offset);
}

/**
 * ags_soundcard_get_buffer:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback buffer. 
 *
 * Returns: current playback buffer
 *
 * Since: 0.4.2
 */
void*
ags_soundcard_get_buffer(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_buffer, NULL);

  return(soundcard_interface->get_buffer(soundcard));
}

/**
 * ags_soundcard_get_next_buffer:
 * @soundcard: an #AgsSoundcard
 *
 * Get future playback buffer.
 *
 * Returns: next playback buffer
 *
 * Since: 0.4.2
 */
void*
ags_soundcard_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_next_buffer, NULL);

  return(soundcard_interface->get_next_buffer(soundcard));
}

/**
 * ags_soundcard_set_bpm:
 * @soundcard: an #AgsSoundcard
 * @bpm: the bpm to set
 *
 * Set current playback bpm. 
 *
 * Since: 0.4.3
 */
void
ags_soundcard_set_bpm(AgsSoundcard *soundcard,
		      gdouble bpm)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_bpm);
  soundcard_interface->set_bpm(soundcard,
			       bpm);
}

/**
 * ags_soundcard_get_bpm:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback bpm. 
 *
 * Returns: bpm
 *
 * Since: 0.4.3
 */
gdouble
ags_soundcard_get_bpm(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_bpm, G_MAXUINT);

  return(soundcard_interface->get_bpm(soundcard));
}

/**
 * ags_soundcard_set_delay_factor:
 * @soundcard: an #AgsSoundcard
 * @delay_factor: the delay factor to set
 *
 * Set current playback delay factor. 
 *
 * Since: 0.7.0
 */
void
ags_soundcard_set_delay_factor(AgsSoundcard *soundcard,
			       gdouble delay_factor)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_delay_factor);
  soundcard_interface->set_delay_factor(soundcard,
					delay_factor);
}

/**
 * ags_soundcard_get_delay_factor:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback delay factor. 
 *
 * Returns: delay factor
 *
 * Since: 0.7.0
 */
gdouble
ags_soundcard_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXDOUBLE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_delay_factor, G_MAXDOUBLE);

  return(soundcard_interface->get_delay_factor(soundcard));
}

/**
 * ags_soundcard_get_absolute_delay:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback delay. 
 *
 * Returns: delay
 *
 * Since: 0.7.100
 */
gdouble
ags_soundcard_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXDOUBLE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_absolute_delay, G_MAXDOUBLE);

  return(soundcard_interface->get_absolute_delay(soundcard));
}

/**
 * ags_soundcard_get_delay:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback delay. 
 *
 * Returns: delay
 *
 * Since: 0.4.2
 */
gdouble
ags_soundcard_get_delay(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXDOUBLE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_delay, G_MAXDOUBLE);

  return(soundcard_interface->get_delay(soundcard));
}

/**
 * ags_soundcard_get_attack:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback attack. 
 *
 * Returns: attack
 *
 * Since: 0.4.2
 */
guint
ags_soundcard_get_attack(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_attack, G_MAXUINT);

  return(soundcard_interface->get_attack(soundcard));
}

/**
 * ags_soundcard_get_delay_counter:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 0.7.6
 */
guint
ags_soundcard_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_delay_counter, G_MAXUINT);

  return(soundcard_interface->get_delay_counter(soundcard));
}

/**
 * ags_soundcard_set_note_offset:
 * @soundcard: an #AgsSoundcard
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 0.4.2
 */
void
ags_soundcard_set_note_offset(AgsSoundcard *soundcard,
			      guint note_offset)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_note_offset);
  soundcard_interface->set_note_offset(soundcard,
				       note_offset);
}

/**
 * ags_soundcard_get_note_offset:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 0.4.2
 */
guint
ags_soundcard_get_note_offset(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_offset, G_MAXUINT);

  return(soundcard_interface->get_note_offset(soundcard));
}

/**
 * ags_soundcard_set_note_offset_absolute:
 * @soundcard: an #AgsSoundcard
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 0.7.103
 */
void
ags_soundcard_set_note_offset_absolute(AgsSoundcard *soundcard,
				       guint note_offset)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_note_offset_absolute);
  soundcard_interface->set_note_offset_absolute(soundcard,
						note_offset);
}

/**
 * ags_soundcard_get_note_offset_absolute:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 0.7.103
 */
guint
ags_soundcard_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_offset_absolute, G_MAXUINT);

  return(soundcard_interface->get_note_offset_absolute(soundcard));
}

/**
 * ags_soundcard_set_loop:
 *
 * Since: 0.7.35
 */
void
ags_soundcard_set_loop(AgsSoundcard *soundcard,
		       guint loop_left, guint loop_right,
		       gboolean loop)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_loop);
  soundcard_interface->set_loop(soundcard,
				loop_left, loop_right,
				loop);
}

/**
 * ags_soundcard_get_loop:
 *
 * Since: 0.7.35
 */
void
ags_soundcard_get_loop(AgsSoundcard *soundcard,
		       guint *loop_left, guint *loop_right,
		       gboolean *do_loop)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->get_loop);
  soundcard_interface->get_loop(soundcard,
				loop_left, loop_right,
				do_loop);
}

/**
 * ags_soundcard_get_loop_offset:
 * @soundcard: an #AgsSoundcard
 *
 * Get current playback loop offset. 
 *
 * Returns: offset
 *
 * Since: 0.7.35
 */
guint
ags_soundcard_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), 0);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_loop_offset, 0);

  return(soundcard_interface->get_loop_offset(soundcard));
}

/**
 * ags_soundcard_set_audio:
 * @soundcard: an #AgsSoundcard
 * @audio: the audio as #GList-struct to set
 *
 * Set audio. 
 *
 * Since: 0.4.3
 */
void
ags_soundcard_set_audio(AgsSoundcard *soundcard,
			GList *audio)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_audio);
  soundcard_interface->set_audio(soundcard,
				 audio);
}

/**
 * ags_soundcard_get_audio:
 * @soundcard: an #AgsSoundcard
 *
 * Get audio. 
 *
 * Returns: a #GList-struct container #AgsAudio
 *
 * Since: 0.4.3
 */
GList*
ags_soundcard_get_audio(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_audio, NULL);

  return(soundcard_interface->get_audio(soundcard));
}
