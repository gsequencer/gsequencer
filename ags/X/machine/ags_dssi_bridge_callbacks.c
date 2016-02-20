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

#include <ags/X/machine/ags_dssi_bridge_callbacks.h>

#include <ags/audio/ags_recall_dssi.h>

void
ags_dssi_bridge_program_changed_callback(GtkComboBox *combo_box, AgsDssiBridge *dssi_bridge)
{
  gchar *name;
  unsigned long bank, program;

  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsChannel *channel;
    
    GList *recall;
    GList *port;

    gchar *specifier;
    
    LADSPA_PortDescriptor *port_descriptor;

    unsigned long i;
    
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       0, &name,
		       1, &bank,
		       2, &program,
		       -1);

    /* update ports */
    dssi_bridge->dssi_descriptor->select_program(dssi_bridge->ladspa_handle,
						 bank,
						 program);

    channel = AGS_MACHINE(dssi_bridge)->audio->input;

    while(channel != NULL){
      recall = channel->recall;

      while((recall = ags_recall_template_find_type(recall, AGS_TYPE_RECALL_DSSI)) != NULL){
	AGS_RECALL_DSSI(recall->data)->bank = bank;
	AGS_RECALL_DSSI(recall->data)->program = program;
      
	port_descriptor = dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortDescriptors;

	for(i = 0; i < dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortCount; i++){
	  if((LADSPA_IS_PORT_CONTROL(port_descriptor[i]) && 
	      (LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	       LADSPA_IS_PORT_OUTPUT(port_descriptor[i])))){
	    specifier = dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortNames[i];
	    port = AGS_RECALL(recall->data)->port;

	    while(port != NULL){
	      if(!g_strcmp0(AGS_PORT(port->data)->specifier,
			    specifier)){
		GValue value = {0,};

		//		g_message(" --- %f\0", dssi_bridge->port_values[i]);

		g_value_init(&value,
			     G_TYPE_FLOAT);
		g_value_set_float(&value,
				  dssi_bridge->port_values[i]);

		//TODO:JK: verify if needed
		ags_port_safe_write(port->data,
				    &value);
	      
		//		g_message("%f ---\0", dssi_bridge->port_values[i]);
	      
		break;
	      }
	
	      port = port->next;
	    }
	  }
	}
      
	recall = recall->next;
      }

      channel = channel->next;
    }
  }
}

