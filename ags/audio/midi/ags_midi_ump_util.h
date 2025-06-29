/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

typedef enum{
  AGS_MIDI_V1_0 = 0x1,
  AGS_MIDI_V2_0 = 0x2,
}AgsMidiProtocol;

typedef enum{
  AGS_MIDI_UMP_FUNCTION_BLOCK_DISCOVERY_INFO_NOTIFICATION         = 1,
  AGS_MIDI_UMP_FUNCTION_BLOCK_DISCOVERY_NAME_NOTIFICATION         = 1 << 1,
  /* reserved = 1 <<  2, */
  /* reserved = 1 <<  3, */
  /* reserved = 1 <<  4, */
  /* reserved = 1 <<  5, */
  /* reserved = 1 <<  6, */
  /* reserved = 1 <<  7, */
}AgsMidiUmpFunctionBlockDiscoveryFilterBitmap;

typedef enum{
  /* reserved = 0x00, */
  AGS_MIDI_INPUT_RX_ONLY                   = 0x01,
  AGS_MIDI_OUTPUT_TX_ONLY                  = 0x02,
  AGS_MIDI_BIDIRECTIONAL_MATCHING_GROUPS   = 0x03,
}AgsMidiDirection;

typedef enum{
  AGS_MIDI_ATTRIBUTE_NO_ATTRIBUTE_DATA     = 0x00,
  AGS_MIDI_ATTRIBUTE_MANUFACTURER_SPECIFIC = 0x01,
  AGS_MIDI_ATTRIBUTE_PROFILE_SPECIFIC      = 0x02,
  AGS_MIDI_ATTRIBUTE_PITCH_7_9             = 0x03,
}AgsMidiAttributeType;

typedef enum{
  AGS_MIDI1_PORT_NOT_MIDI_1_0                  = 0x00,
  AGS_MIDI1_PORT_YES_DONT_RESTRICT_BANDWIDTH   = 0x01,
  AGS_MIDI1_PORT_YES_RESTRICT_BANDWIDTH        = 0x02,
  /* reserved = 0x03, */
}AgsMidi1PortMode;

typedef enum{
  AGS_MIDI2_DETACH_PER_NOTE_CONTROLLERS          = 0x01,
  AGS_MIDI2_RESET_PER_NOTE_CONTROLLERS           = 0x02,
}AgsMidi2PerNoteManagementOptionsFlags;

typedef enum{
  AGS_MIDI2_CC_84_PORTAMENTO          = 0x54,
  AGS_MIDI2_CC_126_OMNI_OFF           = 0x7E,
}AgsMidi2CCIndex;

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

gboolean ags_midi_ump_util_is_data_message_with_sysex(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer);

gboolean ags_midi_ump_util_is_data_message(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer);

gboolean ags_midi_ump_util_is_stream_message(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer);

/* endpoint discovery */
gboolean ags_midi_ump_util_is_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer);
void ags_midi_ump_util_put_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint major,
					      gint minor,
					      gint filter,
					      gchar **extension_name, GValue *extension_value,
					      guint extension_count);
guint ags_midi_ump_util_get_endpoint_discovery(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint *major,
					       gint *minor,
					       gint *filter,
					       gchar ***extension_name, GValue **extension_value,
					       guint *extension_count);

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
						      gchar **extension_name, GValue *extension_value,
						      guint extension_count);
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
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count);

/* device identity notification */
gboolean ags_midi_ump_util_is_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer);
void ags_midi_ump_util_put_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint device_manufacturer,
							gint device_family,
							gint device_family_model,
							gint software_revision,
							gchar **extension_name, GValue *extension_value,
							guint extension_count);
guint ags_midi_ump_util_get_device_identity_notification(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 gint *device_manufacturer,
							 gint *device_family,
							 gint *device_family_model,
							 gint *software_revision,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count);

/* endpoint name notification */
gboolean ags_midi_ump_util_is_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer);
void ags_midi_ump_util_put_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gchar *endpoint_name,
						      gchar **extension_name, GValue *extension_value,
						      guint extension_count);
