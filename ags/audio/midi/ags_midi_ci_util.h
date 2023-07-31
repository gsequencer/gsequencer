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

#ifndef __AGS_MIDI_CI_UTIL_H__
#define __AGS_MIDI_CI_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CI_UTIL         (ags_midi_ci_util_get_type())

#define AGS_MIDI_CI_UTIL_BROADCAST_MUID (0x0fffffff)
#define AGS_MIDI_CI_UTIL_BROADCAST_MUID_ENCODED "\x7f\x7f\x7f\x7f"
#define AGS_MIDI_CI_UTIL_MAX_BROADCAST_MESSAGE_SIZE (512)
#define AGS_MIDI_CI_UTIL_DISCOVERY_TIMEOUT_USEC (3 * AGS_USEC_PER_SEC)

#define AGS_MIDI_CI_UTIL_PROTOCOL_TYPE_MIDI_1_0 (0x01)
#define AGS_MIDI_CI_UTIL_PROTOCOL_TYPE_MIDI_2_0 (0x02)

#define AGS_MIDI_CI_UTIL_VERSION_MIDI_1_0 (0x00)

#define AGS_MIDI_CI_UTIL_PROTOCOL_1_0_EXTENSIONS_SIZE_OF_PACKET(p) (((1 < 6) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)
#define AGS_MIDI_CI_UTIL_PROTOCOL_1_0_EXTENSIONS_JITTER_REDUCTION_TIMESTAMP(p) (((1 < 7) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)

#define AGS_MIDI_CI_UTIL_PROTOCOL_2_0_EXTENSIONS_JITTER_REDUCTION_TIMESTAMP(p) (((1 < 7) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)

typedef struct _AgsMidiCIUtil AgsMidiCIUtil;
typedef guint32 AgsMUID;

typedef enum{
  /* AGS_MIDI_CI_RESERVED       = 0x00, */
  AGS_MIDI_CI_TRANSPORT         = 0x10,
  AGS_MIDI_CI_EVENT_PROCESSOR   = 0x20,
  AGS_MIDI_CI_ENDPOINT          = 0x30,
  AGS_MIDI_CI_TRANSLATOR        = 0x40,
  AGS_MIDI_CI_GATEWAY           = 0x50,
  AGS_MIDI_CI_NODE_SERVER       = 0x60,
  /* AGS_MIDI_CI_RESERVED       = 0x70, */
}AgsMidiCIAuthorityLevel;

typedef enum{
  /* AGS_MIDI_CI_RESERVED                  = 0x00, */
  AGS_MIDI_CI_PROTOCOL_NEGOTIATION         = 0x10,
  AGS_MIDI_CI_PROFILE_CONFIGURATION        = 0x20,
  AGS_MIDI_CI_PROPERTY_EXCHANGE            = 0x30,
  /* AGS_MIDI_CI_RESERVED                  = 0x40, */
  /* AGS_MIDI_CI_RESERVED                  = 0x50, */
  /* AGS_MIDI_CI_RESERVED                  = 0x60, */
  AGS_MIDI_CI_PROPERTY_MANAGEMENT          = 0x70,
}AgsMidiCICategory;

typedef enum{
  /* AGS_MIDI_CI_RESERVED                            = 1, */
  AGS_MIDI_CI_PROTOCOL_NEGOTIATION_SUPPORTED         = 1 <<  1,
  AGS_MIDI_CI_PROFILE_CONFIGURATION_SUPPORTED        = 1 <<  2,
  AGS_MIDI_CI_PROPERTY_EXCHANGE_SUPPORTED            = 1 <<  3,
  /* AGS_MIDI_CI_RESERVED                            = 1 <<  4, */
  /* AGS_MIDI_CI_RESERVED                            = 1 <<  5, */
  /* AGS_MIDI_CI_RESERVED                            = 1 <<  6, */
}AgsMidiCICategorySupport;

typedef enum{
  AGS_MIDI_CI_SUCCESS                = 200,
  AGS_MIDI_CI_ACCEPTED               = 202,
  AGS_MIDI_CI_RESOURCE_UNAVAILABLE   = 341,
  AGS_MIDI_CI_BAD_DATA               = 342,
  AGS_MIDI_CI_TOO_MANY_REQUESTS      = 343,
  AGS_MIDI_CI_BAD_REQUEST            = 400,
  AGS_MIDI_CI_REPLY_NOT_AVAILABLE    = 403,
  AGS_MIDI_CI_RESOURCE_NOT_FOUND     = 404,
  AGS_MIDI_CI_RESOURCE_NOT_ALLOWED   = 405,
  AGS_MIDI_CI_PAYLOAD_TOO_LARGE      = 413,
  AGS_MIDI_CI_UNSUPPORTED_ENCODING   = 415,
  AGS_MIDI_CI_INVALID_VERSION        = 445,
  AGS_MIDI_CI_INTERNAL_ERROR         = 500,  
}AgsMidiCIUtilStatusCode;

