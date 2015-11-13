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

#ifndef __AGS_MESSAGE_H__
#define __AGS_MESSAGE_H__

#include <glib-object.h>

#define AGS_TYPE_MESSAGE                    (ags_message_get_type())
#define AGS_MESSAGE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MESSAGE, AgsMessage))
#define AGS_MESSAGE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_MESSAGE, AgsMessageInterface))
#define AGS_IS_MESSAGE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MESSAGE))
#define AGS_IS_MESSAGE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_MESSAGE))
#define AGS_MESSAGE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_MESSAGE, AgsMessageInterface))

typedef void AgsMessage;
typedef struct _AgsMessageInterface AgsMessageInterface;

typedef enum{
  AGS_MESSAGE_ALL_OFF    = 1,
  AGS_MESSAGE_RESET_ALL  = 1 <<  1,
  AGS_MESSAGE_OMNI_MODE  = 1 <<  2,
  AGS_MESSAGE_MONO_MODE  = 1 <<  3,
  AGS_MESSAGE_POLY_MODE  = 1 <<  4,
}AgsMessageMode;

struct _AgsMessageInterface
{
  GTypeInterface interface;

  void (*key_on)(AgsMessage *message,
		 gchar *note,
		 guint audio_channel,
		 gdouble velocity);
  void (*key_off)(AgsMessage *message,
		  gchar *note,
		  guint audio_channel,
		  gdouble velocity);
  void (*key_pressure)(AgsMessage *message,
		       gchar *note,
		       guint audio_channel,
		       gdouble pressure);

  void (*change_parameter)(AgsMessage *message,
			   gchar *note,
			   guint audio_channel,
			   gchar *control_specifier,
			   gdouble value);

  void (*change_mode)(AgsMessage *message,
		      guint audio_channel,
		      guint mode,
		      gboolean enabled);

  void (*pitch_bend)(AgsMessage *message,
		     gdouble pitch,
		     gdouble transmitter);

  void (*set_bpm)(AgsMessage *message,
		  gdouble bpm);

  void (*set_delay_factor)(AgsMessage *message,
			   gdouble delay_factor);
};

void ags_message_key_on(AgsMessage *message,
			gchar *note,
			guint audio_channel,
			gdouble velocity);
void ags_message_key_off(AgsMessage *message,
			 gchar *note,
			 guint audio_channel,
			 gdouble velocity);
void ags_message_key_pressure(AgsMessage *message,
			      gchar *note,
			      guint audio_channel,
			      gdouble pressure);

void ags_message_change_parameter(AgsMessage *message,
				  gchar *note,
				  guint audio_channel,
				  gchar *control_specifier,
				  gdouble value);

void ags_message_change_mode(AgsMessage *message,
			     guint audio_channel,
			     guint mode,
			     gboolean enabled);


void ags_message_pitch_bend(AgsMessage *message,
			    gdouble pitch,
			    gdouble transmitter);

void ags_message_set_bpm(AgsMessage *message,
			 gdouble bpm);

void ags_message_set_delay_factor(AgsMessage *message,
				  gdouble delay_factor);

#endif /*__AGS_MESSAGE_H__*/