guint ags_midi_ump_util_get_endpoint_name_notification(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gchar **endpoint_name,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count);

/* product instance id notification */
gboolean ags_midi_ump_util_is_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
							       guchar *buffer);
void ags_midi_ump_util_put_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
							    guchar *buffer,
							    gchar *product_instance_id,
							    gchar **extension_name, GValue *extension_value,
							    guint extension_count);
guint ags_midi_ump_util_get_product_instance_id_notification(AgsMidiUmpUtil *midi_ump_util,
							     guchar *buffer,
							     gchar **product_instance_id,
							     gchar ***extension_name, GValue **extension_value,
							     guint *extension_count);

/* stream configuration request */
gboolean ags_midi_ump_util_is_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer);
void ags_midi_ump_util_put_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint protocol,
							gboolean rx_jitter_reduction, gboolean tx_jitter_reduction,
							gchar **extension_name, GValue *extension_value,
							guint extension_count);
guint ags_midi_ump_util_get_stream_configuration_request(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 gint *protocol,
							 gboolean *rx_jitter_reduction, gboolean *tx_jitter_reduction,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count);

/* stream configuration notification */
gboolean ags_midi_ump_util_is_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
								guchar *buffer);
void ags_midi_ump_util_put_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
							     guchar *buffer,
							     gint protocol,
							     gboolean rx_jitter_reduction, gboolean tx_jitter_reduction,
							     gchar **extension_name, GValue *extension_value,
							     guint extension_count);
guint ags_midi_ump_util_get_stream_configuration_notification(AgsMidiUmpUtil *midi_ump_util,
							      guchar *buffer,
							      gint *protocol,
							      gboolean *rx_jitter_reduction, gboolean *tx_jitter_reduction,
							      gchar ***extension_name, GValue **extension_value,
							      guint *extension_count);

/* function block discovery */
gboolean ags_midi_ump_util_is_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer);
void ags_midi_ump_util_put_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
						    guchar *buffer,
						    gint function_block,
						    gint filter,
						    gchar **extension_name, GValue *extension_value,
						    guint extension_count);
guint ags_midi_ump_util_get_function_block_discovery(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer,
						     gint *function_block,
						     gint *filter,
						     gchar ***extension_name, GValue **extension_value,
						     guint *extension_count);


/* function block info notification */
gboolean ags_midi_ump_util_is_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
							       guchar *buffer);
void ags_midi_ump_util_put_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
							    guchar *buffer,
							    gboolean function_block_active,
							    gint function_block,
							    gint direction,
							    gint midi1_port,
							    gint ui_hint,
							    gint first_group,
							    gint group_count,
							    gint message_version,
							    gint max_sysex8_stream_count,
							    gchar **extension_name, GValue *extension_value,
							    guint extension_count);
guint ags_midi_ump_util_get_function_block_info_notification(AgsMidiUmpUtil *midi_ump_util,
							     guchar *buffer,
							     gboolean *function_block_active,
							     gint *function_block,
							     gint *direction,
							     gint *midi1_port,
							     gint *ui_hint,
							     gint *first_group,
							     gint *group_count,
							     gint *message_version,
							     gint *max_sysex8_stream_count,
							     gchar ***extension_name, GValue **extension_value,
							     guint *extension_count);

/* function block name notification */
gboolean ags_midi_ump_util_is_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
							       guchar *buffer);
void ags_midi_ump_util_put_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
							    guchar *buffer,
							    gint function_block,
							    gchar *function_block_name,
							    gchar **extension_name, GValue *extension_value,
							    guint extension_count);
guint ags_midi_ump_util_get_function_block_name_notification(AgsMidiUmpUtil *midi_ump_util,
							     guchar *buffer,
							     gint *function_block,
							     gchar **function_block_name,
							     gchar ***extension_name, GValue **extension_value,
							     guint *extension_count);

/* start of clip */
gboolean ags_midi_ump_util_is_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer);
void ags_midi_ump_util_put_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count);
guint ags_midi_ump_util_get_start_of_clip(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count);

