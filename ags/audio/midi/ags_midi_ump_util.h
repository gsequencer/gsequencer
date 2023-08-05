/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_MIDI_UMP_UTIL_H__
#define __AGS_MIDI_UMP_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_UMP_UTIL         (ags_midi_ump_util_get_type())

typedef struct _AgsMidiUmpUtil AgsMidiUmpUtil;
typedef guint32 AgsUmpWord;

typedef enum{
  AGS_MIDI_UMP_STREAM_MESSAGE_COMPLETE  = 0x0,
  AGS_MIDI_UMP_STREAM_MESSAGE_START     = 0x1,
  AGS_MIDI_UMP_STREAM_MESSAGE_CONTINUE  = 0x2,
  AGS_MIDI_UMP_STREAM_MESSAGE_END       = 0x3,
}AgsMidiUmpStreamMessageFormat;

typedef enum{
  AGS_MIDI_UMP_ENDPOINT_DISCOVERY_ENDPOINT_INFO         = 1,
  AGS_MIDI_UMP_ENDPOINT_DISCOVERY_DEVICE_IDENTITY       = 1 <<  1,
  AGS_MIDI_UMP_ENDPOINT_DISCOVERY_ENDPOINT_NAME         = 1 <<  2,
  AGS_MIDI_UMP_ENDPOINT_DISCOVERY_PRODUCT_INSTANCE_ID   = 1 <<  3,
  AGS_MIDI_UMP_ENDPOINT_DISCOVERY_STREAM_CONFIGURATION  = 1 <<  4,
  /* reserved = 1 <<  5, */
  /* reserved = 1 <<  6, */
  /* reserved = 1 <<  7, */
}AgsMidiUmpEndpointDiscoveryFilterBitmap;

struct _AgsMidiUmpUtil
{
  guint major;
  guint minor;
};

GType ags_midi_ump_util_get_type(void);

AgsMidiUmpUtil* ags_midi_ump_util_alloc();
void ags_midi_ump_util_free(AgsMidiUmpUtil *midi_ump_util);

AgsMidiUmpUtil* ags_midi_ump_util_copy(AgsMidiUmpUtil *midi_ump_util);

gboolean ags_midi_ump_util_is_system_common(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer);
void ags_midi_ump_util_put_system_common(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint group,
					 gint status,
					 guchar data[4]);
guint ags_midi_ump_util_get_system_common(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *status,
					  guchar data[4]);

gboolean ags_midi_ump_util_is_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer);
void ags_midi_ump_util_put_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer);
guint ags_midi_ump_util_get_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer);

gboolean ags_midi_ump_util_is_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer);
void ags_midi_ump_util_put_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint group,
						   gint status,
						   guchar data[16]);
guint ags_midi_ump_util_get_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
						    guchar *buffer,
						    gint *group,
						    gint *status,
						    guchar data[16]);

gboolean ags_midi_ump_util_is_data_message(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer);
void ags_midi_ump_util_put_data_message(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer,
					gint group,
					gint status,
					guchar data[16]);
guint ags_midi_ump_util_get_data_message(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint *group,
					 gint *status,
					 guchar data[16]);

gboolean ags_midi_ump_util_is_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer);
void ags_midi_ump_util_put_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint group,
					       gint status,
					       gint index);
guint ags_midi_ump_util_get_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint *group,
						gint *status,
						gint *index);

gboolean ags_midi_ump_util_is_stream_message(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer);
void ags_midi_ump_util_put_stream_message(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint format,
					  gint status,
					  guchar data[16]);
guint ags_midi_ump_util_get_stream_message(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *format,
					   gint *status,
					   guchar data[16]);

/* endpoint discovery */
gboolean ags_midi_ump_util_is_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer);
void ags_midi_ump_util_put_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint major,
					      gint minor,
					      gint filter,
					      guchar data[16]);
guint ags_midi_ump_util_get_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint *major,
					       gint *minor,
					       gint *filter,
					       guchar data[16]);

/* endpoint info notification */
gboolean ags_midi_ump_util_is_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer);
void ags_midi_ump_util_put_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint major,
						      gint minor,
						      gboolean static_function_blocks,
						      gint function_block_count,
						      gboolean midi_v2_0_support,
						      gboolean midi_v1_0_support,
						      gboolean rx_jitter_reduction,
						      gboolean tx_jitter_reduction,
						      guchar data[16]);
guint ags_midi_ump_util_get_endpoint_info_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint *major,
						       gint *minor,
						       gboolean *static_function_blocks,
						       gint *function_block_count,
						       gboolean *midi_v2_0_support,
						       gboolean *midi_v1_0_support,
						       gboolean *rx_jitter_reduction,
						       gboolean *tx_jitter_reduction,
						       guchar data[16]);

/* device identity notification */
gboolean ags_midi_ump_util_is_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer);
void ags_midi_ump_util_put_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint device_manufacturer,
							gint device_family,
							gint device_family_model,
							gint software_revision,
							guchar data[16]);
guint ags_midi_ump_util_get_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 gint *device_manufacturer,
							 gint *device_family,
							 gint *device_family_model,
							 gint *software_revision,
							 guchar data[16]);

G_END_DECLS

#endif /*__AGS_MIDI_UMP_UTIL_H__*/
