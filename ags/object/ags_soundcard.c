/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

void ags_soundcard_class_init(AgsSoundcardInterface *ginterface);

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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_soundcard = 0;

    ags_type_soundcard = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsSoundcard",
						       sizeof(AgsSoundcardInterface),
						       (GClassInitFunc) ags_soundcard_class_init,
						       0, NULL, 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_soundcard);
  }

  return(g_define_type_id__static);
}

GType
ags_soundcard_format_get_type()
{
  static gsize g_enum_type_id__static;

  if(g_once_init_enter(&g_enum_type_id__static)){
    static const GEnumValue values[] = {
      { AGS_SOUNDCARD_SIGNED_8_BIT, "AGS_SOUNDCARD_SIGNED_8_BIT", "soundcard-signed-8-bit" },
      { AGS_SOUNDCARD_SIGNED_16_BIT, "AGS_SOUNDCARD_SIGNED_16_BIT", "soundcard-signed-16-bit" },
      { AGS_SOUNDCARD_SIGNED_24_BIT, "AGS_SOUNDCARD_SIGNED_24_BIT", "soundcard-signed-24-bit" },
      { AGS_SOUNDCARD_SIGNED_32_BIT, "AGS_SOUNDCARD_SIGNED_32_BIT", "soundcard-signed-32-bit" },
      { AGS_SOUNDCARD_SIGNED_64_BIT, "AGS_SOUNDCARD_SIGNED_64_BIT", "soundcard-signed-64-bit" },
      { AGS_SOUNDCARD_FLOAT, "AGS_SOUNDCARD_FLOAT", "soundcard-float" },
      { AGS_SOUNDCARD_DOUBLE, "AGS_SOUNDCARD_DOUBLE", "soundcard-double" },
      { AGS_SOUNDCARD_COMPLEX, "AGS_SOUNDCARD_COMPLEX", "soundcard-complex" },
      { 0, NULL, NULL }
    };

    GType g_enum_type_id = g_enum_register_static(g_intern_static_string("AgsSoundcardFormat"), values);

    g_once_init_leave(&g_enum_type_id__static, g_enum_type_id);
  }
  
  return(g_enum_type_id__static);
}

