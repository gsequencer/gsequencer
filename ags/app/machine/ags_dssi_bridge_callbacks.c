/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/machine/ags_dssi_bridge_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

void
ags_dssi_bridge_program_changed_callback(GtkComboBox *combo_box, AgsDssiBridge *dssi_bridge)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsLadspaConversion *ladspa_conversion;

    GList *start_bulk_member, *bulk_member;
    GList *start_recall, *recall;
  
    gchar *name;
    gchar *specifier;
    
    guint bank, program;
    unsigned long i;

    /* get program */
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       0, &name,
		       1, &bank,
		       2, &program,
		       -1);
    dssi_bridge->dssi_descriptor->select_program(dssi_bridge->ladspa_handle,
						 (unsigned long) bank,
						 (unsigned long) program);

#ifdef AGS_DEBUG
    g_message("%d %d", bank, program);
#endif
    
    /* play context */
    g_object_get(AGS_MACHINE(dssi_bridge)->audio,
		 "play", &start_recall,
		 NULL);
    
    recall = start_recall;
    
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_DSSI_AUDIO)) != NULL){
      ags_fx_dssi_audio_change_program(recall->data,
				       bank,
				       program);
      
      /* iterate */
      recall = recall->next;
    }
    
    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(dssi_bridge)->audio,
		 "recall", &start_recall,
		 NULL);
    
    recall = start_recall;
    
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_DSSI_AUDIO)) != NULL){
      ags_fx_dssi_audio_change_program(recall->data,
				       bank,
				       program);
      
      /* iterate */
      recall = recall->next;
    }
    
    g_list_free_full(start_recall,
		     g_object_unref);
      
    /* update UI */
    start_bulk_member = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input));
  
    for(i = 0; i < dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortCount; i++){
      /* find bulk member */
      bulk_member = start_bulk_member;

      specifier = dssi_bridge->dssi_descriptor->LADSPA_Plugin->PortNames[i];

#ifdef AGS_DEBUG
      g_message("%s", specifier);
#endif
      
      while(bulk_member != NULL){
	if(AGS_IS_BULK_MEMBER(bulk_member->data) &&
	   !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier,
		      specifier)){
	  GtkWidget *child_widget;

	  AGS_BULK_MEMBER(bulk_member->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;

	  child_widget = ags_bulk_member_get_widget(AGS_BULK_MEMBER(bulk_member->data));
	  
	  ladspa_conversion = (AgsLadspaConversion *) AGS_BULK_MEMBER(bulk_member->data)->conversion;
	  
	  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	    if(dssi_bridge->port_values[i] == 0.0){
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   FALSE);
	    }else{
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   TRUE);
	    }
	  }else if(AGS_IS_DIAL(child_widget)){
	    gdouble val;

	    val = dssi_bridge->port_values[i];
	    
	    if(ladspa_conversion != NULL){
	      //	      val = ags_ladspa_conversion_convert(ladspa_conversion,
	      //				  dssi_bridge->port_values[i],
	      //				  TRUE);
	    }
	    
	    gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment, val);
	    gtk_widget_queue_draw(child_widget);

#ifdef AGS_DEBUG
	    g_message(" --- %f", dssi_bridge->port_values[i]);
#endif
	  }
	
	  AGS_BULK_MEMBER(bulk_member->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);

	  break;
	}

	bulk_member = bulk_member->next;
      }
    }
    
    g_list_free(start_bulk_member);
  }
}
