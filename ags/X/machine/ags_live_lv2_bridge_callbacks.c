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

#include <ags/X/machine/ags_live_lv2_bridge_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <lv2/lv2plug.in/ns/lv2ext/lv2_programs.h>

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

extern GHashTable *ags_live_lv2_bridge_lv2ui_handle;
extern GHashTable *ags_live_lv2_bridge_lv2ui_idle;

void
ags_live_lv2_bridge_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLiveLv2Bridge *live_lv2_bridge)
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

void
ags_live_lv2_bridge_show_gui_callback(GtkMenuItem *item, AgsLiveLv2Bridge *live_lv2_bridge)
{
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

  if(live_lv2_bridge->gui_uri == NULL){
    return;
  }
  
  list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
				       live_lv2_bridge->gui_uri);
  
  if(list == NULL){
    return;
  }

  lv2ui_plugin = list->data;

  if(live_lv2_bridge->ui_handle == NULL){
    gchar *ui_filename;
    gchar *bundle_path;	

    LV2_Feature **feature;

    /* feature array */
    live_lv2_bridge->ui_feature = 
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
	ui_descriptor = lv2ui_descriptor(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect_index);
	ui_filename = AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename;
	
	/* instantiate */
	plugin_widget = NULL;
	
	bundle_path = g_strndup(ui_filename,
				rindex(ui_filename, '/') - ui_filename);

	live_lv2_bridge->ui_handle = ui_descriptor->instantiate(ui_descriptor,
								live_lv2_bridge->uri,
								bundle_path,
								ags_live_lv2_bridge_lv2ui_write_function,
								live_lv2_bridge,
								&plugin_widget,
								feature);
	g_hash_table_insert(ags_live_lv2_bridge_lv2ui_handle,
			    live_lv2_bridge->ui_handle, live_lv2_bridge);

	ui_descriptor->cleanup = ags_live_lv2_bridge_lv2ui_cleanup_function;

	if(ui_descriptor->extension_data != NULL){
	  live_lv2_bridge->ui_feature[0]->data = ui_descriptor->extension_data(LV2_UI__idleInterface);
	  live_lv2_bridge->ui_feature[1]->data = ui_descriptor->extension_data(LV2_UI__showInterface);

	  g_hash_table_insert(ags_live_lv2_bridge_lv2ui_idle,
			      live_lv2_bridge->ui_handle, live_lv2_bridge);
	  g_timeout_add(1000 / 30, (GSourceFunc) ags_live_lv2_bridge_lv2ui_idle_timeout, (gpointer) live_lv2_bridge->ui_handle);
	}	
      }
    }
  }

  if(live_lv2_bridge->ui_feature != NULL &&
     live_lv2_bridge->ui_feature[1]->data != NULL){
    ((struct _LV2UI_Show_Interface *) live_lv2_bridge->ui_feature[1]->data)->show(live_lv2_bridge->ui_handle);
  }
}

gboolean
ags_live_lv2_bridge_delete_event_callback(GtkWidget *widget, GdkEvent *event, AgsLiveLv2Bridge *live_lv2_bridge)
{
  
  return(TRUE);
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
    list = ags_lv2ui_plugin_find_gui_uri(ags_lv2ui_manager_get_instance()->lv2ui_plugin,
					 live_lv2_bridge->gui_uri);

#ifdef AGS_DEBUG
    g_message("%s", live_lv2_bridge->gui_uri);
#endif
    
    if(list != NULL){
      lv2ui_plugin = list->data;
      AGS_BASE_PLUGIN(lv2ui_plugin)->ui_plugin_so = NULL;
    }

    g_hash_table_remove(ags_live_lv2_bridge_lv2ui_handle,
			live_lv2_bridge->ui_handle);

    live_lv2_bridge->ui_handle = NULL;
  }
}

