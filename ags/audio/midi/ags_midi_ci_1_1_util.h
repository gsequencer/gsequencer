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

#ifndef __AGS_MIDI_CI_1_1_UTIL_H__
#define __AGS_MIDI_CI_1_1_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CI_1_1_UTIL         (ags_midi_ci_1_1_util_get_type())

#define AGS_MIDI_CI_1_1_UTIL_BROADCAST_MUID (0x0fffffff)
#define AGS_MIDI_CI_1_1_UTIL_MAX_BROADCAST_MESSAGE_SIZE (512)
#define AGS_MIDI_CI_1_1_UTIL_DISCOVERY_TIMEOUT_USEC (3 * AGS_USEC_PER_SEC)

#define AGS_MIDI_CI_1_1_UTIL_PROTOCOL_TYPE_MIDI_1_0 (0x01)
#define AGS_MIDI_CI_1_1_UTIL_PROTOCOL_TYPE_MIDI_2_0 (0x02)

#define AGS_MIDI_CI_1_1_UTIL_VERSION_MIDI_1_0 (0x00)

#define AGS_MIDI_CI_1_1_UTIL_PROTOCOL_1_0_EXTENSIONS_SIZE_OF_PACKET(p) (((1 < 6) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)
#define AGS_MIDI_CI_1_1_UTIL_PROTOCOL_1_0_EXTENSIONS_JITTER_REDUCTION_TIMESTAMP(p) (((1 < 7) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)

#define AGS_MIDI_CI_1_1_UTIL_PROTOCOL_2_0_EXTENSIONS_JITTER_REDUCTION_TIMESTAMP(p) (((1 < 7) & ((guchar *) p)[0]) != 0 ? TRUE: FALSE)

typedef struct _AgsMidiCI_1_1_Util AgsMidiCI_1_1_Util;
typedef guint32 AgsMUID;

typedef enum{
  /* AGS_MIDI_CI_1_1_RESERVED       = 0x00, */
  AGS_MIDI_CI_1_1_TRANSPORT         = 0x10,
  AGS_MIDI_CI_1_1_EVENT_PROCESSOR   = 0x20,
  AGS_MIDI_CI_1_1_ENDPOINT          = 0x30,
  AGS_MIDI_CI_1_1_TRANSLATOR        = 0x40,
  AGS_MIDI_CI_1_1_GATEWAY           = 0x50,
  AGS_MIDI_CI_1_1_NODE_SERVER       = 0x60,
  /* AGS_MIDI_CI_1_1_RESERVED       = 0x70, */
}AgsMidiCI_1_1_AuthorityLevel;

typedef enum{
  /* AGS_MIDI_CI_1_1_RESERVED                  = 0x00, */
  AGS_MIDI_CI_1_1_PROTOCOL_NEGOTIATION         = 0x10,
  AGS_MIDI_CI_1_1_PROFILE_CONFIGURATION        = 0x20,
  AGS_MIDI_CI_1_1_PROPERTY_EXCHANGE            = 0x30,
  /* AGS_MIDI_CI_1_1_RESERVED                  = 0x40, */
  /* AGS_MIDI_CI_1_1_RESERVED                  = 0x50, */
  /* AGS_MIDI_CI_1_1_RESERVED                  = 0x60, */
  AGS_MIDI_CI_1_1_PROPERTY_MANAGEMENT          = 0x70,
}AgsMidiCI_1_1_Category;

typedef enum{
  /* AGS_MIDI_CI_1_1_RESERVED                            = 1, */
  AGS_MIDI_CI_1_1_PROTOCOL_NEGOTIATION_SUPPORTED         = 1 <<  1,
  AGS_MIDI_CI_1_1_PROFILE_CONFIGURATION_SUPPORTED        = 1 <<  2,
  AGS_MIDI_CI_1_1_PROPERTY_EXCHANGE_SUPPORTED            = 1 <<  3,
  /* AGS_MIDI_CI_1_1_RESERVED                            = 1 <<  4, */
  /* AGS_MIDI_CI_1_1_RESERVED                            = 1 <<  5, */
  /* AGS_MIDI_CI_1_1_RESERVED                            = 1 <<  6, */
}AgsMidiCI_1_1_CategorySupport;

struct _AgsMidiCI_1_1_Util
{
  guchar device_id;
  
  GRand *rand;
  
  //empty
};

GType ags_midi_ci_1_1_util_get_type(void);

AgsMidiCI_1_1_Util* ags_midi_ci_1_1_util_alloc();
void ags_midi_ci_1_1_util_free(AgsMidiCI_1_1_Util *midi_ci_1_1_util);

AgsMidiCI_1_1_Util* ags_midi_ci_1_1_util_copy(AgsMidiCI_1_1_Util *midi_ci_1_1_util);