struct _AgsMidiCIUtil
{
  guchar device_id;
  
  GRand *rand;
  
  //empty
};

GType ags_midi_ci_util_get_type(void);

AgsMidiCIUtil* ags_midi_ci_util_alloc();
void ags_midi_ci_util_free(AgsMidiCIUtil *midi_ci_util);

AgsMidiCIUtil* ags_midi_ci_util_copy(AgsMidiCIUtil *midi_ci_util);

AgsMUID ags_midi_ci_util_generate_muid(AgsMidiCIUtil *midi_ci_util);

void ags_midi_ci_util_put_muid(AgsMidiCIUtil *midi_ci_util,
			       guchar *buffer,
			       AgsMUID muid);
guint ags_midi_ci_util_get_muid(AgsMidiCIUtil *midi_ci_util,
				guchar *buffer,
				AgsMUID *muid);

void ags_midi_ci_util_put_discovery(AgsMidiCIUtil *midi_ci_util,
				    guchar *buffer,
				    guchar version,
				    AgsMUID source,
				    guchar *manufacturer_id,
				    guint16 device_family,
				    guint16 device_family_model_number,
				    guchar *software_revision_level,
				    guchar capability,
				    guint32 max_sysex_message_size);
guint ags_midi_ci_util_get_discovery(AgsMidiCIUtil *midi_ci_util,
				     guchar *buffer,
				     guchar *version,
				     AgsMUID *source,
				     guchar *manufacturer_id,
				     guint16 *device_family,
				     guint16 *device_family_model_number,
				     guchar *software_revision_level,
				     guchar *capability,
				     guint32 *max_sysex_message_size);

void ags_midi_ci_util_put_discovery_reply(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar version,
					  AgsMUID source,
					  AgsMUID destination,
					  gchar *manufacturer_id,
					  guint16 device_family,
					  guint16 device_family_model_number,
					  gchar *software_revision_level,
					  guchar capability,
					  guint32 max_sysex_message_size);
guint ags_midi_ci_util_get_discovery_reply(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *version,
					   AgsMUID *source,
					   AgsMUID *destination,
					   guchar *manufacturer_id,
					   guint16 *device_family,
					   guint16 *device_family_model_number,
					   guchar *software_revision_level,
					   guchar *capability,
					   guint32 *max_sysex_message_size);

void ags_midi_ci_util_put_invalidate_muid(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar version,
					  AgsMUID source,
					  AgsMUID target_muid);
guint ags_midi_ci_util_get_invalidate_muid(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *version,
					   AgsMUID *source,
					   AgsMUID *target_muid);

void ags_midi_ci_util_put_nak(AgsMidiCIUtil *midi_ci_util,
			      guchar *buffer,
			      guchar version,
			      AgsMUID source,
			      AgsMUID destination);
guint ags_midi_ci_util_get_nak(AgsMidiCIUtil *midi_ci_util,
			       guchar *buffer,
			       guchar *version,
			       AgsMUID *source,
			       AgsMUID *destination);

void ags_midi_ci_util_put_initiate_protocol_negotiation(AgsMidiCIUtil *midi_ci_util,
							guchar *buffer,
							guchar version,
							AgsMUID source,
							AgsMUID destination,
							AgsMidiCIAuthorityLevel authority_level,
							guchar number_of_supported_protocols,
							guchar **preferred_protocol_type);
guint ags_midi_ci_util_get_initiate_protocol_negotiation(AgsMidiCIUtil *midi_ci_util,
							 guchar *buffer,
							 guchar *version,
							 AgsMUID *source,
							 AgsMUID *destination,
							 AgsMidiCIAuthorityLevel *authority_level,
							 guchar *number_of_supported_protocols,
							 guchar **preferred_protocol_type);

void ags_midi_ci_util_put_initiate_protocol_negotiation_reply(AgsMidiCIUtil *midi_ci_util,
							      guchar *buffer,
							      guchar version,
							      AgsMUID source,
							      AgsMUID destination,
							      AgsMidiCIAuthorityLevel authority_level,
							      guchar number_of_supported_protocols,
							      guchar **preferred_protocol_type);
