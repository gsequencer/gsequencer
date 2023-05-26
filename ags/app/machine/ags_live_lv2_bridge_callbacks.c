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

#include <ags/app/machine/ags_live_lv2_bridge_callbacks.h>

#include <lv2/lv2plug.in/ns/lv2ext/lv2_programs.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_bulk_member.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern GHashTable *ags_live_lv2_bridge_lv2ui_handle;
extern GHashTable *ags_live_lv2_bridge_lv2ui_idle;

gboolean ags_live_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLiveLv2Bridge *live_lv2_bridge);

void
ags_live_lv2_bridge_lv2ui_cleanup_function(LV2UI_Handle handle)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  live_lv2_bridge = g_hash_table_lookup(ags_live_lv2_bridge_lv2ui_handle,
					handle);
  
  if(live_lv2_bridge != NULL){
    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_handle,
			live_lv2_bridge->ui_handle);
    
    live_lv2_bridge->ui_handle = NULL;
  }
}

void
ags_live_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
  AgsEffectBridge *effect_bridge;
  GtkWidget *child_widget;

  GList *start_list_bulk_member, *list_bulk_member;

  gchar *block_scope;

  gfloat val;
  
  live_lv2_bridge = (AgsLiveLv2Bridge *) controller;
  
  if(live_lv2_bridge == NULL){
    g_warning("ags_live_lv2_bridge_lv2ui_write_function() - live_lv2_bridge == NULL");

    return;
  }

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0){
    return;
  }
  
  if(port_protocol != 0){
    g_warning("ags_live_lv2_bridge_lv2ui_write_function() - unknown lv2 port protocol");

    return;
  }

  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge);
  
  list_bulk_member =
    start_list_bulk_member = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(effect_bridge->bulk_input));

  while(list_bulk_member != NULL){
    if(port_index == AGS_BULK_MEMBER(list_bulk_member->data)->port_index){
      child_widget = ags_bulk_member_get_widget(AGS_BULK_MEMBER(list_bulk_member->data));

      if((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, child_widget)) == NULL ||
	 (!g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_LV2UI)) == FALSE){
	if(block_scope == NULL){
	  g_hash_table_insert(live_lv2_bridge->block_control,
			      child_widget,
			      AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_LV2UI);
	}
	
	val = ((float *) buffer)[0];

	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						TRUE);
	}
      
	if(AGS_IS_DIAL(child_widget)){
	  ags_dial_set_value((AgsDial *) child_widget,
			     val);	
	}else if(GTK_IS_SPIN_BUTTON(child_widget)){
	  gtk_spin_button_set_value((GtkSpinButton *) child_widget,
				    val);
	}else if(GTK_IS_SCALE(child_widget)){
	  gtk_range_set_value((GtkRange *) child_widget,
			      val);
	}else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
				       ((val != 0.0) ? TRUE: FALSE));
	}else if(GTK_IS_BUTTON(child_widget)){
	  g_signal_emit_by_name((GtkButton *) child_widget,
				"clicked");
	}
	      
	if(block_scope == NULL){
	  g_hash_table_insert(live_lv2_bridge->block_control,
			      child_widget,
			      NULL);
	}
      }
      
      break;
    }

    list_bulk_member = list_bulk_member->next;
  }

  g_list_free(start_list_bulk_member);
}

void
ags_live_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsLv2Plugin *lv2_plugin;
    
    LV2_Programs_Interface *program_interface;

    GList *start_plugin_port, *plugin_port;
    GList *start_bulk_member, *bulk_member;
    GList *start_recall, *recall;
  
    gchar *name;
    gchar *specifier;
    
    guint bank, program;

    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 live_lv2_bridge->filename,
						 live_lv2_bridge->effect);

    /* get program */
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       0, &name,
		       1, &bank,
		       2, &program,
		       -1);    
    
#ifdef AGS_DEBUG
    g_message("%d %d", bank, program);
#endif
    
    program_interface = live_lv2_bridge->lv2_descriptor->extension_data(LV2_PROGRAMS__Interface);
    program_interface->select_program(live_lv2_bridge->lv2_handle[0],
				      bank,
				      program);

    g_object_get(lv2_plugin,
		 "plugin-port", &start_plugin_port,
		 NULL);

    /* play context */
    g_object_get(AGS_MACHINE(live_lv2_bridge)->audio,
		 "play", &start_recall,
		 NULL);
      
    recall = start_recall;
      
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_LV2_AUDIO)) != NULL){
      ags_fx_lv2_audio_change_program(recall->data,
				      bank,
				      program);

      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(live_lv2_bridge)->audio,
		 "recall", &start_recall,
		 NULL);
      
    recall = start_recall;
      
    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_LV2_AUDIO)) != NULL){
      ags_fx_lv2_audio_change_program(recall->data,
				      bank,
				      program);

      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     g_object_unref);
    
    /* update UI */
    start_bulk_member = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input));

    plugin_port = start_plugin_port;
  
    while(plugin_port != NULL){
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	  /* find bulk member */
	  bulk_member = start_bulk_member;
	  
	  specifier = AGS_PLUGIN_PORT(plugin_port->data)->port_name;

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
	  	  
	      if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		if(live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index] == 0.0){
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       FALSE);
		}else{
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       TRUE);
		}
	      }else if(AGS_IS_DIAL(child_widget)){
		gfloat port_val;
		gdouble val;
		
		port_val = live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index];

		val = port_val;

		if(AGS_BULK_MEMBER(bulk_member->data)->conversion != NULL){
		  val = ags_conversion_convert(AGS_BULK_MEMBER(bulk_member->data)->conversion,
					       port_val,
					       TRUE);
		}
		
		gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment, val);
		gtk_widget_queue_draw(child_widget);

