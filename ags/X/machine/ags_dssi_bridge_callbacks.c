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

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/audio/ags_recall_dssi.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

void
ags_dssi_bridge_program_changed_callback(GtkComboBox *combo_box, AgsDssiBridge *dssi_bridge)
{
  AgsLadspaConversion *ladspa_conversion;

  GList *bulk_member, *bulk_member_start;
  
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

    gboolean initial_run;

    /* get program */
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       0, &name,
		       1, &bank,
		       2, &program,
		       -1);

    dssi_bridge->dssi_descriptor->select_program(dssi_bridge->ladspa_handle,
						 bank,
						 program);


#ifdef AGS_DEBUG
    g_message("%d %d\0", bank, program);
#endif
    
    /* update ports */
    channel = AGS_MACHINE(dssi_bridge)->audio->input;
    port_descriptor = dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortDescriptors;

    initial_run = TRUE;
    
    while(channel != NULL){
      recall = channel->recall;

      while((recall = ags_recall_find_type(recall, AGS_TYPE_RECALL_DSSI)) != NULL){
	AGS_RECALL_DSSI(recall->data)->bank = bank;
	AGS_RECALL_DSSI(recall->data)->program = program;
      
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
		float port_value;
		
		port_value = (float) dssi_bridge->port_values[i];
		
		g_value_init(&value,
			     G_TYPE_FLOAT);
		g_value_set_float(&value,
				  port_value);

		g_message("%f\0", dssi_bridge->port_values[i]);
		ags_port_safe_write_raw(port->data,
					&value);
		
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

    /* update UI */
    bulk_member_start = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input)->table);
  
    for(i = 0; i < dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortCount; i++){
      AgsDssiPlugin *dssi_plugin;
      AgsPortDescriptor *port_descriptor;
      
      dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_bridge->filename, dssi_bridge->effect);
      port_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->port;
      
      /* ladspa conversion */
      ladspa_conversion = NULL;

      if((AGS_PORT_DESCRIPTOR_BOUNDED_BELOW & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
      }

      if((AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
      }
      if((AGS_PORT_DESCRIPTOR_SAMPLERATE & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}

	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
      }

      if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
	if(ladspa_conversion == NULL ||
	   !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
	  ladspa_conversion = ags_ladspa_conversion_new();
	}
    
	ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
      }

      /* find bulk member */
      bulk_member = bulk_member_start;

      specifier = dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortNames[i];

#ifdef AGS_DEBUG
      g_message("%s\0", specifier);
#endif
      
      while(bulk_member != NULL){
	if(AGS_IS_BULK_MEMBER(bulk_member->data) &&
	   !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier,
		      specifier)){
	  GtkWidget *child_widget;
	
	  AGS_BULK_MEMBER(bulk_member->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;

	  child_widget = gtk_bin_get_child(AGS_BULK_MEMBER(bulk_member->data));

	  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	    if(dssi_bridge->port_values[i] == 0.0){
	      gtk_toggle_button_set_active(child_widget,
					   FALSE);
	    }else{
	      gtk_toggle_button_set_active(child_widget,
					   TRUE);
	    }
	  }else if(AGS_IS_DIAL(child_widget)){
	    if(ladspa_conversion != NULL){
	      AGS_DIAL(child_widget)->adjustment->value = ags_ladspa_conversion_convert(ladspa_conversion,
											dssi_bridge->port_values[i],
											TRUE);
	    }else{
	      AGS_DIAL(child_widget)->adjustment->value = dssi_bridge->port_values[i];
	    }
	    
	    ags_dial_draw(child_widget);

#ifdef AGS_DEBUG
	    g_message(" --- %f\0", dssi_bridge->port_values[i]);
#endif
	  }
	
	  AGS_BULK_MEMBER(bulk_member->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);

	  break;
	}

	bulk_member = bulk_member->next;
      }
    
      /* unref ladspa conversion */
      if(ladspa_conversion != NULL){
	g_object_unref(ladspa_conversion);
      }
    }

    g_list_free(bulk_member_start);
  }
}