/* end of clip */
gboolean ags_midi_ump_util_is_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer);
void ags_midi_ump_util_put_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer,
				       gchar **extension_name, GValue *extension_value,
				       guint extension_count);
guint ags_midi_ump_util_get_end_of_clip(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer,
					gchar ***extension_name, GValue **extension_value,
					guint *extension_count);

/* noop */
gboolean ags_midi_ump_util_is_noop(AgsMidiUmpUtil *midi_ump_util,
				   guchar *buffer);
void ags_midi_ump_util_put_noop(AgsMidiUmpUtil *midi_ump_util,
				guchar *buffer,
				gchar **extension_name, GValue *extension_value,
				guint extension_count);
guint ags_midi_ump_util_get_noop(AgsMidiUmpUtil *midi_ump_util,
				 guchar *buffer,
				 gchar ***extension_name, GValue **extension_value,
				 guint *extension_count);

/* JR clock */
gboolean ags_midi_ump_util_is_jr_clock(AgsMidiUmpUtil *midi_ump_util,
				       guchar *buffer);
void ags_midi_ump_util_put_jr_clock(AgsMidiUmpUtil *midi_ump_util,
				    guchar *buffer,
				    guint16 sender_clock_time,
				    gchar **extension_name, GValue *extension_value,
				    guint extension_count);
guint ags_midi_ump_util_get_jr_clock(AgsMidiUmpUtil *midi_ump_util,
				     guchar *buffer,
				     guint16 *sender_clock_time,
				     gchar ***extension_name, GValue **extension_value,
				     guint *extension_count);

/* JR timestamp */
gboolean ags_midi_ump_util_is_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer);
void ags_midi_ump_util_put_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
					guchar *buffer,
					guint16 sender_clock_timestamp,
					gchar **extension_name, GValue *extension_value,
					guint extension_count);
guint ags_midi_ump_util_get_jr_timestamp(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 guint16 *sender_clock_timestamp,
					 gchar ***extension_name, GValue **extension_value,
					 guint *extension_count);

/* delta clockstamp ticks per quarter note */
gboolean ags_midi_ump_util_is_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
								 guchar *buffer);
void ags_midi_ump_util_put_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							      guchar *buffer,
							      guint16 ticks_per_quarter_note_count,
							      gchar **extension_name, GValue *extension_value,
							      guint extension_count);
guint ags_midi_ump_util_get_delta_clock_ticks_per_quarter_note(AgsMidiUmpUtil *midi_ump_util,
							       guchar *buffer,
							       guint16 *ticks_per_quarter_note_count,
							       gchar ***extension_name, GValue **extension_value,
							       guint *extension_count);

/* delta clockstamp ticks since last event */
gboolean ags_midi_ump_util_is_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
								 guchar *buffer);
void ags_midi_ump_util_put_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							      guchar *buffer,
							      guint16 ticks_since_last_event_count,
							      gchar **extension_name, GValue *extension_value,
							      guint extension_count);
guint ags_midi_ump_util_get_delta_clock_ticks_since_last_event(AgsMidiUmpUtil *midi_ump_util,
							       guchar *buffer,
							       guint16 *ticks_since_last_event_count,
							       gchar ***extension_name, GValue **extension_value,
							       guint *extension_count);

/* MIDI v1.0 channel voice */
gboolean ags_midi_ump_util_is_midi1_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer);

/* MIDI v1.0 note off */
gboolean ags_midi_ump_util_is_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer);
void ags_midi_ump_util_put_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint channel,
					  gint key,
					  gint velocity,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count);
guint ags_midi_ump_util_get_midi1_note_off(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *key,
					   gint *velocity,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count);

/* MIDI v1.0 note on */
gboolean ags_midi_ump_util_is_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer);
void ags_midi_ump_util_put_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint group,
					 gint channel,
					 gint key,
					 gint velocity,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count);
guint ags_midi_ump_util_get_midi1_note_on(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *channel,
					  gint *key,
					  gint *velocity,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count);