guint ags_midi_ci_util_get_initiate_protocol_negotiation_reply(AgsMidiCIUtil *midi_ci_util,
							       guchar *buffer,
							       guchar *version,
							       AgsMUID *source,
							       AgsMUID *destination,
							       AgsMidiCIAuthorityLevel *authority_level,
							       guchar *number_of_supported_protocols,
							       guchar **preferred_protocol_type);

void ags_midi_ci_util_put_set_protocol_type(AgsMidiCIUtil *midi_ci_util,
					    guchar *buffer,
					    guchar version,
					    AgsMUID source,
					    AgsMUID destination,
					    AgsMidiCIAuthorityLevel authority_level,
					    guchar *protocol_type);
guint ags_midi_ci_util_get_set_protocol_type(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *version,
					     AgsMUID *source,
					     AgsMUID *destination,
					     AgsMidiCIAuthorityLevel *authority_level,
					     guchar *protocol_type);

void ags_midi_ci_util_put_confirm_protocol_type(AgsMidiCIUtil *midi_ci_util,
						guchar *buffer,
						guchar version,
						AgsMUID source,
						AgsMUID destination,
						AgsMidiCIAuthorityLevel authority_level);
guint ags_midi_ci_util_get_confirm_protocol_type(AgsMidiCIUtil *midi_ci_util,
						 guchar *buffer,
						 guchar *version,
						 AgsMUID *source,
						 AgsMUID *destination,
						 AgsMidiCIAuthorityLevel *authority_level);

void ags_midi_ci_util_put_confirm_protocol_type_reply(AgsMidiCIUtil *midi_ci_util,
						      guchar *buffer,
						      guchar version,
						      AgsMUID source,
						      AgsMUID destination,
						      AgsMidiCIAuthorityLevel authority_level);
guint ags_midi_ci_util_get_confirm_protocol_type_reply(AgsMidiCIUtil *midi_ci_util,
						       guchar *buffer,
						       guchar *version,
						       AgsMUID *source,
						       AgsMUID *destination,
						       AgsMidiCIAuthorityLevel *authority_level);

void ags_midi_ci_util_put_confirm_protocol_type_established(AgsMidiCIUtil *midi_ci_util,
							    guchar *buffer,
							    guchar version,
							    AgsMUID source,
							    AgsMUID destination,
							    AgsMidiCIAuthorityLevel authority_level);
guint ags_midi_ci_util_get_confirm_protocol_type_established(AgsMidiCIUtil *midi_ci_util,
							     guchar *buffer,
							     guchar *version,
							     AgsMUID *source,
							     AgsMUID *destination,
							     AgsMidiCIAuthorityLevel *authority_level);

void ags_midi_ci_util_put_profile(AgsMidiCIUtil *midi_ci_util,
				  guchar *buffer,
				  guchar version,
				  AgsMUID source,
				  AgsMUID destination);
guint ags_midi_ci_util_get_profile(AgsMidiCIUtil *midi_ci_util,
				   guchar *buffer,
				   guchar *version,
				   AgsMUID *source,
				   AgsMUID *destination);

void ags_midi_ci_util_put_profile_reply(AgsMidiCIUtil *midi_ci_util,
					guchar *buffer,
					guchar version,
					AgsMUID source,
					AgsMUID destination,
					guint16 enabled_profile_count,
					guchar* enabled_profile[5],
					guint16 disabled_profile_count,
					guchar* disabled_profile[5]);
guint ags_midi_ci_util_get_profile_reply(AgsMidiCIUtil *midi_ci_util,
					 guchar *buffer,
					 guchar *version,
					 AgsMUID *source,
					 AgsMUID *destination,
					 guint16 *enabled_profile_count,
					 guchar ***enabled_profile,
					 guint16 *disabled_profile_count,
					 guchar ***disabled_profile);

void ags_midi_ci_util_put_profile_enabled_report(AgsMidiCIUtil *midi_ci_util,
						 guchar *buffer,
						 guchar device_id,
						 guchar version,
						 AgsMUID source,
						 guchar *enabled_profile,
						 guint16 enabled_channel_count);
guint ags_midi_ci_util_get_profile_enabled_report(AgsMidiCIUtil *midi_ci_util,
						  guchar *buffer,
						  guchar *device_id,
						  guchar *version,
						  AgsMUID *source,
						  guchar *enabled_profile,
						  guint16 *enabled_channel_count);