AgsMUID ags_midi_ci_1_1_util_generate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util);

void ags_midi_ci_1_1_util_put_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					guchar *buffer,
					guchar version,
					AgsMUID source,
					guint32 manufacturer_id,
					guint32 device_family,
					guint32 device_family_model_number,
					guint32 software_revision_level,
					guchar capability,
					guint32 max_sysex_message_size);
guint ags_midi_ci_1_1_util_get_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					 guchar *buffer,
					 guchar *version,
					 AgsMUID *source,
					 guint32 *manufacturer_id,
					 guint32 *device_family,
					 guint32 *device_family_model_number,
					 guint32 *software_revision_level,
					 guchar *capability,
					 guint32 *max_sysex_message_size);

void ags_midi_ci_1_1_util_put_discovery_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					      guchar *buffer,
					      guchar version,
					      AgsMUID source,
					      AgsMUID destination,
					      guint32 manufacturer_id,
					      guint32 device_family,
					      guint32 device_family_model_number,
					      guint32 software_revision_level,
					      guchar capability,
					      guint32 max_sysex_message_size);
guint ags_midi_ci_1_1_util_get_discovery_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					       guchar *buffer,
					       guchar *version,
					       AgsMUID *source,
					       AgsMUID *destination,
					       guint32 *manufacturer_id,
					       guint32 *device_family,
					       guint32 *device_family_model_number,
					       guint32 *software_revision_level,
					       guchar *capability,
					       guint32 *max_sysex_message_size);

void ags_midi_ci_1_1_util_put_invalidate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					      guchar *buffer,
					      guchar version,
					      AgsMUID source,
					      AgsMUID target_muid);
guint ags_midi_ci_1_1_util_get_invalidate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					       guchar *buffer,
					       guchar *version,
					       AgsMUID *source,
					       AgsMUID *target_muid);

void ags_midi_ci_1_1_util_put_nak(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
				  guchar *buffer,
				  guchar version,
				  AgsMUID source,
				  AgsMUID destination);
guint ags_midi_ci_1_1_util_get_nak(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
				   guchar *buffer,
				   guchar *version,
				   AgsMUID *source,
				   AgsMUID *destination);

void ags_midi_ci_1_1_util_put_initiate_protocol_negotiation(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
							    guchar *buffer,
							    guchar version,
							    AgsMUID source,
							    AgsMUID destination,
							    AgsMidiCI_1_1_AuthorityLevel authority_level,
							    guchar number_of_supported_protocols,
							    guchar **preferred_protocol_type);
guint ags_midi_ci_1_1_util_get_initiate_protocol_negotiation(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
							     guchar *buffer,
							     guchar *version,
							     AgsMUID *source,
							     AgsMUID *destination,
							     AgsMidiCI_1_1_AuthorityLevel *authority_level,
							     guchar *number_of_supported_protocols,
							     guchar **preferred_protocol_type);

void ags_midi_ci_1_1_util_put_initiate_protocol_negotiation_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
								  guchar *buffer,
								  guchar version,
								  AgsMUID source,
								  AgsMUID destination,
								  AgsMidiCI_1_1_AuthorityLevel authority_level,
								  guchar number_of_supported_protocols,
								  guchar **preferred_protocol_type);
guint ags_midi_ci_1_1_util_get_initiate_protocol_negotiation_reply(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
								   guchar *buffer,
								   guchar *version,
								   AgsMUID *source,
								   AgsMUID *destination,
								   AgsMidiCI_1_1_AuthorityLevel *authority_level,
								   guchar *number_of_supported_protocols,
								   guchar **preferred_protocol_type);

void ags_midi_ci_1_1_util_put_set_protocol_type(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
						guchar *buffer,
						guchar version,
						AgsMUID source,
						AgsMUID destination,
						AgsMidiCI_1_1_AuthorityLevel authority_level,
						guchar *protocol_type);
guint ags_midi_ci_1_1_util_get_set_protocol_type(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
						 guchar *buffer,
						 guchar *version,
						 AgsMUID *source,
						 AgsMUID *destination,
						 AgsMidiCI_1_1_AuthorityLevel *authority_level,
						 guchar *protocol_type);

void ags_midi_ci_1_1_util_put_confirm_protocol_type(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
						    guchar *buffer,
						    guchar version,
						    AgsMUID source,
						    AgsMUID destination,
						    AgsMidiCI_1_1_AuthorityLevel authority_level);
guint ags_midi_ci_1_1_util_get_confirm_protocol_type(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
						     guchar *buffer,
						     guchar *version,
						     AgsMUID *source,
						     AgsMUID *destination,
						     AgsMidiCI_1_1_AuthorityLevel *authority_level);

G_END_DECLS

#endif /*__AGS_MIDI_CI_1_1_UTIL_H__*/
