/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_live_lv2_bridge_callbacks.h>

#include <lv2/lv2plug.in/ns/lv2ext/lv2_programs.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern GHashTable *ags_live_lv2_bridge_lv2ui_handle;
extern GHashTable *ags_live_lv2_bridge_lv2ui_idle;

gboolean ags_live_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLiveLv2Bridge *live_lv2_bridge);

void
ags_live_lv2_bridge_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_LIVE_LV2_BRIDGE)->counter);

  g_object_set(AGS_MACHINE(live_lv2_bridge),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_LIVE_LV2_BRIDGE);

  g_free(str);
}

gboolean
ags_live_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLiveLv2Bridge *live_lv2_bridge)
{
  gtk_widget_hide(widget);

  return(TRUE);
}

void
ags_live_lv2_bridge_show_gui_callback(GtkMenuItem *item, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsLv2Plugin *lv2_plugin;
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2_plugin = live_lv2_bridge->lv2_plugin;
  lv2ui_plugin = live_lv2_bridge->lv2ui_plugin;  
  
  if(lv2_plugin == NULL ||
     lv2ui_plugin == NULL){
    return;
  }

  if(live_lv2_bridge->ui_handle == NULL){
    LV2UI_Descriptor *plugin_descriptor;
    
    GValue *value;
    gchar **parameter_name;
    
    guint local_n_params;
    guint n_params;
    guint i;
    
    local_n_params = 4;
    n_params = 4;
    parameter_name = (gchar **) malloc(5 * sizeof(gchar *));
    value = g_new0(GValue,
		   4);

    parameter_name[0] = g_strdup("uri");
    g_value_init(&(value[0]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(value[0]), lv2_plugin->uri);

    parameter_name[1] = g_strdup("controller");
    g_value_init(&(value[1]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(value[1]), live_lv2_bridge);

    parameter_name[2] = g_strdup("write-function");
    g_value_init(&(value[2]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(value[2]), ags_live_lv2_bridge_lv2ui_write_function);

    parameter_name[3] = g_strdup("instance");
    g_value_init(&(value[3]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(value[3]), live_lv2_bridge->lv2_handle);

    parameter_name[4] = NULL;
        
    live_lv2_bridge->ui_handle = ags_base_plugin_instantiate_with_params(lv2ui_plugin,
									 &n_params, &parameter_name, &value);
    live_lv2_bridge->ui_descriptor = AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_descriptor;

    if(live_lv2_bridge->ui_handle != NULL){
      if(n_params > local_n_params){
	for(i = local_n_params; i < n_params; i++){
	  if(!g_ascii_strncasecmp(parameter_name[i],
				  "widget",
				  7)){
	    live_lv2_bridge->ui_widget = g_value_get_pointer(&(value[i]));
	  }
	}
      }
      
      g_hash_table_insert(ags_live_lv2_bridge_lv2ui_handle,
			  live_lv2_bridge->ui_handle, live_lv2_bridge);

      plugin_descriptor = live_lv2_bridge->ui_descriptor;
      
      if(plugin_descriptor->extension_data != NULL){
	lv2ui_plugin->feature[0]->data = plugin_descriptor->extension_data(LV2_UI__idleInterface);
	lv2ui_plugin->feature[1]->data = plugin_descriptor->extension_data(LV2_UI__showInterface);

	g_hash_table_insert(ags_live_lv2_bridge_lv2ui_idle,
			    live_lv2_bridge->ui_handle, live_lv2_bridge);
	g_timeout_add(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0, (GSourceFunc) ags_live_lv2_bridge_lv2ui_idle_timeout, (gpointer) live_lv2_bridge->ui_handle);
      }
    }

#if 0    
    for(i = 0; i < n_params; i++){
      g_value_unset(&(value[i]));
    }
#endif
    
    g_free(value);
    g_strfreev(parameter_name);
  }

  if(live_lv2_bridge->ui_handle != NULL){
    AgsEffectBridge *effect_bridge;
    GtkWidget *child_widget;

    GList *list_bulk_member, *list_bulk_member_start;

    gchar *str;
	
    /* set inital values */
    effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge);
  
    list_bulk_member =
      list_bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_output)->table);

    live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

    while(list_bulk_member != NULL){
      uint32_t port_index;
      float val;
	  
      child_widget = gtk_bin_get_child(GTK_BIN(AGS_BULK_MEMBER(list_bulk_member->data)));
      port_index = AGS_BULK_MEMBER(list_bulk_member->data)->port_index;
      
      if(AGS_IS_DIAL(child_widget)){
	val = gtk_adjustment_get_value(AGS_DIAL(child_widget)->adjustment);

	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						FALSE);
	}
	
	live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
						   port_index,
						   sizeof(float),
						   0,
						   &val);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(child_widget));
	    
	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						FALSE);
	}

	live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
						   port_index,
						   sizeof(float),
						   0,
						   &val);
      }else if(GTK_IS_SCALE(child_widget)){
	val = gtk_range_get_value(GTK_RANGE(child_widget));
	    
	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						FALSE);
	}

	live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
						   port_index,
						   sizeof(float),
						   0,
						   &val);
      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	val = ((gtk_toggle_button_get_active(child_widget)) ? 1.0: 0.0);
	    
	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						FALSE);
	}

	live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
						   port_index,
						   sizeof(float),
						   0,
						   &val);
      }else if(GTK_IS_BUTTON(child_widget)){
	val = 0.0;
	    
	if(AGS_BULK_MEMBER(list_bulk_member->data)->conversion != NULL){
	  val = (gfloat) ags_conversion_convert(AGS_BULK_MEMBER(list_bulk_member->data)->conversion,
						val,
						FALSE);
	}

	live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
						   port_index,
						   sizeof(float),
						   0,
						   &val);
      }

      list_bulk_member = list_bulk_member->next;
    }

    live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);

    g_list_free(list_bulk_member_start);
  }

  if(live_lv2_bridge->ui_handle != NULL &&
     live_lv2_bridge->ui_feature != NULL &&
     live_lv2_bridge->ui_feature[1] != NULL &&
     live_lv2_bridge->ui_feature[1]->data != NULL){
    ((struct _LV2UI_Show_Interface *) live_lv2_bridge->ui_feature[1]->data)->show(live_lv2_bridge->ui_handle[0]);
  }else if(live_lv2_bridge->ui_widget != NULL){
    GtkWindow *window;

    if(live_lv2_bridge->lv2_window == NULL){
      live_lv2_bridge->lv2_window = 
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_container_add(window,
			live_lv2_bridge->ui_widget);
      gtk_window_set_title(window,
			   live_lv2_bridge->gui_uri);
      
      g_signal_connect(window, "delete-event",
		       G_CALLBACK(ags_live_lv2_bridge_delete_event_callback), live_lv2_bridge);
    }else{
      window = live_lv2_bridge->lv2_window;
    }
    
    gtk_widget_show_all(window);
  }
}