/* MIDI v1.0 polyphonic aftertouch */
gboolean ags_midi_ump_util_is_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
							  guchar *buffer);
void ags_midi_ump_util_put_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint group,
						       gint channel,
						       gint key,
						       gint data,
						       gchar **extension_name, GValue *extension_value,
						       guint extension_count);
guint ags_midi_ump_util_get_midi1_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint *group,
							gint *channel,
							gint *key,
							gint *data,
							gchar ***extension_name, GValue **extension_value,
							guint *extension_count);

/* MIDI v1.0 control change */
gboolean ags_midi_ump_util_is_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer);
void ags_midi_ump_util_put_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint index_key,
						gint data,
						gchar **extension_name, GValue *extension_value,
						guint extension_count);
guint ags_midi_ump_util_get_midi1_control_change(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *index_key,
						 gint *data,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count);

/* MIDI v1.0 program change */
gboolean ags_midi_ump_util_is_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer);
void ags_midi_ump_util_put_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint program,
						gint data,
						gchar **extension_name, GValue *extension_value,
						guint extension_count);
guint ags_midi_ump_util_get_midi1_program_change(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *program,
						 gint *data,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count);

/* MIDI v1.0 channel pressure */
gboolean ags_midi_ump_util_is_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer);
void ags_midi_ump_util_put_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint group,
						  gint channel,
						  gint pressure,
						  gint data,
						  gchar **extension_name, GValue *extension_value,
						  guint extension_count);
guint ags_midi_ump_util_get_midi1_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *group,
						   gint *channel,
						   gint *pressure,
						   gint *data,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count);

/* MIDI v1.0 pitch bend */
gboolean ags_midi_ump_util_is_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer);
void ags_midi_ump_util_put_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer,
					    gint group,
					    gint channel,
					    gint data,
					    gchar **extension_name, GValue *extension_value,
					    guint extension_count);
guint ags_midi_ump_util_get_midi1_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint *group,
					     gint *channel,
					     gint *data,
					     gchar ***extension_name, GValue **extension_value,
					     guint *extension_count);

/* MIDI v2.0 channel voice */
gboolean ags_midi_ump_util_is_midi2_channel_voice(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer);

/* MIDI v2.0 note off */
gboolean ags_midi_ump_util_is_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer);
void ags_midi_ump_util_put_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint channel,
					  gint key,
					  gint attribute_type,
					  gint velocity,
					  gint attribute,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count);
guint ags_midi_ump_util_get_midi2_note_off(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *key,
					   gint *attribute_type,
					   gint *velocity,
					   gint *attribute,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count);

/* MIDI v2.0 note on */
gboolean ags_midi_ump_util_is_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer);
void ags_midi_ump_util_put_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
					 guchar *buffer,
					 gint group,
					 gint channel,
					 gint key,
					 gint attribute_type,
					 gint velocity,
					 gint attribute,
					 gchar **extension_name, GValue *extension_value,
					 guint extension_count);
guint ags_midi_ump_util_get_midi2_note_on(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint *group,
					  gint *channel,
					  gint *key,
					  gint *attribute_type,
					  gint *velocity,
					  gint *attribute,
					  gchar ***extension_name, GValue **extension_value,
					  guint *extension_count);

/* MIDI v2.0 polyphonic aftertouch */
gboolean ags_midi_ump_util_is_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
							  guchar *buffer);
void ags_midi_ump_util_put_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint group,
						       gint channel,
						       gint key,
						       gint data,
						       gchar **extension_name, GValue *extension_value,
						       guint extension_count);
guint ags_midi_ump_util_get_midi2_polyphonic_aftertouch(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint *group,
							gint *channel,
							gint *key,
							gint *data,
							gchar ***extension_name, GValue **extension_value,
							guint *extension_count);


/* MIDI v2.0 registered per note controller */
gboolean ags_midi_ump_util_is_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								   guchar *buffer);
void ags_midi_ump_util_put_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								guchar *buffer,
								gint group,
								gint channel,
								gint key,
								gint data_index,
								gint data,
								gchar **extension_name, GValue *extension_value,
								guint extension_count);
