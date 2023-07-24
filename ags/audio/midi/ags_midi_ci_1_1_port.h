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

#ifndef __AGS_MIDI_CI_1_1_PORT_H__
#define __AGS_MIDI_CI_1_1_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <json-glib/json-glib.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_MIDI_CI_1_1_PORT         (ags_midi_ci_1_1_port_get_type())

typedef struct _AgsMidiCI_1_1_PortDirection AgsMidiCI_1_1_PortDirection;
typedef struct _AgsMidiCI_1_1_PortDirectionClass AgsMidiCI_1_1_PortDirectionClass;

typedef enum AgsMidiCI_1_1_PortFlags{
  AGS_MIDI_CI_1_1_PORT_INITIATOR             = 1,
  AGS_MIDI_CI_1_1_PORT_UNI_DIRECTIONAL       = 1 <<  1,
  AGS_MIDI_CI_1_1_PORT_MIDI_1_0              = 1 <<  2,
  AGS_MIDI_CI_1_1_PORT_MIDI_2_0              = 1 <<  3,
};

typedef enum AgsMidiCI_1_1_PortDirection{
  AGS_MIDI_CI_1_1_PORT_OUTPUT,
  AGS_MIDI_CI_1_1_PORT_INPUT,
};

struct _AgsMidiCI_1_1_Port
{
  GObject gobject;

  AgsMidiCI_1_1_PortFlags flags;
  
  AgsMidiCI_1_1_PortDirection direction;
  
  //empty
};

struct _AgsMidiCI_1_1_PortClass
{
  GObjectClass gobject;

  gsize (*send_bytes)(AgsMidiCI_1_1_Port *port,
		      gchar *buffer, gsize size,
		      GError **error);
  gsize (*receive_bytes)(AgsMidiCI_1_1_Port *port,
			 gchar *buffer, gsize size,
			 GError **error);
};

GType ags_midi_ci_1_1_port_get_type(void);

void ags_midi_ci_1_1_port_inquiry(AgsMidiCI_1_1_Port *port,
				  JsonNode *property_data);
void ags_midi_ci_1_1_port_respond(AgsMidiCI_1_1_Port *port,
				  JsonNode *property_data);

AgsMidiCI_1_1_Port* ags_midi_ci_1_1_port_new();

G_END_DECLS

#endif /*__AGS_MIDI_CI_1_1_PORT_H__*/
