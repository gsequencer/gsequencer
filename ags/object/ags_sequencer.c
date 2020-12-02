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

#include <ags/object/ags_sequencer.h>

void ags_sequencer_class_init(AgsSequencerInterface *ginterface);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sequencer = 0;

    ags_type_sequencer = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsSequencer",
						       sizeof(AgsSequencerInterface),
						       (GClassInitFunc) ags_sequencer_class_init,
						       0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sequencer);
  }

  return g_define_type_id__volatile;
}

GQuark
ags_sequencer_error_quark()
{
  return(g_quark_from_static_string("ags-sequencer-error-quark"));
}

void
ags_sequencer_class_init(AgsSequencerInterface *ginterface)
{
  /**
   * AgsSequencer::tic:
   * @sequencer: the #AgsSequencer object
   *
   * The ::tic signal is emitted every tic of the sequencer. This notifies
   * about a newly played buffer.
   * 
   * Since: 3.0.0
   */
  g_signal_new("tic",
	       G_TYPE_FROM_INTERFACE(ginterface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSequencerInterface, tic),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__VOID,
	       G_TYPE_NONE, 0);

  /**
   * AgsSequencer::offset-changed:
   * @sequencer: the #AgsSequencer object
   * @note_offset: new notation offset
   *
   * The ::offset-changed signal notifies about changed position within
   * notation.
   * 
   * Since: 3.0.0
   */
  g_signal_new("offset-changed",
	       G_TYPE_FROM_INTERFACE(ginterface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsSequencerInterface, offset_changed),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__UINT,
	       G_TYPE_NONE, 1,
	       G_TYPE_UINT);
}

/**
 * ags_sequencer_set_device:
 * @sequencer: the #AgsSequencer
 * @card_id: the device to set
 *
 * Set device.
 *
 * Since: 3.0.0
 */
void
ags_sequencer_set_device(AgsSequencer *sequencer,
			 gchar *card_id)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_device);
  sequencer_interface->set_device(sequencer,
				  card_id);
}

/**
 * ags_sequencer_get_device:
 * @sequencer: the #AgsSequencer
 *
 * Get device.
 *
 * Returns: the device's identifier
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @card_id: (element-type utf8) (out callee-allocates) (array zero-terminated=1) (transfer full): a list containing card ids
 * @card_name: (element-type utf8) (out callee-allocates) (array zero-terminated=1) (transfer full): a list containing card names
 *
 * Retrieve @card_id and @card_name as a list of strings.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Get starting.
 *
 * Returns: %TRUE if starting, else %FALSE
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Get playing.
 *
 * Returns: %TRUE if playing, else %FALSE
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Get recording.
 *
 * Returns: %TRUE if recording, else %FALSE
 *
 * Since: 3.0.0
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
 * ags_sequencer_play_init:
 * @sequencer: the #AgsSequencer
 * @error: an error that may occure
 *
 * Initializes the sequencer for playback.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @error: an error that may occure
 *
 * Plays the current buffer of sequencer.
 *
 * Since: 3.0.0
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
 * ags_sequencer_record_init:
 * @sequencer: the #AgsSequencer
 * @error: an error that may occure
 *
 * Initializes the sequencer for recording.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @error: an error that may occure
 *
 * Records the current buffer of sequencer.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Stops the sequencer from playing to it.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Every call to play may generate a tic. 
 *
 * Since: 3.0.0
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
 * ags_sequencer_offset_changed:
 * @sequencer: the #AgsSequencer
 * @note_offset: the note offset
 *
 * Callback when counter expires minor note offset.
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @buffer_length: the buffer's length
 *
 * Get current playback buffer. 
 *
 * Returns: current playback buffer
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @buffer_length: the buffer's length
 *
 * Get future playback buffer.
 *
 * Returns: next playback buffer
 *
 * Since: 3.0.0
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
 * ags_sequencer_lock_buffer:
 * @sequencer: the #AgsSequencer
 * @buffer: the buffer to lock
 *
 * Lock @buffer. 
 *
 * Since: 3.0.0
 */
void
ags_sequencer_lock_buffer(AgsSequencer *sequencer,
			  void *buffer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->lock_buffer);

  sequencer_interface->lock_buffer(sequencer,
				   buffer);
}

/**
 * ags_sequencer_unlock_buffer:
 * @sequencer: the #AgsSequencer
 * @buffer: the buffer to unlock
 *
 * Unlock @buffer. 
 *
 * Since: 3.0.0
 */
void
ags_sequencer_unlock_buffer(AgsSequencer *sequencer,
			    void *buffer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->unlock_buffer);

  sequencer_interface->unlock_buffer(sequencer,
				     buffer);
}

/**
 * ags_sequencer_set_bpm:
 * @sequencer: the #AgsSequencer
 * @bpm: the bpm to set
 *
 * Set current playback bpm. 
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Get current playback bpm. 
 *
 * Returns: bpm
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @delay_factor: the delay factor to set
 *
 * Set current playback delay factor. 
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 *
 * Get current playback delay factor. 
 *
 * Returns: delay factor
 *
 * Since: 3.0.0
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
 * ags_sequencer_get_start_note_offset:
 * @sequencer: the #AgsSequencer
 *
 * Get start playback note offset. 
 *
 * Returns: the start note offset
 *
 * Since: 3.0.0
 */
guint
ags_sequencer_get_start_note_offset(AgsSequencer *sequencer)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_val_if_fail(AGS_IS_SEQUENCER(sequencer), G_MAXUINT);
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_val_if_fail(sequencer_interface->get_start_note_offset, G_MAXUINT);

  return(sequencer_interface->get_start_note_offset(sequencer));
}

/**
 * ags_sequencer_set_start_note_offset:
 * @sequencer: the #AgsSequencer
 * @start_note_offset: the start note offset to set
 *
 * Set start playback note offset. 
 *
 * Since: 3.0.0
 */
void
ags_sequencer_set_start_note_offset(AgsSequencer *sequencer,
				    guint start_note_offset)
{
  AgsSequencerInterface *sequencer_interface;

  g_return_if_fail(AGS_IS_SEQUENCER(sequencer));
  sequencer_interface = AGS_SEQUENCER_GET_INTERFACE(sequencer);
  g_return_if_fail(sequencer_interface->set_start_note_offset);
  sequencer_interface->set_start_note_offset(sequencer,
					     start_note_offset);
}

/**
 * ags_sequencer_get_note_offset:
 * @sequencer: the #AgsSequencer
 *
 * Get current playback note offset. 
 *
 * Returns: offset
 *
 * Since: 3.0.0
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
 * @sequencer: the #AgsSequencer
 * @note_offset: the note offset to set
 *
 * Set current playback note offset. 
 *
 * Since: 3.0.0
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