guint ags_midi_ump_util_get_midi2_registered_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								 guchar *buffer,
								 gint *group,
								 gint *channel,
								 gint *key,
								 gint *data_index,
								 gint *data,
								 gchar ***extension_name, GValue **extension_value,
								 guint *extension_count);

/* MIDI v2.0 assignable per note controller */
gboolean ags_midi_ump_util_is_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								   guchar *buffer);
void ags_midi_ump_util_put_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								guchar *buffer,
								gint group,
								gint channel,
								gint key,
								gint data_index,
								gint data,
								gchar **extension_name, GValue *extension_value,
								guint extension_count);
guint ags_midi_ump_util_get_midi2_assignable_per_note_controller(AgsMidiUmpUtil *midi_ump_util,
								 guchar *buffer,
								 gint *group,
								 gint *channel,
								 gint *key,
								 gint *data_index,
								 gint *data,
								 gchar ***extension_name, GValue **extension_value,
								 guint *extension_count);

/* MIDI v2.0 per note management */
gboolean ags_midi_ump_util_is_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer);
void ags_midi_ump_util_put_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer,
						     gint group,
						     gint channel,
						     gint key,
						     gint options_flags,
						     gchar **extension_name, GValue *extension_value,
						     guint extension_count);
guint ags_midi_ump_util_get_midi2_per_note_management(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint *group,
						      gint *channel,
						      gint *key,
						      gint *options_flags,
						      gchar ***extension_name, GValue **extension_value,
						      guint *extension_count);

/* MIDI v2.0 control change */
gboolean ags_midi_ump_util_is_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer);
void ags_midi_ump_util_put_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint index_key,
						AgsUmpWord data,
						gchar **extension_name, GValue *extension_value,
						guint extension_count);
guint ags_midi_ump_util_get_midi2_control_change(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *index_key,
						 AgsUmpWord *data,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count);

/* MIDI v2.0 rpn pitch bend range */
gboolean ags_midi_ump_util_is_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer);
void ags_midi_ump_util_put_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint group,
						      gint channel,
						      gint semitones,
						      gint cents,
						      gchar **extension_name, GValue *extension_value,
						      guint extension_count);
guint ags_midi_ump_util_get_midi2_rpn_pitch_bend_range(AgsMidiUmpUtil *midi_ump_util,
						       guchar *buffer,
						       gint *group,
						       gint *channel,
						       gint *semitones,
						       gint *cents,
						       gchar ***extension_name, GValue **extension_value,
						       guint *extension_count);

/* MIDI v2.0 rpn coarse tuning */
gboolean ags_midi_ump_util_is_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer);
void ags_midi_ump_util_put_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint group,
						   gint channel,
						   gint coarse_tuning,
						   gchar **extension_name, GValue *extension_value,
						   guint extension_count);
guint ags_midi_ump_util_get_midi2_rpn_coarse_tuning(AgsMidiUmpUtil *midi_ump_util,
						    guchar *buffer,
						    gint *group,
						    gint *channel,
						    gint *coarse_tuning,
						    gchar ***extension_name, GValue **extension_value,
						    guint *extension_count);

/* MIDI v2.0 rpn tuning program change */
gboolean ags_midi_ump_util_is_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
							      guchar *buffer);
void ags_midi_ump_util_put_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer,
							   gint group,
							   gint channel,
							   gint tuning_program_number,
							   gchar **extension_name, GValue *extension_value,
							   guint extension_count);
guint ags_midi_ump_util_get_midi2_rpn_tuning_program_change(AgsMidiUmpUtil *midi_ump_util,
							    guchar *buffer,
							    gint *group,
							    gint *channel,
							    gint *tuning_program_number,
							    gchar ***extension_name, GValue **extension_value,
							    guint *extension_count);

/* MIDI v2.0 rpn tuning bank select */
gboolean ags_midi_ump_util_is_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
							   guchar *buffer);