#ifdef AGS_DEBUG
		g_message(" --- %f", live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index]);
#endif
	      }
	
	      AGS_BULK_MEMBER(bulk_member->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);

	      break;
	    }
	  
	    bulk_member = bulk_member->next;
	  }	
	}
      }
      
      /* iterate */
      plugin_port = plugin_port->next;
    }

    g_list_free_full(start_plugin_port,
		     g_object_unref);
    
    g_list_free(start_bulk_member);
  }
}

void
ags_live_lv2_bridge_preset_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Conversion *lv2_conversion;
  AgsLv2Plugin *lv2_plugin;
  AgsLv2Preset *lv2_preset;
    
  GList *start_list, *list;
  GList *start_port_preset, *port_preset;
  GList *start_plugin_port, *plugin_port;

  gchar *preset_label;

  /* retrieve lv2 plugin */
  lv2_plugin = live_lv2_bridge->lv2_plugin;
  
  if(lv2_plugin == NULL){
    lv2_plugin =
      live_lv2_bridge->lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
								    live_lv2_bridge->filename,
								    live_lv2_bridge->effect);
    g_object_ref(lv2_plugin);

    live_lv2_bridge->lv2_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;
  }
  
  if(lv2_plugin == NULL){
    return;
  }

  /* preset label */
  preset_label = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
 
  /* preset */
  lv2_preset = NULL;
  list = ags_lv2_preset_find_preset_label(lv2_plugin->preset,
					  preset_label);
  
  if(list != NULL){
    lv2_preset = list->data;
  }

  /* port preset */
  if(lv2_preset == NULL){
    return;
  }

  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);
  
  g_object_get(lv2_preset,
	       "port-preset", &start_port_preset,
	       NULL);
  
  port_preset = start_port_preset;
  
  while(port_preset != NULL){
    plugin_port = ags_plugin_port_find_symbol(start_plugin_port,
					      AGS_LV2_PORT_PRESET(port_preset->data)->port_symbol);

    if(plugin_port != NULL){
      gfloat port_value;
      gdouble value;
      
      port_value = g_value_get_float(AGS_LV2_PORT_PRESET(port_preset->data)->port_value);

      value = port_value;
      
      list =
	start_list = ags_effect_bulk_get_bulk_member(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_input));
    
      while(list != NULL){
	if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
		      AGS_PLUGIN_PORT(plugin_port->data)->port_name)){
	  GtkWidget *child_widget;

	  //	AGS_BULK_MEMBER(list->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;

	  child_widget = ags_bulk_member_get_widget(AGS_BULK_MEMBER(list->data));
	  
	  lv2_conversion = (AgsLv2Conversion *) AGS_BULK_MEMBER(list->data)->conversion;
	  
	  if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	    if(value == 0.0){
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   FALSE);
	    }else{
	      gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					   TRUE);
	    }
	  }else if(AGS_IS_DIAL(child_widget)){
	    if(lv2_conversion != NULL){
	      value = ags_conversion_convert((AgsConversion *) lv2_conversion,
					     port_value,
					     TRUE);
	    }
	    
	    ags_dial_set_value(AGS_DIAL(child_widget),
			       value);
	    gtk_widget_queue_draw(child_widget);
	  }
	
	  //	AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
	  break;
	}

	list = list->next;
      }

      g_list_free(start_list);
    }
    
    /* iterate */
    port_preset = port_preset->next;
  }

  g_list_free_full(start_plugin_port,
		   g_object_unref);
  
  g_list_free(start_port_preset);
}

void
ags_live_lv2_bridge_dial_changed_callback(GtkWidget *dial, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gchar *block_scope;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, dial)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(dial,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(dial,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;
  val = gtk_adjustment_get_value(adjustment);
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }  
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			dial,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);

  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			dial,
			NULL);
  }
}

void
ags_live_lv2_bridge_scale_changed_callback(GtkWidget *scale, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gchar *block_scope;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, scale)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(scale,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(scale,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }

  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			scale,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			scale,
			NULL);
  }
}


void
ags_live_lv2_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gchar *block_scope;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, spin_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(spin_button,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(spin_button,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			spin_button,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
    
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			spin_button,
			NULL);
  }
}

void
ags_live_lv2_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;

  gchar *block_scope;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, check_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(check_button,
							  AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active((GtkToggleButton *) check_button);
  
  port_index = bulk_member->port_index;
  val = is_active ? 1.0: 0.0;
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			check_button,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			check_button,
			NULL);
  }
}

void
ags_live_lv2_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;

  gchar *block_scope;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, toggle_button)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(toggle_button,
							  AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active((GtkToggleButton *) toggle_button);
  
  port_index = bulk_member->port_index;
  val = is_active ? 1.0: 0.0;
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			toggle_button,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			toggle_button,
			NULL);
  }
}

void
ags_live_lv2_bridge_button_clicked_callback(GtkWidget *button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;

  gchar *block_scope;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     ((block_scope = g_hash_table_lookup(live_lv2_bridge->block_control, button)) != NULL &&
      !g_strcmp0(block_scope, AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE)) ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(button,
							  AGS_TYPE_BULK_MEMBER);

  port_index = bulk_member->port_index;
  val = 0.0;
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }

  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			button,
			AGS_LIVE_LV2_BRIDGE_BLOCK_CONTROL_BRIDGE);
  }
  
  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  if(block_scope == NULL){
    g_hash_table_insert(live_lv2_bridge->block_control,
			button,
			NULL);
  }
}
