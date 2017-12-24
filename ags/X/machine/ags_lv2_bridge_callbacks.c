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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

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

extern GHashTable *ags_lv2_bridge_lv2ui_handle;
extern GHashTable *ags_lv2_bridge_lv2ui_idle;

void
ags_lv2_bridge_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLv2Bridge *lv2_bridge)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_LV2_BRIDGE)->counter);

  g_object_set(AGS_MACHINE(lv2_bridge),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_LV2_BRIDGE);

  g_free(str);
}

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
    
    if(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_so == NULL){
      AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_so = dlopen(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
							   RTLD_NOW);
    }
    
    if(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_so){
      lv2ui_descriptor = (LV2UI_Descriptor *) dlsym(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_so,
						    "lv2ui_descriptor");
      
      if(dlerror() == NULL && lv2ui_descriptor){
	AgsEffectBridge *effect_bridge;
	GtkWidget *child_widget;

	GList *list_bulk_member, *list_bulk_member_start;

	gchar *str;
	
	lv2_bridge->ui_descriptor = 
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
	g_hash_table_insert(ags_lv2_bridge_lv2ui_handle,
			    lv2_bridge->ui_handle, lv2_bridge);
	//	ui_descriptor->cleanup = ags_lv2_bridge_lv2ui_cleanup_function;
	
	if(ui_descriptor->extension_data != NULL){
	  lv2_bridge->ui_feature[0]->data = ui_descriptor->extension_data(LV2_UI__idleInterface);
	  lv2_bridge->ui_feature[1]->data = ui_descriptor->extension_data(LV2_UI__showInterface);

	  g_hash_table_insert(ags_lv2_bridge_lv2ui_idle,
			      lv2_bridge->ui_handle, lv2_bridge);
	  g_timeout_add(1000 / 30, (GSourceFunc) ags_lv2_bridge_lv2ui_idle_timeout, (gpointer) lv2_bridge->ui_handle);
	}	

	/* set inital values */	
	effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge);
  
	list_bulk_member =
	  list_bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_input)->table);

	lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

	while(list_bulk_member != NULL){
	  uint32_t port_index;
	  float val;

	  child_widget = gtk_bin_get_child(AGS_BULK_MEMBER(list_bulk_member->data));
	  sscanf(AGS_BULK_MEMBER(list_bulk_member->data)->control_port,
		 "%d/",
		 &port_index);
	  
	  if(AGS_IS_DIAL(child_widget)){
	    val = AGS_DIAL(child_widget)->adjustment->value;
	    
	    lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
						  port_index,
						  sizeof(float),
						  0,
						  &val);
	  }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	    val = GTK_SPIN_BUTTON(child_widget)->adjustment->value;
	    
	    lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
						  port_index,
						  sizeof(float),
						  0,
						  &val);
	  }else if(GTK_IS_SCALE(child_widget)){
	    val = GTK_RANGE(child_widget)->adjustment->value;
	    
	    lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
						  port_index,
						  sizeof(float),
						  0,
						  &val);
	  }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	    val = ((gtk_toggle_button_get_active(child_widget)) ? 1.0: 0.0);
	    
	    lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
						  port_index,
						  sizeof(float),
						  0,
						  &val);
	  }else if(GTK_IS_BUTTON(child_widget)){
	    val = 0.0;
	    
	    lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
						  port_index,
						  sizeof(float),
						  0,
						  &val);
	  }

	  list_bulk_member = list_bulk_member->next;
	}

	lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);

	g_list_free(list_bulk_member_start);

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
ags_lv2_bridge_lv2ui_cleanup_function(LV2UI_Handle handle)
{
  AgsLv2Bridge *lv2_bridge;

  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

  lv2_bridge = g_hash_table_lookup(ags_lv2_bridge_lv2ui_handle,
				   handle);
  
  if(lv2_bridge != NULL){
    g_hash_table_remove(ags_lv2_bridge_lv2ui_handle,
			lv2_bridge->ui_handle);

    lv2_bridge->ui_handle = NULL;
  }
}

