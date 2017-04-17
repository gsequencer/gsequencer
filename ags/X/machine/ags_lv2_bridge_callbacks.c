/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/machine/ags_lv2_bridge_callbacks.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_lv2_conversion.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_lv2ui_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_port.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern GHashTable *ags_lv2_bridge_lv2ui_idle;

void
ags_lv2_bridge_show_gui_callback(GtkMenuItem *item, AgsLv2Bridge *lv2_bridge)
{
  AgsWindow *window;
  GtkWidget *plugin_widget;
  
  AgsLv2uiPlugin *lv2ui_plugin;
  
  GList *list;
  
  gchar *uri;
  
  LV2UI_DescriptorFunction lv2ui_descriptor;
  LV2UI_Descriptor *ui_descriptor;
  
  uint32_t ui_index;

  static const LV2_Feature **feature = {
    NULL,
  };

  window = gtk_widget_get_ancestor(lv2_bridge,
				   AGS_TYPE_WINDOW);
  
  if(lv2_bridge->gui_uri == NULL){
    return;
  }
  
  list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
				       lv2_bridge->gui_uri);
  
  if(list == NULL){
    return;
  }

  lv2ui_plugin = list->data;

  if(lv2_bridge->ui_handle == NULL){
    gchar *ui_filename;
    gchar *bundle_path;	

    LV2_Feature **feature;

    /* feature array */
    lv2_bridge->ui_feature = 
      feature = (LV2_Feature **) malloc(3 * sizeof(LV2_Feature *));

    /* idle interface */
    feature[0] = (LV2_Feature *) malloc(sizeof(LV2_Feature));    
    feature[0]->URI = LV2_UI__idleInterface;
    feature[0]->data = NULL;
    
    /* show interface */
    feature[1] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[1]->URI = LV2_UI__showInterface;
    feature[1]->data = NULL;

    feature[2] = NULL;
    
    if(AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so == NULL){
      AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so = dlopen(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
							RTLD_NOW);
    }
    
    if(AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so){
      lv2ui_descriptor = (LV2UI_Descriptor *) dlsym(AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so,
						    "lv2ui_descriptor\0");
      
      if(dlerror() == NULL && lv2ui_descriptor){
	ui_descriptor = lv2ui_descriptor(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect_index);
	ui_filename = AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename;
	
	/* instantiate */
	plugin_widget = NULL;
	
	bundle_path = g_strndup(ui_filename,
				rindex(ui_filename, '/') - ui_filename);

	lv2_bridge->ui_handle = ui_descriptor->instantiate(ui_descriptor,
							   lv2_bridge->uri,
							   bundle_path,
							   ags_lv2_bridge_lv2ui_write_function,
							   lv2_bridge,
							   &plugin_widget,
							   feature);

	if(ui_descriptor->extension_data != NULL){
	  lv2_bridge->ui_feature[0]->data = ui_descriptor->extension_data(LV2_UI__idleInterface);
	  lv2_bridge->ui_feature[1]->data = ui_descriptor->extension_data(LV2_UI__showInterface);

	  g_hash_table_insert(ags_lv2_bridge_lv2ui_idle,
			      lv2_bridge, ags_lv2_bridge_lv2ui_idle_timeout);
	  g_timeout_add(1000 / 30, (GSourceFunc) ags_lv2_bridge_lv2ui_idle_timeout, (gpointer) lv2_bridge);
	}	
      }
    }
  }

  if(lv2_bridge->ui_feature != NULL &&
     lv2_bridge->ui_feature[1]->data != NULL){
    ((struct _LV2UI_Show_Interface *) lv2_bridge->ui_feature[1]->data)->show(lv2_bridge->ui_handle);
  }
}

gboolean
ags_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLv2Bridge *lv2_bridge)
{
  
  return(TRUE);
}

void
ags_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer)
{
  AgsLv2Bridge *lv2_bridge;
  GtkWidget *widget;

  AgsChannel *channel;
  AgsRecallLv2 *recall_lv2;
  
  GList *effect_plugin;
  GList *recall;
  GList *port;

  gchar *control_port;

  GValue value = {0,};
  
  lv2_bridge = (AgsLv2Bridge *) controller;
  
  if(lv2_bridge == NULL){
    g_warning("ags_lv2_bridge_lv2ui_write_function() - lv2_bridge == NULL\0");
    return;
  }

  channel = AGS_MACHINE(lv2_bridge)->audio->input;
 
  switch(port_protocol){
  case 0:
    {
      if(buffer_size == sizeof(float)){
	g_value_init(&value, G_TYPE_FLOAT);
	g_value_set_float(&value, * ((float *) buffer));
      }else{
	g_value_init(&value, G_TYPE_POINTER);
	g_value_set_pointer(&value, buffer);
      }
    }
    break;
  default:
    g_warning("unknown lv2 port protocol\0");
  }
    
  while(channel != NULL){
    recall = ags_recall_get_by_effect(channel->play,
				      lv2_bridge->filename, lv2_bridge->effect);

    if(recall != NULL){
      recall_lv2 = recall->data;

      port = AGS_RECALL(recall_lv2)->port;
      control_port = g_strdup_printf("%d/%d\0",
				     port_index + 1,
				     g_list_length(port));
	
      while(port != NULL){
	if(!g_ascii_strncasecmp(AGS_PORT(port->data)->control_port,
				control_port,
				strlen(control_port))){
	  ags_port_safe_write(port->data,
			      &value);
	    
	  break;
	}
	  
	port = port->next;
      }

      free(control_port);
    }

    g_list_free(recall);
    channel = channel->next;
  }
}