void ags_midi_ump_util_put_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer,
							gint group,
							gint channel,
							gint tuning_bank_number,
							gchar **extension_name, GValue *extension_value,
							guint extension_count);
guint ags_midi_ump_util_get_midi2_rpn_tuning_bank_select(AgsMidiUmpUtil *midi_ump_util,
							 guchar *buffer,
							 gint *group,
							 gint *channel,
							 gint *tuning_bank_number,
							 gchar ***extension_name, GValue **extension_value,
							 guint *extension_count);

/* MIDI v2.0 rpn mpe mcm */
gboolean ags_midi_ump_util_is_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer);
void ags_midi_ump_util_put_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint group,
					     gint channel,
					     gint channel_count,
					     gchar **extension_name, GValue *extension_value,
					     guint extension_count);
guint ags_midi_ump_util_get_midi2_rpn_mpe_mcm(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint *group,
					      gint *channel,
					      gint *channel_count,
					      gchar ***extension_name, GValue **extension_value,
					      guint *extension_count);

/* MIDI v2.0 program change */
gboolean ags_midi_ump_util_is_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer);
void ags_midi_ump_util_put_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint group,
						gint channel,
						gint option_flags,
						gint program,
						gint bank,
						gchar **extension_name, GValue *extension_value,
						guint extension_count);
guint ags_midi_ump_util_get_midi2_program_change(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer,
						 gint *group,
						 gint *channel,
						 gint *option_flags,
						 gint *program,
						 gint *bank,
						 gchar ***extension_name, GValue **extension_value,
						 guint *extension_count);

/* MIDI v2.0 channel pressure */
gboolean ags_midi_ump_util_is_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer);
void ags_midi_ump_util_put_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint group,
						  gint channel,
						  gint data,
						  gchar **extension_name, GValue *extension_value,
						  guint extension_count);
guint ags_midi_ump_util_get_midi2_channel_pressure(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *group,
						   gint *channel,
						   gint *data,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count);

/* MIDI v2.0 pitch bend */
gboolean ags_midi_ump_util_is_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer);
void ags_midi_ump_util_put_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer,
					    gint group,
					    gint channel,
					    gint data,
					    gchar **extension_name, GValue *extension_value,
					    guint extension_count);
guint ags_midi_ump_util_get_midi2_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer,
					     gint *group,
					     gint *channel,
					     gint *data,
					     gchar ***extension_name, GValue **extension_value,
					     guint *extension_count);

/* MIDI v2.0 per note pitch bend */
gboolean ags_midi_ump_util_is_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
							guchar *buffer);
void ags_midi_ump_util_put_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer,
						     gint group,
						     gint channel,
						     gint key,
						     gint data,
						     gchar **extension_name, GValue *extension_value,
						     guint extension_count);
guint ags_midi_ump_util_get_midi2_per_note_pitch_bend(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer,
						      gint *group,
						      gint *channel,
						      gint *key,
						      gint *data,
						      gchar ***extension_name, GValue **extension_value,
						      guint *extension_count);

/* MIDI v2.0 flex set tempo */
gboolean ags_midi_ump_util_is_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
					     guchar *buffer);
void ags_midi_ump_util_put_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
					  guchar *buffer,
					  gint group,
					  gint channel,
					  gint ten_ns_per_quarter_note,
					  gchar **extension_name, GValue *extension_value,
					  guint extension_count);
guint ags_midi_ump_util_get_flex_set_tempo(AgsMidiUmpUtil *midi_ump_util,
					   guchar *buffer,
					   gint *group,
					   gint *channel,
					   gint *ten_ns_per_quarter_note,
					   gchar ***extension_name, GValue **extension_value,
					   guint *extension_count);

/* MIDI v2.0 flex set time signature */
gboolean ags_midi_ump_util_is_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
						      guchar *buffer);
void ags_midi_ump_util_put_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint group,
						   gint channel,
						   gint numerator,
						   gint denominator,
						   gint thirty_two_ticks,
						   gchar **extension_name, GValue *extension_value,
						   guint extension_count);