void
ags_live_lv2_bridge_lv2ui_write_function(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t port_protocol, const void *buffer)
{
  AgsLiveLv2Bridge *live_lv2_bridge;
  GtkWidget *widget;

  AgsAudio *audio;
  AgsPlayLv2Audio *play_lv2_audio;
  
  GList *effect_plugin;
  GList *start_play;
  GList *start_recall;
  GList *start_effect, *effect;
  GList *port;

  gchar *control_port;

  GValue value = {0,};
  
  live_lv2_bridge = (AgsLiveLv2Bridge *) controller;
  
  if(live_lv2_bridge == NULL){
    g_warning("ags_live_lv2_bridge_lv2ui_write_function() - live_lv2_bridge == NULL");
    return;
  }

  audio = AGS_MACHINE(live_lv2_bridge)->audio;
 
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
    g_warning("unknown lv2 port protocol");
  }

  g_object_get(audio,
	       "play", &start_play,
	       "recall", &start_recall,
	       NULL);
  
  /* play */
  effect =
    start_effect = ags_recall_get_by_effect(start_play,
					    live_lv2_bridge->filename, live_lv2_bridge->effect);
  
  if(effect != NULL){
    play_lv2_audio = effect->data;

    port = AGS_RECALL(play_lv2_audio)->port;
    control_port = g_strdup_printf("%d/%d",
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

  g_list_free(start_recall);
  g_list_free(start_effect);

  /* recall */
  effect =
    start_effect = ags_recall_get_by_effect(start_recall,
					    live_lv2_bridge->filename, live_lv2_bridge->effect);
  
  if(effect != NULL){
    play_lv2_audio = effect->data;

    port = AGS_RECALL(play_lv2_audio)->port;
    control_port = g_strdup_printf("%d/%d",
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

  g_list_free(start_recall);
  g_list_free(start_effect);
}

void
ags_live_lv2_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveLv2Bridge *live_lv2_bridge)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(combo_box,
				   &iter)){
    AgsAudio *audio;

    AgsLv2Plugin *lv2_plugin;
    
    LV2_Programs_Interface *program_interface;

    GList *start_plugin_port, *plugin_port;
    GList *bulk_member, *bulk_member_start;
    GList *recall;
    GList *port;
  
    gchar *name;
    gchar *specifier;
    
    guint bank, program;
    guint i;

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
    audio = AGS_MACHINE(live_lv2_bridge)->audio;

    g_object_get(lv2_plugin,
		 "plugin-port", &start_plugin_port,
		 NULL);
    

    recall = audio->play;

    while((recall = ags_recall_find_type(recall, AGS_TYPE_PLAY_LV2_AUDIO)) != NULL){
      AGS_PLAY_LV2_AUDIO(recall->data)->bank = (uint32_t) bank;
      AGS_PLAY_LV2_AUDIO(recall->data)->program = (uint32_t) program;

      plugin_port = start_plugin_port;
      
      for(i = 0; plugin_port != NULL;){
	if(ags_plugin_port_test_flags(AGS_PLUGIN_PORT(plugin_port->data), AGS_PLUGIN_PORT_CONTROL)){
	  if(ags_plugin_port_test_flags(AGS_PLUGIN_PORT(plugin_port->data), AGS_PLUGIN_PORT_INPUT)){
	    specifier = AGS_PLUGIN_PORT(plugin_port->data)->port_name;
	    port = AGS_RECALL(recall->data)->port;

	    while(port != NULL){
	      if(!g_strcmp0(AGS_PORT(port->data)->specifier,
			    specifier)){
		GValue value = {0,};

#ifdef AGS_DEBUG
		g_message("%s %f", specifier, live_lv2_bridge->port_value[i]);
#endif
		  
		g_value_init(&value,
			     G_TYPE_FLOAT);
		g_value_set_float(&value,
				  live_lv2_bridge->port_value[i]);
		ags_port_safe_write_raw(port->data,
					&value);
	
		break;
	      }
	
	      port = port->next;
	    }
	    
	    i++;
	  }
	}

	plugin_port = plugin_port->next;
      }
      
      recall = recall->next;
    }

    /* update UI */
    bulk_member_start = gtk_container_get_children((GtkContainer *) AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(live_lv2_bridge)->bridge)->bulk_output)->table);

    plugin_port = start_plugin_port;
  
    for(i = 0; plugin_port != NULL;){
      if(ags_plugin_port_test_flags(AGS_PLUGIN_PORT(plugin_port->data), AGS_PLUGIN_PORT_CONTROL)){
	if(ags_plugin_port_test_flags(AGS_PLUGIN_PORT(plugin_port->data), AGS_PLUGIN_PORT_INPUT)){
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
		if(live_lv2_bridge->port_value[i] == 0.0){
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       FALSE);
		}else{
		  gtk_toggle_button_set_active((GtkToggleButton *) child_widget,
					       TRUE);
		}
	      }else if(AGS_IS_DIAL(child_widget)){
		gdouble val;

		val = live_lv2_bridge->port_value[i];
		
		AGS_DIAL(child_widget)->adjustment->value = val;
		ags_dial_draw((AgsDial *) child_widget);

#ifdef AGS_DEBUG
		g_message(" --- %f", live_lv2_bridge->port_value[i]);
#endif
	      }
	
	      AGS_BULK_MEMBER(bulk_member->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);

	      break;
	    }
	  
	    bulk_member = bulk_member->next;
	  }	

	  i++;
	}
      }
      
      plugin_port = plugin_port->next;
    }

    g_list_free(start_plugin_port);
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
  GList *port_preset;
  GList *start_plugin_port, *plugin_port;

  gchar *preset_label;
  
  gdouble value;
  
  preset_label = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  /* retrieve lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       live_lv2_bridge->filename,
					       live_lv2_bridge->effect);
  
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

  g_object_get(AGS_BASE_PLUGIN(lv2_plugin),
	       "plugin-port", &start_plugin_port,
	       NULL);
  
  port_preset = lv2_preset->port_preset;
  
  while(port_preset != NULL){
    plugin_port = ags_plugin_port_find_symbol(start_plugin_port,
					      AGS_LV2_PORT_PRESET(port_preset->data)->port_symbol);
    value = (gdouble) g_value_get_float(AGS_LV2_PORT_PRESET(port_preset->data)->port_value);
    
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