void
ags_live_lv2_bridge_lv2ui_cleanup_function(LV2UI_Handle handle)
{
  AgsLiveLv2Bridge *live_lv2_bridge;

  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

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

  GList *list_bulk_member, *list_bulk_member_start;

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
    list_bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_output)->table);

  while(list_bulk_member != NULL){
    if(port_index == AGS_BULK_MEMBER(list_bulk_member->data)->port_index){
      live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

      child_widget = gtk_bin_get_child(GTK_BIN(AGS_BULK_MEMBER(list_bulk_member->data)));

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
	gtk_button_clicked((GtkButton *) child_widget);
      }
	      
      live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
      
      break;
    }

    list_bulk_member = list_bulk_member->next;
  }

  g_list_free(list_bulk_member_start);
}

void
ags_live_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsChannel *start_input;
    AgsChannel *channel, *next_channel;

    AgsLv2Plugin *lv2_plugin;
    
    LV2_Programs_Interface *program_interface;

    GList *start_plugin_port, *plugin_port;
    GList *bulk_member, *bulk_member_start;
    GList *start_recall, *recall;
    GList *start_port, *port;
  
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

    /* update ports */
    g_object_get(AGS_MACHINE(live_lv2_bridge)->audio,
		 "input", &start_input,
		 NULL);

    channel = start_input;
    
    if(channel != NULL){
      g_object_ref(channel);
    }

    next_channel = NULL;
    
    g_object_get(lv2_plugin,
		 "plugin-port", &start_plugin_port,
		 NULL);

    while(channel != NULL){
      g_object_get(channel,
		   "recall", &start_recall,
		   NULL);
      
      recall = start_recall;
      
      while((recall = ags_recall_find_type(recall, AGS_TYPE_RECALL_LV2)) != NULL){
	AGS_RECALL_LV2(recall->data)->bank = (uint32_t) bank;
	AGS_RECALL_LV2(recall->data)->program = (uint32_t) program;
	
	plugin_port = start_plugin_port;
	
	while(plugin_port != NULL){
	  if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	    if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	      specifier = AGS_PLUGIN_PORT(plugin_port->data)->port_name;
	      port = AGS_RECALL(recall->data)->port;
	      
	      while(port != NULL){
		if(!g_strcmp0(AGS_PORT(port->data)->specifier,
			      specifier)){
		  GValue value = {0,};

#ifdef AGS_DEBUG
		  g_message("%s %f", specifier, live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index]);
#endif
		  
		  g_value_init(&value,
			       G_TYPE_FLOAT);
		  g_value_set_float(&value,
				    live_lv2_bridge->port_value[AGS_PLUGIN_PORT(plugin_port->data)->port_index]);
		  ags_port_safe_write_raw(port->data,
					  &value);
		
		  break;
		}
	
		port = port->next;
	      }
	    }
	  }

	  /* iterate */
	  plugin_port = plugin_port->next;
	}

	/* iterate */
	recall = recall->next;
      }

      g_list_free_full(start_recall,
		       g_object_unref);
      
      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    /* unref */
    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
    
    /* update UI */
    bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output)->table);

    plugin_port = start_plugin_port;
  
    while(plugin_port != NULL){
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	  /* find bulk member */
	  bulk_member = bulk_member_start;
	  
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

	      child_widget = gtk_bin_get_child((GtkBin *) AGS_BULK_MEMBER(bulk_member->data));
	  	  
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
		gtk_widget_queue_draw((AgsDial *) child_widget);

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

    /* unref */
    if(start_input != NULL){
      g_object_unref(start_input);
    }

    g_list_free_full(start_plugin_port,
		     g_object_unref);
    
    g_list_free(bulk_member_start);
  }
}

