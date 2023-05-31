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

#include <ags/audio/midi/ags_midi_ci_1_1_port.h>

void ags_midi_ci_1_1_port_class_init(AgsMidiCI_1_1_PortClass *midi_ci_1_1_port);
void ags_midi_ci_1_1_port_init(AgsMidiCI_1_1_Port *midi_ci_1_1_port);
void ags_midi_ci_1_1_port_finalize(GObject *gobject);

/**
 * SECTION:ags_midi_ci_1_1_port
 * @short_description: MIDI port
 * @title: AgsMidiPort
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ci_1_1_port.h
 *
 * Portity functions for MIDI CI version 1.1.
 */

GType
ags_midi_ci_1_1_port_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ci_1_1_port = 0;

    static const GTypeInfo ags_midi_ci_1_1_port_info = {
      sizeof (AgsMidiCI_1_1_PortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_ci_1_1_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiCI_1_1_Port),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_ci_1_1_port_init,
    };

    ags_type_midi_ci_1_1_port = g_type_register_static(G_TYPE_OBJECT,
						       "AgsMidiCI_1_1_Port", &ags_midi_ci_1_1_port_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ci_1_1_port);
  }

  return g_define_type_id__volatile;
}

void
ags_midi_ci_1_1_port_class_init(AgsMidiCI_1_1_PortClass *midi_ci_1_1_port)
{
  GObjectClass *gobject;
  
  ags_midi_ci_1_1_port_parent_class = g_type_class_peek_parent(midi_ci_1_1_port);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_ci_1_1_port;  

  gobject->finalize = ags_midi_ci_1_1_port_finalize;

  /* AgsMidiCI_1_1_PortClass */
  midi_ci_1_1_port->send_bytes = NULL;
  midi_ci_1_1_port->receive_bytes = NULL;
}

void
ags_midi_ci_1_1_port_init(AgsMidiCI_1_1_Port *midi_ci_1_1_port)
{
  midi_ci_1_1_port->direction = AGS_MIDI_CI_1_1_PORT_INPUT;
}

void
ags_midi_ci_1_1_port_finalize(GObject *gobject)
{
  AgsMidiCI_1_1_Port *midi_ci_1_1_port;
    
  midi_ci_1_1_port = (AgsMidiCI_1_1_Port *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_midi_ci_1_1_port_parent_class)->finalize(gobject);
}

/**
 * ags_midi_ci_1_1_port_new:
 * 
 * Creates a new instance of #AgsMidiCI_1_1_Port
 *
 * Returns: the new #AgsMidiCI_1_1_Port
 * 
 * Since: 5.2.0
 */
AgsMidiCI_1_1_Port*
ags_midi_ci_1_1_port_new()
{
  AgsMidiCI_1_1_Port *midi_ci_1_1_port;

  midi_ci_1_1_port = (AgsMidiCI_1_1_Port *) g_object_new(AGS_TYPE_MIDI_CI_1_1_PORT,
							 NULL);
  
  return(midi_ci_1_1_port);
}