GType
ags_soundcard_capability_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_SOUNDCARD_CAPABILITY_PLAYBACK, "AGS_SOUNDCARD_CAPABILITY_PLAYBACK", "soundcard-capability-playback" },
      { AGS_SOUNDCARD_CAPABILITY_CAPTURE, "AGS_SOUNDCARD_CAPABILITY_CAPTURE", "soundcard-capability-capture" },
      { AGS_SOUNDCARD_CAPABILITY_DUPLEX, "AGS_SOUNDCARD_CAPABILITY_DUPLEX", "soundcard-capability-duplex" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsSoundcardCapability"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

GQuark
ags_soundcard_error_quark()
{
  return(g_quark_from_static_string("ags-soundcard-error-quark"));
}

void
ags_soundcard_class_init(AgsSoundcardInterface *ginterface)
{
  /**
   * AgsSoundcard::tic:
   * @soundcard: the #AgsSoundcard object
   *
   * The ::tic signal is emitted every tic of the soundcard. This notifies
   * about a newly played buffer.
   *
   * Since: 3.0.0
   */
  soundcard_signals[TIC] =
    g_signal_new("tic",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsSoundcard::offset-changed:
   * @soundcard: the #AgsSoundcard object
   * @note_offset: new notation offset
   *
   * The ::offset-changed signal notifies about changed position within
   * notation.
   *
   * Since: 3.0.0
   */
  soundcard_signals[OFFSET_CHANGED] =
    g_signal_new("offset-changed",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, offset_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsSoundcard::stop:
   * @soundcard: the #AgsSoundcard object
   *
   * The ::stop signal is emitted every stop of the soundcard. This notifies
   * about a newly played buffer.
   *
   * Since: 3.0.0
   */
  soundcard_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_INTERFACE(ginterface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSoundcardInterface, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

/**
 * ags_soundcard_set_device:
 * @soundcard: the #AgsSoundcard
 * @card_id: the device to set
 *
 * Set device.
 *
 * Since: 3.0.0
 */
void
ags_soundcard_set_device(AgsSoundcard *soundcard,
			 gchar *card_id)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_device);
  soundcard_interface->set_device(soundcard,
				  card_id);
}

/**
 * ags_soundcard_get_device:
 * @soundcard: the #AgsSoundcard
 *
 * Get device.
 *
 * Returns: the device's identifier
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @channels: (out): the audio channels
 * @rate: (out): the samplerate
 * @buffer_size: (out): the buffer size
 * @format: (out): the format
 *
 * Get presets. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_get_presets(AgsSoundcard *soundcard,
			  guint *channels,
			  guint *rate,
			  guint *buffer_size,
			  AgsSoundcardFormat *format)
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
 * @soundcard: the #AgsSoundcard
 * @channels: the audio channels
 * @rate: the samplerate
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Set presets. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_set_presets(AgsSoundcard *soundcard,
			  guint channels,
			  guint rate,
			  guint buffer_size,
			  AgsSoundcardFormat format)
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
 * @soundcard: the #AgsSoundcard
 * @card_id: (element-type utf8) (out callee-allocates) (array zero-terminated=1) (transfer full): a list containing card ids
 * @card_name: (element-type utf8) (out callee-allocates) (array zero-terminated=1) (transfer full): a list containing card names
 *
 * Retrieve @card_id and @card_name as a list of strings.
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @card_id: the selected soundcard by its string identifier
 * @channels_min: (out): min channels supported
 * @channels_max: (out): max channels supported
 * @rate_min: (out): min samplerate supported
 * @rate_max: (out): max samplerate supported
 * @buffer_size_min: (out): min buffer size supported by soundcard
 * @buffer_size_max: (out): max buffer size supported by soundcard
 * @error: an error that may occure
 *
 * Retrieve detailed information of @card_id soundcard.
 *
 * Since: 3.0.0
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
 * ags_soundcard_get_capability:
 * @soundcard: the #AgsSoundcard
 *
 * Retrieve #AgsSoundcardCapability-enum information.
 *
 * Returns: the capablities flags
 * 
 * Since: 3.0.0
 */
AgsSoundcardCapability
ags_soundcard_get_capability(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  AgsSoundcardCapability capability;
  
  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), 0);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->pcm_info, 0);

  capability = soundcard_interface->get_capability(soundcard);

  return(capability);
}

/**
 * ags_soundcard_is_available:
 * @soundcard: the #AgsSoundcard
 *
 * Get available.
 *
 * Returns: %TRUE if available, else %FALSE
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get starting.
 *
 * Returns: %TRUE if starting, else %FALSE
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get playing.
 *
 * Returns: %TRUE if playing, else %FALSE
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get recording.
 *
 * Returns: %TRUE if recording, else %FALSE
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get playback time as string.
 *
 * Returns: playback time as string
 *
 * Since: 3.0.0
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
 * ags_soundcard_play_init:
 * @soundcard: the #AgsSoundcard
 * @error: an error that may occure
 *
 * Initializes the soundcard for playback.
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @error: an error that may occure
 *
 * Plays the current buffer of soundcard.
 *
 * Since: 3.0.0
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
 * ags_soundcard_record_init:
 * @soundcard: the #AgsSoundcard
 * @error: an error that may occure
 *
 * Initializes the soundcard for recordback.
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @error: an error that may occure
 *
 * Records the current buffer of soundcard.
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Stops the soundcard from playing to it.
 *
 * Since: 3.0.0
 */
void
ags_soundcard_stop(AgsSoundcard *soundcard)
{
  g_signal_emit(soundcard, soundcard_signals[STOP], 0);
}

/**
 * ags_soundcard_tic:
 * @soundcard: the #AgsSoundcard
 *
 * Every call to play may generate a tic. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_tic(AgsSoundcard *soundcard)
{
  g_signal_emit(soundcard, soundcard_signals[TIC], 0);
}

/**
 * ags_soundcard_offset_changed:
 * @soundcard: the #AgsSoundcard
 * @note_offset: the current note offset
 *
 * Callback when counter expires minor note offset.
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback buffer. 
 *
 * Returns: current playback buffer
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get future playback buffer.
 *
 * Returns: next playback buffer
 *
 * Since: 3.0.0
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
 * ags_soundcard_get_prev_buffer:
 * @soundcard: the #AgsSoundcard
 *
 * Get future playback buffer.
 *
 * Returns: prev playback buffer
 *
 * Since: 3.0.0
 */
