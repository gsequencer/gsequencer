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

#include <ags/audio/ags_message.h>

#include <ags/object/ags_marshal.h>

void ags_message_class_init(AgsMessageInterface *interface);

GType
ags_message_get_type()
{
  static GType ags_type_message = 0;

  if(!ags_type_message){
    ags_type_message = g_type_register_static_simple(G_TYPE_INTERFACE,
						     "AgsMessage\0",
						     sizeof(AgsMessageInterface),
						     (GClassInitFunc) ags_message_class_init,
						     0, NULL, 0);
  }

  return(ags_type_message);
}

void
ags_message_class_init(AgsMessageInterface *interface)
{
  /**
   * AgsMessage::key-on:
   * @message: the object
   * @note: the note
   * @audio_channel: the audio channel
   * @velocity: the velocity
   *
   * The ::key-on signal
   */
  g_signal_new("key-on\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, key_on),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__STRING_UINT_DOUBLE,
	       G_TYPE_NONE, 3,
	       G_TYPE_STRING,
	       G_TYPE_UINT,
	       G_TYPE_DOUBLE);

  /**
   * AgsMessage::key-off:
   * @message: the object
   * @note: the note
   * @audio_channel: the audio channel
   * @velocity: the velocity
   *
   * The ::key-off signal
   */
  g_signal_new("key-off\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, key_off),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__STRING_UINT_DOUBLE,
	       G_TYPE_NONE, 3,
	       G_TYPE_STRING,
	       G_TYPE_UINT,
	       G_TYPE_DOUBLE);

  /**
   * AgsMessage::key-pressure:
   * @message: the objectw
   * @note: the note
   * @audio_channel: the audio channel
   * @pressure: the pressure
   *
   * The ::key-pressure signal
   */
  g_signal_new("key-pressure\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, key_pressure),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__STRING_UINT_DOUBLE,
	       G_TYPE_NONE, 3,
	       G_TYPE_STRING,
	       G_TYPE_UINT,
	       G_TYPE_DOUBLE);

  /**
   * AgsMessage::change-parameter:
   * @message: the object
   * @note: the note
   * @audio_channel: the audio channel
   * @control_specifier: the control specifier
   * @value: the value
   *
   * The ::change-parameter signal
   */
  g_signal_new("change-parameter\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, change_parameter),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__STRING_UINT_STRING_DOUBLE,
	       G_TYPE_NONE, 4,
	       G_TYPE_STRING,
	       G_TYPE_UINT,
	       G_TYPE_STRING,
	       G_TYPE_DOUBLE);

  /**
   * AgsMessage::change-mode:
   * @message: the object
   * @audio_channel: the audio channel
   * @mode: the mode
   * @enabled: if %TRUE then enable
   *
   * The ::change-mode signal
   */
  g_signal_new("change-mode\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, change_mode),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__UINT_UINT_BOOLEAN,
	       G_TYPE_NONE, 3,
	       G_TYPE_UINT, G_TYPE_UINT,
	       G_TYPE_BOOLEAN);

  /**
   * AgsMessage::pitch-bend:
   * @message: the object
   * @pitch: the pitch
   * @transmitter: the transmitter
   *
   * The ::pitch-bend signal
   */
  g_signal_new("pitch-bend\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, pitch_bend),
	       NULL, NULL,
	       g_cclosure_user_marshal_VOID__DOUBLE_DOUBLE,
	       G_TYPE_NONE, 2,
	       G_TYPE_DOUBLE, G_TYPE_DOUBLE);

  /**
   * AgsMessage::set-bpm:
   * @message: the object
   * @bpm: the BPM
   * 
   * The ::set-bpm signal
   */
  g_signal_new("set-bpm\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, set_bpm),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);

  /**
   * AgsMessage::set-delay-factor:
   * @message: the object
   * @delay_factor: the delay factor
   *
   * The ::set-delay-factor signal
   */
  g_signal_new("set-delay-factor\0",
	       G_TYPE_FROM_INTERFACE(interface),
	       G_SIGNAL_RUN_LAST,
	       G_STRUCT_OFFSET(AgsMessageInterface, set_delay_factor),
	       NULL, NULL,
	       g_cclosure_marshal_VOID__DOUBLE,
	       G_TYPE_NONE, 1,
	       G_TYPE_DOUBLE);
}

/**
 * ags_message_key_on:
 * @message: the #AgsMessage
 * @note: the note
 * @audio_channel: the audio channel
 * @velocity: the velocity
 *
 * Notifies about key-on event.
 *
 * Since: 0.7.0
 */
void
ags_message_key_on(AgsMessage *message,
		   gchar *note,
		   guint audio_channel,
		   gdouble velocity)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->key_on);
  message_interface->key_on(message,
			    note,
			    audio_channel,
			    velocity);
}