void ags_midi_ci_util_put_profile_disabled_report(AgsMidiCIUtil *midi_ci_util,
						  guchar *buffer,
						  guchar device_id,
						  guchar version,
						  AgsMUID source,
						  guchar *disabled_profile,
						  guint16 disabled_channel_count);
guint ags_midi_ci_util_get_profile_disabled_report(AgsMidiCIUtil *midi_ci_util,
						   guchar *buffer,
						   guchar *device_id,
						   guchar *version,
						   AgsMUID *source,
						   guchar *disabled_profile,
						   guint16 *disabled_channel_count);

void ags_midi_ci_util_put_profile_added(AgsMidiCIUtil *midi_ci_util,
					guchar *buffer,
					guchar device_id,
					guchar version,
					AgsMUID source,
					guchar *add_profile);
guint ags_midi_ci_util_get_profile_added(AgsMidiCIUtil *midi_ci_util,
					 guchar *buffer,
					 guchar *device_id,
					 guchar *version,
					 AgsMUID *source,
					 guchar *add_profile);

void ags_midi_ci_util_put_profile_removed(AgsMidiCIUtil *midi_ci_util,
					  guchar *buffer,
					  guchar device_id,
					  guchar version,
					  AgsMUID source,
					  guchar *remove_profile);
guint ags_midi_ci_util_get_profile_removed(AgsMidiCIUtil *midi_ci_util,
					   guchar *buffer,
					   guchar *device_id,
					   guchar *version,
					   AgsMUID *source,
					   guchar *remove_profile);

void ags_midi_ci_util_put_profile_specific_data(AgsMidiCIUtil *midi_ci_util,
						guchar *buffer,
						guchar version,
						AgsMUID source,
						AgsMUID destination,
						guchar *profile_id,
						guint32 profile_specific_data_length,
						guchar *profile_specific_data);
guint ags_midi_ci_util_get_profile_specific_data(AgsMidiCIUtil *midi_ci_util,
						 guchar *buffer,
						 guchar *version,
						 AgsMUID *source,
						 AgsMUID *destination,
						 guchar *profile_id,
						 guint32 *profile_specific_data_length,
						 guchar *profile_specific_data);

void ags_midi_ci_util_put_property_exchange_capabilities(AgsMidiCIUtil *midi_ci_util,
							 guchar *buffer,
							 guchar midi_channel,
							 guchar version,
							 AgsMUID source,
							 AgsMUID destination,
							 guchar supported_property_exchange_count);
guint ags_midi_ci_util_get_property_exchange_capabilities(AgsMidiCIUtil *midi_ci_util,
							  guchar *buffer,
							  guchar *midi_channel,
							  guchar *version,
							  AgsMUID *source,
							  AgsMUID *destination,
							  guchar *supported_property_exchange_count);

void ags_midi_ci_util_put_property_exchange_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
							       guchar *buffer,
							       guchar midi_channel,
							       guchar version,
							       AgsMUID source,
							       AgsMUID destination,
							       guchar supported_property_exchange_count);
guint ags_midi_ci_util_get_property_exchange_capabilities_reply(AgsMidiCIUtil *midi_ci_util,
								guchar *buffer,
								guchar *midi_channel,
								guchar *version,
								AgsMUID *source,
								AgsMUID *destination,
								guchar *supported_property_exchange_count);

void ags_midi_ci_util_put_get_property_data(AgsMidiCIUtil *midi_ci_util,
					    guchar *buffer,
					    guchar midi_channel,
					    guchar version,
					    AgsMUID source,
					    AgsMUID destination,
					    guchar request_id,
					    guint16 header_data_length,
					    guchar *header_data,
					    guint16 chunk_count,
					    guint16 nth_chunk,
					    guint16 property_data_length,
					    guchar *property_data);
guint ags_midi_ci_util_get_get_property_data(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *midi_channel,
					     guchar *version,
					     AgsMUID *source,
					     AgsMUID *destination,
					     guchar *request_id,
					     guint16 *header_data_length,
					     guchar *header_data,
					     guint16 *chunk_count,
					     guint16 *nth_chunk,
					     guint16 *property_data_length,
					     guchar *property_data);