void
ags_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLv2Bridge *lv2_bridge)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsChannel *channel;

    AgsLv2Plugin *lv2_plugin;
    
    LV2_Programs_Interface *program_interface;

    GList *port_descriptor_start, *port_descriptor;
    GList *bulk_member, *bulk_member_start;
    GList *recall;
    GList *port;
  
    gchar *name;
    gchar *specifier;
    
    guint bank, program;
    guint i;

    lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
						 lv2_bridge->filename,
						 lv2_bridge->effect);

    /* get program */
    gtk_tree_model_get(gtk_combo_box_get_model(combo_box), &iter,
		       0, &name,
		       1, &bank,
		       2, &program,
		       -1);    
    
#ifdef AGS_DEBUG
    g_message("%d %d\0", bank, program);
#endif
    
    program_interface = lv2_bridge->lv2_descriptor->extension_data(LV2_PROGRAMS__Interface);
    program_interface->select_program(lv2_bridge->lv2_handle,
				      bank,
				      program);

    /* update ports */
    channel = AGS_MACHINE(lv2_bridge)->audio->input;
    port_descriptor_start = AGS_BASE_PLUGIN(lv2_plugin)->port;
    
    while(channel != NULL){
      recall = channel->recall;

      while((recall = ags_recall_find_type(recall, AGS_TYPE_RECALL_LV2)) != NULL){
	AGS_RECALL_LV2(recall->data)->bank = (uint32_t) bank;
	AGS_RECALL_LV2(recall->data)->program = (uint32_t) program;

	port_descriptor = port_descriptor_start;
      
	for(i = 0; port_descriptor != NULL;){
	  if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	    if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	      specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;
	      port = AGS_RECALL(recall->data)->port;

	      while(port != NULL){
		if(!g_strcmp0(AGS_PORT(port->data)->specifier,
			      specifier)){
		  GValue value = {0,};

#ifdef AGS_DEBUG
		  g_message("%s %f\0", specifier, lv2_bridge->port_value[i]);
#endif
		  
		  g_value_init(&value,
			       G_TYPE_FLOAT);
		  g_value_set_float(&value,
				    lv2_bridge->port_value[i]);
		  ags_port_safe_write_raw(port->data,
					  &value);
		
		  break;
		}
	
		port = port->next;
	      }
	      
	      i++;
	    }
	  }

	  port_descriptor = port_descriptor->next;
	}
      
	recall = recall->next;
      }

      channel = channel->next;
    }

    /* update UI */
    bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input)->table);

    port_descriptor = port_descriptor_start;
  
    for(i = 0; port_descriptor != NULL;){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  /* find bulk member */
	  bulk_member = bulk_member_start;

	  specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;

#ifdef AGS_DEBUG
	  g_message("%s\0", specifier);
#endif
      
	  while(bulk_member != NULL){
	    if(AGS_IS_BULK_MEMBER(bulk_member->data) &&
	       !g_strcmp0(AGS_BULK_MEMBER(bulk_member->data)->specifier,
			  specifier)){
	      GtkWidget *child_widget;

	      AGS_BULK_MEMBER(bulk_member->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;

	      child_widget = gtk_bin_get_child((GtkBin *) AGS_BULK_MEMBER(bulk_member->data));
	  	  
	      if(GTK_IS_TOGGLE_BUTTON(child_widget)){
		if(lv2_bridge->port_value[i] == 0.0){
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       FALSE);
		}else{
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       TRUE);
		}
	      }else if(AGS_IS_DIAL(child_widget)){
		gdouble val;

		val = lv2_bridge->port_value[i];
		
		AGS_DIAL(child_widget)->adjustment->value = val;
		ags_dial_draw((AgsDial *) child_widget);

#ifdef AGS_DEBUG
		g_message(" --- %f\0", lv2_bridge->port_value[i]);
#endif
	      }
	
	      AGS_BULK_MEMBER(bulk_member->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);

	      i++;

	      break;
	    }
	  
	    bulk_member = bulk_member->next;
	  }	
	}
      }
      
      port_descriptor = port_descriptor->next;
    }

    g_list_free(bulk_member_start);
  }
}

void
ags_lv2_bridge_preset_changed_callback(GtkComboBox *combo_box, AgsLv2Bridge *lv2_bridge)
{
  GtkContainer *container;
  
  AgsLv2Conversion *lv2_conversion;
  AgsLv2Plugin *lv2_plugin;
  AgsLv2Preset *lv2_preset;
    
  GList *list, *list_start;
  GList *port_preset;
  GList *port_descriptor;

  gchar *preset_label;
  
  gdouble value;
  
  preset_label = gtk_combo_box_text_get_active_text(combo_box);

  /* retrieve lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       lv2_bridge->filename,
					       lv2_bridge->effect);
  
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

  container = AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge)->bulk_input)->table;

  port_preset = lv2_preset->port_preset;
  
  while(port_preset != NULL){
    port_descriptor = ags_port_descriptor_find_symbol(AGS_BASE_PLUGIN(lv2_plugin)->port,
						      AGS_LV2_PORT_PRESET(port_preset->data)->port_symbol);
    value = (gdouble) g_value_get_float(AGS_LV2_PORT_PRESET(port_preset->data)->port_value);
    
    list_start = 
      list = gtk_container_get_children(container);
    
    while(list != NULL){
      if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
		    AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name)){
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
	    //	      val = ags_lv2_conversion_convert(lv2_conversion,
	    //				  value,
	    //				  TRUE);
	  }
	    
	  gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				   value);
	  ags_dial_draw((AgsDial *) child_widget);
	}
	
	//	AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
	break;
      }

      list = list->next;
    }

    g_list_free(list_start);
    
    port_preset = port_preset->next;
  }
}