void*
ags_soundcard_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_prev_buffer, NULL);

  return(soundcard_interface->get_prev_buffer(soundcard));
}

/**
 * ags_soundcard_lock_buffer:
 * @soundcard: the #AgsSoundcard
 * @buffer: the buffer to lock
 *
 * Lock @buffer. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_lock_buffer(AgsSoundcard *soundcard,
			  void *buffer)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->lock_buffer);

  soundcard_interface->lock_buffer(soundcard,
				   buffer);
}

/**
 * ags_soundcard_unlock_buffer:
 * @soundcard: the #AgsSoundcard
 * @buffer: the buffer to unlock
 *
 * Unlock @buffer. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_unlock_buffer(AgsSoundcard *soundcard,
			    void *buffer)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->unlock_buffer);

  soundcard_interface->unlock_buffer(soundcard,
				     buffer);
}

/**
 * ags_soundcard_set_bpm:
 * @soundcard: the #AgsSoundcard
 * @bpm: the bpm to set
 *
 * Set current playback bpm. 
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback bpm. 
 *
 * Returns: bpm
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @delay_factor: the delay factor to set
 *
 * Set current playback delay factor. 
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback delay factor. 
 *
 * Returns: delay factor
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback delay. 
 *
 * Returns: delay
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback delay. 
 *
 * Returns: delay
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback attack. 
 *
 * Returns: attack
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 3.0.0
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
 * ags_soundcard_set_start_note_offset:
 * @soundcard: the #AgsSoundcard
 * @start_note_offset: the start note offset to set
 *
 * Set start playback note offset. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_set_start_note_offset(AgsSoundcard *soundcard,
				    guint start_note_offset)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_start_note_offset);
  soundcard_interface->set_start_note_offset(soundcard,
					     start_note_offset);
}

/**
 * ags_soundcard_get_start_note_offset:
 * @soundcard: the #AgsSoundcard
 *
 * Get start playback note offset. 
 *
 * Returns: the start note offset
 *
 * Since: 3.0.0
 */
guint
ags_soundcard_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_start_note_offset, G_MAXUINT);

  return(soundcard_interface->get_start_note_offset(soundcard));
}

/**
 * ags_soundcard_set_note_offset:
 * @soundcard: the #AgsSoundcard
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: the current note offset
 *
 * Since: 3.0.0
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
 * ags_soundcard_get_note_256th_offset:
 * @soundcard: the #AgsSoundcard
 * @note_256th_offset_lower: the return location of offset lower range
 * @note_256th_offset_upper: the return location of offset upper range
 *
 * Get current playback note 256th offset. 
 *
 * Since: 6.1.0
 */
void
ags_soundcard_get_note_256th_offset(AgsSoundcard *soundcard,
				    guint *note_256th_offset_lower,
				    guint *note_256th_offset_upper)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->get_note_256th_offset);

  soundcard_interface->get_note_256th_offset(soundcard,
					     note_256th_offset_lower,
					     note_256th_offset_upper);
}

/**
 * ags_soundcard_get_note_256th_attack_of_16th_pulse:
 * @soundcard: the #AgsSoundcard
 * 
 * Get note 256th attack of current 16th pulse.
 * 
 * Returns: the note 256th attack of 16th pulse
 * 
 * Since: 6.3.0
 */
guint
ags_soundcard_get_note_256th_attack_of_16th_pulse(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_256th_attack_of_16th_pulse, G_MAXUINT);

  return(soundcard_interface->get_note_256th_attack_of_16th_pulse(soundcard));
}

/**
 * ags_soundcard_get_note_256th_attack_of_16th_pulse_position:
 * @soundcard: the #AgsSoundcard
 * 
 * Get note 256th attack position of current 16th pulse.
 * 
 * Returns: the note 256th attack position of 16th pulse
 * 
 * Since: 6.3.0
 */
guint
ags_soundcard_get_note_256th_attack_of_16th_pulse_position(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_256th_attack_of_16th_pulse_position, G_MAXUINT);

  return(soundcard_interface->get_note_256th_attack_of_16th_pulse_position(soundcard));
}

/**
 * ags_soundcard_get_note_256th_attack:
 * @soundcard: the #AgsSoundcard
 * @note_256th_attack_lower: the return location of attack lower range
 * @note_256th_attack_upper: the return location of attack upper range
 *
 * Get current playback note 256th attack. 
 *
 * Since: 6.2.2
 */
