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

#include <ags/object/ags_sequencer.h>

#include <math.h>

void ags_sequencer_class_init(AgsSequencerInterface *interface);

/**
 * SECTION:ags_sequencer
 * @short_description: unique access to sequencers
 * @title: AgsSequencer
 * @section_id: AgsSequencer
 * @include: ags/object/ags_sequencer.h
 *
 * The #AgsSequencer interface gives you a unique access to MIDI devices.
 */

GType
ags_sequencer_get_type()
{
  static GType ags_type_sequencer = 0;

  if(!ags_type_sequencer){
    ags_type_sequencer = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsSequencer",
						       sizeof(AgsSequencerInterface),
						       (GClassInitFunc) ags_sequencer_class_init,
						       0, NULL, 0);
  }

  return(ags_type_sequencer);
}

GQuark
ags_sequencer_error_quark()
{
  return(g_quark_from_static_string("ags-sequencer-error-quark"));
}

void
ags_sequencer_class_init(AgsSequencerInterface *interface)
{
  /**
   * AgsSequencer::tic:
   * @sequencer: the object
   *
   * The ::tic signal is emitted every tic of the sequencer. This notifies
   * about a newly played buffer.
   * 
   * Since: 0.7.122
   */
  g_signal_new("tic",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSequencerInterface, tic),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__VOID,
	       G_TYPE_NONE, 0);

  /**
   * AgsSequencer::offset-changed:
   * @sequencer: the object
   * @note_offset: new notation offset
   *
   * The ::offset-changed signal notifies about changed position within
   * notation.
   * 
   * Since: 0.7.122
   */
  g_signal_new("offset-changed",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSequencerInterface, offset_changed),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__UINT,
	       G_TYPE_NONE, 1,
	       G_TYPE_UINT);
}

/**
 * ags_sequencer_set_application_context:
 * @sequencer: an #AgsSequencer
 * @application_context: the application context to set
 *
 * Set application context.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_application_context(AgsSequencer *sequencer,
				      AgsApplicationContext *application_context)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_application_context);
  sequencer_interface->set_application_context(sequencer,
					       application_context);
}

/**
 * ags_sequencer_get_application_context:
 * @sequencer: an #AgsSequencer
 *
 * Get application context. 
 *
 * Returns: #AgsApplicationContext
 *
 * Since: 0.7.0
 */
AgsApplicationContext*
ags_sequencer_get_application_context(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_application_context, NULL);

  return(sequencer_interface->get_application_context(sequencer));
}

/**
 * ags_sequencer_set_application_mutex:
 * @sequencer: an #AgsSequencer
 * @application_mutex: the application mutex to set
 *
 * Set application mutex.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_application_mutex(AgsSequencer *sequencer,
				    pthread_mutex_t *application_mutex)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_application_mutex);
  sequencer_interface->set_application_mutex(sequencer,
					       application_mutex);
}

/**
 * ags_sequencer_get_application_mutex:
 * @sequencer: an #AgsSequencer
 *
 * Get application mutex. 
 *
 * Returns: #AgsApplicationMutex
 *
 * Since: 0.7.0
 */
pthread_mutex_t*
ags_sequencer_get_application_mutex(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_application_mutex, NULL);

  return(sequencer_interface->get_application_mutex(sequencer));
}

/**
 * ags_sequencer_set_device:
 * @sequencer: an #AgsSequencer
 * @device_id: the device to set
 *
 * Set device.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_device(AgsSequencer *sequencer,
			 gchar *device_id)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_device);
  sequencer_interface->set_device(sequencer,
				  device_id);
}

/**
 * ags_sequencer_get_device:
 * @sequencer: an #AgsSequencer
 *
 * Get device.
 *
 * Returns: the device's identifier
 *
 * Since: 0.7.0
 */
gchar*
ags_sequencer_get_device(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_device, NULL);

  return(sequencer_interface->get_device(sequencer));
}

/**
 * ags_sequencer_list_cards:
 * @sequencer: an #AgsSequencer
 * @card_id: a list containing card ids
 * @card_name: a list containing card names
 *
 * Retrieve @card_id and @card_name as a list of strings.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_list_cards(AgsSequencer *sequencer,
			 GList **card_id, GList **card_name)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->list_cards);
  sequencer_interface->list_cards(sequencer, card_id, card_name);
}

/**
 * ags_sequencer_is_starting:
 * @sequencer: an #AgsSequencer
 *
 * Get starting.
 *
 * Returns: %TRUE if starting, else %FALSE
 *
 * Since: 0.7.0
 */
gboolean
ags_sequencer_is_starting(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), FALSE);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->is_starting, FALSE);

  return(sequencer_interface->is_starting(sequencer));
}

/**
 * ags_sequencer_is_playing:
 * @sequencer: an #AgsSequencer
 *
 * Get playing.
 *
 * Returns: %TRUE if playing, else %FALSE
 *
 * Since: 0.7.0
 */
gboolean
ags_sequencer_is_playing(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), FALSE);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->is_playing, FALSE);

  return(sequencer_interface->is_playing(sequencer));
}

/**
 * ags_sequencer_is_recording:
 * @sequencer: an #AgsSequencer
 *
 * Get recording.
 *
 * Returns: %TRUE if recording, else %FALSE
 *
 * Since: 0.7.0
 */
gboolean
ags_sequencer_is_recording(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), FALSE);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->is_recording, FALSE);

  return(sequencer_interface->is_recording(sequencer));
}