void
ags_live_lv2_bridge_preset_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkContainer *container;
  
  AgsLv2Conversion *lv2_conversion;
  AgsLv2Plugin *lv2_plugin;
  AgsLv2Preset *lv2_preset;
    
  GList *list, *list_start;
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

  container = GTK_CONTAINER(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output)->table);

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
      
      list_start = 
	list = gtk_container_get_children(container);
    
      while(list != NULL){
	if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
		      AGS_PLUGIN_PORT(plugin_port->data)->port_name)){
	  GtkWidget *child_widget;
	  
	  //	AGS_BULK_MEMBER(list->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;

	  child_widget = gtk_bin_get_child((GtkBin *) AGS_BULK_MEMBER(list->data));
	  
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
	      value = ags_conversion_convert(lv2_conversion,
					     port_value,
					     TRUE);
	    }
	    
	    gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				     value);
	    gtk_widget_queue_draw((AgsDial *) child_widget);
	  }
	
	  //	AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
	  break;
	}

	list = list->next;
      }

      g_list_free(list_start);
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

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
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
  
  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_vscale_changed_callback(GtkWidget *vscale, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(vscale,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(vscale,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_hscale_changed_callback(GtkWidget *hscale, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
     live_lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = (AgsBulkMember *) gtk_widget_get_ancestor(hscale,
							  AGS_TYPE_BULK_MEMBER);

  g_object_get(hscale,
	       "adjustment", &adjustment,
	       NULL);

  port_index = bulk_member->port_index;

  val = gtk_adjustment_get_value(adjustment);
  
  if(bulk_member->conversion != NULL){
    val = (gfloat) ags_conversion_convert(bulk_member->conversion,
					  val,
					  FALSE);
  }

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
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

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
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

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
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

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}

void
ags_live_lv2_bridge_button_clicked_callback(GtkWidget *button, AgsLiveLv2Bridge *live_lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LIVE_LV2_BRIDGE_NO_UPDATE & (live_lv2_bridge->flags)) != 0 ||
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

  live_lv2_bridge->flags |= AGS_LIVE_LV2_BRIDGE_NO_UPDATE;

  live_lv2_bridge->ui_descriptor->port_event(live_lv2_bridge->ui_handle[0],
					     port_index,
					     sizeof(float),
					     0,
					     &val);
  
  live_lv2_bridge->flags &= (~AGS_LIVE_LV2_BRIDGE_NO_UPDATE);
}