void
ags_soundcard_get_note_256th_attack(AgsSoundcard *soundcard,
				    guint *note_256th_attack_lower,
				    guint *note_256th_attack_upper)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->get_note_256th_attack);

  soundcard_interface->get_note_256th_attack(soundcard,
					     note_256th_attack_lower,
					     note_256th_attack_upper);
}

/**
 * ags_soundcard_get_note_256th_attack_at_position:
 * @soundcard: the #AgsSoundcard
 * @note_256th_attack_position: the note 256th attack position
 *
 * Get current playback note 256th attack position within 16 times %AGS_SOUNDCARD_DEFAULT_PERIOD.
 *
 * Since: 6.2.2
 */
guint
ags_soundcard_get_note_256th_attack_at_position(AgsSoundcard *soundcard,
						guint note_256th_attack_position)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), 0);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_256th_attack_at_position, 0);

  return(soundcard_interface->get_note_256th_attack_at_position(soundcard,
								note_256th_attack_position));
}

/**
 * ags_soundcard_get_note_256th_attack_position:
 * @soundcard: the #AgsSoundcard
 * @note_256th_attack_position_lower: the return location of attack position lower range
 * @note_256th_attack_position_upper: the return location of attack position upper range
 *
 * Get current playback note 256th attack position within 16 times %AGS_SOUNDCARD_DEFAULT_PERIOD.
 *
 * Since: 6.2.2
 */
void
ags_soundcard_get_note_256th_attack_position(AgsSoundcard *soundcard,
					     guint *note_256th_attack_position_lower,
					     guint *note_256th_attack_position_upper)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->get_note_256th_attack_position);

  soundcard_interface->get_note_256th_attack_position(soundcard,
						      note_256th_attack_position_lower,
						      note_256th_attack_position_upper);
}

/**
 * ags_soundcard_set_note_offset_absolute:
 * @soundcard: the #AgsSoundcard
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 * @loop_left: loop position of region
 * @loop_right: loop position of region
 * @do_loop: if %TRUE do loop, else don't loop
 *
 * Set loop parameters of @soundcard.
 *
 * Since: 3.0.0
 */
void
ags_soundcard_set_loop(AgsSoundcard *soundcard,
		       guint loop_left, guint loop_right,
		       gboolean do_loop)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->set_loop);
  soundcard_interface->set_loop(soundcard,
				loop_left, loop_right,
				do_loop);
}

/**
 * ags_soundcard_get_loop:
 * @soundcard: the #AgsSoundcard
 * @loop_left: (out): return location of loop position's region
 * @loop_right: (out): return location of loop position's region
 * @do_loop: (out): return location of do loop
 *
 * Get loop parameters of @soundcard.
 * 
 * Since: 3.0.0
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
 * @soundcard: the #AgsSoundcard
 *
 * Get current playback loop offset. 
 *
 * Returns: offset
 *
 * Since: 3.0.0
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
 * ags_soundcard_get_sub_block_count:
 * @soundcard: the #AgsSoundcard
 *
 * Get sub block count. 
 *
 * Returns: the sub block count
 *
 * Since: 3.0.0
 */
guint
ags_soundcard_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), 0);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_sub_block_count, 0);

  return(soundcard_interface->get_sub_block_count(soundcard));
}

/**
 * ags_soundcard_trylock_sub_block:
 * @soundcard: the #AgsSoundcard
 * @buffer: the buffer to lock
 * @sub_block: and its sub block
 *
 * Trylock sub block. 
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_soundcard_trylock_sub_block(AgsSoundcard *soundcard,
				void *buffer, guint sub_block)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), FALSE);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->trylock_sub_block, FALSE);

  return(soundcard_interface->trylock_sub_block(soundcard,
						buffer, sub_block));
}

/**
 * ags_soundcard_unlock_sub_block:
 * @soundcard: the #AgsSoundcard
 * @buffer: the buffer to lock
 * @sub_block: and its sub block
 *
 * Unlock sub block. 
 *
 * Since: 3.0.0
 */
void
ags_soundcard_unlock_sub_block(AgsSoundcard *soundcard,
			       void *buffer, guint sub_block)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->unlock_sub_block);

  soundcard_interface->unlock_sub_block(soundcard,
					buffer, sub_block);
}