/**
 * ags_sequencer_play:
 * @sequencer: an #AgsSequencer
 * @error: an error that may occure
 *
 * Initializes the sequencer for playback.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_play_init(AgsSequencer *sequencer,
			GError **error)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->play_init);
  sequencer_interface->play_init(sequencer,
				 error);
}

/**
 * ags_sequencer_play:
 * @sequencer: an #AgsSequencer
 * @error: an error that may occure
 *
 * Plays the current buffer of sequencer.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_play(AgsSequencer *sequencer,
		   GError **error)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->play);
  sequencer_interface->play(sequencer,
			    error);
}

/**
 * ags_sequencer_record:
 * @sequencer: an #AgsSequencer
 * @error: an error that may occure
 *
 * Initializes the sequencer for recordback.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_record_init(AgsSequencer *sequencer,
			  GError **error)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->record_init);
  sequencer_interface->record_init(sequencer,
				   error);
}

/**
 * ags_sequencer_record:
 * @sequencer: an #AgsSequencer
 * @error: an error that may occure
 *
 * Records the current buffer of sequencer.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_record(AgsSequencer *sequencer,
		     GError **error)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->record);
  sequencer_interface->record(sequencer,
			      error);
}

/**
 * ags_sequencer_stop:
 * @sequencer: an #AgsSequencer
 *
 * Stops the sequencer from playing to it.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_stop(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->stop);
  sequencer_interface->stop(sequencer);
}

/**
 * ags_sequencer_tic:
 * @sequencer: an #AgsSequencer
 *
 * Every call to play may generate a tic. 
 *
 * Since: 0.7.0
 */
void
ags_sequencer_tic(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->tic);
  sequencer_interface->tic(sequencer);
}

/**
 * ags_sequencer_tic:
 * @sequencer: an #AgsSequencer
 *
 * Callback when counter expires minor note offset.
 *
 * Since: 0.7.0
 */
void
ags_sequencer_offset_changed(AgsSequencer *sequencer,
			     guint note_offset)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->offset_changed);
  sequencer_interface->offset_changed(sequencer,
				      note_offset);
}

/**
 * ags_sequencer_get_buffer:
 * @sequencer: an #AgsSequencer
 * @buffer_length: the buffer's length
 *
 * Get current playback buffer. 
 *
 * Returns: current playback buffer
 *
 * Since: 0.7.0
 */
void*
ags_sequencer_get_buffer(AgsSequencer *sequencer,
			 guint *buffer_length)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_buffer, NULL);

  return(sequencer_interface->get_buffer(sequencer,
					 buffer_length));
}

/**
 * ags_sequencer_get_next_buffer:
 * @sequencer: an #AgsSequencer
 * @buffer_length: the buffer's length
 *
 * Get future playback buffer.
 *
 * Returns: next playback buffer
 *
 * Since: 0.7.0
 */
void*
ags_sequencer_get_next_buffer(AgsSequencer *sequencer,
			      guint *buffer_length)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_next_buffer, NULL);

  return(sequencer_interface->get_next_buffer(sequencer,
					      buffer_length));
}

/**
 * ags_sequencer_set_bpm:
 * @sequencer: an #AgsSequencer
 * @bpm: the bpm to set
 *
 * Set current playback bpm. 
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_bpm(AgsSequencer *sequencer,
		      gdouble bpm)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_bpm);
  sequencer_interface->set_bpm(sequencer,
			       bpm);
}

/**
 * ags_sequencer_get_bpm:
 * @sequencer: an #AgsSequencer
 *
 * Get current playback bpm. 
 *
 * Returns: bpm
 *
 * Since: 0.7.0
 */
gdouble
ags_sequencer_get_bpm(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), G_MAXUINT);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_bpm, G_MAXUINT);

  return(sequencer_interface->get_bpm(sequencer));
}

/**
 * ags_sequencer_set_delay_factor:
 * @sequencer: an #AgsSequencer
 * @delay_factor: the delay factor to set
 *
 * Set current playback delay factor. 
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_delay_factor(AgsSequencer *sequencer,
			       gdouble delay_factor)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_delay_factor);
  sequencer_interface->set_delay_factor(sequencer,
					delay_factor);
}

/**
 * ags_sequencer_get_delay_factor:
 * @sequencer: an #AgsSequencer
 *
 * Get current playback delay factor. 
 *
 * Returns: delay factor
 *
 * Since: 0.7.0
 */
gdouble
ags_sequencer_get_delay_factor(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), G_MAXUINT);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_delay_factor, G_MAXUINT);

  return(sequencer_interface->get_delay_factor(sequencer));
}

/**
 * ags_sequencer_get_note_offset:
 * @sequencer: an #AgsSequencer
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 0.7.0
 */
guint
ags_sequencer_get_note_offset(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), G_MAXUINT);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_note_offset, G_MAXUINT);

  return(sequencer_interface->get_note_offset(sequencer));
}

/**
 * ags_sequencer_set_note_offset:
 * @sequencer: an #AgsSequencer
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_note_offset(AgsSequencer *sequencer,
			      guint note_offset)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_note_offset);
  sequencer_interface->set_note_offset(sequencer,
				       note_offset);
}

/**
 * ags_sequencer_set_audio:
 * @sequencer: an #AgsSequencer
 * @audio: the audio to set
 *
 * Set audio. 
 *
 * Since: 0.7.0
 */
void
ags_sequencer_set_audio(AgsSequencer *sequencer,
			GList *audio)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_audio);
  sequencer_interface->set_audio(sequencer,
				 audio);
}

/**
 * ags_sequencer_get_audio:
 * @sequencer: an #AgsSequencer
 *
 * Get audio. 
 *
 * Returns: a #GList container #AgsAudio
 *
 * Since: 0.7.0
 */
GList*
ags_sequencer_get_audio(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), NULL);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_audio, NULL);

  return(sequencer_interface->get_audio(sequencer));
}