void
ags_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer)
{
  AgsLv2Bridge *lv2_bridge;
  AgsEffectBridge *effect_bridge;
  GtkWidget *child_widget;

  GList *list_bulk_member, *list_bulk_member_start;

  gchar *str;
  
  lv2_bridge = (AgsLv2Bridge *) controller;
  
  if(lv2_bridge == NULL){
    g_warning("ags_lv2_bridge_lv2ui_write_function() - lv2_bridge == NULL");

    return;
  }

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0){
    return;
  }
  
  if(port_protocol != 0){
    g_warning("ags_lv2_bridge_lv2ui_write_function() - unknown lv2 port protocol");

    return;
  }

  effect_bridge = AGS_EFFECT_BRIDGE(AGS_MACHINE(lv2_bridge)->bridge);
  
  list_bulk_member =
    list_bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(effect_bridge->bulk_input)->table);

  str = g_strdup_printf("%d/",
			port_index);

  while(list_bulk_member != NULL){
    if(!g_ascii_strncasecmp(str,
			    AGS_BULK_MEMBER(list_bulk_member->data)->control_port,
			    strlen(str))){
      lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

      child_widget = gtk_bin_get_child(AGS_BULK_MEMBER(list_bulk_member->data));

      if(AGS_IS_DIAL(child_widget)){
	ags_dial_set_value(child_widget,
			   ((float *) buffer)[0]);
      }else if(GTK_IS_SPIN_BUTTON(child_widget)){
	gtk_spin_button_set_value(child_widget,
				  ((float *) buffer)[0]);
      }else if(GTK_IS_SCALE(child_widget)){
	gtk_range_set_value(child_widget,
			    ((float *) buffer)[0]);
      }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	gtk_toggle_button_set_active(gtk_bin_get_child(list_bulk_member->data),
				     ((((float *) buffer)[0] != 0.0) ? TRUE: FALSE));
      }else if(GTK_IS_BUTTON(child_widget)){
	gtk_button_clicked(child_widget);
      }
	      
      lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
      
      break;
    }

    list_bulk_member = list_bulk_member->next;
  }

  g_list_free(list_bulk_member_start);
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
    g_message("%d %d", bank, program);
#endif
    
    program_interface = lv2_bridge->lv2_descriptor->extension_data(LV2_PROGRAMS__Interface);
    program_interface->select_program(lv2_bridge->lv2_handle[0],
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
		  g_message("%s %f", specifier, lv2_bridge->port_value[i]);
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
		g_message(" --- %f", lv2_bridge->port_value[i]);
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

void
ags_lv2_bridge_dial_changed_callback(GtkWidget *dial, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(dial,
					AGS_TYPE_BULK_MEMBER);

  g_object_get(dial,
	       "adjustment", &adjustment,
	       NULL);

  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  
  val = adjustment->value;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_vscale_changed_callback(GtkWidget *vscale, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(vscale,
					AGS_TYPE_BULK_MEMBER);

  g_object_get(vscale,
	       "adjustment", &adjustment,
	       NULL);

  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  
  val = adjustment->value;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_hscale_changed_callback(GtkWidget *hscale, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(hscale,
					AGS_TYPE_BULK_MEMBER);

  g_object_get(hscale,
	       "adjustment", &adjustment,
	       NULL);

  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  
  val = adjustment->value;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_spin_button_changed_callback(GtkWidget *spin_button, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(spin_button,
					AGS_TYPE_BULK_MEMBER);

  g_object_get(spin_button,
	       "adjustment", &adjustment,
	       NULL);

  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  
  val = adjustment->value;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_check_button_clicked_callback(GtkWidget *check_button, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(check_button,
					AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active(check_button);
  
  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  

  val = is_active ? 1.0: 0.0;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(toggle_button,
					AGS_TYPE_BULK_MEMBER);

  is_active = gtk_toggle_button_get_active(toggle_button);
  
  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  

  val = is_active ? 1.0: 0.0;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}

void
ags_lv2_bridge_button_clicked_callback(GtkWidget *button, AgsLv2Bridge *lv2_bridge)
{
  AgsBulkMember *bulk_member;
  GtkAdjustment *adjustment;

  gboolean is_active;
  uint32_t port_index;
  float val;

  if((AGS_LV2_BRIDGE_NO_UPDATE & (lv2_bridge->flags)) != 0 ||
     lv2_bridge->ui_handle == NULL){
    return;
  }

  bulk_member = gtk_widget_get_ancestor(button,
					AGS_TYPE_BULK_MEMBER);

  sscanf(bulk_member->control_port,
	 "%d/",
	 &port_index);  

  val = 0.0;
  
  lv2_bridge->flags |= AGS_LV2_BRIDGE_NO_UPDATE;

  lv2_bridge->ui_descriptor->port_event(lv2_bridge->ui_handle,
					port_index,
					sizeof(float),
					0,
					&val);
  
  lv2_bridge->flags &= (~AGS_LV2_BRIDGE_NO_UPDATE);
}