void ags_midi_ci_util_put_get_property_data_reply(AgsMidiCIUtil *midi_ci_util,
						  guchar *buffer,
						  guchar midi_channel,
						  guchar version,
						  AgsMUID source,
						  AgsMUID destination,
						  guchar request_id,
						  guint16 header_data_length,
						  guchar *header_data,
						  guint16 chunk_count,
						  guint16 nth_chunk,
						  guint16 property_data_length,
						  guchar *property_data);
guint ags_midi_ci_util_get_get_property_data_reply(AgsMidiCIUtil *midi_ci_util,
						   guchar *buffer,
						   guchar *midi_channel,
						   guchar *version,
						   AgsMUID *source,
						   AgsMUID *destination,
						   guchar *request_id,
						   guint16 *header_data_length,
						   guchar *header_data,
						   guint16 *chunk_count,
						   guint16 *nth_chunk,
						   guint16 *property_data_length,
						   guchar *property_data);

void ags_midi_ci_util_put_set_property_data(AgsMidiCIUtil *midi_ci_util,
					    guchar *buffer,
					    guchar midi_channel,
					    guchar version,
					    AgsMUID source,
					    AgsMUID destination,
					    guchar request_id,
					    guint16 header_data_length,
					    guchar *header_data,
					    guint16 chunk_count,
					    guint16 nth_chunk,
					    guint16 property_data_length,
					    guchar *property_data);
guint ags_midi_ci_util_get_set_property_data(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar *midi_channel,
					     guchar *version,
					     AgsMUID *source,
					     AgsMUID *destination,
					     guchar *request_id,
					     guint16 *header_data_length,
					     guchar *header_data,
					     guint16 *chunk_count,
					     guint16 *nth_chunk,
					     guint16 *property_data_length,
					     guchar *property_data);

void ags_midi_ci_util_put_set_property_data_reply(AgsMidiCIUtil *midi_ci_util,
						  guchar *buffer,
						  guchar midi_channel,
						  guchar version,
						  AgsMUID source,
						  AgsMUID destination,
						  guchar request_id,
						  guint16 header_data_length,
						  guchar *header_data,
						  guint16 chunk_count,
						  guint16 nth_chunk,
						  guint16 property_data_length,
						  guchar *property_data);
guint ags_midi_ci_util_get_set_property_data_reply(AgsMidiCIUtil *midi_ci_util,
						   guchar *buffer,
						   guchar *midi_channel,
						   guchar *version,
						   AgsMUID *source,
						   AgsMUID *destination,
						   guchar *request_id,
						   guint16 *header_data_length,
						   guchar *header_data,
						   guint16 *chunk_count,
						   guint16 *nth_chunk,
						   guint16 *property_data_length,
						   guchar *property_data);

void ags_midi_ci_util_put_subscription(AgsMidiCIUtil *midi_ci_util,
				       guchar *buffer,
				       guchar midi_channel,
				       guchar version,
				       AgsMUID source,
				       AgsMUID destination,
				       guchar request_id,
				       guint16 header_data_length,
				       guchar *header_data,
				       guint16 chunk_count,
				       guint16 nth_chunk,
				       guint16 property_data_length,
				       guchar *property_data);
guint ags_midi_ci_util_get_subscription(AgsMidiCIUtil *midi_ci_util,
					guchar *buffer,
					guchar *midi_channel,
					guchar *version,
					AgsMUID *source,
					AgsMUID *destination,
					guchar *request_id,
					guint16 *header_data_length,
					guchar *header_data,
					guint16 *chunk_count,
					guint16 *nth_chunk,
					guint16 *property_data_length,
					guchar *property_data);

void ags_midi_ci_util_put_subscription_reply(AgsMidiCIUtil *midi_ci_util,
					     guchar *buffer,
					     guchar midi_channel,
					     guchar version,
					     AgsMUID source,
					     AgsMUID destination,
					     guchar request_id,
					     guint16 header_data_length,
					     guchar *header_data,
					     guint16 chunk_count,
					     guint16 nth_chunk,
					     guint16 property_data_length,
					     guchar *property_data);
guint ags_midi_ci_util_get_subscription_reply(AgsMidiCIUtil *midi_ci_util,
					      guchar *buffer,
					      guchar *midi_channel,
					      guchar *version,
					      AgsMUID *source,
					      AgsMUID *destination,
					      guchar *request_id,
					      guint16 *header_data_length,
					      guchar *header_data,
					      guint16 *chunk_count,
					      guint16 *nth_chunk,
					      guint16 *property_data_length,
					      guchar *property_data);

G_END_DECLS

#endif /*__AGS_MIDI_CI_UTIL_H__*/