guint ags_midi_ump_util_get_flex_set_time_signature(AgsMidiUmpUtil *midi_ump_util,
						    guchar *buffer,
						    gint *group,
						    gint *channel,
						    gint *numerator,
						    gint *denominator,
						    gint *thirty_two_ticks,
						    gchar ***extension_name, GValue **extension_value,
						    guint *extension_count);

/* MIDI v2.0 flex set metronome */
gboolean ags_midi_ump_util_is_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
						 guchar *buffer);
void ags_midi_ump_util_put_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
					      guchar *buffer,
					      gint group,
					      gint channel,
					      gint clocks_per_primary_click,
					      gint bar_accent_part_1,
					      gint bar_accent_part_2,
					      gint bar_accent_part_3,
					      gint subdivision_clicks_1,
					      gint subdivision_clicks_2,
					      gchar **extension_name, GValue *extension_value,
					      guint extension_count);
guint ags_midi_ump_util_get_flex_set_metronome(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint *group,
					       gint *channel,
					       gint *clocks_per_primary_click,
					       gint *bar_accent_part_1,
					       gint *bar_accent_part_2,
					       gint *bar_accent_part_3,
					       gint *subdivision_clicks_1,
					       gint *subdivision_clicks_2,
					       gchar ***extension_name, GValue **extension_value,
					       guint *extension_count);

/* MIDI v2.0 flex set key signature */
gboolean ags_midi_ump_util_is_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
						     guchar *buffer);
void ags_midi_ump_util_put_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer,
						  gint group,
						  gint channel,
						  gint sharp_flats,
						  gint tonic_note,
						  gchar **extension_name, GValue *extension_value,
						  guint extension_count);
guint ags_midi_ump_util_get_flex_set_key_signature(AgsMidiUmpUtil *midi_ump_util,
						   guchar *buffer,
						   gint *group,
						   gint *channel,
						   gint *sharp_flats,
						   gint *tonic_note,
						   gchar ***extension_name, GValue **extension_value,
						   guint *extension_count);

/* MIDI v2.0 flex set chord name */
gboolean ags_midi_ump_util_is_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
						  guchar *buffer);
void ags_midi_ump_util_put_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
					       guchar *buffer,
					       gint group,
					       gint channel,
					       gint t_sharp_flats,
					       gint chord_tonic,
					       gint chord_type,
					       gint alter_1_type,
					       gint alter_1_degree,
					       gint alter_2_type,
					       gint alter_2_degree,
					       gint alter_3_type,
					       gint alter_3_degree,
					       gint alter_4_type,
					       gint alter_4_degree,
					       gint b_sharp_flats,
					       gint bass_note,
					       gint bass_chord_type,
					       gint b_alter_1_type,
					       gint b_alter_1_degree,
					       gint b_alter_2_type,
					       gint b_alter_2_degree,
					       gchar **extension_name, GValue *extension_value,
					       guint extension_count);
guint ags_midi_ump_util_get_flex_set_chord_name(AgsMidiUmpUtil *midi_ump_util,
						guchar *buffer,
						gint *group,
						gint *channel,
						gint *t_sharp_flats,
						gint *chord_tonic,
						gint *chord_type,
						gint *alter_1_type,
						gint *alter_1_degree,
						gint *alter_2_type,
						gint *alter_2_degree,
						gint *alter_3_type,
						gint *alter_3_degree,
						gint *alter_4_type,
						gint *alter_4_degree,
						gint *b_sharp_flats,
						gint *bass_note,
						gint *bass_chord_type,
						gint *b_alter_1_type,
						gint *b_alter_1_degree,
						gint *b_alter_2_type,
						gint *b_alter_2_degree,
						gchar ***extension_name, GValue **extension_value,
						guint *extension_count);

/* MIDI v2.0 flex set text */
gboolean ags_midi_ump_util_is_flex_set_text(AgsMidiUmpUtil *midi_ump_util,
					    guchar *buffer);

G_END_DECLS

#endif /*__AGS_MIDI_UMP_UTIL_H__*/
