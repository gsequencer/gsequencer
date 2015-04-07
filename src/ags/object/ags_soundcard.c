/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/object/ags_soundcard.h>

#include <math.h>

void ags_soundcard_class_init(AgsSoundcardInterface *interface);

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
  g_signal_new("offset-changed\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSoundcardInterface, offset_changed),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__UINT,
	       G_TYPE_NONE, 1,
	       G_TYPE_UINT);
}

AgsApplicationContext*
ags_soundcard_get_application_context(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_application_context, NULL);

  return(soundcard_interface->get_application_context(soundcard));
}

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
 * ags_soundcard_play:
 * @soundcard an #AgsSoundcard
 * @card_id a list containing card ids
 * @card_name a list containing card names
 * @error an error that may occure
 *
 * Retrieve @card_id and @card_name as a list of strings.
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
 * ags_soundcard_play:
 * @soundcard an #AgsSoundcard
 * @card_id the selected soundcard by its string identifier
 * @channels_min min channels supported
 * @channels_max max channels supported
 * @rate_min min samplerate supported
 * @rate_max max samplerate supported
 * @buffer_size_min min buffer size supported by soundcard
 * @buffer_size_max max buffer size supported by soundcard
 * @error an error that may occure
 *
 * Retrieve detailed information of @card_id soundcard.
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
 * ags_soundcard_play:
 * @soundcard an #AgsSoundcard
 * @error an error that may occure
 *
 * Initializes the soundcard for playback.
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
 * @soundcard an #AgsSoundcard
 * @error an error that may occure
 *
 * Plays the current buffer of soundcard.
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
 * ags_soundcard_stop:
 * @soundcard an #AgsSoundcard
 *
 * Stops the soundcard from playing to it.
 */
void
ags_soundcard_stop(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->stop);
  soundcard_interface->stop(soundcard);
}

/**
 * ags_soundcard_tic:
 * @soundcard an #AgsSoundcard
 *
 * Every call to play may generate a tic. 
 */
void
ags_soundcard_tic(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->tic);
  soundcard_interface->tic(soundcard);
}

/**
 * ags_soundcard_tic:
 * @soundcard an #AgsSoundcard
 *
 * Callback when counter expires minor note offset.
 */
void
ags_soundcard_offset_changed(AgsSoundcard *soundcard,
			     guint note_offset)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_if_fail(AGS_IS_SOUNDCARD(soundcard));
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_if_fail(soundcard_interface->offset_changed);
  soundcard_interface->offset_changed(soundcard,
				      note_offset);
}

/**
 * ags_soundcard_get_buffer:
 * @soundcard an #AgsSoundcard
 *
 * Get current playback buffer. 
 */
signed short*
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
 * @soundcard an #AgsSoundcard
 *
 * Get future playback buffer.
 */
signed short*
ags_soundcard_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_next_buffer, NULL);

  return(soundcard_interface->get_next_buffer(soundcard));
}

gdouble
ags_soundcard_get_delay(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_delay, G_MAXUINT);

  return(soundcard_interface->get_delay(soundcard));
}

guint
ags_soundcard_get_attack(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_attack, G_MAXUINT);

  return(soundcard_interface->get_attack(soundcard));
}

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

guint
ags_soundcard_get_note_offset(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), G_MAXUINT);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_note_offset, G_MAXUINT);

  return(soundcard_interface->get_note_offset(soundcard));
}

GList*
ags_soundcard_get_audio(AgsSoundcard *soundcard)
{
  AgsSoundcardInterface *soundcard_interface;

  g_return_val_if_fail(AGS_IS_SOUNDCARD(soundcard), NULL);
  soundcard_interface = AGS_SOUNDCARD_GET_INTERFACE(soundcard);
  g_return_val_if_fail(soundcard_interface->get_audio, NULL);

  return(soundcard_interface->get_audio(soundcard));
}

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