/**
 * ags_message_key_on:
 * @message: the #AgsMessage
 * @note: the note
 * @audio_channel: the audio channel
 * @velocity: the velocity
 *
 * Notifies about key-off event.
 *
 * Since: 0.7.0
 */
void
ags_message_key_off(AgsMessage *message,
		    gchar *note,
		    guint audio_channel,
		    gdouble velocity)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->key_off);
  message_interface->key_off(message,
			     note,
			     audio_channel,
			     velocity);
}

/**
 * ags_message_key_pressure:
 * @message: the #AgsMessage
 * @note: the note
 * @audio_channel: the audio channel
 * @pressure: the pressure
 *
 * Notifies about key pressure event.
 *
 * Since: 0.7.0
 */
void
ags_message_key_pressure(AgsMessage *message,
			 gchar *note,
			 guint audio_channel,
			 gdouble pressure)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->key_pressure);
  message_interface->key_pressure(message,
				  note,
				  audio_channel,
				  pressure);
}

/**
 * ags_message_change_parameter:
 * @message: the #AgsMessage
 * @note: the note
 * @audio_channel: the audio channel
 * @control_specifier: the control specifier
 * @value: the value
 *
 * Notifies about changed parameter.
 *
 * Since: 0.7.0
 */
void
ags_message_change_parameter(AgsMessage *message,
			     gchar *note,
			     guint audio_channel,
			     gchar *control_specifier,
			     gdouble value)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->change_parameter);
  message_interface->change_parameter(message,
				      note,
				      audio_channel,
				      control_specifier,
				      value);
}

/**
 * ags_message_change_mode:
 * @message: the #AgsMessage
 * @audio_channel: the audio channel
 * @mode: the mode
 * @enabled: %TRUE if enabled, otherwise %FALSE
 *
 * Notifies about changed mode.
 *
 * Since: 0.7.0
 */
void
ags_message_change_mode(AgsMessage *message,
			guint audio_channel,
			guint mode,
			gboolean enabled)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->change_mode);
  message_interface->change_mode(message,
				 audio_channel,
				 mode,
				 enabled);
}

/**
 * ags_message_pitch_bend:
 * @message: the #AgsMessage
 * @pitch: the pitch
 * @transmitter: the transmitter
 *
 * Notifies about pitch bend.
 *
 * Since: 0.7.0
 */
void
ags_message_pitch_bend(AgsMessage *message,
		       gdouble pitch,
		       gdouble transmitter)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->pitch_bend);
  message_interface->pitch_bend(message,
				pitch,
				transmitter);
}

/**
 * ags_message_set_bpm:
 * @message: the #AgsMessage 
 * @bpm: the BPM
 *
 * Notifies about modified bpm.
 *
 * Since: 0.7.0
 */
void
ags_message_set_bpm(AgsMessage *message,
		    gdouble bpm)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->set_bpm);
  message_interface->set_bpm(message,
			     bpm);
}

/**
 * ags_message_set_delay_factor:
 * @message: the #AgsMessage 
 * @delay_factor: the delay factor
 *
 * Notifies about modified bpm.
 *
 * Since: 0.7.0
 */
void
ags_message_set_delay_factor(AgsMessage *message,
			     gdouble delay_factor)
{
  AgsMessageInterface *message_interface;

  g_return_if_fail(AGS_IS_MESSAGE(message));
  message_interface = AGS_MESSAGE_GET_INTERFACE(message);
  g_return_if_fail(message_interface->set_delay_factor);
  message_interface->set_delay_factor(message,
				      delay_factor);
}
